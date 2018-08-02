/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.propertyEditor;

import java.util.Objects;
import org.springframework.validation.MessageCodesResolver;

/**
 *
 * @author aploese
 */
public class DefaultMessageCodesResolver implements MessageCodesResolver {

    @Override
    public String[] resolveMessageCodes(String errorCode, String objectName, String field, Class fieldType) {
        if (errorCode == null || errorCode.isEmpty()) {
            return new String[0];
        }
        switch (errorCode) {
            case "typeMismatch":
                if (fieldType == Double.TYPE) {
                    return new String[]{"badDecimalFormat"};
                }
                if (fieldType == Integer.TYPE) {
                    return new String[]{"badIntegerFormat"};
                }
            default:
                return new String[]{errorCode};
        }
    }

    @Override
    public String[] resolveMessageCodes(String errorCode, String objectName) {
        return resolveMessageCodes(errorCode, objectName, null, null);
    }
}
