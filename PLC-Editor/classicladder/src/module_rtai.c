/*
    ClassicLadder Realtime module for RTAI
    
    Loosely based on the module_rtlinux.c by Marc Le Douarain

    RTAI support added by Paul Corner as part of the EMC project.
    September 2003

    =======================================================================

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include "rtai.h"
#include "rtai_sched.h"
#include "classicladder.h"
#include "global.h"
#include "calc.h"
#include "hardware.h"

#define PERIOD      100000000	// Task runs at 100mSec
/* I assume that the FPU is being used... Change this to 0 if it isn't. */
#define USE_FPU 1

/* I prefer using the GPL license here..*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Paul Corner");
MODULE_DESCRIPTION("ClassicLadder RTAI module");

int comedi_to_open_mask = 0;
int nbr_timers = 8;
int nbr_monostables = 8;
int nbr_sections = 10;
int nbr_rungs = 10;
int nbr_phys_inputs = 10;
int nbr_phys_outputs = 50;
int nbr_words = 50;
int nbr_arithm_expr = 100;
int nbr_bits = 500;

MODULE_PARM(nbr_rungs, "i");
MODULE_PARM(nbr_bits, "i");
MODULE_PARM(nbr_words, "i");
MODULE_PARM(nbr_timers, "i");
MODULE_PARM(nbr_monostables, "i");
MODULE_PARM(nbr_phys_inputs, "i");
MODULE_PARM(nbr_phys_outputs, "i");
MODULE_PARM(nbr_arithm_expr, "i");
MODULE_PARM(nbr_sections, "i");
MODULE_PARM(comedi_to_open_mask, "i");

static RT_TASK(thread);

static void realtime_task(int arg)
{

  while (1) {

    if (InfosGene->LadderState == STATE_RUN) {
      ReadPhysicalInputs();
      ClassicLadder_RefreshAllSections();
      WritePhysicalOutputs();
    }
  rt_task_wait_period();
  }
  return;
}

int init_module(void)
{
  RTIME tick;
  unsigned long int t1, t2;

  ClassicLadder_AllocAll();
  OpenHardware(comedi_to_open_mask);

  tick = start_rt_timer(nano2count(PERIOD/100));
/* Do we use floating point math at all ?? */
  rt_linux_use_fpu(USE_FPU);	/* declare if we use the FPU         */

  rt_task_init(	&thread,	/* pointer to our RT_TASK             */
		realtime_task,	/* the task function                 */
		0,		/* arg to pass to the rt task        */
		4000,		/* stack size - Increase if lock ups */
		0xff,		/* run at a low priority             */
		USE_FPU,	/* do we use the FPU ?               */
		NULL		/* task has no signal handler..      */
    );
  rt_task_make_periodic(&thread, rt_get_time()+tick, tick*100);
  /* Small bell to print the actual period */
  t1 = ((int)tick)/10000;
  t2 = ((int)tick) - (t1*10000);
  /* We can get away with the standard kernel print as this is not an RT task */
  printk(KERN_INFO "ClassicLadder RTAI module running at %lu.%lumSec\n", t1, t2);
  return 0;
}

void cleanup_module(void)
{
  stop_rt_timer();
  rt_task_delete(&thread);
  ClassicLadder_FreeAll();
}

void CopySizesInfosFromModuleParams(void)
{
  if (nbr_rungs > 0)
    InfosGene->GeneralParams.SizesInfos.nbr_rungs = nbr_rungs;
  if (nbr_bits > 0)
    InfosGene->GeneralParams.SizesInfos.nbr_bits = nbr_bits;
  if (nbr_words > 0)
    InfosGene->GeneralParams.SizesInfos.nbr_words = nbr_words;
  if (nbr_timers > 0)
    InfosGene->GeneralParams.SizesInfos.nbr_timers = nbr_timers;
  if (nbr_monostables > 0)
    InfosGene->GeneralParams.SizesInfos.nbr_monostables = nbr_monostables;
  if (nbr_phys_inputs > 0)
    InfosGene->GeneralParams.SizesInfos.nbr_phys_inputs = nbr_phys_inputs;
  if (nbr_phys_outputs > 0)
    InfosGene->GeneralParams.SizesInfos.nbr_phys_outputs = nbr_phys_outputs;
  if (nbr_arithm_expr > 0)
    InfosGene->GeneralParams.SizesInfos.nbr_arithm_expr = nbr_arithm_expr;
  if (nbr_sections > 0)
    InfosGene->GeneralParams.SizesInfos.nbr_sections = nbr_sections;
}
