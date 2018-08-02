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
package br.org.scadabr.web.mvc.controller.datasources;

import br.org.scadabr.dao.DataSourceDao;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.vo.datasource.PointLocatorVO;
import br.org.scadabr.web.l10n.RequestContextAwareLocalizer;
import br.org.scadabr.web.mvc.AjaxFormPostResponse;
import java.util.Set;
import java.util.logging.Logger;
import javax.inject.Inject;
import javax.validation.ConstraintViolation;
import javax.validation.Validator;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.ResponseBody;

@Controller
@Scope("request")
@RequestMapping("/dataSources/pointLocator")
public class PointLocatorController {

    private static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_WEB);


    @Inject
    private Validator validator;
    @Inject
    private DataSourceDao dataSourceDao;
    @Inject
    private transient RequestContextAwareLocalizer localizer;

    @ModelAttribute("pointLocator")
    protected PointLocatorVO getModel(int id) {
        return dataSourceDao.getPointLocator(id);
    }

    @RequestMapping(params = "id", method = RequestMethod.GET)
    public String getPointLocatorView(@ModelAttribute("pointLocator") PointLocatorVO pointLocator) {
        return "dataSources/" + pointLocator.getClass().getSimpleName();
    }

    @RequestMapping(params = "id", method = RequestMethod.POST)
    public @ResponseBody
    AjaxFormPostResponse<PointLocatorVO> postPointLocator(@ModelAttribute("pointLocator") PointLocatorVO pointLocator) {
        LOG.severe("POINTLOCATOR: " + pointLocator.getClass().getName());
        Set<ConstraintViolation<PointLocatorVO>> constraintViolations = validator.validate(pointLocator);
        if (constraintViolations.isEmpty()) {
            dataSourceDao.savePointLocator(pointLocator);
        }

        return new AjaxFormPostResponse<>(pointLocator, constraintViolations);
    }

}
