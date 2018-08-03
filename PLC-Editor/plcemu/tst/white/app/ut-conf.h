#ifndef _UT_CONF_H_ 
#define _UT_CONF_H_

void ut_load()
{
    config_t conf = init_config();
    int r = PLC_OK;
    r = load_config_line("STEP\t0\n", conf);
    CU_ASSERT(r == PLC_ERR);
    r = load_config_line("STEP\t10\n", conf);
    CU_ASSERT(conf->step == 10);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("SIGENABLE\t29\n", conf);
    CU_ASSERT(r == PLC_ERR);
    r = load_config_line("SIGENABLE\t39\n", conf);
    CU_ASSERT(conf->sigenable == 39);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("PAGELEN\t23\n", conf);
    CU_ASSERT(r == PLC_ERR);
    r = load_config_line("PAGELEN\t32\n", conf);
    CU_ASSERT(conf->page_len == 32);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("PAGEWIDTH\t79\n", conf);
    CU_ASSERT(r == PLC_ERR);
    r = load_config_line("PAGEWIDTH\t80\n", conf);
    CU_ASSERT(conf->page_width == 80);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("NT\t8\n", conf);
    CU_ASSERT(conf->nt == 8);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("NS\t8\n", conf);
    CU_ASSERT(conf->ns == 8);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("NM\t8\n", conf);
    CU_ASSERT(conf->nm == 8);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("NR\t8\n", conf);
    CU_ASSERT(conf->nr == 8);
    CU_ASSERT(r == PLC_OK);
  
    r = load_config_line("DI\t8\n", conf);
    CU_ASSERT(conf->di == 8);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("DQ\t8\n", conf);
    CU_ASSERT(conf->dq == 8);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("AI\t8\n", conf);
    CU_ASSERT(conf->ai == 8);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("AQ\t8\n", conf);
    CU_ASSERT(conf->aq == 8);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("BASE\t0\n", conf);
    CU_ASSERT(r == PLC_ERR);
    r = load_config_line("BASE\t88888\n", conf);
    CU_ASSERT(conf->base == 88888);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("WR_OFFS\t8\n", conf);
    CU_ASSERT(conf->wr_offs == 8);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("RD_OFFS\t8\n", conf);
    CU_ASSERT(conf->rd_offs == 8);
    CU_ASSERT(r == PLC_OK);
  
    r = load_config_line("COMEDI_FILE\t8\n", conf);
    CU_ASSERT(conf->comedi_file == 8);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("COMEDI_SUBDEV_I\t8\n", conf);
    CU_ASSERT(conf->comedi_subdev_i == 8);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("COMEDI_SUBDEV_Q\t8\n", conf);
    CU_ASSERT(conf->comedi_subdev_q == 8);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("SIM_INPUT\tsimulated_hardware\n", conf);
    CU_ASSERT_STRING_EQUAL(conf->sim_in_file, "./simulated_hardware");
    //printf("%s\n", conf->hw);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("SIM_OUTPUT\tsimulated_hardware\n", conf);
    CU_ASSERT_STRING_EQUAL(conf->sim_out_file, "./simulated_hardware");
    //printf("%s\n", conf->hw);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("HW\tsimulated_hardware\n", conf);
    CU_ASSERT_STRING_EQUAL(conf->hw, "simulated_hardware");
    //printf("%s\n", conf->hw);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("PIPE\tsimulated_hardware\n", conf);
    CU_ASSERT_STRING_EQUAL(conf->pipe, "simulated_hardware");
    //printf("%s\n", conf->hw);
    CU_ASSERT(r == PLC_OK);
    
    r = load_config_line("RESPONSE\tsimulated_hardware\n", conf);
    CU_ASSERT_STRING_EQUAL(conf->response_file, "simulated_hardware");
    //printf("%s\n", conf->hw);
    CU_ASSERT(r == PLC_OK);
    
    clear_config(&conf);    
}


void ut_conf()
{
  config_t conf = init_config();
  //defaults
  CU_ASSERT(conf->nt == 4);
  CU_ASSERT(conf->ns == 4);
  CU_ASSERT(conf->nm == 4);
  CU_ASSERT(conf->di == 8);
  CU_ASSERT(conf->dq == 8);
  CU_ASSERT(conf->ai == 4);
  CU_ASSERT(conf->aq == 4);
  
  CU_ASSERT(conf->sigenable == 36);
  CU_ASSERT(conf->page_width == 80);
  CU_ASSERT(conf->page_len == 24);
  
  CU_ASSERT(conf->hw[MAXSTR-1] == 0);
  
  CU_ASSERT(conf->base == ADVANTECH_HISTORICAL_BASE);
  CU_ASSERT(conf->wr_offs == 0);
  CU_ASSERT(conf->rd_offs == 8);
  
  CU_ASSERT(conf->comedi_file == 0);
  CU_ASSERT(conf->comedi_subdev_i == 0);
  CU_ASSERT(conf->comedi_subdev_q == 1);
  
  CU_ASSERT(conf->sim_in_file[MAXSTR-1] == 0);
  CU_ASSERT(conf->sim_out_file[MAXSTR-1] == 0);
  
  CU_ASSERT(conf->step == 1);
  CU_ASSERT(conf->pipe[MAXSTR-1] == 0);
  
  CU_ASSERT(conf->response_file[MAXSTR-1] == 0);
  
  clear_config(&conf);
  CU_ASSERT(conf == NULL);
}

