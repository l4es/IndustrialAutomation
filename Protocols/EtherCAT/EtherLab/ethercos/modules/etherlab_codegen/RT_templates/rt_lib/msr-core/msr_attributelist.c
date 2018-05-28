/************************************************************************************************
*
*           msr_attributelist.c
*
*           Listenhandling
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
*           $RCSfile: lists.c,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2006/10/07 09:03:00 $
*           $State: Exp $
*
*           
**************************************************************************************************/


/*--includes-------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

/*--defines--------------------------------------------------------------------------------------*/

#define DBG 0
//#define STANDALONE   //macht ein Main zum Testen der Software

#ifndef STANDALONE
#include <msr_attributelist.h>
#else
#include "../include/msr_attributelist.h"
#endif


/*--defines--------------------------------------------------------------------------------------*/

/* dieses Marco läuft durch eine vorwärts verkettete Liste ... */
#define FOR_THE_LIST(element,head) for(element = head;element;element = element->next)

//element muß bei Nutzer des Makros als Variable bekannt sein
#define MSR_LIST_APPEND(list,head)                               \
do {                                                                   \
    struct list *prev  = NULL;                                         \
    FOR_THE_LIST(element,head) {                                       \
	prev = element;                                                \
    }                                                                  \
    element = (struct list *)  malloc(sizeof(struct list));            \
    if (element) {                                                     \
       memset(element, 0, sizeof(struct list));                        \
       if (prev)  prev->next = element;                                \
       else head = element;    /* erstes Element */                    \
    }                                                                  \
}  while(0)

#define MSR_CLEAN_LIST(list,head)                                                                \
do {                                                                                             \
    struct list *element, *prev;                                                                 \
    prev=NULL;                                                                                   \
    FOR_THE_LIST(element,head) {                                                                 \
        if (prev) { free(prev); prev=NULL; }                                                  \
    	prev=element;                                                                            \
    }                                                                                            \
    if (prev)                                                                                    \
	free(prev);                                                                           \
    head = NULL;                               \
} while (0)

/*--typedefs/structures--------------------------------------------------------------------------*/

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

/*
***************************************************************************************************
*/


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

struct talist *addattribute(struct talist **head,char *name,char *value){

    struct talist *element = NULL;

    if(!name) return NULL;  //der Name muß schon vorhanden sein

    if(DBG >1) 
	    printf("A-List append: %s = %s\n",name,value);


    //hier auch die Interpretation, ob er ausgetragen werden soll durch no(name) oder un(name)
    if(strstr(name,"no") == name || strstr(name,"un") == name) {
	FOR_THE_LIST(element,*head) {  
	    if(element && strcmp(element->name,name+2) == 0) { //es gibt den Namen schon in der Liste
		if (DBG > 1) printf("Remove Element %s\n",element->name);
		remattribute(head,element->name);
		return NULL;
    	    }
	}
    }


    FOR_THE_LIST(element,*head) {  
	if(element && strcmp(element->name,name) == 0) { //es gibt den Namen schon in der Liste
	    if (DBG > 1) printf("Element found...\n");
	    free(element->value);             //dann nur den neuen Wert zuweisen
	    element->value=NULL;
	    element->value = strdup(value);
	    return element;
	}    
    }
    //sonst anhänden 

    MSR_LIST_APPEND(talist,*head);
    if(element) {
	element->name = strdup(name);
	element->value = strdup(value);
    }
    return element;
}

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

void remattribute(struct talist **head,char *name){
    struct talist *element = NULL;
    struct talist *prev = NULL;

    /* vorhandenens Element in der Liste suchen */
   FOR_THE_LIST(element,*head) {  /* *head */
       if(element && strcmp(element->name,name) == 0) { //es gibt den Namen in der Liste
	   if(prev) { //Zeiger des vorgängers richtig setzen
	       prev->next = element->next;
	   }
	   else { //es gibt keinen Vorgänger also ist es das erste Element
	       if(element->next == NULL) //hat aber keinen Nachfolger, also ist die Liste dann leer !!
		   *head = NULL;
	       else
		   *head = element->next;
	   }
	   //jetzt löschen
	   free(element->name);
	   free(element->value);
	   free(element);
	   element = NULL;
	   return;
       }
	   prev = element; 
   }

}

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

