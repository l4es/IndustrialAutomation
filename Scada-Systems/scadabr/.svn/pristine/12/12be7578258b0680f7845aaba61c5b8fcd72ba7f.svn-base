/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.rt;

import br.org.scadabr.vo.VO;
import javax.validation.ValidationException;

/**
 *
 * @author aploese
 * @param <T>
 */
public abstract class AbstractRT<T extends VO<T>> implements RT<T> {
    
    protected Integer id;
    
    protected String name;

    public AbstractRT(T vo) {
        id = vo.getId();
        name = vo.getName();
    }
    
    /**
     * override and call this to add data to vo
     * @param vo 
     */
    protected T fillVO(T vo) {
        vo.setId(id);
        vo.setName(name);
        return vo;
    }
    
    @Override
    public void patch(T vo) {
        if (this.id != vo.getId()) {
            throw new ValidationException();
        }
        name = vo.getName();
    }
    
    @Override
    public int getId() {
        return id;
    }

    @Override
    public String getName() {
        return name;
    }

}
