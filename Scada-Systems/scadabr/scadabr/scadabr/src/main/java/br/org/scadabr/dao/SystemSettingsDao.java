/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.dao;

import br.org.scadabr.utils.TimePeriods;
import com.serotonin.mango.web.email.MangoEmailContent;
import javax.inject.Named;
import org.joda.time.DateTime;

/**
 *
 * @author aploese
 */
@Named
public interface SystemSettingsDao {

    long getFutureDateLimit();

    String getInstanceDescription();

    String getServletContextPath();

    void setServletContextPath(String contextPath);

    String getNewVersionNotificationLevel();

    public String getFileDataPath();

    HttpClientProxy getHttpClientProxy();

    Email getEmail();
    
    interface Email {
        MangoEmailContent.ContentType getContentType();

        String getFromAddress();

        String getFromName();
        
        String getSmtpHost();
        
        int getSmtpPort();
        
        boolean isAuthorization();
        
        String getSmtpUsername();
        
        String getSmtpPassword();
        
        boolean isTls();
    }

    Events getEvents();
    
    public interface Events {

        TimePeriods getPurgePeriodType();

        int getPurgePeriods();
        
        DateTime getCutoff(DateTime cutOff);
    }
    
    Reports getReports();

    public interface Reports {

        TimePeriods getPurgePeriodType();

        int getPurgePeriods();
        
        DateTime getCutoff(DateTime cutOff);
    }

    public interface HttpClientProxy {

        boolean isEnabled();

        String getServer();

        int getPort();

        String getUsername();

        public String getPassword();
    }

}
