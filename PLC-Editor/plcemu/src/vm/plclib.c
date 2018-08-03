#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "plclib.h"
#include "plcemu.h"
#include "util.h"
#include "config.h"
#include "../hw/hardware.h"
#include "project.h"


const char * LibErrors[N_IE] = 
{
    "Unknown error",
    "Invalid operator",
    "Invalid output",
    "Invalid numeric index",
    "Invalid operand",
    "File does not exist",
    "Unreadable character"
};

const char IlCommands[N_IL_INSN][LABELLEN] =
{ 		"",
		")",
		"RET",
		"JMP",
		"CAL",
		"S",
		"R",
		"LD",
		"ST",
		"AND",
		"OR",
		"XOR",
		"ADD",
		"SUB",
		"MUL",
		"DIV",
		"GT",
		"GE",
		"EQ",
		"NE",
		"LT",
		"LE"
};

const char IlOperands[N_OPERANDS][3] =
{ 		"i",
        "if",
		"f",
		"r",
		"m",
		"mf",
		"c",
		"b",
		"t",
		"q",
		"qf",
		"Q",
		"QF",
		"T",
		"M",
		"MF",
		"W",
		"",
};

const char IlModifiers[N_IL_MODIFIERS][2] =
{
    "!",
    "(",
    " ",
    "?",
};


struct timeval Curtime;

int open_pipe(const char * pipe, plc_t p)
{
    p->com[0].fd = open(pipe, O_NONBLOCK | O_RDONLY);
    p->com[0].events = POLLIN | POLLPRI;
    int r = p->com[0].fd > 0? PLC_OK: PLC_ERR;

    gettimeofday(&Curtime, NULL);
    
    return r;
}

int re(const plc_t p,  int type,  int idx)
{ //return rising edge of operand

	switch (type){
    case BOOL_DI:
		return (p->di[idx].RE);
		break;
    case BOOL_COUNTER:
		return (p->m[idx].PULSE) && (p->m[idx].EDGE);
		break;
	default:
        return PLC_ERR;
	}
}

int fe(const plc_t p,  int type,  int idx)
{ //return falling edge of operand

	switch (type){
    case BOOL_DI:
		return (p->di[idx].FE);
		break;
    case BOOL_COUNTER:
		return (!p->m[idx].PULSE) && (p->m[idx].EDGE);
		break;
	default:
        return PLC_ERR;
	}
}

int set(plc_t p,  int type,  int idx)
{ //set operand
	switch (type){
    case BOOL_DQ:
        if(idx / BYTESIZE >= p->nq)
            return ERR_BADOPERAND;
		p->dq[idx].SET = TRUE;
		p->dq[idx].RESET = FALSE;
		break;
    case BOOL_COUNTER:
        if(idx >= p->nm)
            return ERR_BADOPERAND;
		p->m[idx].SET = TRUE;
		p->m[idx].RESET = FALSE;
		if (!p->m[idx].PULSE)
			p->m[idx].EDGE = TRUE;
		break;
    case BOOL_TIMER:
        if(idx >= p->nt)
            return ERR_BADOPERAND;
		p->t[idx].START = TRUE;
		break;
	default:
        return PLC_ERR;
	}
    return 0;
}

int reset(plc_t p,  int type, int idx)
{ //reset operand
	switch (type){
    case BOOL_DQ:
        if(idx / BYTESIZE >= p->nq)
            return ERR_BADOPERAND;
            
		p->dq[idx].RESET = TRUE;
		p->dq[idx].SET = FALSE;
		break;
    case BOOL_COUNTER:
        if(idx >= p->nm)
            return ERR_BADOPERAND;
            
		p->m[idx].RESET = TRUE;
		p->m[idx].SET = FALSE;
		if (p->m[idx].PULSE)
			p->m[idx].EDGE = TRUE;
		break;
    case BOOL_TIMER:
        if(idx >= p->nt)
            return ERR_BADOPERAND;
            
		p->t[idx].START = FALSE;
		break;
	default:
        return PLC_ERR;
	}
    return 0;
}

int contact(plc_t p, 
             int type, 
             int idx, 
             BYTE val)
{ //contacts an output with a value
	switch (type){
    case BOOL_DQ:
        if(idx / BYTESIZE >= p->nq)
            return ERR_BADOPERAND;
            
		p->dq[idx].Q = val;
		break;
    case BOOL_COUNTER:
        if(idx >= p->nm)
            return ERR_BADOPERAND;
            
		if (p->m[idx].PULSE != val)
			p->m[idx].EDGE = TRUE;
		else
			p->m[idx].EDGE = FALSE;
		p->m[idx].PULSE = val;
		break;
    case BOOL_TIMER:
        if(idx >= p->nt)
            return ERR_BADOPERAND;
            
		p->t[idx].START = TRUE;
		break;
	default:
        return PLC_ERR;
	}
    return 0;
}

int resolve(plc_t p,  int type,  int idx)
{ //return an operand value
	switch (type){
    case BOOL_DQ:
        return p->dq[idx].Q 
           || (p->dq[idx].SET && !p->dq[idx].RESET);
           
    case BOOL_COUNTER:
		return p->m[idx].PULSE;
		
    case BOOL_DI:
		return p->di[idx].I;
		
    case BOOL_BLINKER:
		return p->s[idx].Q;
		
    case BOOL_TIMER:
		return p->t[idx].Q;
		
	default:
        return PLC_ERR;
	}
}

int down_timer(plc_t p,  int idx)
{ //RESET timer
	p->t[idx].START = FALSE;
	p->t[idx].V = 0;
	p->t[idx].Q = 0;
    return 0;
}

