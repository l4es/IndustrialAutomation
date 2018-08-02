/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo;

import org.springframework.beans.factory.annotation.Configurable;
import org.springframework.validation.Errors;
import org.springframework.validation.Validator;

/**
 *
 * @author aploese
 */
@Configurable
@Deprecated //TODO really needed???
public class UserVoValidator implements Validator {
    
    @Override
    public boolean supports(Class<?> clazz) {
        return UserVO.class.isAssignableFrom(clazz);
    }

    @Override
    public void validate(Object target, Errors errors) {
        final UserVO vo = (UserVO) target;
        if (vo.getName().isEmpty()) {
            errors.rejectValue("username", "validate.required");
        }
        if (vo.getEmail().isEmpty()) {
            errors.rejectValue("email", "validate.required");
        }
        if (vo.isNew() && vo.getPasswordHash().isEmpty()) {
            errors.rejectValue("password", "validate.required");
        }
        // Check field lengths
        if (vo.getName().length() > 40) {
            errors.rejectValue("username", "validate.notLongerThan", new Object[]{40}, "validate.notLongerThan");
        }
        if (vo.getEmail().length() > 255) {
            errors.rejectValue("email", "validate.notLongerThan", new Object[]{255}, "validate.notLongerThan");
        }
        if (vo.getPhone().length() > 40) {
            errors.rejectValue("phone", "validate.notLongerThan", new Object[]{40}, "validate.notLongerThan");
        }
    }
    
}
