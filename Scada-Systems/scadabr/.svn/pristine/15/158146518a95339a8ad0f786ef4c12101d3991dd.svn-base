/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.datasource.meta;

import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.timer.cron.CronParser;
import com.serotonin.mango.vo.dataSource.meta.MetaPointLocatorVO;
import java.text.ParseException;
import javax.validation.ConstraintValidator;
import javax.validation.ConstraintValidatorContext;

/**
 *
 * @author aploese
 */
class CronPatternValidator implements ConstraintValidator<CronPattern, MetaPointLocatorVO> {

    @Override
    public void initialize(CronPattern a) {
    }

    @Override
    public boolean isValid(MetaPointLocatorVO vo, ConstraintValidatorContext cvc) {
        if (vo.getUpdateEvent() == UpdateEvent.CRON) {
            try {
                new CronParser().parse(vo.getUpdateCronPattern(), CronExpression.TIMEZONE_UTC);
            } catch (ParseException e) {
                return false;
//                errors.rejectValue("updateCronPattern", "validate.invalidCron", new Object[]{vo.getUpdateCronPattern()}, "validate.invalidCron");
            }
        }
        return true;
    }

}
