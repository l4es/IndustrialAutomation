/*****************************************************************************
 *
 * $Id$
 *
 * vim: tw=78
 *
 * Note about creating new Simulink data types:
 * In Simulink, new data types can be created. These named data types are
 * available in pdserv. These data types are C structures.
 *
 * IMPORTANT: Use this interface carefully. If you mess up, your application
 * will crash!
 *
 * PdServ detects these data types and searches the symbol table for a symbol
 * called "<name>_description". This symbol must be a zero terminated
 * structure vector with the following elements:
 * struct compound_desc {
 *     const char   *fieldName;   // Name of field
 *     size_t        offset;      // Offset of field in the structure
 *                                // Use offsetof() to get this value
 *     uint8_T       slDataId;    // enumerated data type
 *                                // from simstruc_types.h
 *                                // Can also be SS_STRUCT, in which case
 *                                // ->next must be used
 *     size_t        dataSize;    // data size in Bytes
 *     uint8_T       isComplex;   // Used only for primary data types
 *     size_t        numDims;     // number of elements in dimMap that
 *                                // specifies how many dimensions
 *                                // If dimMap == NULL, this value directly
 *                                // specifies the number of elements
 *     const size_t  *dimMap;     // Dimension list. If NULL, numDims
 *                                // specifies number of elements
 *     const struct compound_desc *next; // Pointer to another zero terminated
 *                                       // field set when
 *                                       // slDataId == SS_STRUCT
 * };
 *
 * The symbol name must also have "C" linkage, so add 'extern "C" {}' around
 * this definition (see below)
 *
 * To explain the mechanism behind numDims and dimMap:
 * For:              numDims=   dimMap=
 *      Scalars         1       NULL              double d
 *      Vectors         n       NULL              double h[6]
 *      N-Dim array     N       {d1,d2,...,dN}    double v[2][3][4]
 *
 *
 * For example, suppose you have a new data type called PRIV_TYPE, defined
 * as:
 *
   // In file <privtype.h>

   struct priv_type {
          double dbl[4],
          int16_t i16[5][6];
          char c;
          struct inner {
              int32_t i32;
              char b[40];
          } inner[2];
   };


 * Include the following code in the executable:
 * =========================================================================
 *
   #include "privtype.h"

   // Definition for struct priv_type

   const size_t dimMap[] = {
       5, 6,    // for i16
   };

   #ifdef __cplusplus
   extern "C" {
   #endif
   struct compound_desc {
       const char   *fieldName;
       size_t        offset;
       uint8_T       slDataId;
       size_t        dataSize;
       uint8_T       isComplex;
       size_t        numDims;
       const size_t  *dimMap;
       const struct compound_desc *next;
   } PRIV_TYPE_description[] = {
       {"dbl",   offsetof(struct priv_type, dbl), SS_DOUBLE,
           sizeof(real_T),        0, 4, NULL},

       {"i16",   offsetof(struct priv_type, i16), SS_INT16,
           sizeof(int16_T),       0, 2, dimMap},

       {"c",     offsetof(struct priv_type, c), SS_INT8,
           sizeof(char_T),        0, 1, NULL},

       {"inner", offsetof(struct priv_type, inner), SS_STRUCT,
                sizeof(struct inner),  0, 2, NULL, PRIV_TYPE_description + 5},

       {0,}, // Zero terminator

       {"i32",   offsetof(struct inner, i32), SS_INT32,
           sizeof(int32_T),       0, 1,  NULL},

       {"b",     offsetof(struct inner, b), SS_INT32,
           sizeof(char_T),        0, 40, NULL},

       {0,}, // Zero terminator
   };
   #ifdef __cplusplus
   }
   #endif
 * =========================================================================
 *
 ****************************************************************************/

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <sys/mman.h>
#include <getopt.h>
#include <libgen.h> // basename()
#include <errno.h>
#include <inttypes.h>
#include <unistd.h>  // daemon()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>     // malloc
#include <string.h>
#include <dlfcn.h>
#include <syslog.h>

#include <pdserv.h>

#include "rtmodel.h"
#include "rtwtypes.h"
#include "rt_nonfinite.h"
#include "rt_sim.h"

#ifdef PDSERV_VERSION_CODE
#    if PDSERV_VERSION_CODE >= PDSERV_VERSION(3,0,0)
#        define PDSERV3
#    endif
#endif

/****************************************************************************/

#define MAX_SAFE_STACK (8 * 1024) /** The maximum stack size which is
                                    guranteed safe to access without faulting.
                                   */

/* To quote a string */
#define STR(x) #x
#define QUOTE(x) STR(x)

#define EXPAND_CONCAT(name1, name2) name1 ## name2
#define CONCAT(name1, name2) EXPAND_CONCAT(name1, name2)
#define MODEL_VERSION        CONCAT(MODEL, _version)
#define RT_MODEL             CONCAT(MODEL, _rtModel)

#ifndef max
#define max(x1, x2) ((x1) > (x2) ? (x1) : (x2))
#endif

#ifndef min
#define min(x1, x2) ((x2) > (x1) ? (x1) : (x2))
#endif

#ifndef RT
# error "must define RT"
#endif

#ifndef MODEL
# error "must define MODEL"
#endif

#ifndef NUMST
# error "must define number of sample times, NUMST"
#endif

