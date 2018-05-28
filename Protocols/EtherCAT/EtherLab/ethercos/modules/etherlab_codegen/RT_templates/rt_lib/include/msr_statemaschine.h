/**************************************************************************************************
*
*                          msr_statemaschine.h
*
*           
*           Autor: Wilhelm Hagemeister
*
*           (C) Copyright IgH 2002
*           Ingenieurgemeinschaft IgH
*           Heinz-Bäcker Str. 34
*           D-45356 Essen
*           Tel.: +49 201/61 99 31
*           Fax.: +49 201/61 98 36
*           E-mail: sp@@igh-essen.com
*
*
*           $RCSfile: msr_statemaschine.h,v $
*           $Revision: 1.3 $
*           $Author: hm $
*           $Date: 2006/01/04 11:30:46 $
*           $State: Exp $
*
*
*           $Log: msr_statemaschine.h,v $
*           Revision 1.3  2006/01/04 11:30:46  hm
*           *** empty log message ***
*
*           Revision 1.2  2005/07/01 16:09:00  hm
*           *** empty log message ***
*
*           Revision 1.1  2005/06/14 12:34:59  hm
*           Initial revision
*
*           Revision 1.3  2005/02/08 11:22:26  hm
*           *** empty log message ***
*
*           Revision 1.2  2005/02/08 10:15:18  ab
*           *** empty log message ***
*
*           Revision 1.1  2005/02/08 10:15:08  ab
*           Initial revision
*
*           Revision 1.4  2004/09/30 15:50:32  hm
*           *** empty log message ***
*
*           Revision 1.3  2004/08/17 16:53:18  hm
*           *** empty log message ***
*
*           Revision 1.2  2004/08/05 17:59:01  hm
*           *** empty log message ***
*
*           Revision 1.1  2004/07/22 17:28:02  hm
*           Initial revision
*
*           Revision 1.1  2004/05/28 15:29:12  hm
*           Initial revision
*
*           Revision 1.2  2004/04/09 09:54:31  ab
*           *** empty log message ***
*
*           Revision 1.1  2004/02/06 14:25:50  hm
*           Initial revision
*
*           Revision 1.1  2003/10/19 21:45:52  hm
*           Initial revision
*
*           Revision 1.2  2003/09/25 16:16:25  hm
*           *** empty log message ***
*
*           Revision 1.1  2003/09/25 11:06:38  hm
*           Initial revision
*
*           Revision 1.1  2003/02/13 14:30:27  hm
*           Initial revision
*
*       
*

**************************************************************************************************/


/*--Schutz vor mehrfachem includieren------------------------------------------------------------*/

#ifndef _MSR_STATE_MASCHINE_H_
#define _MSR_STATE_MASCHINE_H_


/*--includes-------------------------------------------------------------------------------------*/

/*--defines--------------------------------------------------------------------------------------*/


#define STATE_STEP_IN 1
#define STATE_RUN 0


#define AFTERWAIT(t,expression) {static int counter = 0; if (sm->flag == STATE_STEP_IN) counter = 0; \
                                 do { if(counter++>t*MSR_ABTASTFREQUENZ) {expression;}; }while(0); }

#define DO_UNTIL(t,expression) {static int counter = 0; if (sm->flag == STATE_STEP_IN) counter = 0; \
                                 do { if(counter++<t*MSR_ABTASTFREQUENZ) {expression;}; }while(0); }

/*--structs/typedefs-----------------------------------------------------------------------------*/

//Ein einzelner State

/*
struct msr_statemaschine *sm;


struct msrState{
    int state;
    void (*sf)(struct msr_statemaschine *sm);
};

*/
struct msr_statemaschine {
    int prevstate;
    int state;
    int flag;
    void *private_data;
    void (*prestate_function)(struct msr_statemaschine *sm);
    void (*state_function)(struct msr_statemaschine *sm);
    void (*poststate_function)(struct msr_statemaschine *sm);
    struct msrState {
	int state;
	void (*sf)(struct msr_statemaschine *sm);
    } msrSFL[];
};

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

/*--prototypes-----------------------------------------------------------------------------------*/

//Zustandswechsel
void switchToState(struct msr_statemaschine *sm,int newstate);

void msr_statemaschine_init(struct msr_statemaschine *sm,int initialstate);

void msr_statemaschine_run(struct msr_statemaschine *sm);


#endif 	// 






