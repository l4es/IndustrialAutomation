/*
* $Id$
*
* This SFunction implements an event generator
*
* Copyright (c) 2008, Richard Hacker
* License: GPL
*
*/

#define S_FUNCTION_NAME  event
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"



#define WIDTH      (int_T)(mxGetScalar(ssGetSFcnParam(S,0)))
#define SINGLESHOT        (mxGetScalar(ssGetSFcnParam(S,1)))
#define DTYPE      (int_T)(mxGetScalar(ssGetSFcnParam(S,2)))
#define TSAMPLE           (mxGetScalar(ssGetSFcnParam(S,3)))
#define PARAM_COUNT                                     4

/*====================*
 * S-function methods *
 *====================*/

/* Function: mdlInitializeSizes =============================================
 * Abstract:
 *    The sizes information is used by Simulink to determine the S-function
 *    block's characteristics (number of inputs, outputs, states, etc.).
 */
static void mdlInitializeSizes(SimStruct *S)
{
    int_T i;

    ssSetNumSFcnParams(S, PARAM_COUNT);  /* Number of expected parameters */
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        /* Return if number of expected != number of actual parameters */
        return;
    }

    for( i = 0; i < PARAM_COUNT; i++)
        ssSetSFcnParamTunable(S, i, SS_PRM_NOT_TUNABLE);

    /* Process input ports */
    if (!ssSetNumInputPorts(S, 0))
        return;
    if (!ssSetNumOutputPorts(S, 1))
        return;

    /* Output port */
    ssSetOutputPortWidth(S,    0, WIDTH);
    switch (DTYPE) {
        case 1:
            ssSetOutputPortDataType(S, 0, SS_BOOLEAN);
            break;
        case 8:
            ssSetOutputPortDataType(S, 0, SS_UINT8);
            break;
        case -8:
            ssSetOutputPortDataType(S, 0, SS_INT8);
            break;
        case 16:
            ssSetOutputPortDataType(S, 0, SS_UINT16);
            break;
        case -16:
            ssSetOutputPortDataType(S, 0, SS_INT16);
            break;
        case 32:
            ssSetOutputPortDataType(S, 0, SS_UINT32);
            break;
        case -32:
            ssSetOutputPortDataType(S, 0, SS_INT32);
            break;
        default:
            ssSetErrorStatus(S, "Unknown data type");
    }

    ssSetNumSampleTimes(S, 1);
    ssSetNumIWork(S, WIDTH);
    ssSetUserData(S, 0);

    ssSetOptions(S,
            SS_OPTION_WORKS_WITH_CODE_REUSE
            | SS_OPTION_RUNTIME_EXCEPTION_FREE_CODE
            | SS_OPTION_CALL_TERMINATE_ON_EXIT);
}

/* Function: mdlInitializeSampleTimes =======================================
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

#define MDL_SET_WORK_WIDTHS
static void mdlSetWorkWidths(SimStruct *S)
{
    int_T width = WIDTH;
    ssParamRec p;
    p.name = "Trigger";
    p.nDimensions = 1;
    p.dimensions = &width;
    p.dataTypeId = SS_UINT32;
    p.complexSignal = 0;
    p.data = mxCalloc(width, sizeof(uint32_T));
    p.dataAttributes = NULL;
    p.nDlgParamIndices = 0;
    p.dlgParamIndices = NULL;
    p.transformed = RTPARAM_TRANSFORMED;
    p.outputAsMatrix = 0;

    mexMakeMemoryPersistent(p.data);
    memset(p.data, 0, width * sizeof(uint32_T));
    ssSetUserData(S, p.data);

    ssSetNumRunTimeParams(S, 1);
    ssSetRunTimeParamInfo(S, 0, &p);

    return;
}

/* Function: mdlOutputs =====================================================
 * Abstract:
 *    In this function, you compute the outputs of your S-function
 *    block. Generally outputs are placed in the output vector, ssGetY(S).
 */
static void mdlOutputs(SimStruct *S, int_T tid)
{
}

/* Function: mdlTerminate ===================================================
 * Abstract:
 *    In this function, you should perform any actions that are necessary
 *    at the termination of a simulation.  For example, if memory was
 *    allocated in mdlStart, this is the place to free it.
 */
static void mdlTerminate(SimStruct *S)
{
    mxFree(ssGetUserData(S));
}

#define MDL_RTW
static void mdlRTW(SimStruct *S)
{
    int_T single_shot = SINGLESHOT;

    if (!ssWriteRTWScalarParam(S, "SingleShot", &single_shot, SS_INT32))
        return;
    if (!ssWriteRTWWorkVect(S, "IWork", 1, "TriggerCounter", WIDTH))
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
