#include "util.h"
#include "plclib.h"
#include "config.h"

config_t init_config()
{
    config_t conf = (config_t)malloc(sizeof(struct config));
    memset(conf, 0, sizeof(struct config));
//registers    
    conf->nt = 4;
    conf->ns = 4;
    conf->nm = 4;
    conf->nr = 4;
    conf->di = 8;
    conf->dq = 8;
    conf->ai = 4;
    conf->aq = 4;
//ui    
    conf->sigenable = 36;
    conf->page_width = 80;
    conf->page_len = 24;
//hardware    
    conf->base = ADVANTECH_HISTORICAL_BASE;
    conf->wr_offs = 0;
    conf->rd_offs = 8;
    conf->comedi_file = 0;
    conf->comedi_subdev_i = 0;
    conf->comedi_subdev_q = 1;
    conf->comedi_subdev_ai = 2;
    conf->comedi_subdev_aq = 3;
//polling    
    conf->step = 1;
    
    return conf;
}

void clear_config(config_t *c)
{
    if (*c != NULL)
        free(*c);
    *c = NULL;
}

int load_config_line(const char * line, config_t conf)
{
    char name[SMALLSTR] = "";
    char val[SMALLBUF] = "";
    
    sscanf(line, "%s\t%s", name, val);
		
	if(conf == NULL)
	    return PLC_ERR;
	    		
	if (!strcmp(name, "STEP")){
	    conf->step = atol(val);
	    plc_log("Found value for STEP: %s ...", val);
	}
	
	if (!strcmp(name, "SIGENABLE")){
	    conf->sigenable = atoi(val);
	    plc_log("Found value for SIGENABLE: %s ...", val);
	}
	if (!strcmp(name, "PAGELEN")){
	    conf->page_len = atoi(val);
	    plc_log("Found value for PAGELEN: %s ...", val);
	}
	if (!strcmp(name, "PAGEWIDTH")){
	    conf->page_width = atoi(val);
	    plc_log("Found value for PAGEWIDTH: %s ...", val);
	}
	
    if (!strcmp(name, "NT")){
	    conf->nt = atoi(val);
	    plc_log("Found value for NT: %s ...", val);
	}	
	if (!strcmp(name, "NS")){
	    conf->ns = atoi(val);
	    plc_log("Found value for NS: %s ...", val);
	}
	if (!strcmp(name, "NM")){
        conf->nm = atoi(val);
	    plc_log("Found value for NM: %s ...", val);
	}
	if (!strcmp(name, "NR")){
        conf->nr = atoi(val);
	    plc_log("Found value for NR: %s ...", val);
	}
	if (!strcmp(name, "DI")){
        conf->di = atoi(val);
	    plc_log("Found value for DI: %s ...", val);
	}
	if (!strcmp(name, "DQ")){
	    conf->dq = atoi(val);
	    plc_log("Found value for DQ: %s ...", val);
	}
	if (!strcmp(name, "AI")){
        conf->ai = atoi(val);
	    plc_log("Found value for AI: %s ...", val);
	}
	if (!strcmp(name, "AQ")){
	    conf->aq = atoi(val);
	    plc_log("Found value for AQ: %s ...", val);
	}		
	if (!strcmp(name, "BASE")){
	    conf->base = atoi(val);
	    plc_log("Found value for BASE: %s ...", val);
	}
	if (!strcmp(name, "WR_OFFS")){
	    conf->wr_offs = atoi(val);
		plc_log("Found value for WR_OFFS: %s ...", val);
	}
	if (!strcmp(name, "RD_OFFS")){
		conf->rd_offs = atoi(val);
	    plc_log("Found value for RD_OFFS: %s ...", val);	
	}
	if (!strcmp(name, "COMEDI_FILE")){
        conf->comedi_file = atoi(val);
	    plc_log("Found value for COMEDI_FILE: %s ...", val);	
	}
	if (!strcmp(name, "COMEDI_SUBDEV_I")){
	    conf->comedi_subdev_i = atoi(val);
	    plc_log("Found value for COMEDI_SUBDEV_I: %s ...",val);	
	}
	if (!strcmp(name, "COMEDI_SUBDEV_Q")){
		conf->comedi_subdev_q = atoi(val);
	    plc_log("Found value for COMEDI_SUBDEV_Q: %s ...", val);
	}
	if (!strcmp(name, "COMEDI_SUBDEV_AI")){
	    conf->comedi_subdev_ai = atoi(val);
	    plc_log("Found value for COMEDI_SUBDEV_AI: %s ...",val);	
	}
	if (!strcmp(name, "COMEDI_SUBDEV_AQ")){
		conf->comedi_subdev_aq = atoi(val);
	    plc_log("Found value for COMEDI_SUBDEV_AQ: %s ...", val);
	}
	if (!strcmp(name, "HW")){
		sprintf(conf->hw, "%s", val);
	    plc_log("Found value for HW: %s ...\n", val);	
	}
	
	if (!strcmp(name, "PIPE")){
	    sprintf(conf->pipe, "%s", val);
	    plc_log("Found value for PIPE: %s ...", val);
	}
	if (!strcmp(name, "RESPONSE")){
	    sprintf(conf->response_file, "%s", val);
	    plc_log("Found value for RESPONSE: %s ...", val);
	}
	if (!strcmp(name, "SIM_INPUT")){
        sprintf(conf->sim_in_file, "./%s", val);
	    plc_log("Found value for SIM_INPUT: %s ...", val);
	}
	if (!strcmp(name, "SIM_OUTPUT")){
	    sprintf(conf->sim_out_file, "./%s", val);
	    plc_log("Found value for SIM_OUTPUT: %s ...", val);
	}
	
	if (conf->step > 0
		&& conf->sigenable > 29
        && conf->page_len > 23
		&& conf->page_width > 79
		&& conf->base > 0){
            return PLC_OK;
	}
    return PLC_ERR;
}

