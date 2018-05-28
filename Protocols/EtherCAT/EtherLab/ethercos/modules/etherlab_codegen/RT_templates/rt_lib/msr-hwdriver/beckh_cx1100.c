/**************************************************************************************************
*
*                          beckh_cx1100.c
*
*           CX1100-Treiber fÅ¸r 2.4 Kernel
*        
*            
*           
*           Autor: Wilhelm Hagemeister
*
*           (C) Copyright IgH 2002
*           Ingenieurgemeinschaft IgH
*           Heinz-BÅ‰cker Str. 34
*           D-45356 Essen
*           Tel.: +49 201/61 99 31
*           Fax.: +49 201/61 98 36
*           E-mail: hm@igh-essen.com
*
*
*           $RCSfile: beckh_cx1100.c,v $
*           $Revision: 1.3 $
*           $Author: hm $
*           $Date: 2008/11/11 16:40:22 $
*           $State: Exp $
*
*
*
*
*
*
**************************************************************************************************/


/*--includes-------------------------------------------------------------------------------------*/
 
#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/config.h>
#include <linux/module.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h> 
#include <linux/fs.h>     /* everything... */
#include <linux/proc_fs.h>
#include <linux/errno.h>  /* error codes */
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/time.h>

#include <beckh_cx1100.h>
#include <msr_utils.h>
#include <msr_reg.h>
#include <msr_messages.h>

#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/hm/projekte/Offene/tkp_toyota_klein_projekte/software/hochdruckaggregat/opt/msr/src/rt/rt_lib/msr-hwdriver/RCS/beckh_cx1100.c,v 1.3 2008/11/11 16:40:22 hm Exp $");

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

extern unsigned long int volatile jiffies;

/*--public data----------------------------------------------------------------------------------*/

void *kbus_base;  /* gemappte Startadresse des KBus*/

/*--local data-----------------------------------------------------------------------------------*/

static struct proc_dir_entry *cx1100_root;

static int KbusRunning = 0;                    /* wenn der Kbus initialisiert ist und lé‰uft wird 
						  dieses Flag = 1 gesetzt */
static unsigned char KbusCounter = 2;
static unsigned int KbusCycleTime = 0;
static unsigned int powerupstart = 0;
static volatile unsigned int NumKbusCycles = 0;
static volatile unsigned int MissedKbusCycles = 0;


/* der erste Buchstabe von Klemme 0 1           2             3           4          */
unsigned char *klemmentyp[] = {"","Digital In","Digital Out","Analog In","Analog Out",
/*                              5                  6               7  8              9*/
			       "Winkel/Weg","Kommunikation","","Powerklemme ","Systemklemme"};



/*Routinen fé¸r Lesen und Schreiben von spezielle Klemmen */
/***************************************************************************************************/
void KLDI(struct cxklemme *self)  /* digital Input */
/***************************************************************************************************/
{
    int j,vbyte;
    int badr;

    for(j=0;j<self->numchannels;j++) {
	badr=(self->iadr+j)/8;
	vbyte = readb(kbus_base+badr);
	self->value[j] = 1 & (vbyte >> ((self->iadr+j)%8));
    }

}

/***************************************************************************************************/
void KLDO(struct cxklemme *self)  /* digital Output */
/***************************************************************************************************/
{
    int j,vbyte;
    int badr;
    int bit,mask;
    for(j=0;j<self->numchannels;j++) {
	/*erstmal das byte lesen */
	badr = (self->oadr+j)/8;
	bit = (self->oadr+j) % 8;
	vbyte = readb(kbus_base+KBUS_OUTPUT_OFFS+badr);
	/*erst das entsprechende bit léˆschen */
	mask = ~((unsigned char)(1 << bit));
	vbyte&=mask;
	/*und jetzt das entsprechende Bit wieder setzten */
	vbyte|= ((1 & self->value[j]) << bit);
	/* und wieder schreiben */
	writeb((unsigned char)(vbyte),kbus_base+KBUS_OUTPUT_OFFS+badr);
    }


}


