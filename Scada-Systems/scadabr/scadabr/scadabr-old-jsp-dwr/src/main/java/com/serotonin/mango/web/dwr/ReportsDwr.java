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
package com.serotonin.mango.web.dwr;

import java.util.List;

import br.org.scadabr.InvalidArgumentException;
import br.org.scadabr.l10n.AbstractLocalizer;
import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.timer.cron.CronParser;
import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.MailingListDao;
import com.serotonin.mango.db.dao.ReportDao;
import com.serotonin.mango.db.dao.WatchListDao;
import com.serotonin.mango.rt.maint.work.ReportWorkItem;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.User;
import com.serotonin.mango.vo.WatchList;
import com.serotonin.mango.vo.permission.Permissions;
import com.serotonin.mango.vo.report.ReportInstance;
import com.serotonin.mango.vo.report.ReportTask;
import com.serotonin.mango.vo.report.ReportPointVO;
import com.serotonin.mango.vo.report.ReportVO;
import com.serotonin.mango.vo.event.RecipientListEntryBean;
import br.org.scadabr.util.ColorUtils;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.l10n.Localizer;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.utils.TimePeriods;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;
import javax.inject.Inject;

/**
 * @author Matthew Lohbihler
 */
public class ReportsDwr extends BaseDwr {

    @Inject
    private ReportDao reportDao; 
    @Inject
    private MailingListDao mailingListDao;
    @Inject
    private WatchListDao watchListDao;
    
    public DwrResponseI18n init() {
        DwrResponseI18n response = new DwrResponseI18n();
        User user = Common.getUser();

        response.addData("points", getReadablePoints());
        response.addData("mailingLists", mailingListDao.getMailingLists());
        response.addData("users", userDao.getUsers());
        response.addData("reports", reportDao.getReports(user.getId()));
        response.addData("instances", getReportInstances(user));

        return response;
    }

    public ReportVO getReport(int id, boolean copy) {
        ReportVO report;
        if (id == Common.NEW_ID) {
            report = new ReportVO();
            report.setName(getMessage("common.newName"));
        } else {
            report = reportDao.getReport(id);

            if (copy) {
                report.setId(Common.NEW_ID);
                report.setName(AbstractLocalizer.localizeI18nKey("common.copyPrefix", getResourceBundle(), report.getName()));
            }

            Permissions.ensureReportPermission(Common.getUser(), report);
        }
        return report;
    }

    public DwrResponseI18n saveReport(int id, String name, List<ReportPointVO> points, int includeEvents,
            boolean includeUserComments, int dateRangeType, int relativeDateType, int previousPeriodCount,
            TimePeriods previousPeriodType, int pastPeriodCount, TimePeriods pastPeriodType, boolean fromNone, int fromYear,
            int fromMonth, int fromDay, int fromHour, int fromMinute, boolean toNone, int toYear, int toMonth,
            int toDay, int toHour, int toMinute, boolean schedule, TimePeriods schedulePeriod, int runDelayMinutes,
            String scheduleCron, boolean email, boolean includeData, boolean zipData,
            List<RecipientListEntryBean> recipients) {

        DwrResponseI18n response = new DwrResponseI18n();

        // Basic validation
        validateData(response, name, points, dateRangeType, relativeDateType, previousPeriodCount, pastPeriodCount);

        if (schedule) {
            if (true) throw new ImplementMeException(); //"Dirty things happend here ... WAS: if (schedulePeriod == ReportVO.SCHEDULE_CRON) {");
            if (ReportVO.isCronScheduled(schedulePeriod)) {
                // Check the cron pattern.
                try {
                    new CronParser().parse(scheduleCron, CronExpression.TIMEZONE_UTC);
                } catch (ParseException e) {
                    response.addContextual("scheduleCron", "reports.validate.cron", e);
                }
            } else {
                if (runDelayMinutes < 0) {
                    response.addContextual("runDelayMinutes", "reports.validate.lessThan0");
                } else if (runDelayMinutes > 59) {
                    response.addContextual("runDelayMinutes", "reports.validate.greaterThan59");
                }
            }
        }

        if (schedule && email && recipients.isEmpty()) {
            response.addContextual("recipients", "reports.validate.needRecip");
        }

        if (!response.isEmpty()) {
            return response;
        }

        User user = Common.getUser();
        ReportVO report;
        if (id == Common.NEW_ID) {
            report = new ReportVO();
            report.setUserId(user.getId());
        } else {
            report = reportDao.getReport(id);
        }

        Permissions.ensureReportPermission(user, report);

        // Update the new values.
        report.setName(name);
        report.setPoints(points);
        report.setIncludeEvents(includeEvents);
        report.setIncludeUserComments(includeUserComments);
        report.setDateRangeType(dateRangeType);
        report.setRelativeDateType(relativeDateType);
        report.setPreviousPeriodCount(previousPeriodCount);
        report.setPreviousPeriodType(previousPeriodType);
        report.setPastPeriodCount(pastPeriodCount);
        report.setPastPeriodType(pastPeriodType);
        report.setFromNone(fromNone);
        report.setFromYear(fromYear);
        report.setFromMonth(fromMonth);
        report.setFromDay(fromDay);
        report.setFromHour(fromHour);
        report.setFromMinute(fromMinute);
        report.setToNone(toNone);
        report.setToYear(toYear);
        report.setToMonth(toMonth);
        report.setToDay(toDay);
        report.setToHour(toHour);
        report.setToMinute(toMinute);
        report.setSchedule(schedule);
        report.setSchedulePeriod(schedulePeriod);
        report.setRunDelayMinutes(runDelayMinutes);
        report.setScheduleCron(scheduleCron);
        report.setEmail(email);
        report.setIncludeData(includeData);
        report.setZipData(zipData);
        report.setRecipients(recipients);

        // Save the report
        reportDao.saveReport(report);

        // Conditionally schedule the report.
        ReportTask.scheduleReportJob(report);

        // Send back the report id in case this was new.
        response.addData("reportId", report.getId());
        return response;
    }

