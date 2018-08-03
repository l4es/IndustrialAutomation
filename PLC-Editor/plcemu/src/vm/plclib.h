#ifndef _PLCLIB_H_
#define _PLCLIB_H_
/**
 *@file plclib.h
 *@brief PLC core virtual machine
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <inttypes.h>
 
#define NEGATE 128//negate second operand, not return value.
#define BOOLEAN   64
#define NOP	0

#define BYTE unsigned char

#define MILLION 1000000
#define THOUSAND 1000
#define PLC_OK 0
#define PLC_ERR -1
#define MAXBUF 256 
#define MAXRUNG 256
#define MAXSTR	1024
#define MEDSTR	256
#define SMALLSTR 128
#define SMALLBUF 64
#define TINYSTR 32
#define COMMLEN 16

#define LWORDSIZE 64
#define DWORDSIZE 32
#define WORDSIZE 16
#define BYTESIZE 8

#define LONG_BYTES 8

#define LABELLEN 4
#define NICKLEN	16
#define FALSE 0
#define TRUE 1

#define FLOAT_PRECISION 0.000001
#define ASCIISTART 0x30

typedef enum{
    ST_STOPPED,
    ST_RUNNING
}STATUSES;

typedef enum{
    N_ERR = -10, ///ERROR CODES are negative
    ERR_OVFLOW,
    ERR_TIMEOUT,
    ERR_BADOPERATOR,
    ERR_BADCOIL,
    ERR_BADINDEX,
    ERR_BADOPERAND,
    ERR_BADFILE,
    ERR_BADCHAR,
}ERRORCODES;

typedef enum{
    IE_PLC,
    IE_BADOPERATOR,
    IE_BADCOIL,
    IE_BADINDEX,
    IE_BADOPERAND,
    IE_BADFILE,
    IE_BADCHAR,
    N_IE 
} IL_ERRORCODES;

typedef enum{
    LANG_LD,
    LANG_IL,
    LANG_ST
}LANGUAGES;

typedef enum{
    T_BOOL, //- 1 bit
    T_BYTE, //- 8 bit (1 byte)
    T_WORD, //- 16 bit (2 byte)
    T_DWORD,//- 32 bit (4 byte)
    T_LWORD,//- 64 bit (8 byte)
    T_REAL,//- (8 byte) double floating point number   
    
    N_TYPES
}DATATYPES;

#define BOOL(x) x > 0 ? TRUE : FALSE 
 
/**
 *IL instructions
 */
typedef enum{
    ///IL OPCODES: 
    IL_NOP,///no operand
    IL_POP,     ///)
    IL_RET,         ///RET
    //arithmetic LABEL
    IL_JMP,         ///JMP
    //subroutine call (unimplemented)
    IL_CAL,         ///CAL
    //boolean, no modifier
    IL_SET,         ///S
    IL_RESET,       ///R
    //any operand, only negation
    IL_LD,          ///LD
    IL_ST,          ///ST
    //any operand, only push
    //boolean, all modifiers
    IL_AND,         ///AND
    IL_OR,          ///OR
    IL_XOR,         ///XOR
    IL_ADD,          ///ADD
    IL_SUB,          ///SUBTRACT
    IL_MUL,          ///MULTIPLY
    IL_DIV,          ///DIVIDE
    IL_GT,          ///GREATER THAN
    IL_GE,          ///GREATER OR EQUAL
    IL_EQ,          ///EQUAL
    IL_NE,          ///NOT EQUAL
    IL_LT,          ///LESS THAN
    IL_LE,          ///LESS OR EQUAL
    N_IL_INSN
}IL_INSN;

#define FIRST_BITWISE IL_AND
#define FIRST_ARITHMETIC IL_ADD
#define FIRST_COMPARISON IL_GT

#define IS_BITWISE(x) (x >= FIRST_BITWISE && x < FIRST_ARITHMETIC)
#define IS_ARITHMETIC(x) (x >= FIRST_ARITHMETIC && x < FIRST_COMPARISON)
#define IS_COMPARISON(x) (x >= FIRST_COMPARISON && x < N_IL_INSN)
#define IS_OPERATION(x) (x >= FIRST_BITWISE && x < N_IL_INSN) 
 
typedef enum{
///operands
    OP_INPUT = 20,  ///i
    OP_REAL_INPUT,  ///if
    OP_FALLING, 	///f
    OP_RISING,      ///r
    OP_MEMORY,  	///m
    OP_REAL_MEMORY, ///mf
    OP_COMMAND,     ///c
    OP_BLINKOUT,    ///b
    OP_TIMEOUT,     ///t
    OP_OUTPUT,   	///q
    OP_REAL_OUTPUT, ///qf
    ///coils 
    OP_CONTACT,     ///Q
    OP_REAL_CONTACT,///QF
    OP_START,       ///T
    OP_PULSEIN,     ///M
    OP_REAL_MEMIN,  ///MF
    OP_WRITE,       ///W
    OP_END,         ///0
    N_OPERANDS
}IL_OPERANDS;

