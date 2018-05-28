/**************************************************************************************************
*
*                          msr_error_reg.h
*
*           Hier werden die Fehlerlisten verwaltet.
*           
*           Autor: Wilhelm Hagemeister
*
*           (C) Copyright IgH 2001
*           Ingenieurgemeinschaft IgH
*           Heinz-Bäcker Str. 34
*           D-45356 Essen
*           Tel.: +49 201/61 99 31
*           Fax.: +49 201/61 98 36
*           E-mail: hm@igh-essen.com
*
*
*           $RCSfile: msr_error_reg.h,v $
*           $Revision: 1.6 $
*           $Author: hm $
*           $Date: 2006/01/19 10:35:27 $
*           $State: Exp $
*
*
*           $Log: msr_error_reg.h,v $
*           Revision 1.6  2006/01/19 10:35:27  hm
*           *** empty log message ***
*
*           Revision 1.5  2006/01/04 11:30:46  hm
*           *** empty log message ***
*
*           Revision 1.4  2005/12/23 14:51:28  hm
*           *** empty log message ***
*
*           Revision 1.3  2005/08/24 16:49:18  hm
*           *** empty log message ***
*
*           Revision 1.2  2005/07/01 16:09:00  hm
*           *** empty log message ***
*
*           Revision 1.1  2005/06/14 12:34:59  hm
*           Initial revision
*
*           Revision 1.6  2005/03/22 13:25:50  fi
*           *** empty log message ***
*
*           Revision 1.5  2005/03/22 12:56:47  ab
*           *** empty log message ***
*
*           Revision 1.4  2005/03/07 11:40:47  hm
*           *** empty log message ***
*
*           Revision 1.3  2005/03/04 10:39:13  ab
*           *** empty log message ***
*
*           Revision 1.2  2004/10/04 10:14:21  hm
*           *** empty log message ***
*
*           Revision 1.1  2004/09/14 10:15:49  hm
*           Initial revision
*
*           Revision 1.6  2004/08/05 17:59:10  hm
*           *** empty log message ***
*
*           Revision 1.5  2004/08/04 14:34:24  hm
*           *** empty log message ***
*
*           Revision 1.4  2004/07/23 14:44:21  hm
*           ..
*
*           Revision 1.3  2004/04/15 14:32:21  hm
*           *** empty log message ***
*
*           Revision 1.2  2003/11/05 16:09:41  hm
*           *** empty log message ***
*
*           Revision 1.1  2003/07/17 09:21:11  hm
*           Initial revision
*
*           Revision 1.1  2003/01/22 10:27:40  hm
*           Initial revision
*
*           Revision 1.4  2002/08/15 08:15:59  hm
*           *** empty log message ***
*
*           Revision 1.3  2002/07/19 15:18:01  sp
*           *** empty log message ***
*
*           Revision 1.2  2002/07/19 07:05:41  sp
*           *** empty log message ***
*
*           Revision 1.1  2002/07/18 11:35:58  hm
*           Initial revision
*
*
*
*
*
**************************************************************************************************/


/*--Schutz vor mehrfachem includieren------------------------------------------------------------*/

#ifndef _MSR_ERROR_REG_H_
#define _MSR_ERROR_REG_H_

/*--includes-------------------------------------------------------------------
------------------*/
#ifdef __KERNEL__ 
#include <linux/time.h>
#else
#include <sys/time.h>
#endif

#include "msr_target.h"
#include "msr_mem.h"
#include "msr_reg.h"
#include "msr_charbuf.h"
#include "msr_statemaschine.h"
/*--defines--------------------------------------------------------------------------------------*/


/*--typedefs/structures--------------------------------------------------------------------------*/



#define  T_INFO     0 /* bit Positionen der Fehlerklassen in der variablen p_err_type*/
#define  T_WARN     1
#define  T_ERR      2
#define  T_CRIT     3
#define  T_BROADCAST 4
#define  CHK_EXCL   0
#define  CHK_INCL   1


/* Bei Ergänzung der Fehlerklassen den String erweitern !!!! ansonsten Kopf ab*/

#define T_ERROR_TYP_STRING "info","warn","error","crit_error","broadcast" //do not touch (Strings haben Bedeutung)

////#define TO_KERN_LOGS KERN_INFO,KERN_WARN,KERN_ERR,KERN_CRIT

enum enum_check_typ {  /* Fehlerüberprüfungskriterien */
    T_CHK_MAX,
    T_CHK_MAX_ABS,
    T_CHK_MIN,
    T_CHK_HIGH,
    T_CHK_LOW,
    T_CHK_USR          /* Fehler wird durch Funktionsaufruf gesetzt */
};


/* Errorstruktur-----------------------------------------------------------------------------*/



