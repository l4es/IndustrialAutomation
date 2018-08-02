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
package br.org.scadabr.web.mvc.controller;

import br.org.scadabr.logger.LogUtils;
import java.text.MessageFormat;
import java.util.logging.Logger;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.servlet.mvc.ParameterizableViewController;

@Controller
@RequestMapping("/help")
@Scope("request")
public class HelpController extends ParameterizableViewController {

    private static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_WEB);

    @RequestMapping(params = "id", method = RequestMethod.GET)
    protected String getHelpById(@RequestParam String id) throws Exception {
        LOG.severe(MessageFormat.format("getHelpById called {0}", id));
    //TODO hardcoded Language to en...
        return "help/en/" + id;
    }

    @RequestMapping(method = RequestMethod.GET)
    protected String getHelp() throws Exception {
        return "help";
    }
}