int load_config(const char * filename, config_t conf)
{
	FILE * fcfg;
	char line[MEDSTR];
	char path[MAXSTR];

    memset(path, 0, MAXSTR);
	sprintf(path, "%s", filename);
	
	int r = PLC_OK;
	int l = 0;

    if ((fcfg = fopen(path, "r"))){
	    plc_log("Looking for configuration from %s ...", path);
		while (fgets(line, MEDSTR, fcfg)){
		    l++;
		    r = load_config_line(line, conf);
		    if(r < PLC_OK){
		        plc_log("%s line %d: configuration error\n", path, l);
		        break;
		    }
		}
		fclose(fcfg);
	}
    return r;
}

void configure(const config_t conf, plc_t plc)
{
    plc->ni = conf->di;
    plc->nq = conf->dq;
    plc->nai = conf->ai;
    plc->naq = conf->aq;
    plc->nt = conf->nt;
    plc->ns = conf->ns;
    plc->nm = conf->nm;
    plc->nmr = conf->nr;
    
	sprintf(plc->hw, "%s", conf->hw);
	
    plc->inputs = (BYTE *) malloc(plc->ni);
    plc->outputs = (BYTE *) malloc(plc->nq);
	plc->edgein = (BYTE *) malloc(plc->ni);
	plc->maskin = (BYTE *) malloc(plc->ni);
	plc->maskout = (BYTE *) malloc(plc->nq);
	plc->maskin_N = (BYTE *) malloc(plc->ni);
    plc->maskout_N = (BYTE *) malloc(plc->nq);
    plc->real_in = (uint64_t *) malloc(conf->ai * sizeof(uint64_t));
    plc->real_out = (uint64_t *) malloc(conf->aq * sizeof(uint64_t));
	plc->mask_ai = (double *) malloc(conf->ai * sizeof(double));
    plc->mask_aq = (double *) malloc(conf->aq * sizeof(double));
	plc->di = (di_t) malloc(
			BYTESIZE * plc->ni * sizeof(struct digital_input));
	plc->dq = (do_t) malloc(
			BYTESIZE * plc->nq * sizeof(struct digital_output));
    
    plc->t = (dt_t) malloc(plc->nt * sizeof(struct timer));
    plc->s = (blink_t) malloc(plc->ns * sizeof(struct blink));
    plc->m = (mvar_t) malloc(plc->nm * sizeof(struct mvar));
    plc->mr = (mreal_t) malloc(plc->nmr * sizeof(struct mreal));
   
    plc->ai = (aio_t) malloc(
			 conf->ai * sizeof(struct analog_io));
	plc->aq = (aio_t) malloc(
			 conf->aq * sizeof(struct analog_io));
   
    memset(plc->real_in, 0, plc->nai*sizeof(uint64_t));
	memset(plc->real_out, 0, plc->naq*sizeof(uint64_t));
    memset(plc->inputs, 0, plc->ni);
	memset(plc->outputs, 0, plc->nq);
    memset(plc->maskin, 0, plc->ni);
	memset(plc->maskout, 0, plc->nq);
	memset(plc->maskin_N, 0, plc->ni);
	memset(plc->maskout_N, 0, plc->nq);
	
    memset(plc->mask_ai, 0, plc->nai * sizeof(double));
	memset(plc->mask_aq, 0, plc->naq * sizeof(double));
    
    memset(plc->di, 0, BYTESIZE * plc->ni * sizeof(struct digital_input));
	memset(plc->dq, 0, BYTESIZE * plc->nq * sizeof(struct digital_output));
    memset(plc->t, 0, plc->nt * sizeof(struct timer));
	memset(plc->s, 0, plc->ns * sizeof(struct blink));
    memset(plc->m, 0, plc->nm * sizeof(struct mvar));
    memset(plc->mr, 0, plc->nmr * sizeof(struct mreal));

    plc_t p_old=NULL;
	p_old = (plc_t) malloc(sizeof(struct PLC_regs));

    p_old->ni = conf->di;
    p_old->nq = conf->dq;
    p_old->nai = conf->ai;
    p_old->naq = conf->aq;
    p_old->nt = conf->nt;
    p_old->ns = conf->ns;
    p_old->nm = conf->nm;
    p_old->nmr = conf->nr;
    
	p_old->inputs = (BYTE *) malloc(conf->di);
	p_old->outputs = (BYTE *) malloc(conf->dq);
	p_old->maskin = (BYTE *) malloc(conf->di);
	p_old->edgein = (BYTE *) malloc(conf->di);
	p_old->maskout = (BYTE *) malloc(conf->dq);
	p_old->maskin_N = (BYTE *) malloc(conf->di);
	p_old->maskout_N = (BYTE *) malloc(conf->dq);
	p_old->di = (di_t) malloc(
			BYTESIZE * conf->di * sizeof(struct digital_input));
	p_old->dq = (do_t) malloc(
			BYTESIZE * conf->dq * sizeof(struct digital_output));
	p_old->t = (dt_t) malloc(conf->nt * sizeof(struct timer));
	p_old->s = (blink_t) malloc(conf->ns * sizeof(struct blink));
	p_old->m = (mvar_t) malloc(conf->nm * sizeof(struct mvar));
    p_old->mr = (mreal_t) malloc(conf->nr * sizeof(struct mreal));
    
    
    p_old->real_in = (uint64_t *) malloc(conf->ai * sizeof(uint64_t));
    p_old->real_out = (uint64_t *) malloc(conf->aq * sizeof(uint64_t));
	p_old->mask_ai = (double *) malloc(conf->ai * sizeof(double));
    p_old->mask_aq = (double *) malloc(conf->aq * sizeof(double));
    p_old->ai = (aio_t) malloc(
			 conf->ai * sizeof(struct analog_io));
	p_old->aq = (aio_t) malloc(
			 conf->aq * sizeof(struct analog_io));
    
	p_old->di = (di_t) malloc(
			BYTESIZE * plc->ni * sizeof(struct digital_input));
	p_old->dq = (do_t) malloc(
			BYTESIZE * plc->nq * sizeof(struct digital_output));
    
	memcpy(p_old->inputs, plc->inputs, conf->di);
	memcpy(p_old->outputs, plc->outputs, conf->dq);
	memset(p_old->real_in, 0, plc->nai*sizeof(uint64_t));
	memset(p_old->real_out, 0, plc->naq*sizeof(uint64_t));
	
	memcpy(p_old->m, plc->m, conf->nm * sizeof(struct mvar));
    memcpy(p_old->mr, plc->mr, conf->nr * sizeof(struct mreal));
    memcpy(p_old->t, plc->t, conf->nt * sizeof(struct timer));
    memcpy(p_old->s, plc->s, conf->ns * sizeof(struct blink));
    
    plc->old = p_old;
	plc->command = 0;
	plc->status = ST_RUNNING;
	plc->step = conf->step;
	plc->response_file = conf->response_file;
}