#define OP_VALID(x) x >= OP_INPUT && x < N_OPERANDS
#define OP_REAL(x) x == OP_REAL_INPUT \
                || x == OP_REAL_MEMORY \
                || x == OP_REAL_OUTPUT \
                || x == OP_REAL_CONTACT \
                || x == OP_REAL_MEMIN

/**
 *IL modifiers
 */
typedef enum{
    IL_NEG = 1, /// '!'
    IL_PUSH,    /// '('
    IL_NORM,    /// ' '
    IL_COND,    ///'?'
    N_IL_MODIFIERS
}IL_MODIFIERS;

#define IS_MODIFIER(x) (x >= IL_NEG && x < N_IL_MODIFIERS) 

typedef enum{///boolean function blocks supported
    BOOL_DI,        ///digital input
    BOOL_DQ,        ///digital output
    BOOL_COUNTER,   ///pulse of counter
    BOOL_TIMER,     ///output of timer
    BOOL_BLINKER,   ///output of blinker
    N_BOOL
}BOOL_FB;


/**
 * @brief what changed since the last cycle
 */
typedef enum{
    CHANGED_I = 0x1,
    CHANGED_O = 0x2,
    CHANGED_M = 0x4,
    CHANGED_T = 0x8,
    CHANGED_S = 0x10,
}CHANGE_DELTA; 

//TODO: add type for checkings and castings
typedef union accdata{
  uint64_t u; 
  double r;
} data_t; //what can the accumulator be


/**
 * @brief The opcode struct
 *AND, OR, XOR, ANDN, ORN, XORN.
 *TODO: byte type operations.
 *if op > 128 then value is negated first.
 */
typedef struct opcode{
    BYTE operation;
    BYTE type;
    BYTE depth;
    union accdata value;
    struct opcode * next;
} * opcode_t;

/**
 * @brief The instruction struct
 */
typedef struct instruction{
    char label[MAXBUF];
    char lookup[MAXBUF]; //label to lookup (applies to JMP etc.)
    BYTE operation;
    BYTE operand;
    BYTE modifier;
    BYTE byte;
    BYTE bit;
} * instruction_t;

/**
 * @brief The instruction list executable rung
 */
typedef struct rung{
  instruction_t * instructions;
  unsigned int insno;///actual no of active lines
  struct rung * next; ///linked list of rungs
  opcode_t stack; ///head of stack
  struct opcode prealloc[MAXBUF]; ///preallocated stack    
  union accdata acc;    ///accumulator
} * rung_t;


/**
 * @brief The digital_input struct
 */
typedef struct digital_input{
    BYTE I;///contact value
    BYTE RE;///rising edge
    BYTE FE;///falling edge
    char nick[NICKLEN];///nickname
} * di_t;

/**
 * @brief The digital_output struct
 */
typedef struct digital_output{
    BYTE Q;//contact
    BYTE SET;//set
    BYTE RESET;//reset
    char nick[NICKLEN];//nickname
} * do_t;

/**
 * @brief The analog_io  struct
 */
typedef struct analog_io{
    double V;/// data value
    double min; ///range for conversion to/from raw data
    double max;
    char nick[NICKLEN];///nickname
} * aio_t;

/**
 * @brief The timer struct.
 * struct which represents  a timer state at a given cycle
 */
typedef struct timer{
    long S;	///scale; S=1000=>increase every 1000 cycles. STEP= 10 msec=> increase every 10 sec
    long sn;///internal counter used for scaling
    long V;	///value
    BYTE Q;	///output
    long P;	///Preset value
    BYTE ONDELAY;///1=on delay, 0 = off delay
    BYTE START;///start command: must be on to count
    //BYTE RESET;///down command: sets V = 0
    char nick[NICKLEN];
} * dt_t;

/**
 * @brief The blink struct
 * struct which represents a blinker
 */
typedef struct blink{
    BYTE Q; ///output
    long S;	///scale; S=1000=>toggle every 1000 cycles. STEP= 10 msec=> toggle every 10 sec
    long sn;///internal counter for scaling
    char nick[NICKLEN];
} * blink_t;

/**
 * @brief The mvar struct
 * struct which represents a memory register / counter
 */