void ut_apply()
{
    struct PLC_regs plc;
    memset(&plc, 0, sizeof(struct PLC_regs));
    config_t conf = init_config();
    load_config_line("HW\tsimulated_hardware\n", conf);
    
    configure(conf, &plc);
    CU_ASSERT_STRING_EQUAL(plc.hw, "simulated_hardware");
    //printf("hw: %s\n", plc.hw);
    CU_ASSERT(plc.ni == 8);
    CU_ASSERT(plc.di[63].I == 0);
    CU_ASSERT(plc.inputs[7] == 0);
    CU_ASSERT(plc.edgein[7] == 0);
    CU_ASSERT(plc.maskin[7] == 0);
    CU_ASSERT(plc.maskin_N[7] == 0);
   
    CU_ASSERT(plc.nq == 8);
    CU_ASSERT(plc.dq[63].Q == 0);
    CU_ASSERT(plc.outputs[7] == 0);
    CU_ASSERT(plc.maskout[7] == 0);
    CU_ASSERT(plc.maskout_N[7] == 0);
   
    CU_ASSERT(plc.nai == 4);
    CU_ASSERT(plc.real_in[3] == 0);
    CU_ASSERT(plc.ai[3].V < FLOAT_PRECISION);
    
    CU_ASSERT(plc.naq == 4);
    CU_ASSERT(plc.real_out[3] == 0);
    CU_ASSERT(plc.aq[3].V < FLOAT_PRECISION);
    
    CU_ASSERT(plc.nt == 4);
    CU_ASSERT(plc.t[3].Q == 0);
    
    CU_ASSERT(plc.ns == 4);
    CU_ASSERT(plc.s[3].Q == 0);
    
    CU_ASSERT(plc.nm == 4);
    CU_ASSERT(plc.m[3].V == 0);
    
    CU_ASSERT(plc.nmr == 4);
    CU_ASSERT(plc.mr[3].V < FLOAT_PRECISION);

    CU_ASSERT(plc.old->ni == 8);
    CU_ASSERT(plc.old->di[63].I == 0);
    CU_ASSERT(plc.old->inputs[7] == 0);
    CU_ASSERT(plc.old->edgein[7] == 0);
    CU_ASSERT(plc.old->maskin[7] == 0);
    CU_ASSERT(plc.old->maskin_N[7] == 0);
   
    CU_ASSERT(plc.old->nq == 8);
    CU_ASSERT(plc.old->dq[63].Q == 0);
    CU_ASSERT(plc.old->outputs[7] == 0);
    CU_ASSERT(plc.old->maskout[7] == 0);
    CU_ASSERT(plc.old->maskout_N[7] == 0);
   
    CU_ASSERT(plc.old->nai == 4);
    CU_ASSERT(plc.old->real_in[3] == 0);
    CU_ASSERT(plc.old->ai[3].V < FLOAT_PRECISION);
    
    CU_ASSERT(plc.old->naq == 4);
    CU_ASSERT(plc.old->real_out[3] == 0);
    CU_ASSERT(plc.old->aq[3].V < FLOAT_PRECISION);
    
    CU_ASSERT(plc.old->nt == 4);
    CU_ASSERT(plc.old->t[3].Q == 0);
    
    CU_ASSERT(plc.old->ns == 4);
    CU_ASSERT(plc.old->s[3].Q == 0);
    
    CU_ASSERT(plc.old->nm == 4);
    CU_ASSERT(plc.old->m[3].V == 0);
    
    CU_ASSERT(plc.old->nmr == 4);
    CU_ASSERT(plc.old->mr[3].V < FLOAT_PRECISION);

 
    CU_ASSERT(plc.step == 1);
    CU_ASSERT(plc.command == 0);
	CU_ASSERT(plc.status = 1);
	
    CU_ASSERT(conf->response_file[MAXSTR-1] == 0);  
    clear_config(&conf);
}

#endif//_UT_CONF_H_