char *getattribute(struct talist *head,char *name) {
    struct talist *element = NULL;

    if(!name) return "";  //der Name muß schon vorhanden sein

    FOR_THE_LIST(element,head) {  
	if(element && strcmp(element->name,name) == 0) { //es gibt den Namen in der Liste
	    return element->value;
	}
    }
    return "";
}

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
int hasattribute(struct talist *head,char *name) {
    struct talist *element = NULL;

    if(!name) return (1 == 0);  //der Name muß schon vorhanden sein

    FOR_THE_LIST(element,head) {  
	if(element && strcmp(element->name,name) == 0) { //es gibt den Namen in der Liste
	    return (0 == 0);
	}
    }
    return (1 == 0);

}

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

void freealist(struct talist **head) {
    struct talist *element=NULL;
    FOR_THE_LIST(element,*head) {  //erst durch die Liste laufen und alle Einträge löschen...
	if (element) {
	    free(element->name);
	    free(element->value);
	}
    }
    MSR_CLEAN_LIST(talist,*head);  //dann die Liste selbst löschen


}


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

char *alisttostr(struct talist *head){
    struct talist *element=NULL;
    int size = 0;
    char *buf;
    int cnt = 0;
    int ecnt = 0;

    FOR_THE_LIST(element,head) {  //erst durch die Liste laufen und zählen, wie groß der String sein muß
	if (element) {
		size+=strlen(element->name)+1+strlen(element->value)+2+1;
	    ecnt++;
            //                     name="value" 
	}
    }

    if(DBG > 1)
	printf("Listsize: %d\n",ecnt);
    buf = (char *)malloc(size+1); //die Null am Ende nicht vergessen              
    FOR_THE_LIST(element,head) {  
	if(cnt > 0)
	    cnt+=sprintf(buf+cnt," %s=\"%s\"",element->name,element->value);
	else  //der erste
	    cnt+=sprintf(buf+cnt,"%s=\"%s\"",element->name,element->value);
    }

    if(cnt == 0)     //Nullstring zurückgeben
	buf[0] = 0;


	return buf;
}

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

#define INXMLFRAME 1
#define OUTXMLFRAME 0

#define INERROR -1
#define INNAME 2
#define INVALUE 3
#define INEQUAL 4
#define INPARAMTHESIS 5
#define ENDVALUE 6