typedef struct mvar{
    uint64_t V;     ///TODO: add type
    BYTE RO;	///1 if read only;
    BYTE DOWN;	///1: can be used as a down counter
    BYTE PULSE;		///pulse for up/downcounting
    BYTE EDGE;		///edge of pulse
    BYTE SET;		///set pulse
    BYTE RESET;		///reset pulse
    char nick[NICKLEN];   ///nickname
} * mvar_t;

/**
 * @brief The mreal struct
 * struct which represents a real number memory register
 */
typedef struct mreal{
    double V;     ///TODO: add type
    BYTE RO;	///1 if read only;
    char nick[NICKLEN];   ///nickname
} * mreal_t;

/**
 * @brief The PLC_regs struct
 * The struct which contains all the software PLC registers
 */
typedef struct PLC_regs{
    char hw[MAXSTR]; ///just a label for the hardware used
    BYTE *inputs;   ///digital input values buffer
    uint64_t *real_in; ///analog raw input values buffer
    BYTE *outputs;  ///digital output values buffer
    uint64_t *real_out; ///analog raw output values buffer
    BYTE *edgein;	///edges buffer
    BYTE *maskin;	///masks used to force values
	BYTE *maskout;
	BYTE *maskin_N;
	BYTE *maskout_N;
	double *mask_ai;
	double *mask_aq;
    BYTE command;   ///serial command from plcpipe
    BYTE response;  ///response to named pipe
    BYTE status;    ///0 = stopped, 1= running
	
	BYTE ni; ///number of bytes for digital inputs 
	di_t di; ///digital inputs
	
	BYTE nq; ///number of bytes for digital outputs
	do_t dq; ///the digital outputs

    BYTE nai; ///number of analog input channels
	aio_t ai; ///the analog inputs
	
	BYTE naq; ///number of analog output channels
	aio_t aq; ///the analog outputs
	
	BYTE nt; ///number of timers
	dt_t t; ///the timers
    
    BYTE ns; ///number of blinkers
	blink_t s; ///the blinkers
	
	BYTE nm; ///number of memory counters
	mvar_t m; ///the memory
	
	BYTE nmr; ///number of memory registers
	mreal_t mr; ///the memory

	rung_t * rungs;
	BYTE rungno; //256 rungs should suffice
	
	long step; //cycle time in milliseconds
	char * response_file; //pipe to send response.
	struct pollfd com[1];  //polling on a file descriptor for external 
	                        //"commands", this will be thrown away and  
	                        //replaced by usleep
	                        //FIXME: throw this feature away
	struct PLC_regs * old; //pointer to previous state
} * plc_t;

/**
 * @brief take the next available member in the preallocated stack
 * @param the rung where the stack belongs
 * @return the candidate stack head
 */
opcode_t take(rung_t r);

/**
 * @brief give the stack head back to the stack
 * @param the head to give
 */
void give( opcode_t head);

/**
 * @brief push an opcode and a value into rung's stack.
 * @param op the operation
 * @param t the type
 * @param val
 * @param the rung //pointer to head of stack
 * @return OK or error
 */
int push( BYTE op, 
          BYTE t, 
          const data_t val,
          rung_t r );
         
/**
 * @brief retrieve stack heads operation and operand,
 * apply it to val and return result
 * @param val
 * @param pointer to head of stack
 * @return result
 */
data_t pop( const data_t val, opcode_t *stack);

/**
 * @brief execute JMP instruction
 * @param the rung
 * @param the program counter (index of instruction in the rung)
 * @return OK or error
 */
int handle_jmp( const rung_t r, unsigned int * pc);

/**
 * @brief execute RESET instruction
 * @param the instruction
 * @param current acc value
 * @param true if we are setting a bit from a variable, 
 * false if we are setting the input of a block
 * @param reference to the plc
 * @return OK or error
 */
int handle_reset( const instruction_t op,
                  const data_t acc,
                  BYTE is_bit,    
                  plc_t p);

/**
 * @brief execute SET instruction
 * @param the instruction
 * @param current acc value
 * @param true if we are setting a bit from a variable, 
 * false if we are setting the input of a block
 * @param reference to the plc
 * @return OK or error
 */
int handle_set( const instruction_t op,
                const data_t acc, 
                BYTE is_bit,
                plc_t p);

/**
 * @brief store value to digital outputs
 * @note values are stored in BIG ENDIAN
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int st_out( const instruction_t op, 
            uint64_t val,
            plc_t p);
          
/**
 * @brief store value to analog outputs
 * @param value
 * @reference to the plc
 * @return OK or error
 */            
int st_out_r( const instruction_t op, 
              double val,
              plc_t p);

