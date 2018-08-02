/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.datasource.meta;

import br.org.scadabr.vo.datasource.DataSourceType;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.dataSource.meta.MetaDataSourceVO;
import javax.inject.Named;

/**
 *
 * @author aploese
 */
@Named
public class MetaDataSourceType extends DataSourceType {
    public static final String KEY= "META";
    public static final int DB_ID = 9;

    @Override
    public String getTypeKey() {
        return KEY;
    }

    @Override
    public Class getTypeClass() {
        return MetaDataSourceVO.class;
    }

    @Override
    public DataSourceVO<?> createDataSourceVO() {
        return new MetaDataSourceVO();
    }

    @Override
    public String getI18nKey() {
        return "dsEdit.meta";
    }

    @Override
    public Object[] getArgs() {
        return null;
    }

}