char *extractalist(struct talist **head,char *buf) {

    char *rbuf,*namebuf,*valuebuf;
    int i,j,n,v;
    int state = OUTXMLFRAME;
    int substate = INNAME;
    rbuf = strdup(buf);    //erst mal Speicher für den Rückgabestring holen
    if(!rbuf) return NULL;


    if(DBG > 2) printf("A-List:Extract: buf %s, head: %p\n",buf,head);
    namebuf = (char *)malloc(strlen(buf));
    if(!namebuf) {
	free(rbuf);
	return NULL;
    }
    valuebuf = (char *)malloc(strlen(buf));
    if(!valuebuf) {
	free(namebuf);
	free(rbuf);
	return NULL;
    }

    //jetzt ein bischen Zustandsmaschine um den Buffer auseinander zu nehmen
    j = 0;
    n = v = 0;
    for(i=0;i<strlen(buf);i++) {
	switch(buf[i]) {
	    case '<':
		state = INXMLFRAME;
		substate = INNAME;
		n = v = 0;
		break;

	    case '>':
		if(state == INXMLFRAME) {
		    state = OUTXMLFRAME;
		    rbuf[j] = 0;
		    namebuf[n] = 0;
		    if(n>0) 
			addattribute(head,namebuf,""); //war nur ein Attribute
		    n = 0;
		}
		else {
		    printf("XML-Statement error in: %s\n",buf);
		//sonst nix tun da Framefehler
		}
		break;

	    case ' ':
		switch(state) {
		    case OUTXMLFRAME:
			rbuf[j++] = buf[i]; 
			break;
		    case INXMLFRAME:
			switch(substate) {
			    case INNAME:
				namebuf[n] = 0;
				if(n>0) 
				    addattribute(head,namebuf,""); //attribute ohne value... das gibts hier
				n = 0;
				//und wir bleiben im substate INNAME
				break;
			    case INVALUE:
				valuebuf[v++] = buf[i];
				break;
			    case ENDVALUE:
				substate = INNAME;
			    default:
				if(DBG >1) printf("A-List, no Substate\n");
				break;
			}
			break;
		    default:
			if(DBG >1) printf("A-List, no State\n");
			break;
		}
		break;

	    case '=':
		switch(state) {
		    case OUTXMLFRAME:
			rbuf[j++] = buf[i]; 
			break;
		    case INXMLFRAME:
			switch(substate) {
			    case INNAME:
				substate = INEQUAL;
				namebuf[n] = 0;
				n = 0;
				break;
			    case INVALUE:
				valuebuf[v++] = buf[i];
				break;
			    default:
				if(DBG >1) printf("A-List, no Substate\n");
				break;
			}
			break;
		    default:
			if(DBG >1) printf("A-List, no State\n");
			break;
		}
		break;

	    case '"':  //FIXME, hier fehlt noch die Unterschiedung in/out xmlframe ??...
		switch(substate) {
		    case INEQUAL:
			substate = INVALUE;
			break;
		    case INVALUE:
			valuebuf[v] = 0;
			v = 0;
			addattribute(head,namebuf,valuebuf);
			n = 0;
			substate = ENDVALUE;
		    default:
			if(DBG >1) printf("A-List, no Substate\n");
			break;
		}
		break;
	    default:   //jedes andere Zeichen
		switch(state) {
		    case OUTXMLFRAME:
			rbuf[j++] = buf[i]; 
			break;
		    case INXMLFRAME:
			switch(substate) {
			    case INNAME:
				namebuf[n++] = buf[i];
				break;
			    case INVALUE:
				valuebuf[v++] = buf[i];
				break;
			    default:
				break;
			}
			break;
		    default:
			if(DBG >1) printf("A-List, no State\n");
			break;
		}
		break;
	}
    }

    rbuf[j] = 0;  //string abschließen
    free(valuebuf);
    free(namebuf);
    return rbuf;
}



#ifdef STANDALONE

void main() {
    char *s1 = "/s1/hallo/\n<mit ohne bla=\"eins\" zwei=\"drei\">ende1/<limit>en de2<nobla>";
    char *s2 = "/s2<nohide>";
    char *s3 = "/s3/<single>";
    char *s4 = "/s4<normal=\"value\">";

    struct talist *alist = NULL;

    char *buf;
    char *rbuf;
    char *xlist;
    int i,j;


    buf = strdup(s1);

   //dann alle \n löschen
    j = 0; 
    for(i=0;i<strlen(buf)+1;i++)  //die Null mitnehmen !!
	if(buf[i] != '\n')
	    buf[j++] = buf[i];

    rbuf = extractalist(&alist,buf);

//    remattribute(&alist,"mit");
//    remattribute(&alist,"limit");
    xlist = alisttostr(alist);

    printf("orig %s\n rbuf: %s\n xml: %s\n",s1,rbuf,xlist);

    printf("has: %d %d\n",hasattribute(alist,"bla"),hasattribute(alist,"bla2"));

    free(rbuf);
    free(xlist);
    freealist(&alist);
/*
    printf("--------------------------------------\n");

    rbuf = extractalist(&alist,s2);
    xlist = alisttostr(alist);
    printf("orig %s\n rbuf: %s\n xml: %s\n",s2,rbuf,xlist);

    free(rbuf);
    free(xlist);
    freealist(&alist);
    printf("--------------------------------------\n");


    rbuf = extractalist(&alist,s3);
    xlist = alisttostr(alist);
    printf("orig %s\n rbuf: %s\n xml: %s\n",s3,rbuf,xlist);

    free(rbuf);
    free(xlist);
    freealist(&alist);
    printf("--------------------------------------\n");

*/
    exit(0);
}


#endif
