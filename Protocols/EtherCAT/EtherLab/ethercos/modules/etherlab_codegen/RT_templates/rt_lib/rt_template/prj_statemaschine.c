/**************************************************************************************************
*
*                          $PRJ$_statemaschine.c
*
*           Zustandsmaschine des Prüfstandes
*           
*           Autor: Wilhelm Hagemeister
*
*           (C) Copyright IgH 2002
*           Ingenieurgemeinschaft IgH
*           Heinz-Bäcker Str. 34
*           D-45356 Essen
*           Tel.: +49 201/61 99 31
*           Fax.: +49 201/61 98 36
*           E-mail: hm@igh-essen.com
*
*
*           $RCSfile: prj_statemaschine.c,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/07/01 10:45:59 $
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

#include <msr_target.h>

#include <linux/config.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/fs.h>		/* everything... */
//#include <linux/param.h> /* fuer HZ */
#include <math.h>

#include <msr_charbuf.h>
#include <msr_utils.h>
#include <msr_proc.h>
#include <msr_controller.h>
#include <msr_messages.h>
#include <msr_watchdog.h>
#include <msr_pwmgen.h>
#include <msr_error_reg.h>
#include <korrelation.h>
#include <msr_reg.h>
#include <msr_statemaschine.h>

#include "msr_io.h"
#include "$PRJ$_statemaschine.h"
#include "$PRJ$_globals.h"
#include "msr_param.h"

#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/hm/projekte/inv_ina_ventil_pruefstand/software/opt/msr/src/rt/rt_lib/rt_template/RCS/prj_statemaschine.c,v 1.1 2005/07/01 10:45:59 hm Exp hm $");

/*--defines--------------------------------------------------------------------------------------*/

/*--data--------------------------------------------------------------------------------------*/


/*--external data--------------------------------------------------------------------------------*/

extern struct msr_char_buf *msr_in_int_charbuffer;
extern struct timeval process_time;	// aktuelle Zeit (UNIXZEIT: wird zu Anfang von run_interrupt genommen)

extern int msr_operating;   // Betriebsstundenzaehler
extern int msr_trigger;

/*--structs/typedefs-----------------------------------------------------------------------------*/


struct msr_statemaschine $PRJ$GlobalStatemaschine = 
{
    msrSFL :    $UPPER(PRJ)$STATEMASCHINE,
    prestate_function : $PRJ$_prestate,
    poststate_function : $PRJ$_poststate,
};

/*--external functions---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------*/

int msr_error_flag = 0;

/*--prototypes-----------------------------------------------------------------------------------*/

#define IS_EXTRN 
#include "$PRJ$_vardef.h"
#undef IS_EXTRN


char *$PRJ$_control_version =
    "$RCSfile: prj_statemaschine.c,v $ $Revision: 1.1 $ $Author: hm $ $Date: 2005/07/01 10:45:59 $";




/*--------------------------------------------------------------*/
void $PRJ$_prestate(struct msr_statemaschine *sm)
/*--------------------------------------------------------------*/
{
#include "$PRJ$_reset_all_outputs.inc"


    //Error checking

    msr_error_flag = msr_check_for_errors(sm);
    
    if ((msr_error_flag >> T_CRIT) & 1) {
	switchToState(sm,T_$UPPER(PRJ)$_KritischerFehler);
	return;
    }

    if ((msr_error_flag >> T_ERR) & 1)
	switchToState(sm,T_$UPPER(PRJ)$_Fehler);


}


/*--------------------------------------------------------------*/
void $PRJ$_poststate(struct msr_statemaschine *sm)
/*--------------------------------------------------------------*/
{

}

/*--------------------------------------------------------------*/
void $PRJ$_statemaschine_init(void)
/*--------------------------------------------------------------*/
{
    msr_statemaschine_init(&$PRJ$GlobalStatemaschine,T_$UPPER(PRJ)$_Startup);


}
/*--------------------------------------------------------------*/
void $PRJ$_statemaschine_run(void)            /* Zustandsmaschine */
/*--------------------------------------------------------------*/
{

    msr_statemaschine_run(&$PRJ$GlobalStatemaschine);

}


/*--------------------------------------------------------------*/
void $PRJ$_calc_controllers(struct msr_statemaschine *sm)
/*--------------------------------------------------------------*/
{
}


