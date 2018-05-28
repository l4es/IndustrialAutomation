/*
 * This S-Function implements the EtherCAT domain status function
 *
 * Copyright (c) 2008, Richard Hacker
 * License: GPL
 */


#define S_FUNCTION_NAME  domain_state
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"

#define MASTER             mxGetScalar(ssGetSFcnParam(S,0))
#define DOMAIN             mxGetScalar(ssGetSFcnParam(S,1))
#define INPUT              mxGetScalar(ssGetSFcnParam(S,2))
#define OUTPUT             mxGetScalar(ssGetSFcnParam(S,3))
#define IO_DOMAIN          mxGetScalar(ssGetSFcnParam(S,4))
#define TSAMPLE            mxGetScalar(ssGetSFcnParam(S,5))
#define PARAM_COUNT                                     6


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
    uint_T width;
    
    ssSetNumSFcnParams(S, PARAM_COUNT);  /* Number of expected parameters */
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        /* Return if number of expected != number of actual parameters */
        return;
    }
    for( i = 0; i < PARAM_COUNT; i++) 
        ssSetSFcnParamTunable(S,i,SS_PRM_NOT_TUNABLE);

    if (!ssSetNumInputPorts(S, 0)) return;

    width = INPUT && OUTPUT && !IO_DOMAIN ? 2 : 1;

    if (!ssSetNumOutputPorts(S, 2)) return;
    ssSetOutputPortWidth(S, 0, width);
    ssSetOutputPortDataType(S, 0, SS_UINT32);
    ssSetOutputPortWidth(S, 1, width);
    ssSetOutputPortDataType(S, 1, SS_UINT8);

    ssSetNumSampleTimes(S, 1);
    ssSetNumPWork(S, width);

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
    uint32_T master = MASTER;
    uint32_T domain = DOMAIN;
    boolean_T input = INPUT;
    boolean_T output = OUTPUT;
    boolean_T io_domain = IO_DOMAIN;

    if (!ssWriteRTWScalarParam(S, "MasterId", &master, SS_UINT32))
        return;
    if (!ssWriteRTWScalarParam(S, "DomainId", &domain, SS_UINT32))
        return;
    if (!ssWriteRTWScalarParam(S, "Input", &input, SS_BOOLEAN))
        return;
    if (!ssWriteRTWScalarParam(S, "Output", &output, SS_BOOLEAN))
        return;
    if (!ssWriteRTWScalarParam(S, "IODomain", &io_domain, SS_BOOLEAN))
        return;
    if (!ssWriteRTWWorkVect(S, "PWork", 1, "DomainPtr", ssGetNumPWork(S)))
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
