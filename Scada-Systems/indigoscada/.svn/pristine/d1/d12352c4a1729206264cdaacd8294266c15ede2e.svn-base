/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#ifndef SCADA_DLL
#define SCADA_DLL

// under Windows we build all the shared components as one DLL
#ifndef NO_QSEXPORT 

	#ifdef SCADA_EXPORTS
	#define QSEXPORT  __declspec(dllexport)
	#else 
	#define QSEXPORT __declspec(dllimport)
	#endif
#else
	#define QSEXPORT
#endif


#endif //SCADA_DLL