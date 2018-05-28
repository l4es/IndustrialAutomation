/**************************************************************************************************
*
*                          beckh_cx1100.h
*
*           CX1100-Treiber für 2.4 Kernel
*        
*            
*           
*           Autor: Wilhelm Hagemeister
*
*           (C) Copyright IgH 2002
*           Ingenieurgemeinschaft IgH
*           Heinz-Bäcker Str. 34
*           D-45356 Essen
*           Tel.: +49 201/61 99 31
*           Fax.: +49 201/61 98 36
*           E-mail: sp@igh-essen.com
*
*
*           $RCSfile: beckh_cx1100.h,v $
*           $Revision: 1.5 $
*           $Author: hm $
*           $Date: 2008/11/11 13:59:42 $
*           $State: Exp $
*
*
*
*
*
*
**************************************************************************************************/


/*--includes-------------------------------------------------------------------------------------*/
 
#ifndef _MSR_BECKHOFFCX1100_H_
#define _MSR_BECKHOFFCX1100_H_

/*--defines--------------------------------------------------------------------------------------*/


#define KBUS_DPRAM_START 0xD0000   /* physikalische Startadresse des DPRAMS */
#define KBUS_DPRAM_LEN    0x100F   /* LŽänge des DPRAMS */

/* Position ab der Startadresse */
#define KBUS_FIRMWARE_OFFS 0xFF0    /* Auslesen der Firmware */
#define KBUS_RESET_OFFS 0x1003      /* Reset des KBUS */
#define CXNAME_OFFS 0x1004          /* Name des CX-devices */
#define CX_IRQFROMHOST 0x0FFE
#define CX_IRQRESPONSE 0x0FFD
#define CX_CYCLETIME   0x0FF6
#define KBUS_INPUT_OFFS 0x0
#define KBUS_OUTPUT_OFFS 0x0200
#define KBUS_IO_SIZE 0x1FE          /* GrŽöŽße Bereich DPRAM Input und Output */

#define MAXDAOUT (32768-1)  /* maximales Ausgangssignale des DA-Wandleres */

/*--typedefs/structures--------------------------------------------------------------------------*/

#define ZERO8 {0,0,0,0,0,0,0,0}
#define VSC  ZERO8,ZERO8,ZERO8    /*Value Status Control */

/* Klemmendefinitionen */
/* ktyp,iotyp,skaling,adr,numchannels,value0-7 */

#define DFILL 0,0,0,0,0,VSC
#define D9210FILL 0,0,0,0,2,VSC      /* die 9210 Klemme wird hier schon belegt, da sich Kanalanzahl nicht aus dem Namen ergeben */
#define AI12BITFILL 0,1.0/3276.8,0,0,0,VSC
#define AI3403FILL 0,1,0,0,0,VSC       /* Skalierung erfolgt extern ??? */
#define AO12BITFILL 0,3276.8,0,0,0,VSC
#define PT100FILL 0,1.0/10.0,0,0,0,VSC
#define D5111pFILL 0,0,0,0,0,VSC //ZERO8,ZERO8,{2,0,0,0,0,0,0,0} /* Belegung des Controlbytes für Periodendauermessung */

/* Prototypen */

struct cxklemme;


void KLAI12bit(struct cxklemme *self); /* analog in 12bit */
void KLAI12bitB(struct cxklemme *self); /* analog in 12bit */
void KLAO12bit(struct cxklemme *self); /* analog in 12bit */

void KLDI(struct cxklemme *self);  /* digital Input */
void KLDO(struct cxklemme *self);  /* digital Output */

void KL5111pi(struct cxklemme *self); /* Periodendauermessung mit KL5111 */
void KL9210I(struct cxklemme *self);
void KL3403I(struct cxklemme *self); /* Leistungsmeßklemme */

/* Klemmenname,  Name,  Beschreibung          mbri,mbro,str */           
#define KL1104 {"1104","24 V, DC Filter 0.3ms        ",1  ,0  ,&KLDI,DFILL}
#define KL1124 {"1124"," 5 V, DC Filter 0.2ms        ",1  ,0  ,&KLDI,DFILL}
#define KL1404 {"1404","24 V, DC Filter 3.0ms        ",1  ,0  ,&KLDI,DFILL}
#define KL1408 {"1408","24 V, DC Filter 3.0ms        ",1  ,0  ,&KLDI,DFILL}
#define KL1418 {"1418","24 V, DC Filter 0.2ms        ",1  ,0  ,&KLDI,DFILL}

#define KL2134 {"2134","24 V, 3.0ms                  ",0  ,1  ,&KLDO,DFILL}
#define KL2124 {"2124"," 5 V, 3.0ms                  ",0  ,1  ,&KLDO,DFILL}
#define KL2404 {"2404","24 V                         ",0  ,1  ,&KLDO,DFILL}
#define KL2408 {"2408","24 V                         ",0  ,1  ,&KLDO,DFILL}

#define KL2602 {"2602","Relais 230V AC               ",0  ,1  ,&KLDO,DFILL}
#define KL2641 {"2641","Relais 230V AC, Handbed.     ",2  ,2  ,&KLDO,DFILL}  //FIXME über status input abfragen
										      
