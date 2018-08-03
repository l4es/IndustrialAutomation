#include <ctype.h>

#include "init.h"
#include "plclib.h"
#include "util.h"

extern struct PLC_regs Plc;

int extract_name(const char* line, char* name, int start )
{
    int j = start;
    int k = 0;
    if(name == NULL)
        return PLC_ERR;
        
    if(start < 0
    || line == NULL
    || strlen(line) < start)
        return PLC_ERR;
            
    memset(name, 0, SMALLSTR);
    while (isspace(line[j]))    //get name
        j++;
    while ((isalpha(line[j]) || line[j] == '_') 
           && k < SMALLSTR
           && j < MAXSTR)
        name[k++] = line[j++];
    return j;
}

int extract_index(const char* line, int *idx, int start)
{
    char idx_str[SMALLSTR];
    int j = start;
    int k = 0;
    
    if(idx == NULL)
        return PLC_ERR;
        
    if(start < 0
    || line == NULL
    || strlen(line) < start)
        return PLC_ERR;
    
    memset(idx_str, 0, SMALLSTR);
    
    while (isspace(line[j]))    //get index
        j++;
    while (isdigit(line[j])
           && k < SMALLSTR
           && j < MAXSTR)
        idx_str[k++] = line[j++];

    *idx = atoi(idx_str);
    return j;
}

int extract_value( const char *line, char *val, int start)
{
    int k = 0;
    int j = start;
    
    if(val == NULL)
        return PLC_ERR;
        
    if(start < 0
    || line == NULL
    || strlen(line) < start)
        return PLC_ERR;
        
    memset(val, 0, SMALLSTR);
    while (isspace(line[j]))    //get value
        j++;

    while(line[j] && k < NICKLEN
       && line[j] != 10
       && line[j] != 13
       && line[j] != ';'
       && line[j] != '\t'
       && k < SMALLSTR
       && j < MAXSTR)
        val[k++] = line[j++];
    return j;
}

int declare_input(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "I")){
        if (idx >= BYTESIZE * Plc.ni)
            return ERR_BADINDEX;
        sprintf(Plc.di[idx].nick, "%s", val);
        plc_log("Variable %s : Digital Input %d ", val, idx);
    }
    return PLC_OK;
}

int declare_output(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "Q")){
        if (idx >= BYTESIZE * Plc.nq)
            return ERR_BADINDEX;
        sprintf(Plc.dq[idx].nick, "%s", val);
        plc_log("Variable %s : Digital Output %d ", val, idx);
    }
    return PLC_OK;
}

int declare_analog_input(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "IF")){
        if (idx >= Plc.nai)
            return ERR_BADINDEX;
        sprintf(Plc.ai[idx].nick, "%s", val);
        plc_log("Variable %s : Analog Input %d ", val, idx);
    }
    return PLC_OK;
}

int declare_analog_output(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "QF")){
        if (idx >= Plc.naq)
            return ERR_BADINDEX;
        sprintf(Plc.aq[idx].nick, "%s", val);
        plc_log("Variable %s : Analog Output %d ", val, idx);
    }
    return PLC_OK;
}

int configure_input_min(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "I_MIN")){
        if (idx >= Plc.nai)
            return ERR_BADINDEX;
        Plc.ai[idx].min = atof(val);
        plc_log("Range min %s : Analog Input %d ", val, idx);  
    }
    return PLC_OK;
}

int configure_output_min(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "Q_MIN")){
        if (idx >= Plc.naq)
            return ERR_BADINDEX;
        Plc.aq[idx].min = atof(val);
        plc_log("Range min %s : Analog Output %d ", val, idx);
    }
    return PLC_OK;
}

int configure_input_max(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "I_MAX")){
        if (idx >= Plc.ni)
            return ERR_BADINDEX;
         Plc.ai[idx].max = atof(val);
         plc_log("Range max %s : Analog Input %d ", val, idx);
    }
    return PLC_OK;
}