/***************************************************************************************************/
void KLAI12bit(struct cxklemme *self)
/***************************************************************************************************/
{
    int j;
    int badr,sadr;
    for(j=0;j<self->numchannels;j++) {
	badr = (self->iadr+j*self->mbri+8)/8;
	sadr = (self->iadr+j*self->mbri)/8;
	self->value[j] = (int)readb(kbus_base+badr)+(((int)(readb(kbus_base+1+badr))) << 8);
	self->status[j] = (unsigned char)readb(kbus_base+sadr);
    }
}

/***************************************************************************************************/
void KLAI12bitB(struct cxklemme *self)
/***************************************************************************************************/
{
    int j;
    int badr,sadr;
    u16 hi, lo;

    for(j=0;j<self->numchannels;j++) {
	badr = (self->iadr + j * self->mbri + 8) / 8;
	sadr = (self->iadr + j * self->mbri) / 8;
        lo = readb(kbus_base + badr);
        hi = readb(kbus_base + badr + 1);
	self->value[j] = (s16) (lo | (hi << 8));
	self->status[j] = (unsigned char) readb(kbus_base + sadr);
    }
}

/***************************************************************************************************/
void KLAO12bit(struct cxklemme *self)
/***************************************************************************************************/
{
    int j;
    int badr;

    for(j=0;j<self->numchannels;j++) {
	badr = (self->oadr+j*self->mbri+8)/8;
	/*die analogen Outs begrenzen */
	if(self->value[j] < -MAXDAOUT) 
	    self->value[j] = -MAXDAOUT;
	else
	    if(self->value[j] >= MAXDAOUT) 
		self->value[j] = MAXDAOUT;
	writeb((unsigned char)(self->value[j] & 0xff),kbus_base+KBUS_OUTPUT_OFFS+badr);
	writeb((unsigned char)(self->value[j] >> 8),kbus_base+KBUS_OUTPUT_OFFS+badr+1);
	//FIXME status byte noch auslesen
    }
	
}

/***************************************************************************************************/
void KL9210I(struct cxklemme *self)
/***************************************************************************************************/
{
    int j;
    int vbyte;
    for(j=0;j<2;j++) {
	vbyte = readb(kbus_base+(self->iadr+j)/8);
	self->value[j] = 1 & (vbyte >> ((self->iadr+j)%8));
    }
}

/***************************************************************************************************/
void KL5111pi(struct cxklemme *self) /* Periodendauermessung mit KL5111 */
/***************************************************************************************************/
{
    int badr;
    /* Controlbyte setzten, damit die Klemme die Periodendauer zuré¸ckgibt */
    self->control[0] = 2;
    writeb(self->control[0],kbus_base+KBUS_OUTPUT_OFFS+(self->oadr/8));
    /*jetzt den Zé‰hlerstand lesen */
    badr = self->iadr/8;

    /*der Wert steht in D2,D3,D4 (wie rum ?)*/
    self->value[0] = (int)readb(kbus_base+badr+3) << 16;  /* D2 lesen */
    self->value[0]+= ((int)readb(kbus_base+badr+5)) << 8;  /* D3 lesen */
    self->value[0]+= ((int)readb(kbus_base+badr+4));  /* D4 lesen */
    self->status[0] = (unsigned char)readb(kbus_base+badr);

}


/***************************************************************************************************/
void KL3403I(struct cxklemme *self) /* LeistungsmeÅﬂklemme */
/***************************************************************************************************/
{
    int j;
    int badr,sadr,cadr;
    u16 hi, lo;

    for(j=0;j<self->numchannels;j++) {
	badr = (self->iadr + j * self->mbri + 8) / 8;  //Lesen
	sadr = (self->iadr + j * self->mbri) / 8;      //Status
	cadr = (self->oadr + j * self->mbro) / 8;      //Control
	//controlbytes setzten
	writeb((self->control[j] & 0x7),kbus_base+KBUS_OUTPUT_OFFS+cadr); //0x7, damit nur die Auswahl der KanÅ‰le zugÅ‰nglich ist
	//Input lesen
        lo = readb(kbus_base + badr);
        hi = readb(kbus_base + badr + 1);
	self->value[j] = (s16) (lo | (hi << 8));
	self->status[j] = (unsigned char) readb(kbus_base + sadr);
    }
}

