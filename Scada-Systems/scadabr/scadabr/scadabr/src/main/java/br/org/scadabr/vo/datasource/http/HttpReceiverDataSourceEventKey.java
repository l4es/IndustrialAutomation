/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.datasource.http;

import br.org.scadabr.rt.event.type.DuplicateHandling;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.vo.event.type.DataSourceEventKey;

/**
 *
 * @author aploese
 */
public class HttpReceiverDataSourceEventKey implements DataSourceEventKey<HttpReceiverDataSourceEventKey> {

    private final int id;
    private final String i18nKey;

    private HttpReceiverDataSourceEventKey(int id, String i18nKey) {
        this.id = id;
        this.i18nKey = i18nKey;
    }

    @Override
    public int getId() {
        return id;
    }

    @Override
    public String getI18nKey() {
        return i18nKey;
    }

    @Override
    public Object[] getArgs() {
        return null;
    }

    @Override
    public DuplicateHandling getDuplicateHandling() {
        return DataSourceEventKey.DEFAULT_DUPLICATE_HANDLING;
    }

    @Override
    public AlarmLevel getDefaultAlarmLevel() {
        return DataSourceEventKey.DEFAULT_ALARMLEVEL;
    }

    @Override
    public String name() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public boolean isStateful() {
        return true;
    }

    @Override
    public String getName() {
        return name();
    }
}