/**
 * @brief store value to memory registers
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int st_mem( const instruction_t op, 
            uint64_t val,
            plc_t p);

/**
 * @brief store value to floating point memory registers 
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int st_mem_r( const instruction_t op, 
              double val,
              plc_t p);


/**
 * @brief execute STORE instruction
 * @param the value to be stored
 * @param the instruction
 * @param reference to the plc
 * @return OK or error
 */
int handle_st( const instruction_t op, 
               const data_t val, 
               plc_t p);
                
/**
 * @brief load value from digital inputs
 * values are loaded in BIG ENDIAN
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int ld_in( const instruction_t op, 
           uint64_t * val,
           plc_t p);
            
/**
 * @brief load rising edge from digital inputs
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int ld_re( const instruction_t op, 
           BYTE * val,
           plc_t p);
            
/**
 * @brief load falling edge from digital inputs
 * @param value
 * @reference to the plc
 * @return OK or error
 */            
int ld_fe( const instruction_t op, 
           BYTE * val,
           plc_t p);
            
/**
 * @brief load value from analog inputs
 * @param value
 * @reference to the plc
 * @return OK or error
 */            
int ld_in_r( const instruction_t op, 
             double * val,
             plc_t p);
            
/**
 * @brief load value from digital outputs
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int ld_out( const instruction_t op, 
            uint64_t * val,
            plc_t p);
          
/**
 * @brief load value from analog outputs
 * @param value
 * @reference to the plc
 * @return OK or error
 */            
int ld_out_r( const instruction_t op, 
              double * val,
              plc_t p);

/**
 * @brief load value from memory registers
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int ld_mem( const instruction_t op, 
            uint64_t * val,
            plc_t p);

/**
 * @brief load value from floating point memory 
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int ld_mem_r( const instruction_t op, 
              double * val,
              plc_t p);
            
/**
 * @brief load value from timer 
 * @param value
 * @reference to the plc
 * @return OK or error
 */
int ld_timer( const instruction_t op, 
              uint64_t * val,
              plc_t p);            

/**
 * @brief execute LOAD instruction
 * @param the instruction
 * @param where to load the value
 * @param reference to the plc
 * @return OK or error
 */
int handle_ld( const instruction_t op, 
               data_t * acc, 
               plc_t p);

/**
 * @brief execute any stack operation
 * @param the intruction
 * @param the rung
 * @param reference to the plc
 * @return OK or error
 */
int handle_stackable(const instruction_t op,
                     rung_t r, 
                     plc_t p);

/**
 * @brief get type of instruction
 * @convention type is encoded in the instruction
 * the length is given by the bit part, and the scalar / real is defined by 
 * the operand (analog / digital)
 * @param instruction
 * @return the type, or error
 */
int get_type(const instruction_t ins);

/**
 * @brief execute IL instruction
 * @param the plc
 * @param the rung
 * @return OK or error
 */
int instruct(plc_t p, rung_t r, unsigned int *pc);

uint64_t operate_u(BYTE op, uint64_t a, uint64_t b);

double operate_d(BYTE op, double a, double b);
float operate_f(BYTE op, float a, float b);

/**
 * @brief operate operator op of type t on data a and b
 * @param operator
 * @param type
 * @param a
 * @param b
 * @return result if available
 */
data_t operate( BYTE op, 
                       BYTE t, 
                       const data_t a, 
                       const data_t b);

/**
  *@brief deepcopy instructions
  *@param from
  *@param to
  */ 
void deepcopy(const instruction_t from, instruction_t to);
    

/**
 * @brief get instruction reference from rung
 * @param r a rung AKA instructions list
 * @param i the instruction reference
 * @param idx the index
 * @return OK or error
 */
int get( const rung_t r, 
         unsigned int idx, 
         instruction_t * i);

/**
 * @brief append instruction to rung
 * @param i a new instruction
 * @param r a rung AKA instructions list
 * @return OK or error
 */
int append( const instruction_t i, rung_t r);

/**
 * @brief clear rung from instructions and free memory
 * @param r a rung AKA instructions list
 */
void clear_rung( rung_t r);

/**
 * @brief lookup instruction by label
 * @param label
 * @param r a rung AKA instructions list
 * @return the index (pc) of the instruction, or error if not found
 */
int lookup( const char * label, rung_t r);

/**
 * @brief intern  labels
 * for each lookup command (JMP etc.) the lookup label is 
 * looked up and if found the resulting index stored to the modifier
 * @param r a rung AKA instructions list
 * @return OK, or error if : a label is not found or found duplicate
 */
int intern( rung_t r);