int task(long timeout, plc_t p, rung_t r)
{
    unsigned int i = 0;
    unsigned int pc = 0;
    struct timeval start;
    struct timeval lapse;
    
    gettimeofday(&start,NULL);

    long delta = 0;
    
    if(r==NULL
    || p==NULL)
        return PLC_ERR;
     
    int rv = 0;    
	while(rv >= PLC_OK && i < r->insno){
	    if(delta >= timeout){
	        rv = ERR_TIMEOUT;
	        break;
	    }    
	    pc = i;
		rv = instruct(p, r, &pc);
		if (rv < PLC_OK){
		    switch(rv){
                case PLC_ERR:
                    plc_log("Instruction %d :%s", i, 
                            LibErrors[IE_PLC]);
                    break;
                case ERR_BADOPERATOR:
                    plc_log("Instruction %d :%s", i, 
                            LibErrors[IE_BADOPERATOR]);
                    break;
                case ERR_BADCOIL:
                    plc_log("Instruction %d :%s", i, 
                            LibErrors[IE_BADCOIL]);
                    break;
                case ERR_BADINDEX:
                    plc_log("Instruction %d :%s", i, 
                            LibErrors[IE_BADINDEX]);
                    break;
                case ERR_BADOPERAND:
                    plc_log("Instruction %d :%s", i, 
                            LibErrors[IE_BADOPERAND]);
                    break;
                case ERR_BADFILE:
                    plc_log("Instruction %d :%s", i, 
                             LibErrors[IE_BADFILE]);
                    break;
                case ERR_BADCHAR:
                    plc_log("Instruction %d :%s", i, 
                             LibErrors[IE_BADCHAR]);
                    break;    
                default: break;
            }
		}
	    gettimeofday(&lapse,NULL);
        delta = lapse.tv_usec - start.tv_usec;
		//plc_log("Instruction %d : OK", i); 
		i = pc;
	}
	return rv;
}

int all_tasks(long timeout, plc_t p)
{
    int i = 0;
    int rv = PLC_OK;
    if(p==NULL)
        return PLC_ERR;
    
    for(;i < p->rungno; i++)
        rv = task(timeout, p, p->rungs[i]);
    
    return rv;
}

/******************************STACK****************************************/
data_t negate(BYTE *op, data_t b)
{
      data_t r = b;
      if (*op & NEGATE){	    //negate b
		*op -= NEGATE;
		r.u = -1 - (b.u);
		//magic?
	}
	return r;
}


uint64_t operate_u(BYTE op, uint64_t a, uint64_t b)
{
    uint64_t r=0;
	
    switch (op){
	//boolean or bitwise, all modifiers,
	case IL_AND:	//AND
		r = a & b;
		break;

	case IL_OR:	//OR
		r = a | b;
		break;

	case IL_XOR:	//XOR
		r = a ^ b;
		break;
	//arithmetic	
	case IL_ADD:
			r = a + b;
		break;
		
	case IL_SUB:
			r = a - b;
	    break;
	    
	case IL_MUL:
		r = (a * b) ;
		break;
		
	case IL_DIV:
		r = b != 0 ? a / b : - 1;
		break;
		
	//comparison
	case IL_GT:
		r = (a > b);
		break;
		
	case IL_GE:
		r = (a >= b);
		break;
		
	case IL_EQ:
		r = (a == b);
		break;
		
	case IL_NE:
		r = (a != b);
		break;
		
	case IL_LT:
		r = (a < b);
		break;
		
	case IL_LE:
		r = (a <= b);
		break;
		
	default:
		break;
	}
    return r;
}

double operate_d(BYTE op, double a, double b)
{
    double r=0;
    switch (op){
    //arithmetic	
	case IL_ADD:
			r = a + b;
		break;
		
	case IL_SUB:
			r = a - b;
	    break;
	    
	case IL_MUL:
		r = (a * b) ;
		break;
		
	case IL_DIV:
		r = b != 0 ? a / b : - 1;
		break;
		
	//comparison
	case IL_GT:
		r = (a > b);
		break;
		
	case IL_GE:
		r = (a >= b);
		break;
		
	case IL_EQ:
		r = (a == b);
		break;
		
	case IL_NE:
		r = (a != b);
		break;
		
	case IL_LT:
		r = (a < b);
		break;
		
	case IL_LE:
		r = (a <= b);
		break;
		
	default:
		break;
	}
    return r;
}

data_t operate( BYTE op, 
                BYTE type, 
                const data_t a, 
                const data_t b )
{
    data_t r;	    //return value
    data_t n = negate(&op, b);
    uint64_t modulo = 1;
	switch(type)
	{
	    case T_REAL:
	        r.r = operate_d(op, a.r, n.r ) ;
	        break;
	    
	    case T_BOOL: 
	        r.u = BOOL(operate_u(op, BOOL(a.u), BOOL(n.u)));
	        break;
	    
	    case T_BYTE:
	        modulo = 1 << BYTESIZE;
	        //r.u = operate_u(op, a.u % 0x100, bu % 0x100) % 0x100;
	        r.u = operate_u(op, a.u % modulo, n.u % modulo) % modulo;
	        break;
	    
	    case T_WORD:
	        modulo = 1 << (2*BYTESIZE);
	        r.u = operate_u(op, a.u % modulo, n.u % modulo) % modulo;
	        break;
	    
	    case T_DWORD:
	        modulo = 0x100000000;
	        r.u = operate_u(op, a.u % modulo, n.u % modulo) % modulo;
	        break;
	   
	    default://64bit uint
	        r.u = operate_u(op, a.u , n.u ) ;
	    break;
	}
	return r;
}

opcode_t take( rung_t r )
{
    if(r->stack == NULL)
        return &(r->prealloc[0]);
    else if(r->stack->depth < MAXBUF - 1)
        return &(r->prealloc[r->stack->depth]);
    else return NULL; 
}

void give( opcode_t head )
{
    memset(head, 0, sizeof(struct opcode));
}

int push( BYTE op,  
          BYTE t,  
          const data_t val, 
          rung_t r )
{//push an opcode and a value into stack.
	struct opcode * p = take(r);
	if(!p)
	    return ERR_OVFLOW;
	//initialize
	p->operation = op;
	p->value = val;
	p->type = t;
	p->next = r->stack;//*stack;
	p->depth = (r->stack == NULL)?1:r->stack->depth + 1;
	//set stack head pointer to point at it
	r->stack = p;
	return PLC_OK;
}

