/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr;

import br.org.scadabr.util.ScadaBrObjectMapper;
import br.org.scadabr.vo.datapoints.PointFolderVO;
import com.serotonin.mango.vo.DoubleDataPointVO;
import javax.annotation.PostConstruct;
import javax.inject.Inject;
import javax.inject.Named;
import javax.inject.Singleton;

/**
 * Register this module at runtime
 * currently only classes to objectmapper
 * @author aploese
 */
@Named
@Singleton
public class ModuleRegistry {
    
    
    @Inject
    private ScadaBrObjectMapper scadaBrObjectMapper;
    
    @PostConstruct
    public void init()  {
        scadaBrObjectMapper.registerSubtypes(DoubleDataPointVO.class, PointFolderVO.class);
   }
    
}
