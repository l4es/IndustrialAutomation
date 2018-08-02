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
package com.serotonin.mango.web;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;

import br.org.scadabr.ShouldNeverHappenException;
import com.serotonin.mango.Common;
import com.serotonin.mango.rt.dataSource.http.HttpReceiverMulticaster;
import com.serotonin.mango.util.DocumentationManifest;

import freemarker.template.Configuration;

@Deprecated // use Bean
public class ContextWrapper {

    private final ServletContext ctx;

    @Deprecated // use Bean
    public ContextWrapper(ServletContext ctx) {
        this.ctx = ctx;
    }

    @Deprecated // use Bean
    public ContextWrapper(HttpServletRequest request) {
        ctx = request.getSession().getServletContext();
    }

    @Deprecated // use Bean
    public Configuration getFreemarkerConfig() {
        return (Configuration) ctx
                .getAttribute(Common.ContextKeys.FREEMARKER_CONFIG);
    }

    @Deprecated // use Bean
    public HttpReceiverMulticaster getHttpReceiverMulticaster() {
        return (HttpReceiverMulticaster) ctx
                .getAttribute(Common.ContextKeys.HTTP_RECEIVER_MULTICASTER);
    }

    @Deprecated // use Bean
    public Integer getDataPointByName(String dataPointQualifiedName) {
        Map<String, Integer> mapping = (Map<String, Integer>) ctx
                .getAttribute(Common.ContextKeys.DATA_POINTS_NAME_ID_MAPPING);
        Integer dataPointId = mapping.get(dataPointQualifiedName);
        if (dataPointId == null) {
            return -1;
        } else {
            return dataPointId;
        }
    }

    @Deprecated // use Bean
    public ServletContext getServletContext() {
        return ctx;
    }

    @Deprecated // use Bean
    public DocumentationManifest getDocumentationManifest() {
        DocumentationManifest dm = (DocumentationManifest) ctx
                .getAttribute(Common.ContextKeys.DOCUMENTATION_MANIFEST);

        if (dm == null) {
            try {
                dm = new DocumentationManifest();
            } catch (Exception e) {
                throw new ShouldNeverHappenException(e);
            }
            ctx.setAttribute(Common.ContextKeys.DOCUMENTATION_MANIFEST, dm);
        }

        return dm;
    }
}
