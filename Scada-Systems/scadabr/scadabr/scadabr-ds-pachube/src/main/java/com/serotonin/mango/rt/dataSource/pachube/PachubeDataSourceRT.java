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
package com.serotonin.mango.rt.dataSource.pachube;

import br.org.scadabr.utils.ImplementMeException;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TimeZone;

import org.apache.commons.httpclient.DefaultHttpMethodRetryHandler;
import org.apache.commons.httpclient.Header;
import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpStatus;
import org.apache.commons.httpclient.methods.GetMethod;
import org.apache.commons.httpclient.methods.PutMethod;
import org.apache.commons.httpclient.methods.StringRequestEntity;
import org.apache.commons.httpclient.params.HttpMethodParams;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.json.JsonArray;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonReader;
import br.org.scadabr.json.JsonValue;
import br.org.scadabr.timer.cron.CronExpression;
import com.serotonin.mango.Common;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.SetPointSource;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.dataSource.DataSourceUtils;
import com.serotonin.mango.rt.dataSource.PollingDataSource;
import com.serotonin.mango.vo.dataSource.pachube.PachubeDataSourceVO;
import br.org.scadabr.web.http.HttpUtils;
import br.org.scadabr.utils.i18n.LocalizableException;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import java.util.Objects;

public class PachubeDataSourceRT extends PollingDataSource<PachubeDataSourceVO> {

    public static final int DATA_RETRIEVAL_FAILURE_EVENT = 1;
    public static final int PARSE_EXCEPTION_EVENT = 2;
    public static final int POINT_WRITE_EXCEPTION_EVENT = 3;

    public static final String HEADER_API_KEY = "X-PachubeApiKey";

    final Log log = LogFactory.getLog(PachubeDataSourceRT.class);
    private final HttpClient httpClient;
    final SimpleDateFormat sdf;

    public PachubeDataSourceRT(PachubeDataSourceVO vo) {
        super(vo, true);
        setPollingPeriod(vo.getUpdatePeriodType(), vo.getUpdatePeriods(), false);

        httpClient = createHttpClient(vo.getTimeoutSeconds(), vo.getRetries());

        sdf = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss'Z'");
        sdf.setTimeZone(TimeZone.getTimeZone("UTC"));
    }

    public static HttpClient createHttpClient(int timeoutSeconds, int retries) {
        HttpClient httpClient = Common.getHttpClient(timeoutSeconds * 1000);
        httpClient.getParams().setParameter(HttpMethodParams.RETRY_HANDLER,
                new DefaultHttpMethodRetryHandler(retries, true));
        return httpClient;
    }
    /*
     @Override
     public void addDataPoint(DataPointRT dataPoint) {
     super.addDataPoint(dataPoint);
     dataPoint.setAttribute(ATTR_UNRELIABLE_KEY, true);
     }

     @Override
     public void removeDataPoint(DataPointRT dataPoint) {
     returnToNormal(PARSE_EXCEPTION_EVENT, System.currentTimeMillis());
     super.removeDataPoint(dataPoint);
     }
     */

    @Override
    public void forcePointRead(DataPointRT dataPoint) {
        PachubePointLocatorRT locator = dataPoint.getPointLocator();
        List<DataPointRT> point = new ArrayList<>(1);
        point.add(dataPoint);
        pollFeed(locator.getFeedId(), point, System.currentTimeMillis());
    }

    @Override
    public void doPoll(long time) {
        throw new ImplementMeException();
        /*
         Map<Integer, List<DataPointRT>> devicePoints = new HashMap<>();

         synchronized (pointListChangeLock) {
         for (DataPointRT dp : dataPoints) {
         PachubePointLocatorRT locator = dp.getPointLocator();

         List<DataPointRT> points = devicePoints.get(locator.getFeedId());
         if (points == null) {
         points = new ArrayList<>();
         devicePoints.put(locator.getFeedId(), points);
         }

         points.add(dp);
         }
         }

         for (Map.Entry<Integer, List<DataPointRT>> entry : devicePoints.entrySet()) {
         pollFeed(entry.getKey(), entry.getValue(), time);
         }
         */
    }