/*
***************************************************************************************************
*
* Function: msr_read_proc_cx1100info
*
* Beschreibung: Output einiger wichtiger Informationen é¸ber Proc
*               
* Parameter: 
*
* RÅ¸ckgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_read_proc_cx1100info(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{

    unsigned int b1,b2;
    int i;
    int cnt = 0;
    char cxname[3];
    unsigned char cx1,cx2;
    unsigned char cy1,cy2;

    struct cxklemme *busklemmen = (struct cxklemme*)data;
    *eof = 1;

    /*reading Firmwareversion from CX1100 */
    b1=readb(kbus_base+KBUS_FIRMWARE_OFFS);
    b2=readb(kbus_base+KBUS_FIRMWARE_OFFS+1);

    /*reading cxname */
    for(i=0;i<2;i++)
	cxname[i] = readb(kbus_base+CXNAME_OFFS+i);
    cxname[2] = 0;  /* abschlieéﬂende 0 */
    cx1 = readb(kbus_base+CXNAME_OFFS+2);
    cx2 = readb(kbus_base+CXNAME_OFFS+3);

    //Cycletime lesen
    cy1 = readb(kbus_base + CX_CYCLETIME);
    cy2 = readb(kbus_base +  CX_CYCLETIME+1);

    cnt = sprintf(buf,"Beckhoff CX Firmwareversion: %#04x, %#04x \nDevice ID: %s%x00-000%i\nKbus-Counter: %i\n",b1,b2,cxname,cx1,cx2,KbusCounter);
//    cnt+=sprintf(buf+cnt,"Kbus-Cyle Time: %d us\n\n",KbusCycleTime);  alte Version

    cnt+=sprintf(buf+cnt,"Kbus-Cyle Time: %d us\n\n",(int)cy1+((int)cy2)*256);

    cnt+=sprintf(buf+cnt,"Valid Kbus-Cyles: %u \n\n",NumKbusCycles);
    cnt+=sprintf(buf+cnt,"Missed Kbus-Cyles: %u \n\n",MissedKbusCycles);

    /* jetzt noch Klemmen auflisten */ 
    i=0;
    cnt+=sprintf(buf+cnt,"Klemmenliste: \n");
    cnt+=sprintf(buf+cnt,"Nr Klemme  Typ             Beschreibung                  Adr-I Bit-I Adr-O Bit-O Channels \n");
    cnt+=sprintf(buf+cnt,"----------------------------------------------------------------------------------------- \n");

    while(1) {
	cnt+=sprintf(buf+cnt,
		     "%2d KL%s  %-15s %s %2d   %2d    %2d   %2d    %2d\n",
		     i,
		     busklemmen[i].name,
		     klemmentyp[busklemmen[i].ktyp],
		     busklemmen[i].description,
		     busklemmen[i].iadr/8,busklemmen[i].iadr%8,
		     busklemmen[i].oadr/8,busklemmen[i].oadr%8,
		     busklemmen[i].numchannels);
	if(busklemmen[i].mbri < 0) break;
	i++;
    }


    /*jetzt noch die Ins */
/*    for(i=0;i<numIn;i++) {
	if((i % 16) == 0) 
	    cnt+=sprintf(buf+cnt,"\nBytes (read): %02x: ",i);
	cnt+=sprintf(buf+cnt,"%02x ",CxIn[i]);
    }
    cnt+=sprintf(buf+cnt,"\n");
*/
    return cnt;

}

