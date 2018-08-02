/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.utils.i18n;

/**
 *
 * @author aploese
 * @param <E>
 */
public interface LocalizableEnum<E extends Enum<E>> extends LocalizableMessage {
    
    /**
     * name() as bean property.
     * @return 
     */
    String getName();
}
