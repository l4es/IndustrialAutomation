/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.utils.i18n;

/**
 *
 * @author aploese
 */
public interface LocalizableMessage {

    String getI18nKey();

    Object[] getArgs();

}