data_t pop( const data_t val, opcode_t *stack )
{//retrieve stack heads operation and operand, apply it to val and return result
	data_t r = val; //return value
	opcode_t p;
	if (*stack != NULL) {
	//safety
		r = operate((*stack)->operation,
		            (*stack)->type,
		            (*stack)->value, 
		            val );//execute instruction
		p = *stack;
		*stack = (*stack)->next;
		//set stack head to point to next opcode in stack
		give(p);
	}
	return r;
}

/*****************************VM*******************************************/

int handle_jmp( const rung_t r, unsigned int * pc)
{
    if(r==NULL
    || pc==NULL)
        return PLC_ERR;
        
    instruction_t op;
    if(get(r, *pc, &op) < PLC_OK)
        return ERR_BADOPERAND;
      
    if(op->operation != IL_JMP)
        return ERR_BADOPERATOR; //sanity
    
    if(!(op->modifier==IL_COND
    && r->acc.u == 0))
        *pc = op->operand;
    else 
        (*pc)++;
    return PLC_OK;
}

int handle_set( const instruction_t op,
                const data_t acc,
                BYTE is_bit,                 
                plc_t p )
{
    int r = PLC_OK;
    if(op==NULL
    || p==NULL)
        return PLC_ERR;
        
    if(op->operation != IL_SET)
        return ERR_BADOPERATOR; //sanity
        
    if(op->modifier == IL_COND
    && acc.u == FALSE)
        return r;
            
    switch (op->operand){
        case OP_CONTACT:	//set output %QX.Y
            if(!is_bit) //only gets called when bit is defined
                r = ERR_BADOPERAND;
            else 
                r = set(p, 
                        BOOL_DQ, 
                        (op->byte) * BYTESIZE + op->bit);
            break;
            
        case OP_START:	//bits are irrelevant
                r = set(p, BOOL_TIMER, op->byte);
            break;
            
        case OP_PULSEIN:	//same here
                r = set(p, BOOL_COUNTER, op->byte);
            break;
            
        default:
            r = ERR_BADOPERAND;
    }
    return r;
}

int handle_reset(const instruction_t op, 
                 const data_t acc,
                 BYTE is_bit,
                 plc_t p)
{
    int r = PLC_OK;
    if(op==NULL
    || p==NULL)
        return PLC_ERR;
    
    if(op->operation != IL_RESET)
        return ERR_BADOPERATOR; //sanity
    
    if(op->modifier == IL_COND
    && acc.u == FALSE)
        return r;
        
    switch (op->operand){
        case OP_CONTACT:	//set output %QX.Y
            if (!is_bit)	//only gets called when bit is defined
                r = ERR_BADOPERAND;
            else
                r = reset(p, BOOL_DQ, (op->byte) * BYTESIZE + op->bit);
            break;
            
        case OP_START:	//bits are irrelevant
                r = reset(p, BOOL_TIMER, op->byte);
            break;
            
        case OP_PULSEIN:	//same here
                r = reset(p, BOOL_COUNTER, op->byte);
            break;
            
        default:
            r = ERR_BADOPERAND;
    }
    return r;
}

int st_out_r( const instruction_t op, 
              double val,
              plc_t p)
{
    if(op->byte >= p->naq)
        return ERR_BADOPERAND;
    BYTE i = op->byte;
    p->aq[i].V = val;    
    return PLC_OK;
}

int st_out( const instruction_t op, 
            uint64_t val,
            plc_t p)
{
    int r = PLC_OK;
    int t = get_type(op);
    BYTE offs = (op->bit / BYTESIZE) - 1;
    int i = 0;
    switch(t){
        case T_BOOL:
            if (op->modifier == IL_NEG)
                val = TRUE - BOOL(val);
            if(op->byte >= p->nq)
                r = ERR_BADOPERAND; 
            else    
                r = contact(p, 
                        BOOL_DQ, 
                        (op->byte) * BYTESIZE + op->bit,
                        BOOL(val));
            break;
            
        case T_BYTE:
        case T_WORD:
        case T_DWORD:
        case T_LWORD:
            if (op->modifier == IL_NEG)
                val = - val;
            if(op->byte + offs >= p->nq)
                r = ERR_BADOPERAND; 
            else for(;i<=offs;i++){
                    p->outputs[op->byte + i] = 
                    (val >> ((offs-i)*BYTESIZE)) % (0x100);    
                }
            
            break;    
                
        default: 
            r = ERR_BADOPERAND;
    }
    return r;    
}

int st_mem_r( const instruction_t op, 
              double val,
              plc_t p)
{
    if(op->byte >= p->nmr)
                return ERR_BADOPERAND; 
    p->mr[op->byte].V = val;
   // plc_log("store %lf to m%d", val, op->byte);
    return PLC_OK; 
}
              
int st_mem( const instruction_t op, 
            uint64_t val,
            plc_t p)
{
    int r = PLC_OK;
    int t = get_type(op);
    BYTE offs = (op->bit / BYTESIZE) - 1;
    uint64_t compl = 0x100;
 
    if(op->byte >= p->nm)
                return ERR_BADOPERAND; 
    switch(t){
        case T_BOOL:
             r = contact(p, 
                         BOOL_COUNTER, 
                         op->byte, 
                         BOOL(val));
             break;
             
        case T_BYTE:
        case T_WORD:
        case T_DWORD:
        case T_LWORD:
             p->m[op->byte].V = 
                 val & ((compl << (BYTESIZE * offs))-1);
            // plc_log("store 0x%lx to m%d", val, op->byte);
             break;           
                 
        default: 
            return ERR_BADOPERAND;
    }
    return r;    
}