#ifndef NCSTATES
# error "must define NCSTATES"
#endif

/*====================*
 * External functions *
 *====================*/

#ifdef __cplusplus
extern "C" {
#endif

struct pdserv *pdserv = NULL;
struct pdserv *get_pdserv_ptr(void)
{
    return pdserv;
}

extern RT_MODEL *MODEL(void);

extern void MdlInitializeSizes(void);
extern void MdlInitializeSampleTimes(void);
extern void MdlStart(void);
extern void MdlOutputs(int_T tid);
extern void MdlUpdate(int_T tid);
extern void MdlTerminate(void);

extern const char *MODEL_VERSION;

#if NCSTATES > 0
extern void rt_ODECreateIntegrationData(RTWSolverInfo *si);
extern void rt_ODEUpdateContinuousStates(RTWSolverInfo *si);

# define rt_CreateIntegrationData(S) \
    rt_ODECreateIntegrationData(rtmGetRTWSolverInfo(S));
# define rt_UpdateContinuousStates(S) \
    rt_ODEUpdateContinuousStates(rtmGetRTWSolverInfo(S));
# else
# define rt_CreateIntegrationData(S)  \
      rtsiSetSolverName(rtmGetRTWSolverInfo(S),"FixedStepDiscrete");
# define rt_UpdateContinuousStates(S) \
      rtmSetT(S, rtsiGetSolverStopTime(rtmGetRTWSolverInfo(S)));
#endif


/* See comment at the top of the file for registering new data types */
struct compound_desc {
    const char   *fieldName;
    size_t        offset;
    uint8_T       slDataId;
    size_t        dataSize;
    uint8_T       isComplex;
    size_t        numDims;
    const size_t  *dimMap;
    const struct compound_desc *next;
};


#ifdef __cplusplus
}
#endif

/* Command-line option variables.  */

char *base_name = NULL; /**< basename of executable for usage() output. */
int priority = -1; /**< Task priority, -1 means RT (maximum). */
char *pdserv_config = NULL; /**< Path to PdServ configuration file. */
bool daemonize = false; /**< Become a daemon. */
char *pidPath = ""; /**< Path of PID file (empty for no PID file). */
int phase = -1;      /**< Phase to start task 0..100 */

static rtwCAPI_ModelMappingInfo* mmi;
static const rtwCAPI_DimensionMap* dimMap;
static const rtwCAPI_DataTypeMap* dTypeMap;
static const rtwCAPI_SampleTimeMap* sampleTimeMap;
static const uint_T* dimArray;
static void ** dataAddressMap;
static void *exe;      /* Pointer to this executable. */

const char* rt_OneStepMain(RT_MODEL *s, uint_T);
const char* rt_OneStepTid(RT_MODEL *s, uint_T);
int get_etl_data_type (const char *mwName,
        uint8_T slDataId, size_t size, unsigned int isComplex);

struct thread_task {
    RT_MODEL *S;
    uint_T sl_tid;
    unsigned int *running;
    const char *err;
    double sample_time;
    struct pdtask *pdtask;
    struct timespec monotonic_time;
    struct timespec world_time;
    pthread_t thread;
    pthread_mutex_t param_lock;
    pthread_rwlock_t signal_lock;
    const char* (*rt_OneStep)(RT_MODEL*, uint_T);
};

# if TID01EQ == 1
#  define FIRST_TID 1
# else
#  define FIRST_TID 0
# endif

#define NSEC_PER_SEC (1000000000)

#undef timeradd
inline void timeradd(struct timespec *t, unsigned int dt)
{
    t->tv_nsec += dt;
    while (t->tv_nsec >= NSEC_PER_SEC) {
        t->tv_nsec -= NSEC_PER_SEC;
        t->tv_sec++;
    }
}

#define DIFF_NS(A, B) (((long long) (B).tv_sec - (A).tv_sec) * NSEC_PER_SEC + \
        (B).tv_nsec - (A).tv_nsec)

#define ABS(X) ((X) >= 0 ? (X) : -(X))

/****************************************************************************/

#if !MT  /* SINGLETASKING */

#define NUMTASKS 1
static struct thread_task task[NUMTASKS];

const struct timespec *
get_etl_world_time(size_t tid)
{
    return &task[0].world_time;
}

/** Perform one step of the model.
 *
 * This function is modeled such that it could be called from an interrupt
 * service routine (ISR) with minor modifications.
 */
const char *
rt_OneStepMain(RT_MODEL *S, uint_T tid)
{
    real_T tnext;

    tnext = rt_SimGetNextSampleHit();
    rtsiSetSolverStopTime(rtmGetRTWSolverInfo(S),tnext);

    MdlOutputs(0);
    MdlUpdate(0);

    rt_SimUpdateDiscreteTaskSampleHits(rtmGetNumSampleTimes(S),
                                       rtmGetTimingData(S),
                                       rtmGetSampleHitPtr(S),
                                       rtmGetTPtr(S));

    if (rtmGetSampleTime(S,0) == CONTINUOUS_SAMPLE_TIME) {
        rt_UpdateContinuousStates(S);
    }

    return rtmGetErrorStatusFlag(S);
}

/****************************************************************************/

#else /* MT */

#define NUMTASKS (NUMST - FIRST_TID)
static struct thread_task task[NUMTASKS];

