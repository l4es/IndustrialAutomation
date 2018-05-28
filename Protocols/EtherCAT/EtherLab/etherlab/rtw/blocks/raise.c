/*
 * $RCSfile: el31xx.c,v $
 * $Revision$
 * $Date$
 *
 * SFunction to implement the raise function
 *
 * Copyright (c) 2006, Richard Hacker
 * License: GPL
 */


#define S_FUNCTION_NAME  raise
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"
#include "get_string.h"

#define ID                             (ssGetSFcnParam(S,0)) 
#define MSG_STR                        (ssGetSFcnParam(S,1)) 
#define AUTO_RESET ((uint_T)mxGetScalar(ssGetSFcnParam(S,2)))
#define TSAMPLE            (mxGetScalar(ssGetSFcnParam(S,3)))
#define PARAM_COUNT                                      4
/*
#define DST_PATH               (ssGetSFcnParam(S,3)) 
*/


/*====================*
 * S-function methods *
 *====================*/

/* Function: mdlInitializeSizes ===============================================
 * Abstract:
 *    The sizes information is used by Simulink to determine the S-function
 *    block's characteristics (number of inputs, outputs, states, etc.).
 */
static void mdlInitializeSizes(SimStruct *S)
{
    uint_T i;
    
    ssSetNumSFcnParams(S, PARAM_COUNT);  /* Number of expected parameters */
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        /* Return if number of expected != number of actual parameters */
        return;
    }
    for( i = 0; i < PARAM_COUNT; i++) 
        ssSetSFcnParamTunable(S,i,SS_PRM_NOT_TUNABLE);

    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortWidth(S, 0, DYNAMICALLY_SIZED);
    ssSetInputPortDataType(S, 0, DYNAMICALLY_TYPED);
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    ssSetInputPortRequiredContiguous(S,0,0);

    if (!ssSetNumOutputPorts(S, 0)) return;

    ssSetNumSampleTimes(S, 1);
    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);
    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, DYNAMICALLY_SIZED);
    ssSetNumPWork(S, 1);
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);

    ssSetOptions(S, 
            SS_OPTION_WORKS_WITH_CODE_REUSE | 
            SS_OPTION_RUNTIME_EXCEPTION_FREE_CODE);
}

/* Function: mdlInitializeSampleTimes =========================================
 * Abstract:
 *    This function is used to specify the sample time(s) for your
 *    S-function. You must register the same number of sample times as
 *    specified in ssSetNumSampleTimes.
 */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, TSAMPLE);
    ssSetOffsetTime(S, 0, 0.0);
}

#define MDL_SET_INPUT_PORT_WIDTH
static void mdlSetInputPortWidth(SimStruct *S, int_T port, int_T width)
{
    ssSetInputPortWidth( S, port, width);
    ssSetNumIWork( S, width);
}

/* Function: mdlOutputs =======================================================
 * Abstract:
 *    In this function, you compute the outputs of your S-function
 *    block. Generally outputs are placed in the input vector, ssGetY(S).
 */
static void mdlOutputs(SimStruct *S, int_T tid)
{
}

/* Function: mdlTerminate =====================================================
 * Abstract:
 *    In this function, you should perform any actions that are necessary
 *    at the termination of a simulation.  For example, if memory was
 *    allocated in mdlStart, this is the place to free it.
 */
static void mdlTerminate(SimStruct *S)
{
}

#define MDL_RTW
static void mdlRTW(SimStruct *S)
{
    const char *id      = getString(S,ID);
    const char *msg_str = getString(S,MSG_STR);
    uint32_T auto_reset = AUTO_RESET;

    if (!ssWriteRTWStrParam(S, "Id", id))
        return;
    if (!ssWriteRTWStrParam(S, "MsgStr", msg_str))
        return;
    if (!ssWriteRTWScalarParam(S, "AutoReset", &auto_reset, SS_UINT32))
        return;
    if (!ssWriteRTWWorkVect(S, "IWork", 1, 
                "RaiseState", ssGetInputPortWidth(S,0)))
        return;
    if (!ssWriteRTWWorkVect(S, "PWork", 1, "Addr", 1))
        return;
}



/*======================================================*
 * See sfuntmpl_doc.c for the optional S-function methods *
 *======================================================*/

/*=============================*
 * Required S-function trailer *
 *=============================*/

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