int handle_st(  const instruction_t op, 
                const data_t acc, 
                plc_t p)
{
    int r = PLC_OK;
    data_t val = acc;
    if(op==NULL
    || p==NULL)
        return PLC_ERR;
        
    if(op->operation != IL_ST)
        return ERR_BADOPERATOR; //sanity
     
    switch (op->operand){
        case OP_REAL_CONTACT:	    //set output %QX.Y
            r = st_out_r(op, val.r, p);
            break;
        
        case OP_CONTACT:	    //set output %QX.Y
            r = st_out(op, val.u, p);
            break;
            
        case OP_START:	    //bits are irrelevant
            r = contact(p, BOOL_TIMER, op->byte, val.u % 2);
            break;
            
        case OP_REAL_MEMIN:
            r = st_mem_r(op, val.r, p);
            break;     
            
        case OP_PULSEIN:
            r = st_mem(op, val.u, p);
            break;
            
        case OP_WRITE:
            p->command = val.u;
            break;
            
        default:
            r = ERR_BADOPERAND;
    }
    return r;
}

uint64_t ld_bytes(BYTE start, 
                  BYTE offset, 
                  BYTE * arr)
{
    uint64_t rv = 0;
    int i = offset;
    for(; i >=0; i--){
        uint64_t u = arr[start + i]; 
        rv +=  u << (BYTESIZE*(offset -i)) ;
    }
    return rv;
}                

int ld_in( const instruction_t op, 
           uint64_t * val,
           plc_t p)
{
    int r = PLC_OK;
    int t = get_type(op);
    BYTE offs = (op->bit / BYTESIZE) - 1;
    uint64_t complement = 0x100;
    
    switch(t){
        case T_BOOL:
            if(op->byte >= p->ni)
                return ERR_BADOPERAND;
            *val = resolve(p, BOOL_DI, 
                            (op->byte) * BYTESIZE + op->bit);
            if (op->modifier == IL_NEG)
                *val = *val?FALSE:TRUE;      
            break;
            
        case T_BYTE:
        case T_WORD:
        case T_DWORD:
        case T_LWORD:
            if(op->byte + offs >= p->ni)
                return ERR_BADOPERAND;
                
            *val = ld_bytes(op->byte, offs, p->inputs);    
            
            if(op->modifier == IL_NEG)
                *val = (complement << offs*BYTESIZE) - *val;  
            break;
      
        default: 
            return ERR_BADOPERAND;
    }
    return r;    
}
            
int ld_re( const instruction_t op, 
           BYTE * val,
           plc_t p )
{
    int r = PLC_OK;
    int t = get_type(op);
    if(op->byte >= p->ni)
                return ERR_BADOPERAND;
    if(t == T_BOOL)
            *val = re(p, BOOL_DI, (op->byte) * BYTESIZE + op->bit);
    else
            r = ERR_BADOPERAND;
    return r;    
}
                   
            
int ld_fe( const instruction_t op, 
           BYTE * val,
           plc_t p)
{
    int r = PLC_OK;
    int t = get_type(op);
    if(op->byte >= p->ni)
                return ERR_BADOPERAND;
    if(t == T_BOOL)
            *val = fe(p, BOOL_DI, 
                      (op->byte) * BYTESIZE + op->bit);
    else 
            r = ERR_BADOPERAND;
    return r;    
}              


int ld_in_r( const instruction_t op, 
             double * val,
             plc_t p)
{
    if(op->byte >= p->nai)
                return ERR_BADOPERAND;
    BYTE i = op->byte;
    *val = p->ai[i].V;
    return PLC_OK;    
}
            

int ld_out_r( const instruction_t op, 
            double * val,
            plc_t p)
{
    if(op->byte >= p->naq)
                return ERR_BADOPERAND;
    BYTE i = op->byte;
    *val = p->aq[i].V;
    return PLC_OK;    
}            
            
int ld_out( const instruction_t op, 
            uint64_t * val,
            plc_t p)
{
    int r = PLC_OK;
    int t = get_type(op);
    BYTE offs = (op->bit / BYTESIZE) - 1;
    uint64_t complement = 0x100;
    switch(t){
        case T_BOOL:
             if(op->byte >= p->nq)
                return ERR_BADOPERAND;
            *val = resolve(p, BOOL_DQ, 
                           (op->byte) * BYTESIZE + op->bit);
            if (op->modifier == IL_NEG)
                *val = *val?FALSE:TRUE;      
            break;
            
        case T_BYTE:
        case T_WORD:
        case T_DWORD:
        case T_LWORD:
            if(op->byte + offs >= p->nq)
                return ERR_BADOPERAND;
                
            *val = ld_bytes(op->byte, offs, p->outputs);    
            
            if(op->modifier == IL_NEG)
                *val = (complement << offs*BYTESIZE) - *val;  
            break;
        
        default: 
            return ERR_BADOPERAND;
    }
    return r;    
}
            
int ld_mem( const instruction_t op, 
            uint64_t * val,
            plc_t p)
{
    int r = PLC_OK;
    int t = get_type(op);
    if(op->byte >= p->nm)
                return ERR_BADOPERAND;
    int offs = (op->bit / BYTESIZE) - 1;
    uint64_t compl = 0x100;
                
    switch(t){
        case T_BOOL:
            *val = resolve(p, BOOL_COUNTER, op->byte);
            if (op->modifier == IL_NEG)
                *val = (*val)?FALSE:TRUE;      
            break;
            
        case T_BYTE:
        case T_WORD:
        case T_DWORD:
        case T_LWORD:
            *val = p->m[op->byte].V & ((compl << offs*BYTESIZE) - 1);
            
            if( op->modifier == IL_NEG )
                *val = (compl << offs*BYTESIZE) - *val;
            break;
                
        default: 
            return ERR_BADOPERAND;
    } 
    return r;    
}

int ld_mem_r( const instruction_t op, 
              double * val,
              plc_t p)
{
    if(op->byte >= p->nmr)
                return ERR_BADOPERAND;
                
    *val = p->mr[op->byte].V;
     if( op->modifier == IL_NEG )
                *val = - *val;
                
    return PLC_OK;   
}