    protected void pollFeed(int feedId, List<DataPointRT> points, long time) {
        Map<String, PachubeValue> data;

        try {
            data = getData(httpClient, feedId, vo.getApiKey());
        } catch (Exception e) {
            LocalizableMessage lm;
            if (e instanceof LocalizableException) {
                lm = (LocalizableException) e;
            } else {
                lm = new LocalizableMessageImpl("event.pachube.feed.retrievalError", feedId, e.getMessage());
            }
            raiseEvent(DATA_RETRIEVAL_FAILURE_EVENT, time, true, lm);

            // Mark points as unreliable.
            for (DataPointRT point : points) {
                point.setAttribute(ATTR_UNRELIABLE_KEY, true);
            }

            return;
        }

        // If we made it this far, everything is good.
        returnToNormal(DATA_RETRIEVAL_FAILURE_EVENT, time);

        // We have the data. Now run the regex.
        LocalizableMessage parseErrorMessage = null;
        for (DataPointRT dp : points) {
            PachubePointLocatorRT locator = dp.getPointLocator();
            PachubeValue dataValue = data.get(locator.getDataStreamId());

            if (dataValue == null) {
                parseErrorMessage = new LocalizableMessageImpl("event.pachube.dataStreamNotFound",
                        locator.getDataStreamId(), feedId);
                dp.setAttribute(ATTR_UNRELIABLE_KEY, true);
            } else {
                try {
                    // Get the value
                    MangoValue value = DataSourceUtils.getValue(dataValue.getValue(), locator.getDataType(),
                            locator.getBinary0Value(), dp.getVo().getTextRenderer(), null, dp.getVoName());

                    // Get the time.
                    long valueTime;
                    if (dataValue.getTimestamp().isEmpty()) {
                        valueTime = time;
                    } else {
                        valueTime = sdf.parse(dataValue.getTimestamp()).getTime();
                    }

                    // Create the pvt
                    PointValueTime pvt = new PointValueTime(value, valueTime);

                    // Save the new value if it is new
                    if (!Objects.equals(dp.getPointValue(), pvt)) {
                        dp.updatePointValue(new PointValueTime(value, valueTime));
                    }
                    dp.setAttribute(ATTR_UNRELIABLE_KEY, false);
                } catch (LocalizableException e) {
                    if (parseErrorMessage == null) {
                        parseErrorMessage = e;
                    }
                    dp.setAttribute(ATTR_UNRELIABLE_KEY, true);
                } catch (ParseException e) {
                    if (parseErrorMessage == null) {
                        parseErrorMessage = new LocalizableMessageImpl("event.valueParse.timeParsePoint",
                                dataValue.getTimestamp(), dp.getVoName());
                    }
                    dp.setAttribute(ATTR_UNRELIABLE_KEY, true);
                }
            }
        }

        if (parseErrorMessage != null) {
            raiseEvent(PARSE_EXCEPTION_EVENT, time, false, parseErrorMessage);
        } else {
            returnToNormal(PARSE_EXCEPTION_EVENT, time);
        }
    }

    public static Map<String, PachubeValue> getData(HttpClient client, int feedId, String apiKey)
            throws LocalizableException {
        GetMethod method = null;

        try {
            method = new GetMethod("http://www.pachube.com/api/feeds/" + feedId + ".json");
            method.addRequestHeader(new Header(HEADER_API_KEY, apiKey));
            method.addRequestHeader("User-Agent", "ScadaBR Pachube data source");

            int responseCode = client.executeMethod(method);
            if (responseCode != HttpStatus.SC_OK) {
                throw new LocalizableException("event.pachube.feed.response", feedId, responseCode);
            }

            String json = HttpUtils.readResponseBody(method);

            Map<String, PachubeValue> result = new HashMap<>();

            JsonReader reader = new JsonReader(json);
            JsonObject content = reader.inflate().toJsonObject();
            JsonArray streams = content.getJsonArray("datastreams");
            if (streams != null) {
                for (JsonValue element : streams.getElements()) {
                    JsonObject stream = element.toJsonObject();
                    String id = stream.getString("id");
                    JsonArray values = stream.getJsonArray("values");
                    if (values != null && !values.getElements().isEmpty()) {
                        JsonObject sample = values.getElements().get(0).toJsonObject();
                        result.put(id, new PachubeValue(sample.getString("value"), sample.getString("recorded_at")));
                    }
                }
            }

            return result;
        } catch (LocalizableException e) {
            throw e;
        } catch (Exception e) {
            throw wrapException(e);
        } finally {
            if (method != null) {
                method.releaseConnection();
            }
        }
    }

    @Override
    public void setPointValue(DataPointRT dataPoint, PointValueTime valueTime, SetPointSource source) {
        PachubePointLocatorRT pl = dataPoint.getPointLocator();

        try {
            PutMethod method = new PutMethod("http://www.pachube.com/api/feeds/" + pl.getFeedId() + "/datastreams/"
                    + pl.getDataStreamId() + ".csv");
            method.addRequestHeader(new Header(HEADER_API_KEY, vo.getApiKey()));

            try {
                method.setRequestEntity(new StringRequestEntity(valueTime.getValue().toString(), "text/csv", "UTF-8"));
            } catch (UnsupportedEncodingException e) {
                throw new ShouldNeverHappenException(e);
            }

            HttpUtils.execute(httpClient, method);
            dataPoint.setPointValue(valueTime, source);

            // Deactivate any existing event.
            returnToNormal(POINT_WRITE_EXCEPTION_EVENT, valueTime.getTime());
        } catch (IOException e) {
            // Raise an event.
            raiseEvent(POINT_WRITE_EXCEPTION_EVENT, valueTime.getTime(), true, new LocalizableMessageImpl(
                    "event.exception2", dataPoint.getVoName(), e.getMessage()));
        }
    }

    @Override
    protected CronExpression getCronExpression() throws ParseException {
        throw new ImplementMeException();
    }
}
