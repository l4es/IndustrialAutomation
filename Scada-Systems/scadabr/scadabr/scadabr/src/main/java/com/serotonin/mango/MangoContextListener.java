/*
 Mango - Open Source M2M - http://mango.serotoninsoftware.com
 Copyright (C) 2006-2011 Serotonin Software Technologies Inc.
 @author Matthew Lohbihler
    
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/* modified for NORD Electric by MCA Sistemas */
package com.serotonin.mango;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import javax.servlet.ServletContext;
import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;


import br.org.scadabr.dao.SystemSettingsDao;
import br.org.scadabr.jdbc.DatabaseAccessFactory;
import br.org.scadabr.l10n.JsL10N;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.rt.SchedulerPool;
import br.org.scadabr.timer.cron.CronExpression;
import com.serotonin.mango.rt.EventManager;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.rt.dataSource.http.HttpReceiverMulticaster;
import com.serotonin.mango.rt.event.type.SystemEventType;
import com.serotonin.mango.rt.maint.DataPurge;
import com.serotonin.mango.util.BackgroundContext;
import com.serotonin.mango.web.ContextWrapper;
import br.org.scadabr.vo.event.type.SystemEventKey;

import freemarker.cache.FileTemplateLoader;
import freemarker.cache.MultiTemplateLoader;
import freemarker.cache.TemplateLoader;
import freemarker.template.Configuration;
import freemarker.template.DefaultObjectWrapper;
import java.text.ParseException;
import java.util.Objects;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.inject.Inject;
import org.springframework.web.context.support.SpringBeanAutowiringSupport;

