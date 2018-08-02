/***************************************************************************
 *   Copyright (C) 2005 to 2007 by Jonathan Duddington                     *
 *   email: jonsd@users.sourceforge.net                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write see:                           *
 *               <http://www.gnu.org/licenses/>.                           *
 ***************************************************************************/

#include "StdAfx.h"

#include "speech.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef PLATFORM_DOS
#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <winreg.h>
#else
#include <unistd.h>
#endif
#endif

#ifndef NEED_GETOPT
#include <getopt.h>
#endif
#include <time.h>
#include <signal.h>
#include <locale.h>
#include <sys/stat.h>

#include "speak_lib.h"
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "translate.h"



extern void Write4Bytes(FILE *f, int value);
char path_home[N_PATH_HOME];    // this is the espeak-data directory

char filetype[5];
char wavefile[200];
int (* uri_callback)(int, const char *, const char *) = NULL;
int (* phoneme_callback)(const char *) = NULL;

FILE *f_wave = NULL;
int quiet = 0;
unsigned int samples_total = 0;
unsigned int samples_split = 0;
unsigned int wavefile_count = 0;
int end_of_sentence = 0;



void DisplayVoices(FILE *f_out, char *language);

USHORT voice_pcnt[N_PEAKS+1][3];



int GetFileLength(const char *filename)
{//====================================
	struct stat statbuf;

	if(stat(filename,&statbuf) != 0)
		return(0);

	if((statbuf.st_mode & S_IFMT) == S_IFDIR)
//	if(S_ISDIR(statbuf.st_mode))
		return(-2);  // a directory

	return(statbuf.st_size);
}  // end of GetFileLength


char *Alloc(int size)
{//==================
	char *p;
	if((p = (char *)malloc(size)) == NULL)
		fprintf(stderr,"Can't allocate memory\n");
	return(p);
}

void Free(void *ptr)
{//=================
	if(ptr != NULL)
		free(ptr);
}


void DisplayVoices(FILE *f_out, char *language)
{//============================================
	int ix;
	const char *p;
	int len;
	int count;
	const espeak_VOICE *v;
	const char *lang_name;
	char age_buf[12];
	const espeak_VOICE **voices;
	espeak_VOICE voice_select;

	static char genders[4] = {' ','M','F',' '};

	if((language != NULL) && (language[0] != 0))
	{
		// display only voices for the specified language, in order of priority
		voice_select.languages = language;
		voice_select.age = 0;
		voice_select.gender = 0;
		voice_select.name = NULL;
		voices = espeak_ListVoices(&voice_select);
	}
	else
	{
		voices = espeak_ListVoices(NULL);
	}

	fprintf(f_out,"Pty Language Age/Gender VoiceName       File        Other Langs\n");

	for(ix=0; (v = voices[ix]) != NULL; ix++)
	{
		count = 0;
		p = v->languages;
		while(*p != 0)
		{
			len = strlen(p+1);
			lang_name = p+1;

			if(v->age == 0)
				strcpy(age_buf,"   ");
			else
				sprintf(age_buf,"%3d",v->age);

			if(count==0)
			{
				fprintf(f_out,"%2d  %-12s%s%c  %-17s %-11s ",
               p[0],lang_name,age_buf,genders[v->gender],v->name,v->identifier);
			}
			else
			{
				fprintf(f_out,"(%s %d)",lang_name,p[0]);
			}
			count++;
			p += len+2;
		}
//		if(scores)
//			fprintf(f_out,"%3d  ",v->score);
		fputc('\n',f_out);
	}
}   //  end of DisplayVoices




/*static int OpenWaveFile(const char *path, int rate)
//=================================================
{
	// Set the length of 0x7ffff000 for --stdout
	// This will be changed to the correct length for -w (write to file)
	static unsigned char wave_hdr[44] = {
		'R','I','F','F',0x24,0xf0,0xff,0x7f,'W','A','V','E','f','m','t',' ',
		0x10,0,0,0,1,0,1,0,  9,0x3d,0,0,0x12,0x7a,0,0,
		2,0,0x10,0,'d','a','t','a',  0x00,0xf0,0xff,0x7f};

	if(path == NULL)
		return(2);

	if(strcmp(path,"stdout")==0)
		f_wave = stdout;
	else
		f_wave = fopen(path,"wb");

	if(f_wave != NULL)
	{
		fwrite(wave_hdr,1,24,f_wave);
		Write4Bytes(f_wave,rate);
		Write4Bytes(f_wave,rate * 2);
		fwrite(&wave_hdr[32],1,12,f_wave);
		return(0);
	}
	return(1);
}   //  end of OpenWaveFile




static void CloseWaveFile()
//=========================
{
   unsigned int pos;

   if((f_wave == NULL) || (f_wave == stdout))
      return;

   fflush(f_wave);
   pos = ftell(f_wave);

	fseek(f_wave,4,SEEK_SET);
	Write4Bytes(f_wave,pos - 8);

	fseek(f_wave,40,SEEK_SET);
	Write4Bytes(f_wave,pos - 44);


   fclose(f_wave);
   f_wave = NULL;

} // end of CloseWaveFile*/