const struct timespec *
get_etl_world_time(size_t tid)
{
    return &task[tid].world_time;
}

/** Perform one step of the model.
 *
 * This function is modeled such that it could be called from an interrupt
 * service routine (ISR) with minor modifications.
 *
 * This routine is modeled for use in a multitasking environment and therefore
 * needs to be fully re-entrant when it is called from an interrupt service
 * routine.
 *
 * Note:
 *      Error checking is provided which will only be used if this routine
 *      is attached to an interrupt.
 */
const char *
rt_OneStepMain(RT_MODEL *S, uint_T tid)
{
    real_T tnext;

    /*******************************************
     * Step the model for the base sample time *
     *******************************************/

    tnext = rt_SimUpdateDiscreteEvents(
                rtmGetNumSampleTimes(S),
                rtmGetTimingData(S),
                rtmGetSampleHitPtr(S),
                rtmGetPerTaskSampleHitsPtr(S));

    rtsiSetSolverStopTime(rtmGetRTWSolverInfo(S),tnext);

    MdlOutputs(FIRST_TID);
    MdlUpdate(FIRST_TID);

    if (rtmGetSampleTime(S,0) == CONTINUOUS_SAMPLE_TIME) {
        rt_UpdateContinuousStates(S);
    } else {
        rt_SimUpdateDiscreteTaskTime(rtmGetTPtr(S), rtmGetTimingData(S), 0);
    }

#if FIRST_TID == 1
    rt_SimUpdateDiscreteTaskTime(rtmGetTPtr(S), rtmGetTimingData(S), 1);
#endif

    return rtmGetErrorStatusFlag(S);
}

/****************************************************************************/

/** Perform one step of the model subrates
 *
 * This routine is modeled for use in a multitasking environment and therefore
 * needs to be fully re-entrant when it is called from an interrupt service
 * routine.
 *
 * Note:
 *      Error checking is provided which will only be used if this routine
 *      is attached to an interrupt.
 */
const char *
rt_OneStepTid(RT_MODEL *S, uint_T tid)
{
    MdlOutputs(tid);

    MdlUpdate(tid);

    rt_SimUpdateDiscreteTaskTime(rtmGetTPtr(S), rtmGetTimingData(S), tid);

    return rtmGetErrorStatusFlag(S);

}

/****************************************************************************/

#endif /* MT */

/** Run the main task.
 */
void *run_task(void *p)
{
    struct thread_task *thread = p;
    unsigned int dt = 1.0e9 * thread->sample_time + 0.5;
    uint32_t exec_ns = 0, period_ns = 0, overruns = 0;
    struct timespec start_time,
                    last_start_time = thread->monotonic_time,
                    end_time = thread->monotonic_time;

    syslog(LOG_INFO, "Starting task with dt = %u ns.", dt);

    while (!thread->err && *thread->running
            && !clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,
                &thread->monotonic_time, 0)) {

        clock_gettime(CLOCK_MONOTONIC, &start_time);

        pthread_rwlock_wrlock(&thread->signal_lock);

        clock_gettime(CLOCK_REALTIME, &thread->world_time);

#ifndef PDSERV3
        if (thread == &task[0]) {
            pdserv_get_parameters(pdserv, thread->pdtask,
                    &thread->world_time);
        }
#endif

        /* Lock parameters and execute task */
        pthread_mutex_lock(&thread->param_lock);
        thread->err = thread->rt_OneStep(thread->S, thread->sl_tid);
        pthread_mutex_unlock(&thread->param_lock);
        pdserv_update(thread->pdtask, &thread->world_time);

        /* Calculate timing statistics */
        period_ns = DIFF_NS(last_start_time, start_time);
        exec_ns = DIFF_NS(last_start_time, end_time);
        last_start_time = start_time;
        pdserv_update_statistics(thread->pdtask,
                1.0e-9 * exec_ns, 1.0e-9 * period_ns, overruns);

        pthread_rwlock_unlock(&thread->signal_lock);

        timeradd(&thread->monotonic_time, dt);

        clock_gettime(CLOCK_MONOTONIC, &end_time);

        if (DIFF_NS(end_time, thread->monotonic_time) < 0)
            overruns++;
    }

    *thread->running = 0;

    return 0;
}

/****************************************************************************/

/** Create a new compound data type
 */
size_t make_compound(int compound,
        const struct compound_desc* compound_desc, size_t offset)
{
    int dt;
    //printf("%s() compound=%i, offset=%zu\n", __func__, compound, offset);

    for (; compound_desc->fieldName; compound_desc++) {
        if (compound_desc->slDataId == SS_STRUCT) {
            dt = pdserv_create_compound(
                    compound_desc->fieldName, compound_desc->dataSize);
            make_compound(dt, compound_desc->next,
                    offset + compound_desc->offset);
        }
        else
            dt = get_etl_data_type(
                    compound_desc->fieldName,
                    compound_desc->slDataId,
                    compound_desc->dataSize,
                    compound_desc->isComplex);

        pdserv_compound_add_field( compound,
                compound_desc->fieldName, dt, offset + compound_desc->offset,
                compound_desc->numDims, compound_desc->dimMap);
    }

    return offset;
}

/** Get the compound data type as expected by EtherLab.
 */
