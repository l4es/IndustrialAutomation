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

#ifndef SCADA_UTILITY_H
	#define SCADA_UTILITY_H


#ifndef UTILITIES_DLL_EXP_IMP
	#ifdef WIN32
		#ifdef UTILITIES_EXPORTS
		#define UTILITIES_DLL_EXP_IMP  __declspec(dllexport)
		#else 
		#define UTILITIES_DLL_EXP_IMP __declspec(dllimport)
		#endif
	#else
		// under UNIX we have no such thing as exports
		#define UTILITIES_DLL_EXP_IMP 
	#endif
#endif


extern "C" {
	
	 UTILITIES_DLL_EXP_IMP bool IsSingleInstance (const char* Name);
	 UTILITIES_DLL_EXP_IMP const char* GetScadaDateBuild(void);
	 UTILITIES_DLL_EXP_IMP const char* GetScadaTimeBuild(void);
	 UTILITIES_DLL_EXP_IMP int IsNAN_Double(double);
	 UTILITIES_DLL_EXP_IMP int IsNAN_Float(float);
	 UTILITIES_DLL_EXP_IMP int flp_cmp(double x1, double x2);
}


#endif