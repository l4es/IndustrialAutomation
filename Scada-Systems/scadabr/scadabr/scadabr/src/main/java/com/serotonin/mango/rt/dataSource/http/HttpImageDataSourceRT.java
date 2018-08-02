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
package com.serotonin.mango.rt.dataSource.http;

import br.org.scadabr.utils.ImplementMeException;
import java.io.ByteArrayOutputStream;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpStatus;
import org.apache.commons.httpclient.methods.GetMethod;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import br.org.scadabr.io.StreamUtils;
import br.org.scadabr.rt.RT;
import br.org.scadabr.rt.SchedulerPool;
import br.org.scadabr.rt.WrongEdgeTypeException;
import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.timer.cron.SystemRunnable;
import com.serotonin.mango.Common;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataSource.PollingDataSource;
import com.serotonin.mango.vo.dataSource.http.HttpImageDataSourceVO;
import br.org.scadabr.utils.i18n.LocalizableException;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.vo.EdgeType;
import br.org.scadabr.vo.VO;
import br.org.scadabr.vo.datasource.http.HttpImageDataSourceEventKey;
import java.text.ParseException;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;
import br.org.scadabr.vo.EdgeConsumer;

/**
 * @author Matthew Lohbihler
 */
@Configurable
public class HttpImageDataSourceRT extends PollingDataSource<HttpImageDataSourceVO> {

    static final Log LOG = LogFactory.getLog(HttpImageDataSourceRT.class);

    public static final int DATA_RETRIEVAL_FAILURE_EVENT = 1;
    public static final int FILE_SAVE_EXCEPTION_EVENT = 2;

    @Autowired
    private Common common;
    @Autowired
    private SchedulerPool schedulerPool;
    
    public HttpImageDataSourceRT(HttpImageDataSourceVO vo) {
        super(vo, true);
        setPollingPeriod(vo.getUpdatePeriodType(), vo.getUpdatePeriods(), false);
    }

    @Override
    public void setPointValue(DataPointRT dataPoint, PointValueTime valueTime, VO<?> source) {
        // no op
    }

    @Override
    public void doPoll(long time) {
        updateChangedPoints();
        ImageRetrieverMonitor monitor = new ImageRetrieverMonitor();

        // Add all of the retrievers to the monitor.
        for (DataPointRT dp : enabledDataPoints.values()) {
            ImageRetriever retriever = new ImageRetriever(monitor, dp, time);
            monitor.addRetriever(retriever);
        }

        while (!monitor.isEmpty()) {
            synchronized (monitor) {
                try {
                    monitor.wait(1000);
                } catch (InterruptedException e) {
                    // no op
                }
            }
        }

        // Check the results.
        if (monitor.getRetrievalFailure() != null) {
            raiseAlarm(HttpImageDataSourceEventKey.DATA_RETRIEVAL_FAILURE, time, monitor.getRetrievalFailure());
        } else {
             clearAlarm(HttpImageDataSourceEventKey.DATA_RETRIEVAL_FAILURE, time);
        }

        if (monitor.getSaveFailure() != null) {
            raiseAlarm(HttpImageDataSourceEventKey.FILE_SAVE_EXCEPTION, time, monitor.getSaveFailure());
        } else {
            clearAlarm(HttpImageDataSourceEventKey.FILE_SAVE_EXCEPTION, time);
        }
    }