#define KL3201 {"3201","PT100                        ",3*8,3*8,&KLAI12bit,PT100FILL}
#define KL3202 {"3202","PT100                        ",3*8,3*8,&KLAI12bit,PT100FILL}
#define KL3002 {"3002","+/-10V,12bit                 ",3*8,3*8,&KLAI12bitB,AI12BITFILL}
#define KL3051 {"3051","4-20mA,12bit                 ",3*8,3*8,&KLAI12bit,AI12BITFILL}
#define KL3062 {"3062","0-10V, 12bit                 ",3*8,3*8,&KLAI12bit,AI12BITFILL}
#define KL3064 {"3064","0-10V, 12bit                 ",3*8,3*8,&KLAI12bit,AI12BITFILL}
			
							      
#define KL3403 {"3403","Leistungsmeßklemme           ",3*8,3*8,&KL3403I,AI3403FILL}
#define KL3404 {"3404","+/-10V, 12bit                ",3*8,3*8,&KLAI12bitB,AI12BITFILL}
 										      
#define KL4002 {"4002","0-10V, 12bit                 ",3*8,3*8,&KLAO12bit,AO12BITFILL}
#define KL4004 {"4004","0-10V, 12bit                 ",3*8,3*8,&KLAO12bit,AO12BITFILL}
#define KL4021 {"4021","4-20mA, 12bit                ",3*8,3*8,&KLAO12bit,AO12BITFILL}
#define KL4032 {"4032","+/-10V, 12bit                ",3*8,3*8,&KLAO12bit,AO12BITFILL}
#define KL4034 {"4034","+/-10V, 12bit                ",3*8,3*8,&KLAO12bit,AO12BITFILL}


#define KL5111 {"5111","Inkrementalgeber             ",6*8,6*8,NULL,DFILL} 
#define KL5111p {"5111","Periodendauermessung        ",6*8,6*8,&KL5111pi,DFILL}

#define KL9010 {"9010","Busendklemme                 ",-1 , -1,NULL,DFILL}
#define KL9110 {"9110","Einspeiseklemme 24V, Diagnose",1  , 0 ,&KL9210I,D9210FILL}
#define KL9187 {"9187","Einspeiseklemme 0V           ",0  , 0 ,NULL,DFILL}
#define KL9186 {"9186","Einspeiseklemme 24V          ",0  , 0 ,NULL,DFILL}
#define KL9210 {"9210","Systemklemme, Sicherung      ",1  , 0 ,&KL9210I,D9210FILL}
#define KL9505 {"9505","Netzteilklemme 5V            ",0  , 0 ,NULL,DFILL}




struct cxklemme {
    char *name;          /* Bezeichnung */
    char *description;   /* Beschreibung */
    int mbri;            /* Mapping Breite jedes Kanals der Klemme im Prozessabbild (Input) in bit wenn -1 dann letzte Klemme !!!*/
    int mbro;            /* Mapping Breite jedes Kanals der Klemme im Prozessabbild (Output) in bit */
    void (*pIO)(struct cxklemme *self);  /* Zeiger auf eine Funktion, die das Lesen und Schreiben auf den KBUS Žübernimmt, Adr: Startadresse in bit */

    int ktyp;            /* Erste Nummer der Klemmenbezeichnung */
    double skaling;      /* Skalierung (Analog KanŽäle in V, Temperaturen in Grad C)*/
    int iadr;             /* in bit ab start(wird bei Initialisierung belegt) Inputbereich*/
    int oadr;             /* in bit ab start(wird bei Initialisierung belegt) Outputbereich*/
    int numchannels;     /* Anzahl KanŽäle je Klemme */
    int value[8];        /* die Werte (roh) */
    unsigned char status[8]; /* das Statusbyte lesend */
    unsigned char control[8]; /* das Controlbyte schreibend */
};



#define KBUSget(klemme,modul,kanal) ((klemme[modul].skaling !=0) ? \
(klemme[modul].skaling*klemme[modul].value[kanal]) : (klemme[modul].value[kanal]))

#define KBUSgetstatus(klemme,modul,kanal) (klemme[modul].status[kanal])

#define KBUSsetcontrol(klemme,modul,kanal,v) (klemme[modul].control[kanal] = v)

#define KBUSset(klemme,modul,kanal,v) ((klemme[modul].skaling !=0) ? \
(klemme[modul].value[kanal] = v*klemme[modul].skaling) : (klemme[modul].value[kanal] = v))


//filter wird bisher nicht genutzt FIXME
#define KBUS_I_SKAL(x,skale,offset,filter) ((x)*(skale)+(offset))

//filter wird bisher nicht genutzt FIXME
#define KBUS_O_SKAL(varname,skale,offset,filter) ((varname)*(skale)+(offset))




/*--prototypes-----------------------------------------------------------------------------------*/

int cx1100_trigger_kbus(struct cxklemme klemmen[],int num); //struct cx1100_io_struct *iodata);

/*
***************************************************************************************************
*
* Function: cx1100_init/cleanup()
*
* Beschreibung: Initialisierung des KBUS
*               
* Parameter: 
*
* Rückgabe: >= 0: ok, <0 Fehler
*               
* Status: exp
*
***************************************************************************************************
*/
int cx1100_init(struct cxklemme klemmen[],int num);
void cx1100_cleanup(void);

#endif
