/**
 * @brief add a new rung to a plc
 * @param the plc
 * @return reference to the new rung
 */
rung_t mk_rung( plc_t p);

/**
 * @brief get rung reference from plc
 * @param p a plc
 * @param r the rung reference
 * @param idx the index
 * @return OK or error
 */
int get_rung( const plc_t p, unsigned int idx, rung_t *r);

/**
 * @brief task to execute IL rung
 * @param timeout (usec)
 * @param pointer to PLC registers
 * @param pointer to IL rung
 * @return OK or error
 */
int task( long timeout, plc_t p, rung_t r);

/**
 * @brief task to execute all rungs
 * @param timeout (usec)
 * @param pointer to PLC registers
 * @return OK or error
 */
int all_tasks( long timeout, plc_t p);

/**
 * @brief custom project init code as plugin
 * @return OK or error
 */
int project_init();

/**
 * @brief custom project task code as plugin
 * @param pointer to PLC registers
 * @return OK or error
 */
int project_task( plc_t p);

/**
 * @brief PLC task executed in a loop
 * @param pointer to PLC registers
 * @return OK or error code
 */
int plc_task( plc_t p);

/**
 * @brief Pipe initialization executed once
 * @param pipe to poll
 * @param ref to plc
 * @return OK or error
 */
int open_pipe(const char * pipe, plc_t p);

/**
 * @brief PLC realtime loop
 * Anything in this function normally (ie. when not in error)
 * satisfies the realtime @conditions:
 * 1. No disk I/O
 * 2. No mallocs
 * This way the time it takes to execute is predictable
 * Heavy parts can timeout
 * The timing is based on poll.h
 * which is also realtime when using a preempt scheduler
 * @param true if anything got updated in the last cycle
 * @param the PLC
 * @return OK or error
 */
int plc_func( BYTE *update, plc_t p);

/**
 * @brief is input forced
 * @param reference to plc
 * @param input index
 * @return true if forced, false if not, error if out of bounds
 */
int is_input_forced(const plc_t p, BYTE i);

/**
 * @brief is output forced
 * @param reference to plc
 * @param input index
 * @return true if forced, false if not, error if out of bounds
 */
int is_output_forced(const plc_t p, BYTE i);


/**
 * @brief decode inputs
 * @param pointer to PLC registers
 * @return true if input changed
 */
unsigned char dec_inp( plc_t p);

/**
 * @brief encode outputs
 * @param pointer to PLC registers
 * @return true if output changed
 */
unsigned char enc_out( plc_t p);

/**
 * @brief write values to memory variables
 * @param pointer to PLC registers
 */
void write_mvars( plc_t p);

/**
 * @brief read_mvars
 * @param pointer to PLC registers
 */
void read_mvars( plc_t p);

/**
 * @brief rising edge of input
 * @param pointer to PLC registers
 * @param type is Digital Input Or Counter
 * @param index
 * @return true if rising edge of operand, false or error otherwise
 */
int re( const plc_t p, 
        int type, 
        int idx);

/**
 * @brief falling edge of input
 * @param pointer to PLC registers
 * @param type is Digital Input Or Counter
 * @param index
 * @return true if falling edge of operand, false or error otherwise
 */
int fe( const plc_t p, 
        int type, 
        int idx);

/**
 * @brief set output
 * @param pointer to PLC registers
 * @param type is Digital Output, Timer or Counter
 * @param index
 * @return OK if success or error code
 */
int set( plc_t p, 
         int type, 
         int idx);

/**
 * @brief reset output
 * @param pointer to PLC registers
 * @param type is Digital Output, Timer or Counter
 * @param index
 * @return OK if success or error code
 */
int reset( plc_t p, 
           int type,
           int idx);

/**
 * @brief contact value to output
 * @param pointer to PLC registers
 * @param type is Digital Output, Timer or Counter
 * @param index
 * @param value
 * @return OK if success or error code
 */
int contact( plc_t p, 
             int type, 
             int idx, 
             BYTE val);

/**
 * @brief resolve an operand value
 * @param pointer to PLC registers
 * @param type of operand
 * @param index
 * @return return value or error code
 */
int resolve( plc_t p, 
             int type, 
             int idx);

/**
 * @brief reset timer
 * @param pointer to PLC registers
 * @param index
 * @return OK if success or error code
 */
int down_timer( plc_t p, int idx);

/*******************debugging tools****************/
void dump_label( char * label, char * dump);
void dump_instruction( instruction_t ins, char * dump);
void dump_rung( rung_t ins, char * dump);
void compute_variance( double x);
void get_variance( double * mean, double * var);
unsigned long get_loop();

#endif //_PLCLIB_H_
