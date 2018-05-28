/*
 *
 * SFunction to register input variables
 *
 * Copyright (c) 2012 Richard Hacker
 * License: GPL
 */


#define S_FUNCTION_NAME  rtipc_rx
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"
#include "get_string.h"

#define ID                             (ssGetSFcnParam(S,0)) 
#define DTYPE      ((uint_T)mxGetScalar(ssGetSFcnParam(S,1)))
#define LEN                             ssGetSFcnParam(S,2)
#define FORCE              (mxGetScalar(ssGetSFcnParam(S,3)))
#define TSAMPLE            (mxGetScalar(ssGetSFcnParam(S,4)))
#define PARAM_COUNT                                      5


struct {
    uint_T type;
    uint_T len;
    char_T *ctype;
} ss_dtype_properties[] = {
    {0,}, 
    {SS_BOOLEAN, sizeof(boolean_T), "boolean_T"},
    {SS_INT8,  sizeof(int8_T),      "int8_T"  }, 
    {SS_UINT8, sizeof(uint8_T),     "uint8_T" }, 
    {SS_INT16,  sizeof(int16_T),    "int16_T" }, 
    {SS_UINT16, sizeof(uint16_T),   "uint16_T"}, 
    {SS_INT32,  sizeof(int32_T),    "int32_T" }, 
    {SS_UINT32, sizeof(uint32_T),   "uint32_T"}, 
    {SS_SINGLE, sizeof(real32_T),   "real32_T"}, 
    {SS_DOUBLE, sizeof(real_T),     "real_T"  }, 
};

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
    int_T i;
    DECL_AND_INIT_DIMSINFO(di);
    const mxArray *lenArray = LEN;
    real_T *len = mxGetPr(lenArray);

    ssSetNumSFcnParams(S, PARAM_COUNT);  /* Number of expected parameters */
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        /* Return if number of expected != number of actual parameters */
        return;
    }
    for( i = 0; i < PARAM_COUNT; i++) 
        ssSetSFcnParamTunable(S,i,SS_PRM_NOT_TUNABLE);

    if (DTYPE == 0 || DTYPE > 9) {
        ssSetErrorStatus(S, "Unknown source data type");
        return;
    }
    
    ssAllowSignalsWithMoreThan2D(S);

    if (!ssSetNumInputPorts(S, 0)) return;

    if (!mxIsDouble(lenArray)) {
        ssSetErrorStatus(S, "Port dimension must be a double vector.");
        return;
    }

    di.numDims = mxGetNumberOfElements(lenArray);
    di.dims = mxCalloc(di.numDims, sizeof(int_T));
    di.width = 1;

    for (i = 0; i < di.numDims; i++) {
        di.dims[i] = len[i];
        if (di.dims[i] <= 0) {
            ssSetErrorStatus(S,
                    "Elements of port dimension must be positive integers.");
            return;
        }

        di.width *= di.dims[i];
    }

    if (!ssSetNumOutputPorts(S, 2)) return;
    ssSetOutputPortDimensionInfo(S, 0, &di);
    ssSetOutputPortDataType(S, 0,
		    ss_dtype_properties[DTYPE].type);
    ssSetOutputPortWidth(S, 1, 1);
    ssSetOutputPortDataType(S, 1, SS_BOOLEAN);

    ssSetNumSampleTimes(S, 1);

    if (FORCE) {
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
    rtp.nDimensions = ssGetOutputPortNumDimensions(S,0);
    rtp.dimensions = ssGetOutputPortDimensions(S,0);
    rtp.dataTypeId = ssGetOutputPortDataType(S,0);
    rtp.complexSignal = 0;
    rtp.data = mxCalloc(ssGetOutputPortWidth(S,0),
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
