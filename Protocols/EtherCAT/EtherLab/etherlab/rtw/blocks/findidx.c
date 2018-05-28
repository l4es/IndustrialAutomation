/*
* $Id$
*
* This SFunction implements an event generator
*
* Copyright (c) 2008, Richard Hacker
* License: GPL
*
*/

#define S_FUNCTION_NAME  findidx
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"

#define TYPE       (int_T)(mxGetScalar(ssGetSFcnParam(S,0)))
#define TSAMPLE           (mxGetScalar(ssGetSFcnParam(S,1)))
#define PARAM_COUNT                                     2

/*====================*
 * S-function methods *
 *====================*/

/* Comparison functions
 * Abstract:
 *      These functions compare two values:
 *      le := v1 <= v2
 *      lt := v1 <  v2
 *      ge := v1 >= v2
 *      gt := v1 >  v2
 */
static int le(double u1, double u2) {
    return u1 <= u2;
}
static int lt(double u1, double u2) {
    return u1 < u2;
}
static int ge(double u1, double u2) {
    return u1 >= u2;
}
static int gt(double u1, double u2) {
    return u1 > u2;
}


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
    if (!ssSetNumInputPorts(S, 1))
        return;
    ssSetInputPortWidth(S, 0, DYNAMICALLY_SIZED);
    ssSetInputPortDataType(S, 0, DYNAMICALLY_TYPED);
    ssSetInputPortDirectFeedThrough(S, 0, 1);

    /* Output port */
    if (!ssSetNumOutputPorts(S, 3))
        return;

    /* Found port */
    ssSetOutputPortWidth(S,    0, 1);
    ssSetOutputPortDataType(S, 0, SS_UINT32);

    /* Index port */
    ssSetOutputPortWidth(S,    1, 1);
    ssSetOutputPortDataType(S, 1, SS_UINT32);

    /* Value port */
    ssSetOutputPortWidth(S,    2, 1);
    ssSetOutputPortDataType(S, 2, DYNAMICALLY_TYPED);

    switch (TYPE) {
        case 1:
            ssSetUserData(S, lt);
            break;
        case 2:
            ssSetUserData(S, le);
            break;
        case 3:
            ssSetUserData(S, gt);
            break;
        case 4:
            ssSetUserData(S, ge);
            break;
    }

    ssSetNumSampleTimes(S, 1);

    ssSetOptions(S,
            SS_OPTION_WORKS_WITH_CODE_REUSE
            | SS_OPTION_RUNTIME_EXCEPTION_FREE_CODE);
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

#define MDL_SET_INPUT_PORT_DATA_TYPE
static void mdlSetInputPortDataType(SimStruct *S, int_T port, DTypeId id)
{
    ssSetInputPortDataType(S,0,id);
    ssSetOutputPortDataType(S,2,id);
}

/* Function: getVal ===========
 * Abstract:
 *      Return the value of an input port as a real_T
 */
static double getVal(DTypeId dtype, int idx, InputPtrsType pp, InputRealPtrsType pr)
{
    switch (dtype) {
        case SS_BOOLEAN:
            return *((InputBooleanPtrsType)pp)[idx];
        case SS_UINT8:
            return *((InputUInt8PtrsType)pp)[idx];
        case SS_INT8:
            return *((InputInt8PtrsType)pp)[idx];
        case SS_UINT16:
            return *((InputUInt16PtrsType)pp)[idx];
        case SS_INT16:
            return *((InputInt16PtrsType)pp)[idx];
        case SS_UINT32:
            return *((InputUInt32PtrsType)pp)[idx];
        case SS_INT32:
            return *((InputInt32PtrsType)pp)[idx];
        case SS_SINGLE:
            return *((InputReal32PtrsType)pp)[idx];
        case SS_DOUBLE:
        default:
            return *pr[idx];
    }
}

/* Function: setVal ===========
 * Abstract:
 *      Return the value of an input port as a real_T
 */
static void setVal(DTypeId dtype, void *pp, real_T *pr, real_T val)
{
    switch (dtype) {
        case SS_BOOLEAN:
            *(boolean_T*)pp = val;
            break;
        case SS_UINT8:
            *(uint8_T*)pp = val;
            break;
        case SS_INT8:
            *(int8_T*)pp = val;
            break;
        case SS_UINT16:
            *(uint16_T*)pp = val;
            break;
        case SS_INT16:
            *(int16_T*)pp = val;
            break;
        case SS_UINT32:
            *(uint32_T*)pp = val;
            break;
        case SS_INT32:
            *(int32_T*)pp = val;
            break;
        case SS_SINGLE:
            *(real32_T*)pp = val;
            break;
        case SS_DOUBLE:
            *pr = val;
            break;
    }
}

/* Function: mdlOutputs =====================================================
 * Abstract:
 *    In this function, you compute the outputs of your S-function
 *    block. Generally outputs are placed in the output vector, ssGetY(S).
 */
static void mdlOutputs(SimStruct *S, int_T tid)
{
    int_T (*cmp)(double,double) = ssGetUserData(S);
    int_T i, n = ssGetInputPortWidth(S,0);
    InputRealPtrsType u_rptr = ssGetInputPortRealSignalPtrs(S,0);
    InputPtrsType u_ptr      = ssGetInputPortSignalPtrs(S,0);
    int32_T *count      = ssGetOutputPortSignal(S,0);
    int32_T *index      = ssGetOutputPortSignal(S,1);
    real_T *y_pr        = ssGetOutputPortRealSignal(S,2);
    void *y_p           = ssGetOutputPortSignal(S,2);
    real_T v1, v2;
    DTypeId dtype = ssGetInputPortDataType(S,0);

    *count = 0;
    *index = 0;
    v2 = getVal(dtype, 0, u_ptr, u_rptr);
    setVal(dtype, y_p, y_pr, v2);

    for (i = 0; i < n; i++) {
        v1 = getVal(dtype, i, u_ptr, u_rptr);
        if (v1 == v2)
            (*count)++;
        if (cmp(v1, v2)) {
            if (v1 != v2)
                *count = 1;
            *index = i;
            v2 = v1;
            setVal(dtype, y_p, y_pr, v1);
        }
    }

    if (*count == n)
        *count = 0;
}

/* Function: mdlTerminate ===================================================
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
    int_T single_shot = TYPE;

    if (!ssWriteRTWScalarParam(S, "FindType", &single_shot, SS_INT32))
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
