package br.org.scadabr.rt.scripting;

import java.util.List;
import java.util.Map;

import javax.script.ScriptEngine;
import javax.script.ScriptEngineManager;
import javax.script.ScriptException;

import br.org.scadabr.rt.scripting.context.ScriptContextObject;
import br.org.scadabr.vo.scripting.ContextualizedScriptVO;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.db.IntValuePair;
import br.org.scadabr.utils.TimePeriods;
import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.UserDao;
import com.serotonin.mango.rt.dataImage.IDataPoint;
import com.serotonin.mango.rt.dataSource.meta.AlphanumericPointWrapper;
import com.serotonin.mango.rt.dataSource.meta.BinaryPointWrapper;
import com.serotonin.mango.rt.dataSource.meta.DataPointStateException;
import com.serotonin.mango.rt.dataSource.meta.MultistatePointWrapper;
import com.serotonin.mango.rt.dataSource.meta.NumericPointWrapper;
import com.serotonin.mango.rt.dataSource.meta.ScriptExecutor;
import com.serotonin.mango.rt.dataSource.meta.WrapperContext;
import com.serotonin.mango.vo.User;
import javax.script.Invocable;

public class ContextualizedScriptRT extends ScriptRT {

    private static final String SCRIPT_PREFIX = "function __scriptExecutor__() {";
    private static final String SCRIPT_SUFFIX = "\r\n}";

    private static String SCRIPT_FUNCTION_PATH;
    private static String FUNCTIONS;

    public static void setScriptFunctionPath(String path) {
        SCRIPT_FUNCTION_PATH = path;
    }

    public ContextualizedScriptRT(ContextualizedScriptVO vo) {
        super(vo);
    }

    @Override
    public void execute() throws ScriptException {
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
        WrapperContext wrapperContext = new WrapperContext(System
                .currentTimeMillis());

        // Add constants to the context.
        engine.put("SECOND", TimePeriods.SECONDS.mangoDbId);
        engine.put("MINUTE", TimePeriods.MINUTES.mangoDbId);
        engine.put("HOUR", TimePeriods.HOURS.mangoDbId);
        engine.put("DAY", TimePeriods.DAYS.mangoDbId);
        engine.put("WEEK", TimePeriods.WEEKS.mangoDbId);
        engine.put("MONTH", TimePeriods.MONTHS.mangoDbId);
        engine.put("YEAR", TimePeriods.YEARS.mangoDbId);
        engine.put("CONTEXT", wrapperContext);
        Map<String, IDataPoint> context = null;

        try {

            context = new ScriptExecutor()
                    .convertContext(((ContextualizedScriptVO) vo)
                            .getPointsOnContext());

        } catch (DataPointStateException e1) {
            e1.printStackTrace();
            throw new ScriptException(e1.getMessage());
        }

        // Put the context variables into the engine with engine scope.
        for (String varName : context.keySet()) {
            IDataPoint point = context.get(varName);
            switch (point.getDataType()) {
                case BINARY:
                    engine.put(varName, new BinaryPointWrapper(point,
                            wrapperContext));
                    break;
                case MULTISTATE:
                    engine.put(varName, new MultistatePointWrapper(point,
                            wrapperContext));
                    break;
                case NUMERIC:
                    engine.put(varName, new NumericPointWrapper(point,
                            wrapperContext));
                    break;
                case ALPHANUMERIC:
                    engine.put(varName, new AlphanumericPointWrapper(point,
                            wrapperContext));
                    break;
                default:
                    throw new ShouldNeverHappenException("Unknown data type id: "
                            + point.getDataType());
            }
        }

        List<IntValuePair> objectsContext = ((ContextualizedScriptVO) vo)
                .getObjectsOnContext();

        User user = UserDao.getInstance().getUser(vo.getUserId());
        for (IntValuePair object : objectsContext) {
            ScriptContextObject o = ScriptContextObject.Type.valueOf(
                    object.getKey()).createScriptContextObject();
            o.setUser(user);
            engine.put(object.getValue(), o);
        }

        // Create the script.
        String script = SCRIPT_PREFIX + getScript() + SCRIPT_SUFFIX;

        // Execute.
        Object result;
        try {
            engine.eval(script);
            Invocable inv = (Invocable) engine;
            result = inv.invokeFunction("__scriptExecutor__");
        } catch (NoSuchMethodException e) {
            throw new RuntimeException(e);
        } catch (ScriptException e) {
            throw e;
        }

        // Check if a timestamp was set
    }
}
