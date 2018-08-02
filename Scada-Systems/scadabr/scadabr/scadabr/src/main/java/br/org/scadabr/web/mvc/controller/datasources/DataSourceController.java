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
import br.org.scadabr.web.l10n.RequestContextAwareLocalizer;
import br.org.scadabr.web.mvc.AjaxFormPostResponse;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import java.util.Set;
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
@RequestMapping("/dataSources/dataSource")
public class DataSourceController {

    @Inject
    private Validator validator;
    @Inject
    private DataSourceDao dataSourceDao;

    @ModelAttribute("dataSource")
    protected DataSourceVO getModel(int id) {
        return dataSourceDao.getDataSource(id);
    }

    @RequestMapping(params = "id", method = RequestMethod.GET)
    public String getDataSourceView(int id) {
        return "dataSources/dataSource";
    }

    @RequestMapping(params = "id", method = RequestMethod.POST)
    public @ResponseBody AjaxFormPostResponse<DataSourceVO> postDataSource(@ModelAttribute("dataSource") DataSourceVO dataSource) {
        Set<ConstraintViolation<DataSourceVO>> constraintViolations = validator.validate(dataSource);
        if (constraintViolations.isEmpty()) {
            dataSourceDao.saveDataSource(dataSource);
        }
        return new AjaxFormPostResponse<>(dataSource, constraintViolations);
    }

}
