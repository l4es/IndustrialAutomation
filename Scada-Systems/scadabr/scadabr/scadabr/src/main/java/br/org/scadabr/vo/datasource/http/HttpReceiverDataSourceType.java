/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.datasource.http;

import br.org.scadabr.vo.datasource.DataSourceType;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.dataSource.http.HttpReceiverDataSourceVO;
import javax.inject.Named;

/**
 *
 * @author aploese
 */
@Named
public class HttpReceiverDataSourceType extends DataSourceType {
    public static final String KEY= "HTTP_RECIEVER";
    public static final int DB_ID = 7;

    @Override
    public String getTypeKey() {
        return KEY;
    }

    @Override
    public Class getTypeClass() {
        return HttpReceiverDataSourceVO.class;
    }

    @Override
    public DataSourceVO<?> createDataSourceVO() {
      return new HttpReceiverDataSourceVO();
    }

    @Override
    public String getI18nKey() {
        return "dsEdit.httpReceiver";
    }

    @Override
    public Object[] getArgs() {
        return null;
    }
    
}