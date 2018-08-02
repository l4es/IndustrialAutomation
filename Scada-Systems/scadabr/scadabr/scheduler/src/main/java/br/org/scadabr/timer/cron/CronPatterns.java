/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package br.org.scadabr.timer.cron;

import br.org.scadabr.utils.i18n.LocalizableMessage;

/**
 *
 * @author aploese
 */
public enum CronPatterns implements LocalizableMessage {
    _1_SECOND("0 */1 * * * * * *"),
    _2_SECONDS("0 */2 * * * * * *"),
    _5_SECONDS("0 */5 * * * *  **"),
    _10_SECONDS("0 */10 * * * * * *"),
    _15_SECONDS("0 */15 * * * * * *"),
    _20_SECONDS("0 */20 * * * * * *"),
    _30_SECONDS("0 */30 * * * * * *"),

    _1_MINUTE("0 0 */1 * * * * *"),
    _2_MINUTES("0 0 */2 * * * * *"),
    _5_MINUTES("0 0 */5 * * * * *"),
    _10_MINUTE("0 0 */10 * * * * *"),
    _15_MINUTES("0 0 */15 * * * * *"),
    _20_MINUTE("0 0 */20 * * * * *"),
    _30_MINUTE("0 0 */30 * * * * *"),
    
    _1_HOUR("0 0 0 */1 * * * *"),
    _2_HOURS("0 0 0 */2 * * * *"),
    
    DAILY_AT_MIDNIGHT("0 0 0 0 */1 * * *"),
    DAILY_AT_NOON("0 0 0 12 */1 * * *");

    public final String i18nKey;
    public final String pattern;

    //TODO makup proper I18N key !!!
    private CronPatterns(String pattern) {
        this.pattern = pattern;
        this.i18nKey = name();
    }

    @Override
    public String getI18nKey() {
        return i18nKey;
    }

    public String getName() {
        return name();
    }
    
    public String getPattern() {
        return pattern;
    }

    @Override
    public Object[] getArgs() {
        return null;
    }

    
}