int get_compound_data_type(const char *mwName, size_t size)
{
    struct compound_list {
        struct compound_list *next;
        const char *mwName;
        int dtype;
    };
    static struct compound_list compound_list_head = {
        &compound_list_head
    };
    struct compound_list *list;
    const void *compound_desc;
    char *compound_name;
    size_t n;

    /* Try to find the compound in the list */
    for (list = &compound_list_head;
            list->next != &compound_list_head; list = list->next) {
        //printf("diff=, name=%s\n", list->mwName);
        if (!strcmp(mwName, list->mwName))
            return list->dtype;
    }

    /* Look for a symbol <mwName>_description */
    if (!exe) {
        exe = dlopen(NULL, RTLD_LAZY);
        if (!exe)
            return 0;
    }
    n = strlen(mwName);
    compound_name = malloc(n + 20);
    strcpy(compound_name, mwName);
    strcpy(compound_name+n, "_description");
    compound_desc = dlsym(exe, compound_name);
    free(compound_name);
    if (!compound_desc)
        return 0;

    /* Append to end of list */
    list->next = calloc(1, sizeof(struct compound_list));
    list->mwName = mwName;
    list->dtype = pdserv_create_compound(mwName, size);

    list->next->next = &compound_list_head;

    make_compound(list->dtype, compound_desc, 0);

    //printf("%s exe=%p desc=%p\n", compound_name, exe, compound_desc);
    return list->dtype;
}

/** Get the data type as expected by EtherLab.
 */
int get_etl_data_type (const char *mwName,
        uint8_T slDataId, size_t size, unsigned int isComplex)
{
    static int pd_complex[pd_datatype_end];

    int pd_type;
    switch (slDataId) {
        case SS_DOUBLE:  pd_type = pd_double_T;        break;
        case SS_SINGLE:  pd_type = pd_single_T;        break;
        case SS_INT8:    pd_type = pd_sint8_T;         break;
        case SS_UINT8:   pd_type = pd_uint8_T;         break;
        case SS_INT16:   pd_type = pd_sint16_T;        break;
        case SS_UINT16:  pd_type = pd_uint16_T;        break;
        case SS_INT32:   pd_type = pd_sint32_T;        break;
        case SS_UINT32:  pd_type = pd_uint32_T;        break;
        case SS_BOOLEAN: pd_type = pd_boolean_T;       break;
        case SS_STRUCT:
                         pd_type = get_compound_data_type(mwName, size);
                         break;
        default:         pd_type = 0;                  break;
    }

    //printf("new datat type %i\n", pd_type);
    if (!isComplex || !pd_type || slDataId == SS_STRUCT)
        return pd_type;

//    printf("complex type %i %i %i\n",
//            dataTypeIndex, pd_type, pd_complex[pd_type]);
    if (pd_complex[pd_type])
        return pd_complex[pd_type];

    pd_complex[pd_type] = pdserv_create_compound( mwName, size);
    pdserv_compound_add_field( pd_complex[pd_type],
            "Re", pd_type, 0, 1, NULL);
    pdserv_compound_add_field( pd_complex[pd_type],
            "Im", pd_type, size / 2, 1, NULL);

    return pd_complex[pd_type];
}

/****************************************************************************/

int write_parameter(
        const struct pdvariable* param,
        void *dst, const void* src, size_t len,
        struct timespec* time,
        void* priv_data)
{
    struct thread_task *p_task = task + NUMTASKS;
    while (p_task != task)
        pthread_mutex_lock(&(--p_task)->param_lock);

    memcpy(dst, src, len);
    clock_gettime(CLOCK_REALTIME, time);

    p_task = task + NUMTASKS;
    while (p_task != task)
        pthread_mutex_unlock(&(--p_task)->param_lock);

    return 0;
}

/****************************************************************************/

int read_signal(
        const struct pdvariable* signal,
        void* dst, const void* src, size_t len,
        struct timespec* time,
        void* priv_data)
{
    struct thread_task* task = priv_data;

    pthread_rwlock_rdlock(&task->signal_lock);
    memcpy(dst, src, len);
    if (time)
        *time = task->world_time;
    pthread_rwlock_unlock(&task->signal_lock);

    return 0;
}


/****************************************************************************/

/** Register a signal with PdServ.
 */
