/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo;

import br.org.scadabr.utils.i18n.LocalizableEnum;

/**
 *
 * @author aploese
 */
public enum LoggingTypes implements LocalizableEnum< LoggingTypes> {

    ON_CHANGE(1, "pointEdit.logging.type.change"),
    ALL(2, "pointEdit.logging.type.all"),
    NONE(3, "pointEdit.logging.type.never"),
    INTERVAL(4, "pointEdit.logging.type.interval"),
    ON_TS_CHANGE(5, "pointEdit.logging.type.tsChange");

    public final int mangoDbId;
    public final String i18nKey;

    private LoggingTypes(int mangoDbId, String i18nKey) {
        this.mangoDbId = mangoDbId;
        this.i18nKey = i18nKey;
    }

    @Override
    public String getName() {
        return name();
    }
    
    @Override
    public String getI18nKey() {
        return i18nKey;
    }

    @Override
    public Object[] getArgs() {
        return null;
    }

    public static LoggingTypes fromMangoDbId(int mangoDbId) {
        switch (mangoDbId) {
            case 1:
                return LoggingTypes.ON_CHANGE;
            case 2:
                return LoggingTypes.ALL;
            case 3:
                return LoggingTypes.NONE;
            case 4:
                return LoggingTypes.INTERVAL;
            case 5:
                return LoggingTypes.ON_TS_CHANGE;
            default:
                throw new RuntimeException("Cant get LoggingTypes from mangoDbId: " + mangoDbId);
        }
    }
}
