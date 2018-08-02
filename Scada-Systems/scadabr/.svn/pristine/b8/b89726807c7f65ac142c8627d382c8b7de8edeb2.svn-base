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
package br.org.scadabr.web.mvc.controller.dataPoints;

import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.vo.datapoints.NumberDataPointVO;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.vo.DataPointVO;
import java.text.MessageFormat;
import java.util.Calendar;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.inject.Inject;
import javax.validation.Validator;


import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;

@Controller
@RequestMapping("/dataPointDetails/")
@Scope("request")
class DetailsControllerDP {

    private static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_WEB);

    @Inject
    private Validator validator;
    
    @Inject
    private RuntimeManager runtimeManager;

    public DetailsControllerDP() {
        super();
    }

    @RequestMapping(value = "editProperties", params = "id", method = RequestMethod.GET)
    protected String getEditProperties(@ModelAttribute("dataPoint") DataPointVO dpvo) throws Exception {
        LOG.severe(MessageFormat.format("getEditProperties called {0}", dpvo.getId()));
        return "dataPointDetails/" + dpvo.getClass().getSimpleName();
    }

    /*
    @RequestMapping(value = "editProperties", params = "id", method = RequestMethod.POST)
    protected @ResponseBody AjaxFormPostResponse<DataPointVO> postEditProperties(@ModelAttribute("dataPoint") DataPointVO dataPoint) throws BindException {
        LOG.log(Level.SEVERE, "postEditProperties called {0}", dataPoint);
        Set<ConstraintViolation<DataPointVO>> constraintViolations = validator.validate(dataPoint);
        if (constraintViolations.isEmpty()) {
            dataPointDao.saveDataPoint(dataPoint);
        }
        return new AjaxFormPostResponse<>(dataPoint, constraintViolations);
    }
*/
    @ModelAttribute
    protected void getModel(@RequestParam int id, Model model) {
        LOG.log(Level.SEVERE, "getModel called {0}", id);
        final DataPointVO dataPointVO = runtimeManager.getVo(id, DataPointVO.class);
        model.addAttribute("dataPoint", dataPointVO);
    }

    @RequestMapping(value = "renderChart", method = RequestMethod.GET)
    protected String getRenderChart(@RequestParam int id, Model model) throws Exception {
        LOG.log(Level.SEVERE, "getRenderChart called {0}", id);
        DataPointVO dp = (DataPointVO) model.asMap().get("dataPoint");
        // Determine our image chart rendering capabilities.
        Calendar c = Calendar.getInstance();
        model.addAttribute("toTimestamp", c.getTimeInMillis());
        if (dp instanceof NumberDataPointVO) {
            final NumberDataPointVO r = (NumberDataPointVO) dp;
            switch (r.getChartTimePeriods()) {
                case YEARS:
                    c.add(Calendar.YEAR, -r.getNumberOfChartPeriods());
                    break;
                case MONTHS:
                    c.add(Calendar.MONTH, -r.getNumberOfChartPeriods());
                    break;
                case WEEKS:
                    c.add(Calendar.DATE, -r.getNumberOfChartPeriods() * 7);
                    break;
                case DAYS:
                    c.add(Calendar.DATE, -r.getNumberOfChartPeriods());
                    break;
                case HOURS:
                    c.add(Calendar.HOUR_OF_DAY, -r.getNumberOfChartPeriods());
                    break;
                case MINUTES:
                    c.add(Calendar.MINUTE, -r.getNumberOfChartPeriods());
                    break;
                case SECONDS:
                    c.add(Calendar.SECOND, -r.getNumberOfChartPeriods());
                    break;
                case MILLISECONDS:
                    c.add(Calendar.MILLISECOND, -r.getNumberOfChartPeriods());
                    break;
                default:
                    throw new RuntimeException("Not implemented Yet!");
            }
        } else {
            c.add(Calendar.DATE, -1);
        }
        model.addAttribute("fromTimestamp", c.getTimeInMillis());
        return "dataPointDetails/renderChart";
    }

}