struct msr_error_list
{
    void *p_adr;                 /* Voidpointer zur Adresse der Variablen, die überprüft werden soll*/
    enum enum_var_typ var_typ; /* Variablentyp */  
    int ftyp; /* Fehlerklasse */
    enum enum_check_typ p_chk_typ;      /* Kriterium */
    double limit;                /* Grenzwert bei überwachten analoger Werten */
    char *p_string;              /* Fehlerbeschreibungsstring */
    struct msr_error_list *next; /* nächster Eintrag in der Liste */
    time_t flag;                    /* <> 0 Fehler aufgetreten, Bei Fehler wird die aktuelle Zeit, bei 
				       Zeit, bei der der Fehler zum erstenmal aufgetreten ist gespeichert,
				       und zwar nur die Sekunden aus dem struct timeval */
    struct msr_statemaschine* statemaschine; /*Zustandsmaschinenstruktur in deren Zustände überprüft werden
					      sollen*/
    int inclexcl;                  /* wenn 0:dann werden die Zustände in der Liste ausgeschlossen,
				      wenn 1: dann werden nur die Zustände in der Liste überprüft */
    int numstates;                 /* Anzahl bei der Überprüfung auszuschließender Zustände */
    int *statelist;             /* die Liste der auszuschließende Zustände */            
};

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

/*--prototypes-----------------------------------------------------------------------------------*/

/*
***************************************************************************************************
*
* Function: msr_reg_chk_failure (alte Version)
*
* Beschreibung: Registriert einen Fehlerüberprüfungsvariable
*
* Parameter: adr: Zeiger auf eine double Variable, die überprüft werden soll
*            var_typ: Typ der Variablen
*            chk: Kriterium
*            limit:
*            ftyp: Fehlerklasse
*            str: Beschreibungsstring
*
* Rückgabe:  Adresse des elementes wenn alles ok, sonst < 0
*               
* Status: exp
*
***************************************************************************************************
*/
/* Variable, [ERR_MAX,ERR_MIN],Grenze,Fehlerklasse,Fehlerstring */
int  msr_reg_chk_failure(void *adr,enum enum_var_typ var_typ,enum enum_check_typ chk,double limit,int ftyp,char *str);

/*
***************************************************************************************************
*
* Function: msr_reg_chk_failure_sub_state
*
* Beschreibung: Registriert einen Fehlerüberprüfungsvariable, es muß eine Liste von Zuständen auch von 
*               Unterzustandmaschinen
*               in denen die Variable überprüft werden soll, sowie ein Flag ob die Zustandliste exlusiv oder
*               inclusive behandelt werden soll !!!!!
*
* Parameter: adr: Zeiger auf eine Variable, die überprüft werden soll
*            var_typ: Typ der Variablen
*            chk: Kriterium
*            limit:
*            ftyp: Fehlerklasse
*            str: Beschreibungsstring
*            statemaschine: Adresse der Zustandmaschinenstruktur wenn NULL, dann wird in allen Zustandsmaschinen kontrolliert !!
*            flag: CHK_INCL-> Zustände inkludieren, CHK_EXCL-> Zustände exkludieren
*            numargs: Anzahl zu excludierender Zustände
*            ...: die Zustände
*
* Rückgabe:  Adresse des elementes wenn alles ok, sonst < 0
*               
* Status: exp 
* 
***************************************************************************************************
*/

int  msr_reg_chk_failure_sub_state(void *adr,enum enum_var_typ var_typ,enum enum_check_typ chk,double limit,int ftyp,char *str,struct msr_statemaschine* statemaschine,int flag ,int numargs,...);
/*
***************************************************************************************************
*
* Function: msr_raise_error
*
* Beschreibung: Löst einen Fehler "manuell" aus
*
* Parameter: 
*
* Rückgabe:  globales Fehlerflag
*               
* Status: exp
*
***************************************************************************************************
*/
void msr_raise_error(struct msr_error_list *element);



/*
***************************************************************************************************
*
* Function: msr_check_for_errors_sub_state
*
* Beschreibung: Überprüft alle registrierten Fehlerzustände für die übergebene Zustandmaschine und setzt die
* Bits in msr_error_flags
* die Flags können dann in der Statemaschine ausgewertet werden.
*                
* Status: exp
*
* Rückgabe: Alg. Fehlerflag
*
***************************************************************************************************
*/

int  msr_check_for_errors(struct msr_statemaschine* statemaschine);


/*
***************************************************************************************************
*
* Function: msr_clean_error_list
*
* Beschreibung: Gibt Speicherplatz wieder frei
*
* Parameter: 
*
* Rückgabe:  globales Fehlerflag
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_clean_error_list(void);


/*
***************************************************************************************************
*
* Function: msr_reset_all_errors
*
* Beschreibung: Setzt alle Fehler zurück
*
* Parameter: 
*
* Rückgabe:  
*               
* Status: exp
*
***************************************************************************************************
*/

void  msr_reset_all_errors(struct msr_param_list *self);


/*
***************************************************************************************************
*
* Function: msr_get_next_error
*
* Beschreibung: diese Funktion läuft durch alle Meldungen und gibt jeweils die nächste anliegende Fehlermeldung zurück
*               wenn keine Meldungen anliegen, wird NULL zurückgegeben
*               die Funktion ist nicht reentrant, d.h. suksessive Aufrufe geben immer den nächsten Fehler zurück, egal, wer
*               aufruft
*
* Parameter: 
*
* Rückgabe:  String mit der nächsten Fehlermeldung oder NULL
*               
* Status: exp
*
***************************************************************************************************
*/

char *msr_get_next_error(void);

#endif 	// _REG_H_