const char *register_signal(struct thread_task *task,
        const rtwCAPI_Signals* signals, size_t idx)
{
    uint_T addrMapIndex    = rtwCAPI_GetSignalAddrIdx(signals, idx);
    /* size_t sysNum = rtwCAPI_GetSignalSysNum(signals, idx); */
    const char *blockPath  = rtwCAPI_GetSignalBlockPath(signals, idx);
    const char *signalName = rtwCAPI_GetSignalName(signals, idx);
    uint16_T portNumber      = rtwCAPI_GetSignalPortNumber(signals, idx);
    uint16_T dataTypeIndex   = rtwCAPI_GetSignalDataTypeIdx(signals, idx);
    uint16_T dimIndex        = rtwCAPI_GetSignalDimensionIdx(signals, idx);
    uint8_T  sTimeIndex      = rtwCAPI_GetSignalSampleTimeIdx(signals, idx);

    const void *address =
        rtwCAPI_GetDataAddress(dataAddressMap, addrMapIndex);
    uint_T dimArrayIndex = rtwCAPI_GetDimArrayIndex(dimMap, dimIndex);
    int data_type = get_etl_data_type(
            rtwCAPI_GetDataTypeMWName(dTypeMap, dataTypeIndex),
            rtwCAPI_GetDataTypeSLId(dTypeMap, dataTypeIndex),
            rtwCAPI_GetDataTypeSize(dTypeMap, dataTypeIndex),
            rtwCAPI_GetDataIsComplex(dTypeMap, dataTypeIndex));
    size_t pathLen = strlen(blockPath) + strlen(signalName) + 9;
    uint8_T ndim = rtwCAPI_GetNumDims(dimMap, dimIndex);
#if !MT
    const real_T *sampleTime =
        rtwCAPI_GetSamplePeriodPtr(sampleTimeMap, sTimeIndex);
#endif
    int8_T tid = rtwCAPI_GetSampleTimeTID(sampleTimeMap, sTimeIndex);
    uint_T decimation;
    boolean_T related;
    const char *prev_signal_path, *next_signal_path;

    struct pdvariable *signal;
    char *path;
    size_t arrayDim[2], *dim = arrayDim;

    size_t i;
    const char *err = 0;

    /* Only allow built-in data types */
    if (!data_type)
        return NULL;

    path = malloc(pathLen);
    if (!path) {
        err = "No memory";
        goto out;
    }

    /* Check that the data type is compatible */
    if (rtwCAPI_GetDataIsPointer(dTypeMap, dataTypeIndex)) {
        err = "Cannot interact with pointer data types.";
        goto out;
    }

    /* Find out whether this signal path is the same as a neighbour. Then
     * the path must be modified to make it unique.
     * If it has an alias, use it, otherwise hopefully the port number
     * can be used to make it unique.
     *
     * Note: * idx + 1 is valid, since the list is null terminated
     */
    prev_signal_path =
        idx ? rtwCAPI_GetSignalBlockPath(signals, idx-1) : NULL;
    next_signal_path = rtwCAPI_GetSignalBlockPath(signals, idx+1);
    related =
        (prev_signal_path && !strcmp(blockPath, prev_signal_path))
        || (next_signal_path && !strcmp(blockPath, next_signal_path));

    /* Simulink Coder adds model name to the path. This is totally useless,
     * so remomve it */
    blockPath = strchr(blockPath, '/');
    if (!blockPath) {
        err = "No '/' in path";
        goto out;
    }

    /* Format block path */
    if (related) {
        if (*signalName && strlen(signalName)) {
            /* Add alias to signal name for related signals */
            snprintf(path, pathLen, "%s/%s", blockPath, signalName);
            signalName = NULL;
        }
        else {
            /* No alias, so add portNumber to identify related signals */
            snprintf(path, pathLen, "%s/%u", blockPath, portNumber);
        }
    }
    else {
        strncpy(path, blockPath, pathLen);
    }

#if !MT
    decimation =
        tid >= 0 && *sampleTime ? *sampleTime/task[0].sample_time + 0.5 : 1;
#else
    decimation = 1;
    if (tid >= 0) {
        task += tid - (tid && FIRST_TID);
    }
#endif

    if (ndim == 1) {
        /* Only one dimension */
        dim[0] = dimArray[dimArrayIndex];
    }
    else if (ndim == 2) {
        if (dimArray[dimArrayIndex] == 1
                || dimArray[dimArrayIndex + 1] == 1) {
            ndim = 1;
            dim[0] = dimArray[dimArrayIndex] * dimArray[dimArrayIndex + 1];
        }
        else if (rtwCAPI_GetOrientation(dimMap, dimIndex)
                == rtwCAPI_MATRIX_COL_MAJOR) {
            dim[0] = dimArray[dimArrayIndex + 1];
            dim[1] = dimArray[dimArrayIndex];
        }
        else {
            dim[0] = dimArray[dimArrayIndex];
            dim[1] = dimArray[dimArrayIndex + 1];
        }
    }
    else {
        /* Multidimenstional array. Copy array specification */
        dim = calloc(ndim, sizeof(size_t));
        if (!dim) {
            err = "No memory";
            goto out;
        }

        /* Reverse the order of the nD-Matrix.
         * Matlab's Matrices run coherently from the first to the last index,
         * while in C it is exactly the other way round. The intention here
         * is to present the arrays in a way that is compatable to C.
         *
         * e.g. in Matlab A(2,1,1) and A(3,1,1) are adjacent, whereas
         * in C, A[1][1][2] and A[1][1][3] are adjacent.
         */
        for (i = 0; i < ndim; ++i)
            dim[i] = dimArray[dimArrayIndex + (ndim - 1) - i];
    }

#if 0
    printf("%s task[%u], decim=%u, dt=%u, %p, ndim=%u, %p\n",
            path, tid - (tid && FIRST_TID),
            decimation, data_type, address, ndim, dim);
#endif

    //printf("Reg with dt=%i\n", data_type);
#ifdef PDSERV3
    signal = pdserv_signal(task->pdtask, decimation,
            path, data_type, address, ndim, dim, read_signal, task);
#else
    signal = pdserv_signal(task->pdtask, decimation,
            path, data_type, address, ndim, dim);
#endif

    if (signal && !related && signalName && *signalName)
        pdserv_set_alias(signal, signalName);

out:
    free(path);
    if (dim != arrayDim)
        free(dim);
    if (err) {
        printf("%s\n", err);
    }
    return err;
}

