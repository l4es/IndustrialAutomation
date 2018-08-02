/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.mvc;

import java.util.Collection;
import java.util.LinkedList;
import java.util.Set;
import javax.validation.ConstraintViolation;

/**
 *
 * @author aploese
 * @param <T>
 */
public class AjaxFormPostResponse<T extends Object> {
    private final Object target;
    private final Collection<JsonConstraintViolation> constraintViolations = new LinkedList<>();
    
    
    public AjaxFormPostResponse(T target, Set<ConstraintViolation<T>> constraintViolations) {
        this.target = target;
        for (ConstraintViolation cv : constraintViolations) {
            this.constraintViolations.add(new JsonConstraintViolation(cv.getPropertyPath().toString(), cv.getInvalidValue(), cv.getMessage()));
        }
    }

    public Object getTarget() {
        return target;
    }
    
    public Collection<JsonConstraintViolation> getConstraintViolations() {
        return constraintViolations;
    }
}