void MarkerEvent(int type, unsigned int char_position, int value, unsigned char *out_ptr)
{//======================================================================================
// Do nothing in the command-line version.
	if(type == 2)
		end_of_sentence = 1;
}  // end of MarkerEvent


/*static int WavegenFile(void)
{//=========================
	int finished;
	unsigned char wav_outbuf[512];
	char fname[210];

	out_ptr = out_start = wav_outbuf;
	out_end = wav_outbuf + sizeof(wav_outbuf);

	finished = WavegenFill(0);

	if(quiet)
		return(finished);

	if(f_wave == NULL)
	{
		sprintf(fname,"%s_%.2d%s",wavefile,++wavefile_count,filetype);
		if(OpenWaveFile(fname, samplerate) != 0)
			return(1);
	}

	if(end_of_sentence)
	{
		end_of_sentence = 0;
		if((samples_split > 0 ) && (samples_total > samples_split))
		{
			CloseWaveFile();
			samples_total = 0;
		}
	}

	if(f_wave != NULL)
	{
		samples_total += (out_ptr - wav_outbuf)/2;
		fwrite(wav_outbuf, 1, out_ptr - wav_outbuf, f_wave);
	}
	return(finished);
}  // end of WavegenFile*/



static int initialise(void)
{//========================
	int param;
	int result;

	// It seems that the wctype functions don't work until the locale has been set
	// to something other than the default "C".  Then, not only Latin1 but also the
	// other characters give the correct results with iswalpha() etc.
#ifdef PLATFORM_RISCOS
   setlocale(LC_CTYPE,"ISO8859-1");
#else
	if(setlocale(LC_CTYPE,"en_US.UTF-8") == NULL)
	{
		if(setlocale(LC_CTYPE,"UTF-8") == NULL)
			setlocale(LC_CTYPE,"");
	}
#endif


	WavegenInit(22050,0);   // 22050
	if((result = LoadPhData()) != 1)
	{
		if(result == -1)
		{
//			fprintf(stderr,"Failed to load espeak-data\n");
			return ERR_FAILED_TO_LOAD_DATA;
		}
		else
			fprintf(stderr,"Wrong version of espeak-data 0x%x (expects 0x%x) at %s\n",result,version_phdata,path_home);
	}
	LoadConfig();
	SetVoiceStack(NULL);
	SynthesizeInit();

	for(param=0; param<N_SPEECH_PARAM; param++)
		param_stack[0].parameter[param] = param_defaults[param];

	return(0);
}


static void StopSpeak(int unused)
{//==============================
	signal(SIGINT,SIG_IGN);
	// DEBUG
//	printf("\n*** Interrupting speech output (use Ctrl-D to actually quit).\n");
	fflush(stdout);
	SpeakNextClause(NULL,NULL,5);
	signal(SIGINT,StopSpeak);
}  //  end of StopSpeak()

#ifdef NEED_GETOPT
	struct option {
		char *name;
		int has_arg;
		int *flag;
		int val;
	};
	int optind;
	static int optional_argument;
	static const char *arg_opts = "abfgklpsvw";  // which options have arguments
	static char *opt_string="";
#define no_argument 0
#define required_argument 1
#define optional_argument 2
#endif