    @Override
    public void wireEdgeAsSrc(RT<?> dest, EdgeType edgeType) throws WrongEdgeTypeException {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void wireEdgeAsDest(RT<?> src, EdgeType edgeType) throws WrongEdgeTypeException {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void iterateEdgesAsSrc(EdgeConsumer edgeIterator, EdgeType... edgeTypes) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void iterateEdgesAsDest(EdgeConsumer edgeIterator, EdgeType... edgeTypes) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void patch(HttpImageDataSourceVO vo) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public HttpImageDataSourceVO getVO() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    class ImageRetrieverMonitor {

        private final List<ImageRetriever> retrievers = new ArrayList<>();
        private LocalizableMessage retrievalFailure;
        private LocalizableMessage saveFailure;

        synchronized void addRetriever(ImageRetriever retriever) {
            retrievers.add(retriever);
            schedulerPool.execute(retriever);
        }

        synchronized void removeRetriever(ImageRetriever retriever) {
            retrievers.remove(retriever);

            if (retrievalFailure == null && retriever.getRetrievalFailure() != null) {
                retrievalFailure = retriever.getRetrievalFailure();
            }

            if (saveFailure == null && retriever.getSaveFailure() != null) {
                saveFailure = retriever.getSaveFailure();
            }

            if (retrievers.isEmpty()) {
                    notifyAll();
            }
        }

        public LocalizableMessage getRetrievalFailure() {
            return retrievalFailure;
        }

        public LocalizableMessage getSaveFailure() {
            return saveFailure;
        }

        public boolean isEmpty() {
            return retrievers.isEmpty();
        }
    }

    class ImageRetriever implements SystemRunnable {

        private final ImageRetrieverMonitor monitor;
        private final DataPointRT dp;
        private final long time;
        private LocalizableMessage retrievalFailure;
        private LocalizableMessage saveFailure;

        ImageRetriever(ImageRetrieverMonitor monitor, DataPointRT dp, long time) {
            this.monitor = monitor;
            this.dp = dp;
            this.time = time;
        }

        @Override
        public void run() {
            try {
                executeImpl();
            } finally {
                monitor.removeRetriever(this);
            }
        }

        private void executeImpl() {
            throw new ImplementMeException();
            /*
            HttpImagePointLocatorVO vo = (HttpImagePointLocatorVO)dp.getVo().getPointLocator();

            byte[] data;
            try {
                data = getData(vo.getUrl(), vo.getTimeoutSeconds(), vo.getRetries(), vo.getReadLimit());
            } catch (Exception e) {
                if (e instanceof LocalizableException) {
                    retrievalFailure = ((LocalizableException) e);
                } else {
                    retrievalFailure = new LocalizableMessageImpl("event.httpImage.retrievalError", vo.getUrl(),
                            e.getMessage());
                }
                LOG.info("Error retrieving page '" + vo.getUrl() + "'", e);
                return;
            }

            try {
                if (vo.getScaleType() == HttpImagePointLocatorVO.Scale.PERCENT) {
                    // Percentage scale the image.
                    PercentScaledImage scaler = new PercentScaledImage(((float) vo.getScalePercent()) / 100);
                    data = ImageUtils.scaleImage(scaler, data, new JpegImageFormat(0.85f));
                } else if (vo.getScaleType() == HttpImagePointLocatorVO.Scale.BOX) {
                    // Box scale the image.
                    BoxScaledImage scaler = new BoxScaledImage(vo.getScaleWidth(), vo.getScaleHeight());
                    data = ImageUtils.scaleImage(scaler, data, new JpegImageFormat(0.85f));
                }
            } catch (Exception e) {
                saveFailure = new LocalizableMessageImpl("event.httpImage.scalingError", e.getMessage());
                LOG.info("Error scaling image", e);
                return;
            }

            // Save the new image
            try {
                dp.updatePointValue(new ImageValueTime(data, ImageValue.TYPE_JPG, dp.getId(), time));
            } catch (ImageSaveException e) {
                saveFailure = new LocalizableMessageImpl("event.httpImage.saveError", e.getMessage());
                LOG.info("Error saving image data", e);
            }
            */
        }

        public LocalizableMessage getRetrievalFailure() {
            return retrievalFailure;
        }

        public LocalizableMessage getSaveFailure() {
            return saveFailure;
        }
/*
        @Override
        public int getPriority() {
            return WorkItem.PRIORITY_HIGH;
        }
        */
    }

    public byte[] getData(String url, int timeoutSeconds, int retries, int readLimitKb)
            throws LocalizableException {
        byte[] data;
        while (true) {
            HttpClient client = common.getHttpClient(timeoutSeconds * 1000);
            GetMethod method = null;
            LocalizableException lEx;

            try {
                method = new GetMethod(url);
                int responseCode = client.executeMethod(method);

                if (responseCode == HttpStatus.SC_OK) {
                    ByteArrayOutputStream baos = new ByteArrayOutputStream();
                    StreamUtils.transfer(method.getResponseBodyAsStream(), baos, readLimitKb * 1024);
                    data = baos.toByteArray();
                    break;
                }
                lEx = new LocalizableException("event.http.response", url, responseCode);
            } catch (Exception e) {
                lEx = PollingDataSource.wrapException(e);
            } finally {
                if (method != null) {
                    method.releaseConnection();
                }
            }

            if (retries <= 0) {
                throw lEx;
            }
            retries--;

            // Take a little break instead of trying again immediately.
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                // no op
            }
        }

        return data;
    }

    @Override
    protected CronExpression getCronExpression() throws ParseException {
        throw new ImplementMeException();
    }

}
