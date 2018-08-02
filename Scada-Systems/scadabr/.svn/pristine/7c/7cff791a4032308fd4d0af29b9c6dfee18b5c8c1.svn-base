/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.datasource;

import br.org.scadabr.dao.DataSourceDao;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import javax.inject.Inject;
import javax.validation.ConstraintValidator;
import javax.validation.ConstraintValidatorContext;

/**
 *
 * @author aploese
 */
public class UniqueDsXidValidator implements ConstraintValidator<UniqueDsXid, DataSourceVO> {
    
    @Inject
    private DataSourceDao dataSourceDao;
    private String msg;
    
    
    @Override
    public void initialize(UniqueDsXid a) {
        msg = a.message();
    }

    @Override
    public boolean isValid(DataSourceVO vo, ConstraintValidatorContext cvc) {
        
        final boolean result = dataSourceDao.isXidUnique(vo);
        
        if (!result) {
            cvc.disableDefaultConstraintViolation();
            cvc.buildConstraintViolationWithTemplate(msg).addPropertyNode("xid").addConstraintViolation();
        }
                
        return result;
    }

}
