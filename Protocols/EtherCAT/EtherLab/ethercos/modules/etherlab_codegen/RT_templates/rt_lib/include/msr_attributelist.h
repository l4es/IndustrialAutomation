/************************************************************************************************
*
*           msr_attributelist.h
*
*           Eine Liste von Attributen mit Name und value
*
*           
* 
*           (C) Copyright IgH 2005
*           Ingenieurgemeinschaft IgH
*           Heinz-Bäcker Str. 34
*           D-45356 Essen
*           Tel.: +49 201/61 99 31
*           Fax.: +49 201/61 98 36
*           E-mail: hm@igh-essen.com
*
*
*           $RCSfile: lists.h,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2006/10/07 08:54:31 $
*           $State: Exp $
*
*           
**************************************************************************************************/


#ifndef _MSR_ATTRIBUTELIST_H_
#define _MSR_ATTRIBUTELIST_H_

/*--includes-------------------------------------------------------------------------------------*/

/*--defines--------------------------------------------------------------------------------------*/

/*--typedefs/structures--------------------------------------------------------------------------*/

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

struct talist {   //Atribbuteliste 
    char *name;
    char *value;   
    struct talist *next;  //das nächste Element
};


/*
*******************************************************************************
*
* Function: addattribute
*
* Beschreibung: Hängt ein Attribute an die Liste an, wenn es den Namen schon gibt, wird nur der Wert
*              aktualisiert
*
*
* Parameter: head: Kopf der Liste
*            name: Name des Attributes
*            value: Wert des Attributes
*
* Rückgabe:  Zeiger auf das aktuelle Attribute
*
* Status:
*
*******************************************************************************
*/

struct talist *addattribute(struct talist **head,char *name,char *value);

/*
*******************************************************************************
*
* Function: remattribute
*
* Beschreibung: Löscht ein Attribute aus der Liste
*
*
* Parameter: name: Name des Attributes
*            value: Wert des Attributes
*
* Rückgabe:  keine
*
* Status:
*
*******************************************************************************
*/

void remattribute(struct talist **head,char *name);

/*
*******************************************************************************
*
* Function: getattribute
*
* Beschreibung: Gibt den Wert eines Attributes zurück
*
*
* Parameter: head: Kopf der Liste
*            name: Name des Attributes
*            
*
* Rückgabe:  value: Wert des Attributes
*
* Status:
*
*******************************************************************************
*/

char *getattribute(struct talist *head,char *name);

/*
*******************************************************************************
*
* Function: hasattribute
*
* Beschreibung: Überprüft, ob ein Attribute exisiert
*
*
* Parameter: head: Kopf der Liste
*            name: Name des Attributes
*            
*
* Rückgabe:  value: true/false
*
* Status:
*
*******************************************************************************
*/

int hasattribute(struct talist *head,char *name);


/*
*******************************************************************************
*
* Function: freealist
*
* Beschreibung: Gibt die ganze Liste frei (inkl. aller Strings), nach dem Aufruf ist head = NULL
*
*
* Parameter: head der Liste
*
* Rückgabe:  keine
*
* Status:
*
*******************************************************************************
*/

void freealist(struct talist **head); 


/*
*******************************************************************************
*
* Function: extractalist
*
* Beschreibung: Holt aus einem String alle <> Statements raus und erzeugt die Attributeliste
*
*
* Parameter: head der Liste
*            String
*
* Rückgabe:  der String (ohne die ganzen <> ....)
*
* Status:
*
*******************************************************************************
*/

char *extractalist(struct talist **head,char *buf);

/*
*******************************************************************************
*
* Function: alisttostr
*
* Beschreibung: Hängt die ganze Liste aneinander in der Form name1 name2 name3="value3" name4="value" usw.
*               vorne steht ein Leerzeichen, wird meistens gebraucht....
*
*
* Parameter: head der Liste
*
* Rückgabe:  der String (muß sonst wo freigegeben werden)
*
* Status:
*
*******************************************************************************
*/

char *alisttostr(struct talist *head);
#endif