public class MangoContextListener implements ServletContextListener {

    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_CORE);

    @Inject
    private SystemSettingsDao systemSettingsDao;
    @Inject
    private DatabaseAccessFactory databaseAccessFactory;
    @Inject
    private RuntimeManager runtimeManager;
    @Inject
    private EventManager eventManager;
    @Inject
    private SchedulerPool schedulerPool;
            

    @Override
    public void contextInitialized(ServletContextEvent evt) {
        LOG.info("Mango context starting");

        // Get a handle on the context.
        ServletContext ctx = evt.getServletContext();
        try {
            JsL10N jsL10N = new JsL10N();
            jsL10N.init(ctx.getResource("/"));
        } catch (IOException ex) {
            throw new RuntimeException(ex);
        }
        
        
        // Create the common reference to the context
        Common.ctx = new ContextWrapper(ctx);

        SpringBeanAutowiringSupport.processInjectionBasedOnCurrentContext(this);

        // Create all the stuff we need.
//TODO        freemarkerInitialize(ctx);
//TODO        imageSetInitialize(ctx);
        databaseAccessFactory.startDB();

        // Check if the known servlet context path has changed.
        String knownContextPath = systemSettingsDao.getServletContextPath();
        if (knownContextPath != null) {
            String contextPath = ctx.getContextPath();
            if (!Objects.equals(knownContextPath, contextPath)) {
                LOG.log(Level.SEVERE, "Mango''s known servlet context path has changed from {0} to {1}. Are there two instances of Mango running?", new Object[]{knownContextPath, contextPath});
            }
        }

        systemSettingsDao.setServletContextPath(ctx.getContextPath());

        utilitiesInitialize(ctx);
        eventManager.initialize();
        runtimeManagerInitialize(ctx);
        maintenanceInitialize();
        // Notify the event manager of the startup.
        new SystemEventType(SystemEventKey.SYSTEM_STARTUP).fire("event.system.startup");

        LOG.info("Mango context started");
    }

    @Override
    public void contextDestroyed(ServletContextEvent evt) {
        LOG.info("Mango context terminating");

        if (eventManager != null) {
            // Notify the event manager of the shutdown.
            new SystemEventType(SystemEventKey.SYSTEM_SHUTDOWN).fire("event.system.shutdown");
        }
        //Logout the User
        runtimeManager.getUserSessionContextBeans().stream().forEach((us) -> {
            us.systemShutdown();
        });
        Logger.getLogger(MangoContextListener.class.getName()).log(Level.INFO, "Shutdown Event created");

        // Get a handle on the context.
        ContextWrapper ctx = new ContextWrapper(evt.getServletContext());

        // Stop everything.
        terminateRuntimeManager();
        Logger.getLogger(MangoContextListener.class.getName()).log(Level.INFO, "RuntimeManger terminated");
        terminateEventManager();
        Logger.getLogger(MangoContextListener.class.getName()).log(Level.INFO, "EventManager terminated");
        utilitiesTerminate(ctx);
        Logger.getLogger(MangoContextListener.class.getName()).log(Level.INFO, "utilitues terminated");


        if (databaseAccessFactory != null) {
            databaseAccessFactory.stopDB();
            databaseAccessFactory = null;
            LOG.info("Database terminated");
        }

        Common.ctx = null;

        LOG.info("Mango context terminated");
    }

    //
    //
    // Utilities.
    //
    private void utilitiesInitialize(ServletContext ctx) {
        // HTTP receiver multicaster
        ctx.setAttribute(Common.ContextKeys.HTTP_RECEIVER_MULTICASTER, new HttpReceiverMulticaster());

    }

    private void utilitiesTerminate(ContextWrapper ctx) {
    }

    private void terminateEventManager() {
        if (eventManager != null) {
            eventManager.terminate();
            eventManager.joinTermination();
        }
    }

    //
    //
    // Runtime manager
    //
    private void runtimeManagerInitialize(ServletContext ctx) {

        File safeFile = null;
        // Check for safe mode.
        try {
            String s = ctx.getRealPath("/SAFE");
            if (s != null) {
                safeFile = new File(s);
            }
        } catch (Throwable t) {
            LOG.log(Level.SEVERE, "Save file ", t);
        }
        boolean safe = false;
        if (safeFile != null) {
            if (safeFile.exists() && safeFile.isFile()) {
                // Indicate that we're in safe mode.
                StringBuilder sb = new StringBuilder();
                sb.append("\r\n");
                sb.append("*********************************************************\r\n");
                sb.append("*                    NOTE                               *\r\n");
                sb.append("*********************************************************\r\n");
                sb.append("* ScadaBR is starting in safe mode. All data sources,   *\r\n");
                sb.append("* point links, scheduled events, compound events, and   *\r\n");
                sb.append("* publishers will be disabled. To disable safe mode,    *\r\n");
                sb.append("* remove the SAFE file from the ScadaBR application     *\r\n");
                sb.append("* directory.                                            *\r\n");
                sb.append("*                                                       *\r\n");
                sb.append("* To find all objects that were automatically disabled, *\r\n");
                sb.append("* search for Audit Events on the alarms page.           *\r\n");
                sb.append("*********************************************************");
                LOG.warning(sb.toString());
                safe = true;
            }
        }
        try {
            if (safe) {
                BackgroundContext.set("common.safeMode");
            }
            runtimeManager.initialize();
        } catch (Exception e) {
            LOG.log(Level.SEVERE, "RuntimeManager initialization failure", e);
        } finally {
            if (safe) {
                BackgroundContext.remove();
            }
        }
    }

    private void terminateRuntimeManager() {
        if (runtimeManager != null) {
            runtimeManager.terminate();
            runtimeManager.joinTermination();
        }
    }

    //
    //
    // Freemarker
    //
    private void freemarkerInitialize(ServletContext ctx) {
        Configuration cfg = new Configuration();
        try {
            List<TemplateLoader> loaders = new ArrayList<>();

            // Add the override template dir
            try {
                loaders.add(new FileTemplateLoader(new File(ctx
                        .getRealPath("/WEB-INF/ftl-override"))));
            } catch (FileNotFoundException e) {
                // ignore
            }

            // Add the default template dir
            loaders.add(new FileTemplateLoader(new File(ctx
                    .getRealPath("/WEB-INF/ftl"))));

            cfg.setTemplateLoader(new MultiTemplateLoader(loaders
                    .toArray(new TemplateLoader[loaders.size()])));
        } catch (IOException e) {
            LOG.log(Level.SEVERE, "Exception defining Freemarker template directories", e);
        }
        cfg.setObjectWrapper(new DefaultObjectWrapper());
        ctx.setAttribute(Common.ContextKeys.FREEMARKER_CONFIG, cfg);
    }

    //
    //
    // Maintenance processes
    //
    private void maintenanceInitialize() {
        // Processes are scheduled in the timer, so they are canceled when it
        // stops.
        try {
            DataPurge.DataPurgeTask dpt = new DataPurge.DataPurgeTask("0 0 */5 * * * * *", CronExpression.TIMEZONE_UTC);
            System.err.println("CONTEXT DATA PURGE");
            schedulerPool.schedule(dpt);
        } catch (ParseException e) {
            throw new RuntimeException(e);
        }
        // The version checking job reschedules itself after each execution so
        // that requests from the various Mango
        // instances even out over time.

        //TODO ask serotoninsoftware for new versions ??? Not anymore ;-) VersionCheck.start("0 0 0 0 * * * *");
        // new WorkItemMonitor();

        // MemoryCheck.start();
    }
}
