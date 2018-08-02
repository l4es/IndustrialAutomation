/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.datasource.meta;

import br.org.scadabr.rt.event.type.DuplicateHandling;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.vo.event.type.DataSourceEventKey;

/**
 *
 * @author aploese
 */
public enum MetaDataSourceEventKey implements DataSourceEventKey<MetaDataSourceEventKey> {

    CONTEXT_POINT_DISABLED(1, "event.ds.contextPoint"),
    SCRIPT_ERROR(2, "event.ds.scriptError"),
    RESULT_TYPE_ERROR(3, "event.ds.resultType");

    public static Object fromId(int id) {
        switch (id) {
            case 1:
                return CONTEXT_POINT_DISABLED;
            case 2:
                return SCRIPT_ERROR;
            case 3:
                return RESULT_TYPE_ERROR;
            default:
                throw new IndexOutOfBoundsException("Cant get MetaDataSourceEventKey from: " + id);
        }
    }

    private final int id;
    private final String i18nKey;

    private MetaDataSourceEventKey(int id, String i18nKey) {
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
    public boolean isStateful() {
        return true;
    }

    @Override
    public String getName() {
        return name();
    }
}