/***************************************************************************************************/
int msr_read_proc_cx1100IO(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
/***************************************************************************************************/
{
    int i,j,cnt;

    struct cxklemme *busklemmen = (struct cxklemme*)data;

    *eof = 1;
    cnt = sprintf(buf,"Data I/O:\n");
    /* jetzt noch Klemmen auflisten */ 
    i=0;
    while(1) {
	cnt+=sprintf(buf+cnt,
		     "%2d KL%s %-12s ",
		     i,
		     busklemmen[i].name,
		     klemmentyp[busklemmen[i].ktyp]);
	if(busklemmen[i].numchannels > 0) {
	    for(j=0;j<busklemmen[i].numchannels;j++) {
		cnt+=sprintf(buf+cnt,", %5d(%2x)",busklemmen[i].value[j],busklemmen[i].status[j]);
	    }
	}
	cnt+=sprintf(buf+cnt,"\n");
	if(busklemmen[i].mbri < 0 || busklemmen[i].mbro < 0) break;
	i++;
    }

    cnt+=sprintf(buf+cnt,"\n");
    return cnt;

}

/***************************************************************************************************/
int msr_read_proc_cx1100dpRamIn(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
/***************************************************************************************************/
{
    int i,cnt;

    *eof = 1;
    /*jetzt noch die Ins */
    cnt = sprintf(buf,"DPRAM Input Data:");
    for(i=0;i<0x1FE;i++) {
	if((i % 16) == 0) 
	    cnt+=sprintf(buf+cnt,"\nBytes: %3i: ",i);
	cnt+=sprintf(buf+cnt,"%02x ",readb(kbus_base+i));
    }
    cnt+=sprintf(buf+cnt,"\n");

    return cnt;

}

/*
***************************************************************************************************
*
* Function: cx1100_startup
*
* Beschreibung: Reboot des Controllers
*               
* Parameter: 
*
* RÅ¸ckgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/
void cx1100_startup() 
{
    /* Reset the KBUS */
    writeb(1,kbus_base+KBUS_RESET_OFFS);
    //   mdelay(1);
    writeb(0,kbus_base+KBUS_RESET_OFFS);
    msr_print_info("msr_modul: cx1100, Powerup Kbus at Jiffie: %lu",jiffies);

    powerupstart = jiffies;

    /*vier Sekunden warten, bis der Controller rebootet ist */
    /* nach Angaben von Hr. Thome, Tel am 02.02.2004 */
    msr_print_info("msr_modul: cx1100, Init Controller Jiffie: %lu",jiffies);

    do {
	mdelay(500);
    }
    while(jiffies-powerupstart < HZ*4);
    msr_print_info("msr_modul: cx1100, Init Controller done, Jiffie: %lu",jiffies);


    /* ersten Zyklus triggern */
    KbusCounter++;
    writeb(KbusCounter,kbus_base+CX_IRQFROMHOST);
    KbusRunning = 1; 

}

/*
***************************************************************************************************
*
* Function: cx1100_measure_times  FIXME alt, wird nicht mehr verwendet....
*
* Beschreibung: Zykluszeit des Kbus messen
*               
* Parameter: 
*
* RÅ¸ckgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/
void cx1100_measure_times()  //FIXME bei spé‰terer Implementierung kann die Kbuszeit é¸bers dPRAM abgefragt werden
{
    struct timeval st,en,res;
    unsigned char k;
    
    unsigned int cnt = 0;
    int i;

    /*vier Sekunden warten, bis der Controller rebootet ist */
    /* nach Angaben von Hr. Thome, Tel am 02.02.2004 */
    msr_print_info("msr_modul: cx1100, Init Controller Jiffie: %lu",jiffies);

    do {
	mdelay(500);
    }
    while(jiffies-powerupstart < HZ*4);
    msr_print_info("msr_modul: cx1100, Init Controller done, Jiffie: %lu",jiffies);


    /*jetzt 10 Kbuszyklen abwarten */
    do_gettimeofday(&st);
    /* Zyklus triggern */
    KbusCounter++;
    writeb(KbusCounter,kbus_base+CX_IRQFROMHOST);

    for(i=0;i<10;i++) {
	cnt = jiffies;
	while((k=readb(kbus_base+CX_IRQFROMHOST)) != readb(kbus_base+CX_IRQRESPONSE)){
	    udelay(100);
	    if(jiffies-cnt > HZ/10) {/* maximal 100 ms warten fé¸r einen Zyklus*/
		msr_print_info("msr_modul: cx1100, Kbuscycletime > 100ms! (Jiffie: %lu)",jiffies);
		return;
	    }
	}
	k++;
	KbusCounter = k;
	writeb(KbusCounter,kbus_base+CX_IRQFROMHOST);
    }
    do_gettimeofday(&en);
    /*jetzt die Zeitdifferenz berechnen */
    timeval_subtract(&res,&en,&st);
    KbusCycleTime = (res.tv_sec*1000000+res.tv_usec)/10;  /* da 10 Zyklen */
    msr_print_info("msr_modul: cx1100, KbusCylceTime: %d us",KbusCycleTime);
    KbusRunning = 1; 

}

