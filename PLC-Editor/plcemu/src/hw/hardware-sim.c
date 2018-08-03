#include <fcntl.h>

#include "plclib.h"
#include "util.h"
#include "plcemu.h"
#include "config.h"
#include "hardware.h"

FILE * Ifd = NULL;
FILE * Qfd = NULL;
char * BufIn = NULL;
char * BufOut = NULL;
char * AdcIn = NULL;
char * AdcOut = NULL;

char SimInFile[MAXSTR];
char SimOutFile[MAXSTR];

extern struct PLC_regs Plc;

void hw_config(const config_t conf)
{
    sprintf(SimInFile, "%s", conf->sim_in_file);
    sprintf(SimOutFile, "%s", conf->sim_out_file);
}

int enable_bus() /* Enable bus communication */
{
    int r = PLC_OK;
    /*open input and output streams*/
    if(!(Ifd=fopen(SimInFile, "r+")))
    {
        plc_log("Failed to open simulation input from %s", SimInFile);
        r = PLC_ERR;
    }
    //else
      //  plc_log("Opened simulation input from %s", SimInFile);

    if(!(Qfd=fopen(SimOutFile, "w+")))
    {
        plc_log("Failed to open simulation output to %s", SimOutFile);
        r = PLC_ERR;
    }
    //else
      //  plc_log("Opened simulation output to %s", SimOutFile);

    if(!(BufIn = (char * )malloc(sizeof(char) * Plc.ni)))
        r = PLC_ERR;
    else
        memset(BufIn, 0, sizeof(BYTE)*Plc.ni);

    if(!(BufOut = (char * )malloc(sizeof(char) * Plc.nq)))
        r = PLC_ERR;
    else
        memset(BufOut, 0, sizeof(char) * Plc.nq);
    
    if(!(AdcIn = (char * )malloc(sizeof(char) * LONG_BYTES * Plc.nai)))
        r = PLC_ERR;
    else
        memset(AdcIn, 0, sizeof(BYTE) * LONG_BYTES * Plc.nai);

    if(!(AdcOut = (char * )malloc(sizeof(char) * LONG_BYTES * Plc.naq)))
        r = PLC_ERR;
    else
        memset(AdcOut, 0, sizeof(char) * LONG_BYTES * Plc.naq);
    return r;
}

int disable_bus() /* Disable bus communication */
{
    int r = 1;
    /*close streams*/
    if( !Ifd
    ||  !fclose(Ifd))
        r = -1;
    //plc_log("Closed simulation input");
    if( !Qfd
    ||  !fclose(Qfd))
        r = -1;
    //plc_log("Closed simulation output"); 
    if(BufIn){
        free(BufIn);
        BufIn = NULL;
    }
    if(BufOut){
        free(BufOut);
        BufOut = NULL;
    }
    return r;
}

int io_fetch()
{
    unsigned int digital = Plc.ni;
    unsigned int analog = Plc.nai;
    int bytes_read = 0;
    
    bytes_read = fread(BufIn, 
                        sizeof(BYTE), 
                        digital, 
                        Ifd?Ifd:stdin);
    int i = 0;
    for(; i < bytes_read; i++)
        if(BufIn[i] >= ASCIISTART)
            BufIn[i] -= ASCIISTART;

    bytes_read += fread(AdcIn, 
                        sizeof(BYTE), 
                        LONG_BYTES*analog, 
                        Ifd?Ifd:stdin);
   
    if(bytes_read < digital + LONG_BYTES*analog){
        //plc_log("failed to read from %s, reopening", SimInFile);
        if(Ifd
        && feof(Ifd))
            rewind(Ifd);
        else{
            disable_bus();
            enable_bus();
        }
    }
    return bytes_read;
}

int io_flush()
{
    int bytes_written = 0;
    unsigned int digital = Plc.nq;
    unsigned int analog = Plc.naq;
    bytes_written = fwrite(BufOut, 
                        sizeof(BYTE), 
                        digital, 
                        Qfd?Qfd:stdout);
    bytes_written += fwrite(AdcOut, 
                            sizeof(BYTE), 
                            analog * LONG_BYTES, 
                            Qfd?Qfd:stdout);
    fputc('\n',Qfd?Qfd:stdout);
    fflush(Qfd);
    return bytes_written;
}

void dio_read(unsigned int n, BYTE* bit)
{	//write input n to bit
    unsigned int b, position;
    position = n / BYTESIZE;
    BYTE i = 0;
    if(strlen(BufIn) > position)
    /*read a byte from input stream*/
        i = BufIn[position];
	b = (i >> n % BYTESIZE) % 2;
	*bit = (BYTE) b;
}

void dio_write(const unsigned char *buf,  int n,  int bit)

{	//write bit to n output
	BYTE q;
    unsigned int position = n / BYTESIZE;
    q = buf[position];
    q |= bit << n % BYTESIZE;
    /*write a byte to output stream*/
    q+=ASCIISTART; //ASCII
   // plc_log("Send %d to byte %d", q, position);
     if(strlen(BufOut) >= position)
         BufOut[position] = q;
}

void dio_bitfield(const unsigned char* mask, unsigned char *bits)
{	//simultaneusly write output bits defined by mask and read all inputs
    /* FIXME
    int i=0;
    unsigned int w = (unsigned int) (*mask);
    unsigned int b = (unsigned int) (*bits);
    comedi_dio_bitfield(it, Comedi_subdev_i, w, &b);*/
}



void data_read(unsigned int index, uint64_t* value)
{
    unsigned int pos = index*LONG_BYTES;
    int i = LONG_BYTES - 1;
    *value = 0;
    if(strlen(AdcIn) > pos) {
       uint64_t mult = 1;
       for(; i >= 0 ; i--){
            *value += (uint64_t)AdcIn[pos + i] * mult;
            mult *= 0x100;
        }    
    }
}


void data_write(unsigned int index, uint64_t value)
{
    unsigned int pos = index*LONG_BYTES;
    sprintf(AdcOut + pos, "%lx", value);
    return; 
}