/****************************************************************************/

/** Register a parameter with PdServ.
 */
const char *register_parameter( struct pdserv *pdserv,
        const rtwCAPI_BlockParameters* params, size_t idx)
{
    uint_T addrMapIndex = rtwCAPI_GetBlockParameterAddrIdx(params, idx);
    const char *blockPath = rtwCAPI_GetBlockParameterBlockPath(params, idx);
    const char *paramName = rtwCAPI_GetBlockParameterName(params, idx);
    uint16_T dataTypeIndex = rtwCAPI_GetBlockParameterDataTypeIdx(params, idx);
    uint16_T dimIndex = rtwCAPI_GetBlockParameterDimensionIdx(params, idx);

    void *address = rtwCAPI_GetDataAddress(dataAddressMap, addrMapIndex);
    uint_T dimArrayIndex = rtwCAPI_GetDimArrayIndex(dimMap, dimIndex);
    size_t pathLen = strlen(blockPath) + strlen(paramName) + 9;
    uint8_T ndim = rtwCAPI_GetNumDims(dimMap, dimIndex);

    int data_type = get_etl_data_type(
            rtwCAPI_GetDataTypeMWName(dTypeMap, dataTypeIndex),
            rtwCAPI_GetDataTypeSLId(dTypeMap, dataTypeIndex),
            rtwCAPI_GetDataTypeSize(dTypeMap, dataTypeIndex),
            rtwCAPI_GetDataIsComplex(dTypeMap, dataTypeIndex));

    char *path;
    size_t arrayDim[2], *dim = arrayDim;

    size_t i;
    const char *err = 0;

    /* Only allow built-in data types */
    if (!data_type)
        return NULL;

    path = malloc(pathLen);
    if (!path) {
        err = "No memory";
        goto out;
    }

    /* Check that the data type is compatible */
    if (rtwCAPI_GetDataIsPointer(dTypeMap, dataTypeIndex)) {
        err = "Cannot interact with pointer data types.";
        goto out;
    }

    blockPath = strchr(blockPath, '/');
    if (!blockPath) {
        err = "No '/' in path";
        goto out;
    }

    snprintf(path, pathLen, "%s/%s", blockPath, paramName);

    if (ndim == 1) {
        /* Only one dimension */
        dim[0] = dimArray[dimArrayIndex];
    }
    else if (ndim == 2) {
        if (dimArray[dimArrayIndex] == 1
                || dimArray[dimArrayIndex + 1] == 1) {
            ndim = 1;
            dim[0] = dimArray[dimArrayIndex] * dimArray[dimArrayIndex + 1];
        }
        else if (rtwCAPI_GetOrientation(dimMap, dimIndex)
                == rtwCAPI_MATRIX_COL_MAJOR) {
            dim[0] = dimArray[dimArrayIndex + 1];
            dim[1] = dimArray[dimArrayIndex];
        }
        else {
            dim[0] = dimArray[dimArrayIndex];
            dim[1] = dimArray[dimArrayIndex + 1];
        }
    }
    else {
        /* Multidimenstional array. Copy array specification */
        dim = calloc(ndim, sizeof(size_t));
        if (!dim) {
            err = "No memory";
            goto out;
        }

        for (i = 0; i < ndim; ++i)
            dim[i] = dimArray[dimArrayIndex + (ndim - 1) - i];
    }

#ifdef PDSERV3
    pdserv_parameter(pdserv, path, 0666, data_type, address, ndim, dim,
            write_parameter, 0);
#else
    pdserv_parameter(pdserv, path, 0666, data_type, address, ndim, dim, 0, 0);
#endif

out:
    free(path);
    if (dim != arrayDim)
        free(dim);
    if (err) {
        printf("%s\n", err);
    }
    return err;
}

/****************************************************************************/

/** Initialize all model variables.
 */
const char *
rtw_capi_init(RT_MODEL *S,
        struct pdserv *pdserv, struct thread_task *task)
{
    const rtwCAPI_Signals* signals;
    const rtwCAPI_BlockParameters* params;
    size_t i;

    mmi = &(rtmGetDataMapInfo(S).mmi);
    dimMap = rtwCAPI_GetDimensionMap(mmi);
    dTypeMap = rtwCAPI_GetDataTypeMap(mmi);
    dimArray = rtwCAPI_GetDimensionArray(mmi);
    sampleTimeMap = rtwCAPI_GetSampleTimeMap(mmi);
    dataAddressMap = rtwCAPI_GetDataAddressMap(mmi);

    signals = rtwCAPI_GetSignals(mmi);
    for (i = 0; signals && i < rtwCAPI_GetNumSignals(mmi); ++i) {
        register_signal(task, signals, i);
    }

    params = rtwCAPI_GetBlockParameters(mmi);
    for (i = 0; params && i < rtwCAPI_GetNumBlockParameters(mmi); ++i) {
        register_parameter(pdserv, params, i);
    }

    return NULL;
}

/****************************************************************************/

/** Execute model.
 */