/* Hilfsfunktion zum schnellen Zugriff auf den Kbus*/
/*clear_kbus_buf() 
{
    kbus_buffer = 0;
    old_kbus_adr = 0;
}

unsigned char readb_kbus_buf(int adr)
{

    if(adr != old_kbus_adr)
	kbus_buffer = readb(adr);
    old_kbus_adr = adr;
    return(kbus_buffer);
}


writeb_kbus_buf(unsigned char val, int adr)
{
    if(adr != old_kbus_adr && old_kbus_adr != 0)
	writeb(kbus_buffer,old_kbus_adr);
    old_kbus_adr = adr;
    kbus_buffer = val;
}


flush_kbus_buf()
{
    if(old_kbus_adr != 0)
	writeb(kbus_buffer,old_kbus_adr);
    clear_kbus_buf();
}
*/
/*
***************************************************************************************************
*
* Function: cx1100_trigger_kbus
*
* Beschreibung: é‹bergibt I/O-Werte und triggert den Kbus-Zyklus
*               
* Parameter: 
*
* RÅ¸ckgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/

int cx1100_trigger_kbus(struct cxklemme klemmen[],int num) 
{
    int i;
    static int cnt_delay_cycles = 0;
    volatile unsigned char n,k;
    unsigned char cy1,cy2;


    if(KbusRunning == 0)   //warten, bis Freigabe der Interruptroutine 
	return 0;

/*
    if(KbusRunning == 0) {  //3 Sekunden warten
	if (jiffies-powerupstart > HZ*4) {
	    KbusRunning = 1; 
	    // und ersten Kbus Zyklus starten 
	    KbusCounter = 2;
	    writeb(KbusCounter,kbus_base+CX_IRQFROMHOST);
	    msr_print_info("msr_modul: cx1100, Start Kbuscylces Jiffie: %lu",jiffies);
	}
	return 0;
    }
*/
    n=readb(kbus_base+CX_IRQRESPONSE);
    k=readb(kbus_base+CX_IRQFROMHOST);

    /*FIXME hier kann der Zugriff noch optimiert werden, indem eine Kopie des dPRAM geholt wird, bearbeitet wird
      und wieder geschrieben wird */

//    clear_kbus_buf();

    if(n == k) {/* ok Kbuszyklus ist durch */
	NumKbusCycles++;
	cnt_delay_cycles = 0;
	/* hier werden jetzt die Werte ausgelesen und geschrieben */
	for(i=0;i<num;i++) {
	    if (klemmen[i].pIO != NULL)
		klemmen[i].pIO(&klemmen[i]);
	}
	k++;
	KbusCounter = k;
	writeb(KbusCounter,kbus_base+CX_IRQFROMHOST);

	//Cycletime noch lesen und aktualisieren
	cy1 = readb(kbus_base + CX_CYCLETIME);
	cy2 = readb(kbus_base +  CX_CYCLETIME+1);
	KbusCycleTime = (unsigned int)cy1+((unsigned int)cy2)*256;
    }
    else {
	// printk("cx1100: n %d, k %d\n",n,k);
	MissedKbusCycles++;
	cnt_delay_cycles++;
    }


    return cnt_delay_cycles;  // und die Anzahl der Wartezyklen zuré¸ckgeben
}