int ld_timer( const instruction_t op, 
                uint64_t * val,
                plc_t p)
{
    int r = PLC_OK;
    int t = get_type(op);
    int offs = (op->bit / BYTESIZE) - 1;
    uint64_t compl = 0x100;
   
/*a convention: bit is irrelevant, 
  but defining it means we are referring to t.Q, otherwise t.V
*/
    if(op->byte >= p->nt)
                return ERR_BADOPERAND;
    switch(t){
        case T_BOOL:
            *val = resolve(p, BOOL_TIMER, op->byte);
            if (op->modifier == IL_NEG)
                *val = *val?FALSE:TRUE;      
     
            break;
            
        case T_BYTE:
        case T_WORD:
        case T_DWORD:
        case T_LWORD:
            *val = p->t[op->byte].V & ((compl << offs*BYTESIZE) - 1);
            
            if( op->modifier == IL_NEG )
                *val = (compl << offs*BYTESIZE) - *val;
            break;
                
        default: 
            r = ERR_BADOPERAND;
    }
    return r;    
}
            
int handle_ld(  const instruction_t op, 
                data_t * acc, 
                plc_t p)
{
    int r = 0;
    BYTE edge = 0; 
    if(op==NULL
    || p==NULL
    || acc==NULL)
        return PLC_ERR;
    
    if((op->operation != IL_LD 
    && op->operation < FIRST_BITWISE)  
    || op->operation >= N_IL_INSN)
        return ERR_BADOPERATOR; //sanity
        
    switch (op->operand){
        case OP_OUTPUT:	//set output %QX.Y
            r = ld_out(op, &(acc->u), p);
            break; 
            
        case OP_INPUT:	//load input %IX.Y
            r = ld_in(op, &(acc->u), p);
            break;
        
        case OP_REAL_OUTPUT:	//set output %QX.Y
            r = ld_out_r(op, &(acc->r), p);
            break; 
            
        case OP_REAL_INPUT:	//load input %IX.Y
            r = ld_in_r(op, &(acc->r), p);
            break;
            
        case OP_MEMORY:
            r = ld_mem(op, &(acc->u), p);
            break;
            
        case OP_REAL_MEMORY:
            r = ld_mem_r(op, &(acc->r), p);
            break;    
            
        case OP_TIMEOUT:
            r = ld_timer(op, &(acc->u), p);
            break;
            
        case OP_BLINKOUT:	//bit is irrelevant
            if(op->byte >= p->ns)
                return ERR_BADOPERAND;
            acc->u = resolve(p, BOOL_BLINKER, op->byte);
            break;
            
        case OP_COMMAND:
            acc->u = p->command;
            break;
            
        case OP_RISING:	//only boolean
            r = ld_re(op, &edge, p);
            acc->u = edge;
            break;
            
        case OP_FALLING:	//only boolean
            r = ld_fe(op, &edge, p);
            acc->u = edge;
            break;
            
        default:
            r = ERR_BADOPERAND;
            break;
    }
    return r;
}

int handle_stackable(   const instruction_t op, 
                        rung_t r,  
                        plc_t p)
{//all others (stackable operations)
    int rv = 0;
    data_t val;
    val.u = 0;
    BYTE stackable = 0;
    if(r==NULL
    || p==NULL)
        return PLC_ERR;
    
    if(op->operation < FIRST_BITWISE 
    || op->operation >=  N_IL_INSN)
        return ERR_BADOPERATOR; //sanity
    
    int type = get_type(op);
    if(type == PLC_ERR)
        return ERR_BADOPERAND;
    
    struct instruction loader;
    deepcopy(op, &loader);
    loader.operation = IL_LD;
    loader.modifier = IL_NORM;
    
    stackable = op->operation;
    
    if (op->modifier == IL_NEG)
        stackable += NEGATE;
        
    if (op->modifier == IL_PUSH){
        push(stackable, type, r->acc, r); 
        rv = handle_ld(&loader,  &(r->acc), p);	   
    }
    else{    
        rv = handle_ld( &loader, &val, p);
        r->acc = operate(stackable, type, r->acc, val);
    }
    return rv;
}

int get_type(const instruction_t ins)
{
    int rv = PLC_ERR;
    
    if(ins != NULL 
    && OP_VALID(ins->operand)){
        BYTE x = ins->bit;
        if(OP_REAL(ins->operand))
             rv = T_REAL;
        else{
            switch(x){
                case BYTESIZE:
                    rv = T_BYTE;
                    break;
                    
                case WORDSIZE:
                    rv = T_WORD;
                    break;
                    
                case DWORDSIZE:
                    rv = T_DWORD;
                    break;
                    
                case LWORDSIZE:
                    rv = T_LWORD;
                    break;
                    
                default:
                    if(0 <= x
                    && x < BYTESIZE)
                        rv = T_BOOL; 
            }
        }
    }
    return rv;
}

int instruct(plc_t p, rung_t r, unsigned int *pc)
{
    BYTE type = 0;
    int error = 0;
    instruction_t op;
    BYTE increment = TRUE;
    if(r==NULL
    || p==NULL
    || *pc >= r->insno){   
        (*pc)++;
        return PLC_ERR;
    }
    if(get(r, *pc , &op) < PLC_OK){   
        (*pc)++;
        return ERR_BADOPERAND;
    }
    
    type = get_type(op);
    if(type == PLC_ERR)
        return ERR_BADOPERAND;
    
    /*char dump[MAXSTR] = "";
    dump_instruction(op, dump);
    plc_log("%d.%s", *pc, dump);
    */
    
	switch (op->operation){
//IL OPCODES: no operand
	case IL_POP: //POP
		r->acc = pop(r->acc, &(r->stack));
		break;
	case IL_NOP: 
//null operation	
	case IL_CAL: 
//subroutine call (unimplemented) retrieve subroutine line, set pc
	case IL_RET: 
//unimplemented yet: retrieve  previous program , counter, set pc
		break;
//arithmetic LABEL
	case IL_JMP:			//JMP
        error = handle_jmp(r, pc);
        increment = FALSE;
//retrieve line number from label, set pc
		break;
//boolean, no modifier, outputs.
	case IL_SET:	//S
        error = handle_set( op, 
                            r->acc,//.u % 0x100, 
                            type == T_BOOL, 
                            p);
		break;
	case IL_RESET:	//R
        error = handle_reset( op,                                 
                              r->acc,//.u % 0x100, 
                              type == T_BOOL,
                              p);
		break;
    case IL_LD:	//LD
        error = handle_ld( op, &(r->acc), p);
		break;
	case IL_ST:	//ST: output
		//if negate, negate acc
        error = handle_st( op, r->acc, p);
//any operand, only push
		break;
    default:
        error = handle_stackable( op,  r,  p);
	}
	if(increment == TRUE)
	    (*pc)++;
    return error;
}

