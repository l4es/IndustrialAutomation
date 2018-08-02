/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.datasource.meta;

import br.org.scadabr.db.IntValuePair;
import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.timer.cron.CronParser;
import br.org.scadabr.utils.ImplementMeException;
import com.serotonin.mango.vo.dataSource.meta.MetaPointLocatorVO;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.List;
import javax.validation.ConstraintValidator;
import javax.validation.ConstraintValidatorContext;

/**
 *
 * @author aploese
 */
class InputvariablesValidator implements ConstraintValidator<InputVariables, MetaPointLocatorVO> {

    @Override
    public void initialize(InputVariables a) {
    }

    @Override
    public boolean isValid(MetaPointLocatorVO vo, ConstraintValidatorContext cvc) {
        throw new ImplementMeException();
        /*
         List<String> varNameSpace = new ArrayList<>();
         for (IntValuePair point : vo.getContext()) {
         String varName = point.getValue();
         if (varName.isEmpty()) {
         errors.rejectValue("context", "validate.allVarNames");
         break;
         }

         if (!validateVarName(varName)) {
         errors.rejectValue("context", "validate.invalidVarName", new Object[]{varName}, "validate.invalidVarName");
         break;
         }

         if (varNameSpace.contains(varName)) {
         errors.rejectValue("context", "validate.duplicateVarName", new Object[]{varName}, "validate.duplicateVarName");
         break;
         }

         varNameSpace.add(varName);
         }
         */
    }

    private boolean validateVarName(String varName) {
        char ch = varName.charAt(0);
        if (!Character.isLetter(ch) && ch != '_') {
            return false;
        }
        for (int i = 1; i < varName.length(); i++) {
            ch = varName.charAt(i);
            if (!Character.isLetterOrDigit(ch) && ch != '_') {
                return false;
            }
        }
        return true;
    }

}