/**************************************************************************************************/
void cx1100_reg_klemmen(struct cxklemme klemmen[],int num)
/**************************************************************************************************/
{
    int i;
    int InputAdr = 0;
    int OutputAdr = 0;

    for(i=0;i<num;i++) {
	/* Anzahl der Kané‰le belegen aus der Bezeichnung, falls nicht schon angegeben 
	   (aber nur, wenn es Å¸berhaupt Inputs und Outputs gibt)*/
	if (klemmen[i].numchannels == 0 && (klemmen[i].mbri > 0 || klemmen[i].mbro > 0))
	    klemmen[i].numchannels=(int)klemmen[i].name[3]-48;
	/*Typ ebenfalls aus dem Namen */
	klemmen[i].ktyp=(int)klemmen[i].name[0]-48;
	/*jetzt ermitteln ob Input oder Output */
	/* und mapping der Analogen Module */
	switch(klemmen[i].ktyp) {
	    case 3:   /* Analog Input, Weg/Winkelmessung */
	    case 4:   /* Analog Output */
	    case 5:   /* Analog Input, Weg/Winkelmessung */
		klemmen[i].iadr = InputAdr;
		klemmen[i].oadr = OutputAdr;
		/*und Adresse weiterschieben */
		InputAdr+= klemmen[i].numchannels*klemmen[i].mbri;
		OutputAdr+= klemmen[i].numchannels*klemmen[i].mbro;
		break;
	    default:
		break;
	}

    }

    /* jetzt noch die digitalen Klemmen mappen */
    for(i=0;i<num;i++) {
	switch(klemmen[i].ktyp) {
	    case 1: /* Digital Input */
	    case 2: /* Digital Output */
	    case 9: /* Systemklemmen (kéˆnnen auch IO haben)*/
		klemmen[i].iadr = InputAdr;
		klemmen[i].oadr = OutputAdr;
		/*und Adresse weiterschieben */
		InputAdr+= klemmen[i].numchannels*klemmen[i].mbri;
		OutputAdr+= klemmen[i].numchannels*klemmen[i].mbro;
		break;
	    default:
		break;
	}
    }

}


/*
***************************************************************************************************
*
* Function: cx1100_init/cleanup()
*
* Beschreibung: Initialisierung des KBUS
*               
* Parameter: 
*
* RÅ¸ckgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/
int cx1100_init(struct cxklemme klemmen[],int num)
{
    /*nachsehen, ob Speicher verfé¸gbar ist */
    if(check_mem_region(KBUS_DPRAM_START,KBUS_DPRAM_LEN)) {
	printk("CX1100: KBUS DPRAM not available (maybe not a CX1100)\n");
	return -EBUSY;
    }
    else
	printk("CX1100: KBUS DPRAM avialable... \n");

    /*jetzt reservieren */
    request_mem_region(KBUS_DPRAM_START,KBUS_DPRAM_LEN,"CX1100");
    kbus_base = ioremap(KBUS_DPRAM_START,KBUS_DPRAM_LEN);


    /* Procfileentry erzeugen */
    cx1100_root = proc_mkdir("cx1100", 0);
    create_proc_read_entry("Info", 0, cx1100_root, msr_read_proc_cx1100info, (void *)klemmen);
    create_proc_read_entry("DPRAMinput", 0, cx1100_root, msr_read_proc_cx1100dpRamIn, NULL);
    create_proc_read_entry("ModulIO", 0, cx1100_root, msr_read_proc_cx1100IO,  (void *)klemmen);

    /*Parameter und Kané‰le registrieren */
    msr_reg_kanal("/CX1100/Kbus/cycle counter","inc",&KbusCounter,TUCHAR);
    msr_reg_kanal("/CX1100/Kbus/Cycle Time","us",&KbusCycleTime,TUINT);

    /*Klemmen initialisieren */
    cx1100_reg_klemmen(klemmen,num);

    /* Rest KBUS und den ersten KBUSZyklus starten */
    cx1100_startup();
    /*Zeiten messen und Kbus Zyklus starten */
    //cx1100_measure_times(); 
    return 0;    
}


/**************************************************************************************************/
void cx1100_cleanup(void)
/**************************************************************************************************/
{

    /* Procentrys entfernen */
     remove_proc_entry("ModulIO", NULL);
     remove_proc_entry("Info", cx1100_root);
     remove_proc_entry("DPRAMinput", cx1100_root);
     remove_proc_entry("cx1100", NULL);

     /*Speicherbereich wieder freigeben */
    iounmap(kbus_base);
    release_mem_region(KBUS_DPRAM_START,KBUS_DPRAM_LEN);
    printk("CX1100: Cleanup... ok\n");
}
 


