/**********************************rung*************************************/
void deepcopy(const instruction_t from, instruction_t to)
{
    //deepcopy        
        to->operation = from->operation;
        to->operand = from->operand;
        to->modifier = from->modifier;
        to->byte = from->byte;
        to->bit = from->bit;
        if(from->label != NULL)
            strcpy(to->label, from->label);
         if(from->lookup != NULL)
            strcpy(to->lookup, from->lookup);
}

int get(const rung_t r, const unsigned int idx, instruction_t *i)
{
    if(r==NULL
    || idx >= r->insno)
        return PLC_ERR;
    *i = r->instructions[idx];       
    return PLC_OK;
}

int append(const instruction_t i, rung_t r)
{
    if(r==NULL || r->insno == MAXBUF)
         return PLC_ERR;
    if(i!=NULL){
        if(r->instructions == NULL){//lazy allocation
            r->instructions = 
                (instruction_t *)malloc(MAXBUF*sizeof(instruction_t));
            memset(r->instructions, 0, MAXBUF*sizeof(instruction_t));
        }
        if(lookup(i->label, r) >=0)
            return PLC_ERR; //dont allow duplicate labels
            
        instruction_t ins = (instruction_t)malloc(sizeof(struct instruction));
        memset(ins, 0, sizeof(struct instruction));
        deepcopy(i, ins);
            
        r->instructions[(r->insno)++] = ins;
    }
    return PLC_OK;
}

void clear_rung(rung_t r)
{ 
    int i = 0;
    if( r!=NULL
    &&  r->instructions != NULL){
        for(;i<MAXBUF;i++){
            if(r->instructions[i]!=NULL)
                free(r->instructions[i]);
        }
        free(r->instructions);
        r->instructions = NULL;
        r->insno = 0;
        //TODO: also free rung, return null
    }      
}

int lookup(const char * label, rung_t r)
{
    int ret = PLC_ERR;
    if (label == NULL
    || r == NULL)
        return ret;
    
    int i = 0;
    instruction_t ins = NULL;
    for(; i < r->insno; i++){
        get(r, i, &ins);
        if(strlen(ins->label) > 0
        && strcmp(ins->label, label) == 0){
            ret = i;
            break;
        }
    }
    return ret;    
}

int intern(rung_t r)
{
    if (r == NULL)
        return PLC_ERR;
    
    int i = 0;
    instruction_t ins = NULL;
    for(; i < r->insno; i++){
        get(r, i, &ins);
        if(strlen(ins->lookup) > 0){
            int l = lookup(ins->lookup, r);
            if(l < 0)
                return PLC_ERR;
            else
                ins->operand = l;
        } 
    }
    return PLC_OK;
}

rung_t mk_rung(plc_t p)
{
    rung_t r = (rung_t)malloc(sizeof(struct rung));
    memset(r, 0, sizeof(struct rung));
   
    if(p->rungs == NULL){//lazy allocation
       p->rungs = (rung_t *)malloc(MAXRUNG*sizeof(rung_t));
       memset(p->rungs, 0, MAXRUNG*sizeof(rung_t));
    }
    p->rungs[p->rungno++] = r;
    return r;
}

int get_rung(const plc_t p, const unsigned int idx, rung_t *r)
{
    if(p==NULL
    || idx >= p->rungno)
        return PLC_ERR;
    *r = p->rungs[idx];       
    return PLC_OK;
}

