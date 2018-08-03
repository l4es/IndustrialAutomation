/* Classic Ladder Project */
/* Copyright (C) 2001 Marc Le Douarain */
/* mavati@club-internet.fr */
/* http://www.multimania.com/mavati/classicladder */
/* December 2001 */
/* --------------------------------------------------- */
/* RTLinux module to refresh rungs & scan physical I/O */
/* --------------------------------------------------- */
/* This library is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU Lesser General Public */
/* License as published by the Free Software Foundation; either */
/* version 2.1 of the License, or (at your option) any later version. */

/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU */
/* Lesser General Public License for more details. */

/* You should have received a copy of the GNU Lesser General Public */
/* License along with this library; if not, write to the Free Software */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include <rtl.h>
#include <time.h>
#include <pthread.h>

#include "classicladder.h"
#include "global.h"
#include "calc.h"
#include "hardware.h"

MODULE_LICENSE("LGPL");
MODULE_AUTHOR("Marc Le Douarain");
MODULE_DESCRIPTION("ClassicLadder RT module");

int comedi_to_open_mask;
int nbr_rungs, nbr_bits,nbr_words, nbr_timers, nbr_monostables;
int nbr_phys_inputs, nbr_phys_outputs, nbr_arithm_expr, nbr_sections;
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

pthread_t thread;

void * start_routine(void *arg)
{
	struct sched_param p;
	p . sched_priority = 1;
	pthread_setschedparam (pthread_self(), SCHED_FIFO, &p);

	pthread_make_periodic_np (pthread_self(), gethrtime(), TIME_REFRESH_RUNG_MS*1000000);

	while (1) {
		pthread_wait_np ();

		if (InfosGene->LadderState==STATE_RUN)
		{

			ReadPhysicalInputs();

			ClassicLadder_RefreshAllSections();

			WritePhysicalOutputs();

		}
	}
	return 0;
}

int init_module(void) {
	ClassicLadder_AllocAll( );
	OpenHardware( comedi_to_open_mask );
	return pthread_create ( &thread, NULL, start_routine, 0 );
}

void cleanup_module(void) {
	pthread_delete_np ( thread );
	ClassicLadder_FreeAll( );
}

void CopySizesInfosFromModuleParams( void )
{
	if ( nbr_rungs>0 )
		InfosGene->GeneralParams.SizesInfos.nbr_rungs = nbr_rungs;
	if ( nbr_bits>0 )
		InfosGene->GeneralParams.SizesInfos.nbr_bits = nbr_bits;
	if ( nbr_words>0 )
		InfosGene->GeneralParams.SizesInfos.nbr_words = nbr_words;
	if ( nbr_timers>0 )
		InfosGene->GeneralParams.SizesInfos.nbr_timers = nbr_timers;
	if ( nbr_monostables>0 )
		InfosGene->GeneralParams.SizesInfos.nbr_monostables = nbr_monostables;
	if ( nbr_phys_inputs>0 )
		InfosGene->GeneralParams.SizesInfos.nbr_phys_inputs = nbr_phys_inputs;
	if ( nbr_phys_outputs>0 )
		InfosGene->GeneralParams.SizesInfos.nbr_phys_outputs = nbr_phys_outputs;
	if ( nbr_arithm_expr>0 )
		InfosGene->GeneralParams.SizesInfos.nbr_arithm_expr = nbr_arithm_expr;
	if ( nbr_sections>0 )
		InfosGene->GeneralParams.SizesInfos.nbr_sections = nbr_sections;
}