const char *init_application(RT_MODEL *S)
{
    const char *errmsg;

    /************************
     * Initialize the model *
     ************************/
    MdlInitializeSizes();
    MdlInitializeSampleTimes();

    if ((errmsg = rt_SimInitTimingEngine(
                    rtmGetNumSampleTimes(S),
                    rtmGetStepSize(S),
                    rtmGetSampleTimePtr(S),
                    rtmGetOffsetTimePtr(S),
                    rtmGetSampleHitPtr(S),
                    rtmGetSampleTimeTaskIDPtr(S),
                    rtmGetTStart(S),
                    &rtmGetSimTimeStep(S),
                    &rtmGetTimingData(S)))) {
        return errmsg;
    }
    rt_CreateIntegrationData(S);

    MdlStart();
    if ((errmsg = rtmGetErrorStatus(S))) {
        MdlTerminate();
        return errmsg;
    }

    return NULL;
}

/****************************************************************************/

/** Return the current system time.
 *
 * This is a callback needed by pdserv.
 */
int gettime(struct timespec *time)
{
    return clock_gettime(CLOCK_REALTIME, time);
}

/****************************************************************************/

/** Cause a stack fault before entering cyclic operation.
 */
void stack_prefault(void)
{
    unsigned char dummy[MAX_SAFE_STACK];

    memset(dummy, 0, MAX_SAFE_STACK);
}

/****************************************************************************/

/** Remove the PID file.
 */
void remove_pid_file()
{
    int ret;

    ret = unlink(pidPath);
    if (ret == -1) {
        fprintf(stderr, "Failed to remove PID file \"%s\": %s\n",
                pidPath, strerror(errno));
    }
}

/****************************************************************************/

/** Create the PID file.
 */
void create_pid_file()
{
    int fd, ret, len;
    char str[32];

    fd = open(pidPath, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd == -1) {
        fprintf(stderr, "Failed to create PID file \"%s\": %s\n",
                pidPath, strerror(errno));
        return;
    }

    len = snprintf(str, sizeof(str), "%i\n", getpid());

    ret = write(fd, str, len);
    if (ret == -1) {
        fprintf(stderr, "Failed to write to PID file \"%s\": %s\n",
                pidPath, strerror(errno));
        goto out_unlink;
    }

    if (ret != len) {
        fprintf(stderr, "Failed to write to PID file \"%s\"."
                " Written %i of %i bytes.", pidPath, ret, len);
        goto out_unlink;
    }

    return;

out_unlink:
    close(fd);
    remove_pid_file();
}

/****************************************************************************/

/** Output the usage.
 */
void usage(FILE *f)
{
    fprintf(f,
            "Usage: %s [OPTIONS]\n"
            "Options:\n"
            "  --priority       -p <PRIO>  Set task priority. Default: RT.\n"
            "  --pdserv-config  -c <PATH>  PdServ configuration file.\n"
            "                              Default: None (use defaults).\n"
            "  --pid-path       -i <PATH>  Write PID file. Default: "
                                           "No PID file.\n"
            "  --start-phase    -f         Timing phase to start off (0..99)\n"
            "                              Default: -1 (None)\n"
            "  --daemon         -d         Become a daemon before cyclic "
                                           "operation.\n"
            "  --help           -h         Show this help.\n",
            base_name);
}

/****************************************************************************/

/** Get the command-line options.
 */
void get_options(int argc, char **argv)
{
    int c, arg_count;

    static struct option longOptions[] = {
        //name,           has_arg,           flag, val
        {"priority",      required_argument, NULL, 'p'},
        {"pdserv-config", required_argument, NULL, 'c'},
        {"pid-file",      required_argument, NULL, 'i'},
        {"start-phase",   required_argument, NULL, 'f'},
        {"daemon",        no_argument,       NULL, 'd'},
        {"help",          no_argument,       NULL, 'h'},
        {}
    };

    do {
        c = getopt_long(argc, argv, "p:c:i:f:dh", longOptions, NULL);

        switch (c) {
            case 'p':
                if (!strcmp(optarg, "RT")) {
                    priority = -1;
                } else {
                    char *end;
                    priority = strtoul(optarg, &end, 10);
                    if (!*optarg || *end) {
                        fprintf(stderr, "Invalid priority: %s\n", optarg);
                        exit(1);
                    }
                }
                break;

            case 'c':
                pdserv_config = optarg;
                break;

            case 'i':
                pidPath = optarg;
                break;

            case 'f':
                phase = atoi(optarg);
                if (phase >= 100) {
                    fprintf(stderr, "Invalid phase: %s\n", optarg);
                    exit(1);
                }
                break;

            case 'd':
                daemonize = true;
                break;

            case 'h':
                usage(stdout);
                exit(0);

            case '?':
                usage(stderr);
                exit(1);

            default:
                break;
        }
    }
    while (c != -1);

    arg_count = argc - optind;

    if (arg_count) {
        fprintf(stderr, "%s takes no arguments!\n", base_name);
        usage(stderr);
        exit(1);
    }
}

/****************************************************************************/

/** Process main function.
 */