/*****************realtime loop************************************/
int timeval_subtract(struct timeval *result, 
                     struct timeval *x,
		             struct timeval *y)
{ 
/* Subtract the `struct timeval' values X and Y,
 storing the result in RESULT.
 Return 1 if the difference is negative, otherwise 0.  */
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec){
		int nsec = (y->tv_usec - x->tv_usec) / MILLION + 1;
		y->tv_usec -= MILLION * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > MILLION){
		int nsec = (x->tv_usec - y->tv_usec) / MILLION;
		y->tv_usec += MILLION * nsec;
		y->tv_sec -= nsec;
	}
	/* Compute the time remaining to wait.
	 tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;
	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

void read_inputs(plc_t p)
{
    int i=0;
    int n=0;
    int j=0;
    
    BYTE i_bit = 0;
    
    io_fetch();//for simulation
    
    for (i = 0; i < p->ni; i++){	//for each input byte
        p->inputs[i] = 0;
        for (j = 0; j < BYTESIZE; j++){	//read n bit into in
            n = i * BYTESIZE + j;
            i_bit = 0;
            dio_read(n, &i_bit);
            p->inputs[i] |= i_bit << j;
        }	//mask them
    }
    
    for (i = 0; i < p->nai; i++){	//for each input sample
        data_read(i, &p->real_in[i]);
    }
}

void write_outputs(plc_t p)
{
    int j=0;
    int n=0;
    int q_bit=0;
    
    int i=0;
    for (i = 0; i < p->nq; i++){	
        for (j = 0; j < BYTESIZE; j++){	//write n bit out
            n = BYTESIZE * i + j;
            q_bit = (p->outputs[i] >> j) % 2;
            dio_write(p->outputs, n, q_bit);
        }
    }
    for (i = 0; i < p->naq; i++){	//for each output sample
        data_write(i, p->real_out[i]);
    }
    io_flush();//for simulation
}

int is_input_forced(const plc_t p, BYTE i)
{
    if(i < p->nai)
        return p->mask_ai[i] < p->ai[i].max
            && p->mask_ai[i] > p->ai[i].min;
    return PLC_ERR;
}

int is_output_forced(const plc_t p, BYTE i)
{
    if(i < p->naq)
        return p->mask_ai[i] < p->ai[i].max
            && p->mask_ai[i] > p->ai[i].min;
        return PLC_ERR;
}


BYTE dec_inp(plc_t p)
{ //decode input bytes
	BYTE i = 0;
	BYTE j = 0;
	BYTE i_changed = FALSE;
	
	for (; i < p->ni; i++){
	    p->inputs[i] = (p->inputs[i] | p->maskin[i]) & ~p->maskin_N[i];
        
        if (p->inputs[i] != p->old->inputs[i])
            i_changed = TRUE;
            
        p->edgein[i] = (p->inputs[i]) ^ (p->old->inputs[i]);
    
	    for(; j < BYTESIZE; j++){
	        unsigned int n = BYTESIZE * i + j; 
		    p->di[n].I = (p->inputs[i] >> j) % 2;
		    p->di[n].RE = (p->di[n].I)
				&& ((p->edgein[i] >> j) % 2);
		    p->di[n].FE = (!p->di[n].I)
				&& ((p->edgein[i] >> j) % 2);
	    }
	}
	
	for (i = 0; i < p->nai; i++){
	    if(is_input_forced(p, i))
            p->ai[i].V = p->mask_ai[i];       
        else {
	        double denom = (double)UINT64_MAX;   
            double v = p->real_in[i]; 
            double min = p->ai[i].min;
            double max = p->ai[i].max;
            p->ai[i].V = min + ((max - min) * (v/denom));
        }
        
        if (abs(p->ai[i].V - p->old->ai[i].V) > FLOAT_PRECISION)
            i_changed = TRUE;
	}
	
	memset(p->outputs, 0, p->nq);
	return i_changed;
}

BYTE enc_out(plc_t p)
{ //encode digital outputs to output bytes
	BYTE i = 0;
	BYTE j = 0;
	BYTE o_changed=FALSE;
	BYTE out[p->nq];
	
	memcpy(out, p->outputs, p->nq);
	
	for (; i < p->nq ; i++){//write masked outputs
        for(; j < BYTESIZE; j++){
            unsigned int n = BYTESIZE * i + j;
            out[i] |= (p->dq[n].Q 
                      || (p->dq[n].SET && !p->dq[n].RESET))
				      << j;
	    }
	    
	    p->outputs[i] = (out[i] | p->maskout[i]) & ~p->maskout_N[i];
	    if (p->outputs[i] != p->old->outputs[i])
            o_changed = TRUE;
	}
	
	for (i = 0; i < p->naq; i++){
	    double min = p->aq[i].min;
        double max = p->aq[i].max;
        double val = p->aq[i].V;
        if(is_output_forced(p, i))
            val = p->mask_aq[i];    
        
        p->real_out[i] = UINT64_MAX * ((val - min)/(max-min));
        
        if (abs(p->aq[i].V - p->old->aq[i].V) > FLOAT_PRECISION)
            o_changed = TRUE;
    }
    
    return o_changed;
}

void read_mvars(plc_t p)
{
	int i;
	for (i = 0; i < p->nm; i++){
		if (p->m[i].SET || p->m[i].RESET)
			p->m[i].PULSE = p->m[i].SET && !p->m[i].RESET;
	}
}

void write_mvars(plc_t p)
{
	int i;
	for (i = 0; i < p->nm; i++){
		if (!p->m[i].RO){
			if (p->m[i].PULSE && p->m[i].EDGE) {//up/down counting
				p->m[i].V += (p->m[i].DOWN) ? -1 : 1;
				p->m[i].EDGE = FALSE;
			}
		}
	}
}

BYTE check_pulses(plc_t p)
{
    BYTE m_changed=0;
    int i=0;
    for (i = 0; i < p->nm; i++){//check counter pulses
        if (p->m[i].PULSE != p->old->m[i].PULSE){
            p->m[i].EDGE = TRUE;
            m_changed = TRUE;
        }
    }
    return m_changed;
}

BYTE save_state(BYTE mask,
                plc_t p)
{
    BYTE update = FALSE;
    if (mask & CHANGED_I){// Input changed!
        memcpy(p->old->inputs, p->inputs, p->ni);
        update = TRUE;
    }
    if (mask & CHANGED_O){// Output changed!"
        memcpy(p->old->outputs, p->outputs, p->nq);
        update = TRUE;
    }
    if (mask & CHANGED_M){
        memcpy(p->old->m, p->m, p->nm * sizeof(struct mvar));
        update = TRUE;

    }
    if (mask & CHANGED_T){
        memcpy(p->old->t, p->t, p->nt * sizeof(struct timer));
        update = TRUE;

    }
    if (mask & CHANGED_S){
        memcpy(p->old->s, p->s, p->ns * sizeof(struct blink));
        update = TRUE;
    }
    return update;
}

void write_response(plc_t p)
{
    int rfd = 0; //response file descriptor
    rfd = open(p->response_file, O_NONBLOCK | O_WRONLY);
    //dummy code until this feature goes away
    if(write(rfd, &(p->response), 1) < 0)
        p->response = PLC_ERR;
    else 
        p->response = PLC_OK;
    close(rfd);
}


BYTE manage_timers(plc_t p)
{
    int i=0;
    BYTE t_changed = 0;
    for (i = 0; i < p->nt; i++){
        if (p->t[i].V < p->t[i].P && p->t[i].START){
            if (p->t[i].sn < p->t[i].S)
                p->t[i].sn++;
            else{
                t_changed = TRUE;
                p->t[i].V++;
                p->t[i].sn = 0;
            }
            p->t[i].Q = (p->t[i].ONDELAY) ? 0 : 1;	//on delay
        }
        else if (p->t[i].START){
            p->t[i].Q = (p->t[i].ONDELAY) ? 1 : 0;	//on delay
        }
    }
    return t_changed;
}

BYTE manage_blinkers(plc_t p)
{
    BYTE s_changed=0;
    int i=0;
    for (i = 0; i < p->ns; i++){
        if (p->s[i].S > 0){	//if set up
            if (p->s[i].sn > p->s[i].S){
                s_changed = TRUE;
                p->s[i].Q = (p->s[i].Q) ? 0 : 1;	//toggle
                p->s[i].sn = 0;
            }
            else
                p->s[i].sn++;
        }
    }
    return s_changed;
}

//manage serial comm
void manage_com(plc_t p)
{
    BYTE com[2];
    if (read(p->com[0].fd, com, 2)){
        if (com[0] == 0)
            com[0] = 0;	//NOP
        else
            p->command = com[0] - ASCIISTART;
        //plc_log("LAST command:%d, %s", p->command,
          //     com_nick[com[0] - ASCIISTART]);
    }
}

int plc_func(BYTE *update, plc_t p)
{
	struct timeval tp; //time for poll
	struct timeval tn; //time since beginning of last output
	struct timeval dt;
    long timeout = p->step * THOUSAND; //timeout in usec
    long poll_time = 0;
    long io_time = 0;
	static long run_time = 0;
	int written=FALSE;
    int r = PLC_OK;
    BYTE change_mask = 0;
	BYTE i_changed = FALSE;
	BYTE o_changed = FALSE;
	BYTE m_changed = FALSE;
	BYTE t_changed = FALSE;
	BYTE s_changed = FALSE;
    //char test[NICKLEN];
    dt.tv_sec = 0;
    dt.tv_usec = 0;
	if ((p->status) % 2){//run
//remaining time = step 
        read_inputs(p);
        t_changed = manage_timers(p);
        s_changed = manage_blinkers(p);
        read_mvars(p);
        
        gettimeofday(&tn,NULL);
//dt = time for input + output	
//how much time passed since previous cycle?
        timeval_subtract(&dt, &tn, &Curtime);
        dt.tv_usec = dt.tv_usec % (THOUSAND * p->step);
        io_time = dt.tv_usec;// / THOUSAND;
        timeout -= io_time;
        timeout -= run_time;
//plc_log("I/O time approx:%d microseconds",dt.tv_usec);
//poll on plcpipe for command, for max STEP msecs
        written = poll(p->com, 0, timeout / THOUSAND);
//TODO: when a truly asunchronous UI is available, 
//replace poll() with sleep() for better accuracy
        gettimeofday(&tp, NULL);	//how much time did poll wait?
        timeval_subtract(&dt, &tp, &tn);
        poll_time =  dt.tv_usec;
//plc_log("Poll time approx:%d microseconds",dt.tv_usec);
//dt = time(input) + time(poll)
        
        if (written)
            manage_com(p);
		else if (written == 0)
			p->command = 0;
		else{
		    r = PLC_ERR;
            plc_log("PIPE ERROR\n");
			p->command = 0;
		}
        i_changed = dec_inp(p); //decode inputs
//TODO: a better user plugin system when function blocks are implemented
		project_task(p); //plugin code

        if(r >= PLC_OK)
            r = all_tasks(p->step * THOUSAND, p);
                
        gettimeofday(&Curtime, NULL);	//start timing next cycle
        timeval_subtract(&dt, &Curtime, &tp);
        run_time =  dt.tv_usec;
        compute_variance((double)(run_time + poll_time + io_time));
        
        if(r == ERR_TIMEOUT){    
            plc_log("timeout! i/o: %d us, poll: %d us, run: %d us",
                    io_time, poll_time, run_time);
        }
        o_changed = enc_out(p);
		p->command = 0;

        write_outputs(p);

        m_changed = check_pulses(p);
        write_mvars(p);
        change_mask |= CHANGED_I * i_changed;
        change_mask |= CHANGED_O * o_changed;
        change_mask |= CHANGED_M * m_changed;
        change_mask |= CHANGED_T * t_changed;
        change_mask |= CHANGED_S * s_changed;
        *update = save_state(change_mask, p);
	}
    else{
        usleep(p->step * THOUSAND);
        timeout = 0;
    }
    
    return r;
}

/*******************debugging tools***************/
/*TODO: factor out utilities*/
void dump_label(char * label, char * dump)
{
    char buf[NICKLEN] = "";
    if(label[0] != 0)
        sprintf(buf, "%s:", label);
    strcat(dump, buf);
}
void dump_instruction(instruction_t ins, char * dump)
{
    if(ins == NULL)
        return;
    char buf[8] = "";
    dump_label(ins->label, dump);
    strcat(dump, IlCommands[ins->operation]);
    if(ins->operation >= IL_RET){
        strcat(dump, IlModifiers[ins->modifier - 1]);
        if(ins->operation == IL_JMP){
            sprintf(buf, "%d", ins->operand);
            strcat(dump, buf);
        }
        else {
            strcat(dump, IlOperands[ins->operand - OP_INPUT]);    
            sprintf(buf, "%d/%d", ins->byte, ins->bit);
            strcat(dump, buf);
        }
    }
    strcat(dump, "\n");
}

void dump_rung(rung_t r, char * dump)
{
    if(r == NULL
    || dump == NULL)
        return;
    instruction_t ins;
    unsigned int pc = 0;
    char buf[4] = "";
    for(;pc<r->insno;pc++){
        if(get(r, pc, &ins) < PLC_OK)
            return;
        sprintf(buf, "%d.",pc);
        strcat(dump, buf);   
        dump_instruction(ins, dump);
    }    
    //printf("%s", dump);
}

double Mean = 0;
double M2 = 0;
unsigned long Loop = 0;

void compute_variance(double x)
{
    if(Loop == 0){//overflow
       Mean = 0;
       M2 = 0; 
    }    
    Loop++;    
    double delta = x - Mean;
    Mean += delta / (double)Loop;
    M2 += delta * (x - Mean);    
}

void get_variance(double * mean, double * var)
{
    *mean = Mean;
    if(Loop > 1)
        *var = M2/(double)(Loop - 1);
}

unsigned long get_loop()
{
    return Loop;
}