int DoSpeak(int pitch_adjustment, int speed,int wordgap,char *voicename,char *p_text)
//==============================
{
	FILE *f_text=NULL;
//	const char *p_text=NULL;
	char data_path[200]="";   // use default path for espeak-data
//	int c;
//	int value;
//	int speed=170;
//	int ix;
//	char *optarg2;
	int amp = 100;     // default
//	int wordgap = 0;
	int speaking = 0;
//	int flag_compile = 0;
//	int pitch_adjustment = 50;
	espeak_VOICE voice_select;
//	char filename[200];
//	char voicename[40];

//	voicename[0] = 0;
	mbrola_name[0] = 0;
	wavefile[0] = 0;
//	filename[0] = 0;
	option_linelength = 0;
	option_phonemes = 0;
//	option_waveout = 0;
	option_wordgap = 0;
	option_endpause = 1;
	option_phoneme_input = 1;
	option_multibyte = espeakCHARS_AUTO;  // auto
	f_trans = stdout;

/*		switch (c)
		{
		case 'b':
			// input character encoding, 8bit, 16bit, UTF8
			option_multibyte = espeakCHARS_8BIT;
			if((sscanf(optarg2,"%d",&value) == 1) && (value <= 4))
				option_multibyte= value;
			break;

		case 'h':
			printf("\nspeak text-to-speech: %s\n%s",version_string,help_text);
			exit(0);
			break;

		case 'k':
			option_capitals = atoi(optarg2);
			break;

		case 'x':
			option_phonemes = 1;
			break;

		case 'X':
			option_phonemes = 2;
			break;

		case 'm':
			option_ssml = 1;
			break;

		case 'p':
			pitch_adjustment = atoi(optarg2);
			if(pitch_adjustment > 99) pitch_adjustment = 99;
			break;

		case 'q':
			quiet = 1;
			break;

		case 'f':
			strncpy0(filename,optarg2,sizeof(filename));
			break;

		case 'l':
			value = 0;
			value = atoi(optarg2);
			option_linelength = value;
			break;

		case 'a':
			amp = atoi(optarg2);
			break;

		case 's':
			speed = atoi(optarg2);
			break;

		case 'g':
			wordgap = atoi(optarg2);
			break;

		case 'v':
			strncpy0(voicename,optarg2,sizeof(voicename));
			break;

		case 'w':
			option_waveout = 1;
			strncpy0(wavefile,optarg2,sizeof(wavefile));
			break;

		case 'z':
			option_endpause = 0;
			break;

		case 0x100:		// --stdin
			flag_stdin = 1;
			break;

		case 0x105:		// --stdout
			option_waveout = 1;
			strcpy(wavefile,"stdout");
			break;

		case 0x101:    // --compile-debug
		case 0x102:		// --compile
			if(optarg2 != NULL)
				strncpy0(voicename,optarg2,sizeof(voicename));
			flag_compile = c;
			break;

		case 0x103:		// --punct
			option_punctuation = 1;
			if(optarg2 != NULL)
			{
				ix = 0;
				while((ix < N_PUNCTLIST) && ((option_punctlist[ix] = optarg2[ix]) != 0)) ix++;
				option_punctlist[N_PUNCTLIST-1] = 0;
				option_punctuation = 2;
			}
			break;

		case 0x104:   // --voices
			init_path(argv[0],data_path);
			DisplayVoices(stdout,optarg2);
			exit(0);

		case 0x106:   // -- split
			if(optarg2 == NULL)
				samples_split = 30;  // default 30 minutes
			else
				samples_split = atoi(optarg2);
			break;

		case 0x107:  // --path
			data_path = optarg2;
			break;

		case 0x108:  // --phonout
			if((f_trans = fopen(optarg2,"w")) == NULL)
			{
				fprintf(stderr,"Can't write to: %s\n",optarg2);
				f_trans = stderr;
			}
			break;

		default:
			exit(0);
		}
	}*/

#ifdef ENV_LINUX
	strcpy(data_path,PATH_ESPEAK_DATA);
	strcpy(path_home,PATH_ESPEAK_DATA);
#else
	strcpy(path_home,"flowplugins\\espeak-data");
#endif
	initialise();


/*	if(flag_compile)
	{
		LoadVoice(voicename,5);

#ifdef PLATFORM_DOS
		char path_dsource[sizeof(path_home)+20];
		strcpy(path_dsource,path_home);
		path_dsource[strlen(path_home)-11] = 0;  // remove "espeak-data" from the end
		strcat(path_dsource,"dictsource\\");
		CompileDictionary(path_dsource,dictionary_name,NULL,NULL, flag_compile & 0x1);
#else
#ifdef PLATFORM_WINDOWS
		char path_dsource[sizeof(path_home)+20];
		strcpy(path_dsource,path_home);
		path_dsource[strlen(path_home)-11] = 0;  // remove "espeak-data" from the end
		strcat(path_dsource,"dictsource\\");
		CompileDictionary(path_dsource,dictionary_name,NULL,NULL, flag_compile & 0x1);
#else
		CompileDictionary(NULL,dictionary_name,NULL,NULL, flag_compile & 0x1);
#endif
#endif
		exit(0);
	}*/


	if(voicename[0] == 0)
		strcpy(voicename,"default");

	if(SetVoiceByName(voicename) != EE_OK)
	{
		memset(&voice_select,0,sizeof(voice_select));
		voice_select.languages = voicename;
		if(SetVoiceByProperties(&voice_select) != EE_OK)
		{
//			fprintf(stderr,"%svoice '%s'\n",err_load,voicename);
			return ERR_LOADING_VOICE;
		}
	}

	SetParameter(espeakRATE,speed,0);
	SetParameter(espeakVOLUME,amp,0);
	SetParameter(espeakCAPITALS,option_capitals,0);
	SetParameter(espeakPUNCTUATION,option_punctuation,0);
	SetParameter(espeakWORDGAP,wordgap,0);

	if(pitch_adjustment != 50)
	{
		SetParameter(espeakPITCH,pitch_adjustment,0);
	}
	DoVoiceChange(voice);

/*	if(filename[0]==0)
	{
		if((optind < argc) && (flag_stdin == 0))
		{
			// there's a non-option parameter, and no -f or --stdin
			// use it as text
			p_text = argv[optind];
		}
		else
		{
			f_text = stdin;
			if(flag_stdin == 0)
				option_linelength = -1;  // single input lines on stdin
		}
	}
	else
	{
		f_text = fopen(filename,"r");
	}*/

/*	if((f_text == NULL) && (p_text == NULL))
	{
		fprintf(stderr,"%sfile '%s'\n",err_load,filename);
		exit(1);
	}*/

/*	if(option_waveout || quiet)
	{
		if(quiet)
		{
			// no sound output
			OpenWaveFile(NULL,samplerate);
			option_waveout = 1;
		}
		else
		{
			// write sound output to a WAV file
			samples_split = (samplerate * samples_split) * 60;

			if(samples_split)
			{
				// don't open the wav file until we start generating speech
				char *extn;
				extn = strrchr(wavefile,'.');
				if((extn != NULL) && ((wavefile + strlen(wavefile) - extn) <= 4))
				{
					strcpy(filetype,extn);
					*extn = 0;
				}
			}
			else
			if(OpenWaveFile(wavefile,samplerate) != 0)
			{
				fprintf(stderr,"Can't write to output file '%s'\n'",wavefile);
				exit(3);
			}
		}

		InitText(0);
		SpeakNextClause(f_text,p_text,0);

		ix = 1;
		for(;;)
		{
			if(WavegenFile() != 0)
			{
				if(ix == 0)
					break;   // finished, wavegen command queue is empty
			}

			if(Generate(phoneme_list,&n_phoneme_list,1)==0)
			{
				ix = SpeakNextClause(NULL,NULL,1);
			}
		}

		CloseWaveFile();
	}
	else*/
	{
		// Silence on ^C or SIGINT
//		signal(SIGINT,StopSpeak);

		// output sound using portaudio
		WavegenInitSound();

		InitText(0);
		SpeakNextClause(f_text,p_text,0);

		if(option_quiet)
		{
			while(SpeakNextClause(NULL,NULL,1) != 0);
			return(0);
		}

#ifdef USE_PORTAUDIO
		speaking = 1;
		while(speaking)
		{
			// NOTE: if nanosleep() isn't recognised on your system, try replacing
			// this by  sleep(1);
#ifdef PLATFORM_WINDOWS
			Sleep(300);   // 0.3s
#else
#ifdef USE_NANOSLEEP
			struct timespec period;
			struct timespec remaining;
			period.tv_sec = 0;
			period.tv_nsec = 300000000;  // 0.3 sec
			nanosleep(&period,&remaining);
#else
			sleep(1);
#endif
#endif
			if(SynthOnTimer() != 0)
				speaking = 0;
		}
#else
		fprintf(stderr,"-w option must be used because the program was built without a sound interface\n");
#endif  // USE_PORTAUDIO
	}

	if((f_trans != stdout) && (f_trans != stderr))
		fclose(f_trans);  // needed for WinCe
	return(0);
}
