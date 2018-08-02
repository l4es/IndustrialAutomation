/*
 Mango - Open Source M2M - http://mango.serotoninsoftware.com
 Copyright (C) 2006-2009 Serotonin Software Technologies Inc.
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
package br.org.scadabr.web.mvc.controller.rest;

import br.org.scadabr.dao.EventDao;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.web.l10n.RequestContextAwareLocalizer;
import com.serotonin.mango.web.UserSessionContextBean;
import br.org.scadabr.web.mvc.controller.jsonrpc.JsonEventInstance;
import java.util.logging.Logger;
import javax.inject.Inject;
import org.springframework.context.annotation.Scope;
import org.springframework.web.bind.annotation.PathVariable;

import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

@RestController
@Scope("request")
@RequestMapping(value = "/rest/events/")
public class RestEventsController {

    private static final Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_WEB);

    @Inject
    private EventDao eventDao;

    @Inject
    private RequestContextAwareLocalizer localizer;

    @Inject
    private UserSessionContextBean userSessionContextBean;

    //TODO timestamps as plain time in millis???
    //TODO localize event messages
    @RequestMapping(method = RequestMethod.GET)
    public Object getEvents() {
        return JsonEventInstance.wrap(eventDao.getPendingEvents(userSessionContextBean.getUser()), localizer);
    }

    //TODO timestamps as plain time in millis???
    //TODO localize event messages or not??
    @RequestMapping(value = "{id}", method = RequestMethod.GET)
    public JsonEventInstance getEvent(@PathVariable int id) {
        return JsonEventInstance.wrap(eventDao.getEventInstance(id), localizer);
    }
}
