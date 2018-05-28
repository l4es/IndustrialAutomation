/*
 *
 * SFunction to register output variables
 *
 * Copyright (c) 2012, Richard Hacker
 * License: GPL
 */


#define S_FUNCTION_NAME  rtipc_tx
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"
#include "get_string.h"

#define ID                             (ssGetSFcnParam(S,0)) 
#define FORCE              (mxGetScalar(ssGetSFcnParam(S,1)))
#define TSAMPLE            (mxGetScalar(ssGetSFcnParam(S,2)))
#define PARAM_COUNT                                      3


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
    DimsInfo_T dimsInfo;

    ssSetNumSFcnParams(S, PARAM_COUNT);  /* Number of expected parameters */
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        /* Return if number of expected != number of actual parameters */
        return;
    }
    for( i = 0; i < PARAM_COUNT; i++) 
        ssSetSFcnParamTunable(S,i,SS_PRM_NOT_TUNABLE);

    ssAllowSignalsWithMoreThan2D(S);

    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortWidth(S, 0, DYNAMICALLY_SIZED);
    ssSetInputPortDataType(S, 0, DYNAMICALLY_TYPED);
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    ssSetInputPortRequiredContiguous(S, 0, 1);
    dimsInfo.width = DYNAMICALLY_SIZED;
    dimsInfo.numDims = DYNAMICALLY_SIZED;
    dimsInfo.dims = NULL;
    if (!ssSetInputPortDimensionInfo(S, 0, &dimsInfo))
        return;

    if (!ssSetNumOutputPorts(S, 0)) return;

    ssSetNumSampleTimes(S, 1);

    if (FORCE && ssGetInputPortConnected(S,0)) {
        ssSetNumIWork(S, 1);
        ssSetNumPWork(S, 1);
    }

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

/* Function: mdlSetWorkWidths =================================================
 */
#define MDL_SET_WORK_WIDTHS
static void mdlSetWorkWidths(SimStruct *S)
{
    ssParamRec rtp;
    int_T dimensions[] = {1};
    uint8_T value[] = {0};

    if (!ssGetNumIWork(S))
        return;

    ssSetNumRunTimeParams(S, 2);

    rtp.name = "ForceOutput";
    rtp.nDimensions = 1;
    rtp.dimensions = dimensions;
    rtp.dataTypeId = SS_UINT8;
    rtp.complexSignal = 0;
    rtp.data = mxCalloc(1, ssGetDataTypeSize(S,SS_UINT8));
    rtp.dataAttributes = NULL;
    rtp.nDlgParamIndices = 0;
    rtp.dlgParamIndices = NULL;
    rtp.transformed = RTPARAM_TRANSFORMED;
    rtp.outputAsMatrix = 0;
    mexMakeMemoryPersistent(rtp.data);
    ssSetRunTimeParamInfo(S, 0, &rtp);

    rtp.name = "ForceValue";
    rtp.nDimensions = ssGetInputPortNumDimensions(S,0);
    rtp.dimensions = ssGetInputPortDimensions(S,0);
    rtp.dataTypeId = ssGetInputPortDataType(S,0);
    rtp.complexSignal = 0;
    rtp.data = mxCalloc(ssGetInputPortWidth(S,0),
            ssGetDataTypeSize(S,rtp.dataTypeId));
    rtp.dataAttributes = NULL;
    rtp.nDlgParamIndices = 0;
    rtp.dlgParamIndices = NULL;
    rtp.transformed = RTPARAM_TRANSFORMED;
    rtp.outputAsMatrix = 0;
    mexMakeMemoryPersistent(rtp.data);
    ssSetRunTimeParamInfo(S, 1, &rtp);
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
    if (!ssGetNumIWork(S))
        return;

    mxFree(ssGetRunTimeParamInfo(S, 0)->data);
    mxFree(ssGetRunTimeParamInfo(S, 1)->data);
}

#define MDL_RTW
static void mdlRTW(SimStruct *S)
{
    const char *id      = getString(S,ID);

    if (!ssWriteRTWStrParam(S, "VarName", id))
        return;
    if (ssGetNumIWork(S)) {
        if (!ssWriteRTWWorkVect(S, "PWork", 1, "Pdo", 1))
            return;

        if (!ssWriteRTWWorkVect(S, "IWork", 1, "Force", 1))
            return;
    }
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
