/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo;

import br.org.scadabr.rt.RT;
import br.org.scadabr.util.ChangeComparable;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonTypeInfo;
import java.io.Serializable;
import javax.validation.ValidationException;

/**
 *
 * @author aploese
 * @param <T>
 */
@JsonTypeInfo(use = JsonTypeInfo.Id.NAME, include=JsonTypeInfo.As.PROPERTY, property = "scadaBrType", visible = false)
//@JsonTypeInfo(use=JsonTypeInfo.Id.CLASS, include=JsonTypeInfo.As.PROPERTY, property="javaClass")
//@JsonTypeResolver(ScadaBrTypeResolverBuilder.class)
public interface VO<T extends VO<T>> extends Serializable, Cloneable, ChangeComparable<T> {

    @Override
    Integer getId();

    void setId(Integer id);

    String getName();

    void setName(String name);

    @JsonIgnore
    default boolean isNew() {
        return getId() == null;
    }

    NodeType getNodeType();

    default public void setNodeType(NodeType nodeType) {
        if (!getNodeType().equals(nodeType)) {
            throw new ValidationException("NodeType mismatch.");
        }
    }
    
    RT<T> createRT();

}