    public DwrResponseI18n runReport(String name, List<ReportPointVO> points, int includeEvents,
            boolean includeUserComments, int dateRangeType, int relativeDateType, int previousPeriodCount,
            TimePeriods previousPeriodType, int pastPeriodCount, TimePeriods pastPeriodType, boolean fromNone, int fromYear,
            int fromMonth, int fromDay, int fromHour, int fromMinute, boolean toNone, int toYear, int toMonth,
            int toDay, int toHour, int toMinute, boolean email, boolean includeData, boolean zipData,
            List<RecipientListEntryBean> recipients) {
        DwrResponseI18n response = new DwrResponseI18n();

        // Basic validation
        validateData(response, name, points, dateRangeType, relativeDateType, previousPeriodCount, pastPeriodCount);

        if (response.isEmpty()) {
            ReportVO report = new ReportVO();
            report.setName(name);
            report.setUserId(Common.getUser().getId());
            report.setPoints(points);
            report.setIncludeEvents(includeEvents);
            report.setIncludeUserComments(includeUserComments);
            report.setDateRangeType(dateRangeType);
            report.setRelativeDateType(relativeDateType);
            report.setPreviousPeriodCount(previousPeriodCount);
            report.setPreviousPeriodType(previousPeriodType);
            report.setPastPeriodCount(pastPeriodCount);
            report.setPastPeriodType(pastPeriodType);
            report.setFromNone(fromNone);
            report.setFromYear(fromYear);
            report.setFromMonth(fromMonth);
            report.setFromDay(fromDay);
            report.setFromHour(fromHour);
            report.setFromMinute(fromMinute);
            report.setToNone(toNone);
            report.setToYear(toYear);
            report.setToMonth(toMonth);
            report.setToDay(toDay);
            report.setToHour(toHour);
            report.setToMinute(toMinute);
            report.setEmail(email);
            report.setIncludeData(includeData);
            report.setZipData(zipData);
            report.setRecipients(recipients);

            ReportWorkItem.queueReport(report);
        }

        return response;
    }

    public void deleteReport(int id) {

        ReportVO report = reportDao.getReport(id);
        if (report != null) {
            Permissions.ensureReportPermission(Common.getUser(), report);
            ReportTask.unscheduleReportJob(report);
            reportDao.deleteReport(id);
        }
    }

