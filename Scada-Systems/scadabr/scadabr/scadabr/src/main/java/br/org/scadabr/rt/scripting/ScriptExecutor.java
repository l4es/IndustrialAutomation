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
package br.org.scadabr.rt.scripting;

import br.org.scadabr.DataType;
import java.io.IOException;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.script.ScriptEngine;
import javax.script.ScriptEngineManager;
import javax.script.ScriptException;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.db.IntValuePair;
import br.org.scadabr.io.StreamUtils;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.utils.TimePeriods;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.rt.dataImage.AlphaNumericValueTime;
import com.serotonin.mango.rt.dataImage.BooleanValueTime;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.DoubleValueTime;
import com.serotonin.mango.rt.dataImage.IDataPoint;
import com.serotonin.mango.rt.dataImage.MultistateValueTime;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import java.io.InputStream;
import javax.script.Invocable;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

/**
 * @author Matthew Lohbihler
 */
@Configurable
public class ScriptExecutor {

    private static final String SCRIPT_PREFIX = "function __scriptExecutor__() {";
    private static final String SCRIPT_SUFFIX = "\r\n}";
    private String scriptFunctionPath = "scriptFunctions.js";
    private String functions;

    @Autowired
    private RuntimeManager runtimeManager;

    public ScriptExecutor() {
        loadFunctions();
    }

    public Map<String, IDataPoint> convertContext(List<IntValuePair> context) throws DataPointStateException {
        RuntimeManager rtm = runtimeManager;

        Map<String, IDataPoint> converted = new HashMap<>();
        for (IntValuePair contextEntry : context) {
            DataPointRT point = rtm.getDataPoint(contextEntry.getKey());
            if (point == null) {
                throw new DataPointStateException(contextEntry.getKey(), "event.meta.pointMissing");
            }
            converted.put(contextEntry.getValue(), point);
        }

        return converted;
    }

    public PointValueTime execute(String script, Map<String, IDataPoint> context, long runtime,
            int dataPointId, DataType dataType, long timestamp) throws ScriptException, ResultTypeException {

        // Create the script engine.
        ScriptEngineManager manager;
        try {
            manager = new ScriptEngineManager();
        } catch (Exception e) {
            throw new ScriptException(e);
        }
        ScriptEngine engine = manager.getEngineByName("JavaScript");
        // engine.getContext().setErrorWriter(new PrintWriter(System.err));
        // engine.getContext().setWriter(new PrintWriter(System.out));

        // Create the wrapper object context.
        WrapperContext wrapperContext = new WrapperContext(runtime);

        // Add constants to the context.
        engine.put("SECOND", TimePeriods.SECONDS.getId());
        engine.put("MINUTE", TimePeriods.MINUTES.getId());
        engine.put("HOUR", TimePeriods.HOURS.getId());
        engine.put("DAY", TimePeriods.DAYS.getId());
        engine.put("WEEK", TimePeriods.WEEKS.getId());
        engine.put("MONTH", TimePeriods.MONTHS.getId());
        engine.put("YEAR", TimePeriods.YEARS.getId());
        engine.put("CONTEXT", wrapperContext);

        // Put the context variables into the engine with engine scope.
        for (String varName : context.keySet()) {
            IDataPoint point = context.get(varName);
            DataType dt = point.getDataType();
            switch (dt) {
                case BOOLEAN:
                    engine.put(varName, new BooleanPointWrapper(point, wrapperContext));
                    break;
                case MULTISTATE:
                    engine.put(varName, new MultistatePointWrapper(point, wrapperContext));
                    break;
                case DOUBLE:
                    engine.put(varName, new DoublePointWrapper(point, wrapperContext));
                    break;
                case ALPHANUMERIC:
                    engine.put(varName, new AlphanumericPointWrapper(point, wrapperContext));
                    break;
                default:
                    throw new ShouldNeverHappenException("Unknown data type : " + point.getDataType());
            }
        }

        // Create the script.
        script = SCRIPT_PREFIX + script + SCRIPT_SUFFIX + functions;

        // Execute.
        Object result = null;
        try {
            engine.eval(script);
            Invocable inv = (Invocable) engine;
            result = inv.invokeFunction("__scriptExecutor__");
        } catch (NoSuchMethodException e) {
            throw new RuntimeException(e);
        } catch (ScriptException e) {
            throw prettyScriptMessage(e);
        }

        // Check if a timestamp was set
        Object ts = engine.get("TIMESTAMP");
        if (ts != null) {
            // Check the type of the object.
            if (ts instanceof Number) // Convert to long
            {
                timestamp = ((Number) ts).longValue();
            }
            // else if (ts instanceof ScriptableObject && "Date".equals(((ScriptableObject)ts).getClassName())) {
            // // A native date
            // // It turns out to be a crazy hack to try and get the value from a native date, and the Rhino source
            // // code FTP server is not responding, so, going to have to leave this for now.
            // }
        }

        if (result == null) {
            return null;
        } else if (result instanceof AbstractPointWrapper) {
            throw new ImplementMeException();
            //return ((AbstractPointWrapper) result).getValueImpl();
        } else {
            switch (dataType) {
                case BOOLEAN:
                    return new BooleanValueTime((Boolean) result, dataPointId, timestamp);
                case MULTISTATE:
                    return new MultistateValueTime(((Number) result).byteValue(), dataPointId, timestamp);
                case DOUBLE:
                    return new DoubleValueTime(((Number) result).doubleValue(), dataPointId, timestamp);
                case ALPHANUMERIC:
                    return new AlphaNumericValueTime((String) result, dataPointId, timestamp);
                default:
                    throw new ResultTypeException("event.script.convertError", result, dataType);
            }
        }
    }

    public static ScriptException prettyScriptMessage(ScriptException e) {
        while (e.getCause() instanceof ScriptException) {
            e = (ScriptException) e.getCause();
        }

        // Try to make the error message look a bit nicer.
        List<String> exclusions = new ArrayList<>();
        exclusions.add("sun.org.mozilla.javascript.internal.EcmaError: ");
        exclusions.add("sun.org.mozilla.javascript.internal.EvaluatorException: ");
        String message = e.getMessage();
        for (String exclude : exclusions) {
            if (message.startsWith(exclude)) {
                message = message.substring(exclude.length());
            }
        }
        return new ScriptException(message, e.getFileName(), e.getLineNumber(), e.getColumnNumber());
    }

    private void loadFunctions() {
        StringWriter sw = new StringWriter();
        try (InputStream is = ScriptExecutor.class.getResourceAsStream(scriptFunctionPath)) {
            StreamUtils.transfer(is, sw);
        } catch (IOException e) {
            throw new ShouldNeverHappenException(e);
        }
        functions = sw.toString();
    }

    public static void validateScript(String script, List<IntValuePair> context, int dataPointId, DataType dataType) {

        ScriptExecutor executor = new ScriptExecutor();
        try {
            Map<String, IDataPoint> convertedContext = executor.convertContext(context);
            PointValueTime pvt = executor.execute(script, convertedContext,
                    System.currentTimeMillis(), dataPointId, dataType, -1);
            if (pvt.getTimestamp() == -1) {
//                response.addContextual("script", "dsEdit.meta.test.success", pvt.getValue());
            } else {
//                response.addContextual("script", "dsEdit.meta.test.successTs", pvt.getValue(), new Date(pvt.getTime()));
            }
        } catch (DataPointStateException e) {
//            response.addContextual("context", e);
        } catch (ScriptException e) {
//            response.addContextual("script", "dsEdit.meta.test.scriptError", e);
        } catch (ResultTypeException e) {
//            response.addContextual("script", e);
        }

//        return response;
    }

}
