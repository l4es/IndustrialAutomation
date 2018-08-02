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
package com.serotonin.mango.view.component;

import br.org.scadabr.DataType;
import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.rt.scripting.ScriptExecutor;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Map;

import javax.script.ScriptEngine;
import javax.script.ScriptEngineManager;
import javax.script.ScriptException;

import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.view.ImplDefinition;
import com.serotonin.mango.vo.DataPointVO;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.utils.ImplementMeException;
import com.serotonin.mango.view.text.TextRenderer;
import java.util.EnumSet;

/**
 * @author Matthew Lohbihler
 */
public class ScriptComponent<T extends PointValueTime> extends PointComponent<T> {

    public static ImplDefinition DEFINITION = new ImplDefinition("script", "SCRIPT", "graphic.script",
            EnumSet.of(DataType.BOOLEAN, DataType.MULTISTATE, DataType.DOUBLE, DataType.ALPHANUMERIC));

    private static final String SCRIPT_PREFIX = "function __scriptRenderer__() {";
    private static final String SCRIPT_SUFFIX = "\r\n}\r\n__scriptRenderer__();";

    private String script;

    @Override
    public String snippetName() {
        return "scriptContent";
    }

    public String getScript() {
        return script;
    }

    public void setScript(String script) {
        this.script = script;
    }

    @Override
    public void addDataToModel(Map<String, Object> model, T value) {
        String result;

        if (value == null) {
            result = "--";
        } else {
            // Create the script engine.
            ScriptEngineManager manager = new ScriptEngineManager();
            ScriptEngine engine = manager.getEngineByName("js");

            DataPointVO point = tgetDataPoint();

            // Put the values into the engine scope.
            engine.put("value", value.getValue());
            engine.put("htmlText", getHtmlText(point, value));
            engine.put("renderedText", getRenderedText(point, value));
            engine.put("time", value.getTimestamp());
            engine.put("pointComponent", this);
            engine.put("point", point);
            if (true) {
                throw new ImplementMeException();
            }
            /* TODO
             // Copy properties from the model into the engine scope.
             engine.put(BaseDwr.MODEL_ATTR_EVENTS, model.get(BaseDwr.MODEL_ATTR_EVENTS));
             engine.put(BaseDwr.MODEL_ATTR_HAS_UNACKED_EVENT, model.get(BaseDwr.MODEL_ATTR_HAS_UNACKED_EVENT));
             engine.put(BaseDwr.MODEL_ATTR_RESOURCE_BUNDLE, model.get(BaseDwr.MODEL_ATTR_RESOURCE_BUNDLE));
             */
            // Create the script.
            String evalScript = SCRIPT_PREFIX + script + SCRIPT_SUFFIX;

            // Execute.
            try {
                Object o = engine.eval(evalScript);
                if (o == null) {
                    result = null;
                } else {
                    result = o.toString();
                }
            } catch (ScriptException e) {
                e = ScriptExecutor.prettyScriptMessage(e);
                result = e.getMessage();
            }
        }

        model.put("scriptContent", result);
    }

    @Override
    public ImplDefinition definition() {
        return DEFINITION;
    }

    //
    // /
    // / Serialization
    // /
    //
    private static final long serialVersionUID = -1;
    private static final int version = 1;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);

        SerializationHelper.writeSafeUTF(out, script);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            script = SerializationHelper.readSafeUTF(in);
        }
    }

    public static String getHtmlText(DataPointVO point, PointValueTime pointValue) {
                throw new ImplementMeException();
        /* TODO half localized data...

        if (point == null) {
            return "-";
        }
        if (point.getId() != point.getId()) {
            throw new ShouldNeverHappenException("Point and pointvalue mismatch for ScriptComponent");
        }
        String text = point.getTextRenderer().getText(pointValue, TextRenderer.HINT_FULL);
        String colour = point.getTextRenderer().getColour(pointValue);
        return getHtml(colour, text, point.getDataType() == DataType.ALPHANUMERIC);
*/
    }

    private static String getHtml(String colour, String text, boolean detectOverflow) {

        if (text != null && detectOverflow && text.length() > 30) {
            text = encodeDQuot(text);
            if (colour == null || colour.isEmpty()) {
                return "<input type='text' readonly='readonly' class='ovrflw' value=\"" + text + "\"/>";
            } else {
                return "<input type='text' readonly='readonly' class='ovrflw' style='color:" + colour + ";' value=\""
                        + text + "\"/>";
            }
        } else if (colour == null || colour.isEmpty()) {
            return text;
        } else {
            return "<span style='color:" + colour + ";'>" + text + "</span>";
        }
    }

    public static String getRenderedText(DataPointVO point, PointValueTime pointValue) {
        throw new ImplementMeException();
        /* TODO half localized data...
        if (point == null) {
            return "-";
        }
        return point.getTextRenderer().getText(pointValue, TextRenderer.HINT_FULL);
*/
    }

    public static String encodeDQuot(String s) {
        return s.replaceAll("\"", "&quot;");
    }

}