int configure_output_max(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "Q_MAX")){
        if (idx >= Plc.nq)
            return ERR_BADINDEX;
        Plc.aq[idx].max = atof(val);
        plc_log("Range max %s : Analog Output %d ", val, idx);
    }
    return PLC_OK;
}

int declare_register(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "M")){
        if (idx >= Plc.nm)
            return ERR_BADINDEX;
        sprintf(Plc.m[idx].nick, "%s", val);
        plc_log("Variable %s : Memory Register %d ", val, idx);
    }
    return PLC_OK;
}

int declare_register_r(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "MF")){
        if (idx >= Plc.nmr)
            return ERR_BADINDEX;
        sprintf(Plc.mr[idx].nick, "%s", val);
        plc_log("Variable %s : Memory Register %d ", val, idx);
    }
    return PLC_OK;
}
 
int declare_timer(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "T")){
        if (idx >= Plc.nt)
            return ERR_BADINDEX;
        sprintf(Plc.t[idx].nick, "%s", val);
        plc_log("Variable %s : Timer %d ", val, idx);
    }
    return PLC_OK;
}

int declare_blinker(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "B")){
        if (idx >= Plc.ns)
            return ERR_BADINDEX;
        sprintf(Plc.s[idx].nick, "%s", val);
        plc_log("Variable %s : Blinking Timer %d ", val, idx);
    }
    return PLC_OK;
}

int declare_serial(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "COM")){
        if (idx >= MEDSTR)
            return ERR_BADINDEX;
        //FIXME sprintf(com_nick[idx], "%s", val);
        plc_log("Variable %s : Serial Command %d ", val, idx);
    }
    return PLC_OK;
}

int init_register(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "MEMORY")){
        if (idx >= Plc.nm)
            return ERR_BADINDEX;
        Plc.m[idx].V = (uint64_t)atol(val);

        plc_log("Initial value %s : Register %d ", val, idx);
    }
    return PLC_OK;
}

int init_register_r(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "MEM_R")){
        if (idx >= Plc.nmr)
            return ERR_BADINDEX;

        Plc.mr[idx].V = atol(val);
        plc_log("Initial float value %s : Register %d ", val, idx);
    }
    return PLC_OK;
}

int define_reg_direction(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "COUNT")){
        if (idx >= Plc.nm)
            return ERR_BADINDEX;
        if (!strcmp(val, "DOWN")){
            Plc.m[idx].DOWN = TRUE;
            plc_log("Counter %d : Downcount ", idx);
        }
            
        else
            return ERR_BADOPERATOR;
            
    }
    return PLC_OK;
}

int define_reg_readonly(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "COUNTER")){
        if (idx >= Plc.nm)
            return ERR_BADINDEX;
        if (!strcmp(val, "OFF")){
            Plc.m[idx].RO = TRUE;
            plc_log("Register %d : Const ", idx);            
        }
        else
            return ERR_BADOPERATOR;
    }
    return PLC_OK;
}

int init_timer_set(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "TIME")){
        if (idx >= Plc.nt)
            return ERR_BADINDEX;
        Plc.t[idx].S = atoi(val);
        plc_log("Timer %d : Set %s ", idx, val);
    }
    return PLC_OK;
}

int init_timer_preset(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "PRESET")){
        if (idx >= Plc.nt)
            return ERR_BADINDEX;
        Plc.t[idx].P = atoi(val);
        plc_log("Timer %d : Preset %s ", idx, val);
    }
    return PLC_OK;
}

int init_timer_delay(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "DELAY")){
        if (idx >= Plc.nt)
            return ERR_BADINDEX;
        if (!strcmp(val, "ON")){
            Plc.t[idx].ONDELAY = TRUE;
            plc_log("Timer %d : On delay", idx);
        }
        else
            return ERR_BADOPERAND;
    }
    return PLC_OK;
}

int init_blinker_set(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "BLINK")){
        if (idx >= Plc.ns)
            return ERR_BADINDEX;
        Plc.s[idx].S = atoi(val);
        plc_log("Blinker %d : Set %s ", idx, val);
    }
    return PLC_OK;
}

