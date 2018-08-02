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




import br.org.scadabr.vo.datasource.DataSourcesRegistry;
import javax.inject.Inject;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

@Controller
@Scope("request")
@RequestMapping("/dataSources")
public class DataSourcesController {
    
    @Inject
    private DataSourcesRegistry dataSourcesRegistry;

    @RequestMapping(method = RequestMethod.GET)
    public String initializeForm() {
        return "dataSources";
    }

    @ModelAttribute
    protected void getModel(Model model) {
        model.addAttribute("dataSourceTypes", dataSourcesRegistry.getDataSourceTypes());
//        model.addAttribute("defaultDataSourceType", DataSourceVO.Type.M_BUS.name());
    }
    
    
}