    private void validateData(DwrResponseI18n response, String name, List<ReportPointVO> points, int dateRangeType,
            int relativeDateType, int previousPeriodCount, int pastPeriodCount) {
        if (name.isEmpty()) {
            response.addContextual("name", "reports.validate.required");
        }
        if (name.length() > 100) {
            response.addContextual("name", "reports.validate.longerThan100");
        }
        if (points.isEmpty()) {
            response.addContextual("points", "reports.validate.needPoint");
        }
        if (dateRangeType != ReportVO.DATE_RANGE_TYPE_RELATIVE && dateRangeType != ReportVO.DATE_RANGE_TYPE_SPECIFIC) {
            response.addGeneric("reports.validate.invalidDateRangeType");
        }
        if (relativeDateType != ReportVO.RELATIVE_DATE_TYPE_PAST
                && relativeDateType != ReportVO.RELATIVE_DATE_TYPE_PREVIOUS) {
            response.addGeneric("reports.validate.invalidRelativeDateType");
        }
        if (previousPeriodCount < 1) {
            response.addContextual("previousPeriodCount", "reports.validate.periodCountLessThan1");
        }
        if (pastPeriodCount < 1) {
            response.addContextual("pastPeriodCount", "reports.validate.periodCountLessThan1");
        }

        User user = Common.getUser();
        for (ReportPointVO point : points) {
            Permissions.ensureDataPointReadPermission(user, dataPointDao.getDataPoint(point.getPointId()));

            try {
                if (!point.getColour().isEmpty()) {
                    ColorUtils.toColor(point.getColour());
                }
            } catch (InvalidArgumentException e) {
                response.addContextual("points", "reports.validate.colour", point.getColour());
            }
        }
    }

    public List<Map<String, Object>> deleteReportInstance(int instanceId) {
        User user = Common.getUser();
        reportDao.deleteReportInstance(instanceId, user.getId());
        return getReportInstances(user);
    }

    public List<Map<String, Object>> getReportInstances() {
        return getReportInstances(Common.getUser());
    }

    private List<Map<String, Object>> getReportInstances(User user) {
        final List<ReportInstance> ris = reportDao.getReportInstances(user.getId());
        final List<Map<String, Object>> result = new ArrayList<>(ris.size());
        final Locale locale = getLocale();
        for (ReportInstance ri : ris) {
            final Map<String, Object> entry = new HashMap<>();
            entry.put("id", ri.getId());
            entry.put("name", ri.getName());
            entry.put("reportStartTime", AbstractLocalizer.localizeTimeStamp(ri.getReportStartTime(), true, locale));
            entry.put("reportEndTime", AbstractLocalizer.localizeTimeStamp(ri.getReportEndTime(), true, locale));
            entry.put("runStartTime", AbstractLocalizer.localizeTimeStamp(ri.getRunStartTime(), true, locale));
            entry.put("runEndTime", AbstractLocalizer.localizeTimeStamp(ri.getRunEndTime(), true, locale));
            entry.put("state", ri.getState());
            entry.put("runDuration", String.format("%d s %d ms", ri.getRunDuration() / 1000, ri.getRunDuration() % 1000));
            entry.put("recordCount", ri.getRecordCount());
            entry.put("includeEvents", ri.getIncludeEvents());
            entry.put("includeUserComments", ri.isIncludeUserComments());
            result.add(entry);
        }
        return result;
    }

    public void setPreventPurge(int instanceId, boolean value) {
        reportDao.setReportInstancePreventPurge(instanceId, value, Common.getUser().getId());
    }

    public ReportVO createReportFromWatchlist(int watchListId) {
        WatchList watchList = watchListDao.getWatchList(watchListId);
        if (watchList == null) {
            return null;
        }

        ReportVO report = new ReportVO();
        report.setName(AbstractLocalizer.localizeI18nKey("common.copyPrefix", getResourceBundle(), watchList.getName()));
        for (DataPointVO dp : watchList.getPointList()) {
            ReportPointVO rp = new ReportPointVO();
            rp.setPointId(dp.getId());
            rp.setColour(dp.getChartColour());
            rp.setConsolidatedChart(true);
            report.getPoints().add(rp);
        }

        return report;
    }

    /**
     * @return the reportDao
     */
    public ReportDao getReportDao() {
        return reportDao;
    }

    /**
     * @param reportDao the reportDao to set
     */
    public void setReportDao(ReportDao reportDao) {
        this.reportDao = reportDao;
    }

    /**
     * @return the mailingListDao
     */
    public MailingListDao getMailingListDao() {
        return mailingListDao;
    }

    /**
     * @param mailingListDao the mailingListDao to set
     */
    public void setMailingListDao(MailingListDao mailingListDao) {
        this.mailingListDao = mailingListDao;
    }

    /**
     * @return the watchListDao
     */
    public WatchListDao getWatchListDao() {
        return watchListDao;
    }

    /**
     * @param watchListDao the watchListDao to set
     */
    public void setWatchListDao(WatchListDao watchListDao) {
        this.watchListDao = watchListDao;
    }

}