int main(int argc, char **argv)
{
    RT_MODEL *S;
    unsigned int running = 1;
    const char *err = NULL;
    struct thread_task* p_task;
    pthread_rwlockattr_t rwlock_attr;

    /* Set defaults for command-line options. */
    base_name = basename(argv[0]);

    get_options(argc, argv);

    if (daemonize) {
        int ret;
        fprintf(stderr, "Now becoming a daemon.\n");
        ret = daemon(0, 0);
        if (ret != 0) {
            fprintf(stderr, "Failed to become daemon: %s\n", strerror(errno));
            pdserv_exit(pdserv);
            err = "daemon() failed.";
            goto out;
        }
    }

    if (!(pdserv = pdserv_create(QUOTE(MODEL), MODEL_VERSION, gettime))) {
        err = "Failed to init pdserv.";
        goto out;
    }

    if (pdserv_config) {
        pdserv_config_file(pdserv, pdserv_config);
    }

    /* Initialize model */
    S = MODEL();

    /* Initialize rwlock attributes */
    pthread_rwlockattr_init(&rwlock_attr);
    pthread_rwlockattr_setkind_np(&rwlock_attr,
            PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);

    /* Create necessary pdserv tasks */
    for (p_task = task; p_task != task + NUMTASKS; ++p_task) {
        p_task->S = S;
        p_task->sl_tid = (p_task - task) + FIRST_TID;
        p_task->sample_time = rtmGetSampleTime(S, p_task->sl_tid);
        p_task->pdtask = pdserv_create_task(pdserv, p_task->sample_time, 0);

        pthread_rwlock_init(&p_task->signal_lock, &rwlock_attr);
        pthread_mutex_init(&p_task->param_lock, NULL);
    }

    pthread_rwlockattr_destroy(&rwlock_attr);

    /* Register signals and parameters */
    if ((err = rtw_capi_init(S, pdserv, task))) {
        pdserv_exit(pdserv);
        goto out;
    }

    /* Prepare process-data interface, create threads, etc. */
    pdserv_prepare(pdserv);

    /* Lock all memory forever. */
    if (mlockall(MCL_CURRENT | MCL_FUTURE))
        fprintf(stderr, "mlockall() failed: %s\n", strerror(errno));

    /* Set task priority. */
    {
        struct sched_param param = {};
        if (priority == -1)
            priority = sched_get_priority_max(SCHED_FIFO);

        param.sched_priority = priority;
        if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
            fprintf(stderr,
                    "Setting SCHED_FIFO with priority %i failed: %s\n",
                    param.sched_priority, strerror(errno));

            /* Reset priority, so that sub-threads start */
            priority = -1;
        }
    }

    /* Provoke the first stack fault before cyclic operation. */
    stack_prefault();

    if ((err = init_application(S))) {
        pdserv_exit(pdserv);
        goto out;
    }

    if (pidPath[0])
        create_pid_file();

    openlog("rttask", LOG_PID, LOG_DAEMON);

    /* Delay start time of slowest task to an integral multiple of
     * its sample time. This allows synchronization of real time
     * threads, specifically to enable phasing */
    p_task = task + NUMTASKS-1;
    clock_gettime(CLOCK_MONOTONIC, &p_task->monotonic_time);
    if (phase >= 0) {
        uint64_t t64 = 1000000000ULL * p_task->monotonic_time.tv_sec
            + p_task->monotonic_time.tv_nsec;
        uint64_t dt = p_task->sample_time * 1e9;
        unsigned int phase_shift = dt - (t64 % dt);

        syslog(LOG_INFO, "Delay starting time by %u ns.", phase_shift);
        timeradd(&p_task->monotonic_time, phase_shift);
    }

    /* Start sub-threads */
    for (p_task = task; p_task != task + NUMTASKS; ++p_task) {
        p_task->monotonic_time = task[NUMTASKS-1].monotonic_time;
        p_task->running = &running;
        p_task->err = 0;

        if (phase >= 0) {
            /* Delay start time as required by phase */
            timeradd(&p_task->monotonic_time,
                    1.0e9 * p_task->sample_time * phase / 100);
        }

        if (p_task == task)
            p_task->rt_OneStep = rt_OneStepMain;
#if MT
        else {
            struct sched_param param = {
                .sched_priority = priority - (p_task - task),
            };
            pthread_attr_t attr;

            /* Setup scheduler */
            pthread_attr_init(&attr);
            pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
            pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
            pthread_attr_setschedparam(&attr, &param);

            p_task->rt_OneStep = rt_OneStepTid;
            pthread_create(&p_task->thread,
                    priority == -1 ? NULL : &attr, run_task, p_task);

            pthread_attr_destroy(&attr);
        }
#endif
    }

    syslog(LOG_INFO, "Starting main thread with dt = %u ns.",
            (unsigned int)(p_task->sample_time * 1e9 + 0.5));

    /* Now run main task */
    run_task(&task[0]);

    /* Collect tasks and report errors */
    for (p_task = task; p_task != task + NUMTASKS; ++p_task) {
        if (p_task != task)
            pthread_join(p_task->thread, 0);

        if (p_task->err)
            fprintf(stderr, "Task %zi had an error: %s\n",
                    p_task - task, p_task->err);
    }

    /* Clean up */
    pdserv_exit(pdserv);
    MdlTerminate();
    if (pidPath[0])
        remove_pid_file();

out:
    if (err) {
        fprintf(stderr, "Fatal error: %s\n", err);
        syslog(LOG_INFO, "Exiting with error.");
        closelog();
        return 1;
    }
    else {
        syslog(LOG_INFO, "Exiting gracefully.");
        closelog();
        return 0;
    }
}

/****************************************************************************/
