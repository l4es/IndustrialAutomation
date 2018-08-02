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
package com.serotonin.mango.vo.dataSource.meta;

import br.org.scadabr.DataType;
import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.dao.DataPointDao;
import br.org.scadabr.db.IntValuePair;
import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.timer.cron.CronParser;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.datasource.PointLocatorVO;
import br.org.scadabr.vo.datasource.meta.CronPattern;
import br.org.scadabr.vo.datasource.meta.UpdateEvent;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonView;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataSource.meta.MetaPointLocatorRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.dataSource.AbstractPointLocatorVO;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.TimeZone;
import javax.validation.constraints.Min;
import javax.validation.constraints.NotNull;
import javax.validation.constraints.Size;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

/**
 * @author Matthew Lohbihler
 */
@Configurable
@CronPattern
//TODO @InputVariables
public class MetaPointLocatorVO<T extends PointValueTime> extends AbstractPointLocatorVO<T> {

    @Autowired
    private DataPointDao dataPointDao;

    private List<IntValuePair> context = new ArrayList<>();

    @NotNull
    @Size(min = 9) // stands for mininimum text required "return 0;"
    private String script;

    private boolean settable;
    private UpdateEvent updateEvent = UpdateEvent.CONTEXT_UPDATE;

    private String updateCronPattern;

    @Min(0)
    private int executionDelaySeconds;

    public MetaPointLocatorVO() {
        super();
    }

    public MetaPointLocatorVO(DataType dataType) {
        super(dataType);
        if (dataType != DataType.DOUBLE) {
            throw new ImplementMeException();
        }
    }

    @Override
    public MetaPointLocatorRT createRuntime() {
        return new MetaPointLocatorRT(this);
    }

    @Override
    public LocalizableMessage getConfigurationDescription() {
        if (script == null || script.length() < 40) {
            return new LocalizableMessageImpl("common.default", "'" + script + "'");
        } else {
            return new LocalizableMessageImpl("common.default", "'" + script.substring(0, 40) + "'");
        }
    }

    public List<IntValuePair> getContext() {
        return context;
    }

    public void setContext(List<IntValuePair> context) {
        this.context = context;
    }

    public String getScript() {
        return script;
    }

    public void setScript(String script) {
        this.script = script;
    }

    public int getExecutionDelaySeconds() {
        return executionDelaySeconds;
    }

    public void setExecutionDelaySeconds(int executionDelaySeconds) {
        this.executionDelaySeconds = executionDelaySeconds;
    }

    @Override
    public boolean isSettable() {
        return settable;
    }

    public void setSettable(boolean settable) {
        this.settable = settable;
    }

    public UpdateEvent getUpdateEvent() {
        return updateEvent;
    }

    public List<UpdateEvent> getUpdateEvents() {
        return Arrays.asList(UpdateEvent.values());
    }

    public void setUpdateEvent(UpdateEvent updateEvent) {
        this.updateEvent = updateEvent;
    }

    public String getUpdateCronPattern() {
        return updateCronPattern;
    }

    public void setUpdateCronPattern(String updateCronPattern) {
        this.updateCronPattern = updateCronPattern;
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        super.addProperties(list);
        AuditEventType.addPropertyMessage(list, "dsEdit.settable", settable);
        AuditEventType.addPropertyMessage(list, "dsEdit.meta.scriptContext", contextToString());
        AuditEventType.addPropertyMessage(list, "dsEdit.meta.script", script);
        AuditEventType.addPropertyMessage(list, "dsEdit.meta.event", updateEvent);
        if (updateEvent == UpdateEvent.CRON) {
            AuditEventType.addPropertyMessage(list, "dsEdit.meta.event.cron", updateCronPattern);
        }
        AuditEventType.addPropertyMessage(list, "dsEdit.meta.delay", executionDelaySeconds);
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, PointLocatorVO<T> o) {
        super.addProperties(list);
        final MetaPointLocatorVO<T> from = (MetaPointLocatorVO<T>) o;
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.settable", from.settable, settable);
        if (!context.equals(context)) {
            AuditEventType.addPropertyChangeMessage(list, "dsEdit.meta.scriptContext", from.contextToString(),
                    contextToString());
        }
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.meta.script", from.script, script);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.meta.event", from.updateEvent, updateEvent);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.meta.event.cron", from.updateCronPattern, updateCronPattern);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.meta.delay", from.executionDelaySeconds,
                executionDelaySeconds);
    }

    private String contextToString() {
        StringBuilder sb = new StringBuilder();
        boolean first = true;
        for (IntValuePair ivp : context) {
            DataPointVO dp = dataPointDao.getDataPoint(ivp.getKey());
            if (first) {
                first = false;
            } else {
                sb.append(", ");
            }

            if (dp == null) {
                sb.append("?=");
            } else {
                sb.append(dp.getName()).append("=");
            }
            sb.append(ivp.getValue());
        }
        return sb.toString();
    }

    //
    //
    // Serialization
    //
    private static final long serialVersionUID = -1;
    private static final int version = 1;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);
        out.writeObject(context);
        out.writeObject(script);
        out.writeBoolean(settable);
        out.writeInt(updateEvent.getId());
        out.writeObject(updateCronPattern);
        out.writeInt(executionDelaySeconds);
    }

    @SuppressWarnings("unchecked")
    private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
        final int ver = in.readInt();
        switch (ver) {
            case 1:
                context = (List<IntValuePair>) in.readObject();
                script = (String)in.readObject();
                settable = in.readBoolean();
                updateEvent = UpdateEvent.fromId(in.readInt());
                updateCronPattern = (String)in.readObject();
                executionDelaySeconds = in.readInt();
                break;
            default:
                throw new ShouldNeverHappenException("Version mismatch");

        }
    }

    @JsonIgnore
    public boolean isScriptEmpty() {
        return script == null ? true : script.isEmpty();
    }

    @JsonIgnore
    public CronExpression getCronExpression() {
        try {
            switch (updateEvent) {
                case CONTEXT_UPDATE:
                    throw new ShouldNeverHappenException("Context update has no cron pattern");
                case CRON:
                    return new CronParser().parse(updateCronPattern, TimeZone.getTimeZone("UTC"));
                default:
                    return new CronParser().parse(updateEvent.getCronPattern(), TimeZone.getTimeZone("UTC"));
            }
        } catch (ParseException pe) {
            throw new ShouldNeverHappenException(pe.getMessage());
        }
    }
}
