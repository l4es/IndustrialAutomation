/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.dao;

import com.serotonin.mango.rt.event.EventInstance;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.report.ReportChartCreator;
import com.serotonin.mango.vo.report.ReportInstance;
import com.serotonin.mango.vo.report.ReportUserComment;
import com.serotonin.mango.vo.report.ReportVO;
import java.util.List;
import java.util.ResourceBundle;

/**
 *
 * @author aploese
 */
public interface ReportDao {

    Iterable<ReportVO> getReports();

    void reportInstanceData(int id, ReportChartCreator.StreamHandler handler);

    List<EventInstance> getReportInstanceEvents(int id);

    List<ReportUserComment> getReportInstanceUserComments(int id);

    int purgeReportsBefore(long millis);

    void saveReportInstance(ReportInstance reportInstance);

    PointInfo createPointInfo(DataPointVO point, String colour, boolean consolidatedChart);

    int runReport(ReportInstance reportInstance, List<PointInfo> points, ResourceBundle bundle);

    void deleteReportInstance(int id, int id0);

    public static interface PointInfo {

        DataPointVO getPoint();

        String getColour();

        boolean isConsolidatedChart();
    }

}
