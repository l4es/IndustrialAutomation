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
package br.org.scadabr.vo.datasource.vmstat;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;


import com.serotonin.mango.rt.dataSource.DataSourceRT;
import br.org.scadabr.rt.datasource.vmstat.VMStatDataSourceRT;
import br.org.scadabr.rt.event.type.DuplicateHandling;
import br.org.scadabr.vo.event.AlarmLevel;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.rt.event.type.EventType;
import com.serotonin.mango.util.ExportCodes;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.event.EventTypeVO;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.web.i18n.LocalizableI18nKey;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;

/**
 * @author Matthew Lohbihler
 */

public class VMStatDataSourceVO extends DataSourceVO<VMStatDataSourceVO> {

    @Override
    protected void addEventTypes(List<EventTypeVO> ets) {
        ets.add(createEventType(VMStatDataSourceRT.DATA_SOURCE_EXCEPTION_EVENT, new LocalizableMessageImpl(
                "event.ds.dataSource"), DuplicateHandling.IGNORE_SAME_MESSAGE, AlarmLevel.URGENT));
        ets.add(createEventType(VMStatDataSourceRT.PARSE_EXCEPTION_EVENT, new LocalizableMessageImpl(
                "event.ds.dataParse")));
    }

    private static final ExportCodes EVENT_CODES = new ExportCodes();

    static {
        EVENT_CODES.addElement(VMStatDataSourceRT.DATA_SOURCE_EXCEPTION_EVENT, "DATA_SOURCE_EXCEPTION");
        EVENT_CODES.addElement(VMStatDataSourceRT.PARSE_EXCEPTION_EVENT, "PARSE_EXCEPTION");
    }

    @Override
    public ExportCodes getEventCodes() {
        return EVENT_CODES;
    }

    public enum OutputScale implements LocalizableI18nKey {

        NONE("dsEdit.vmstat.scale.none"),
        LOWER_K("dsEdit.vmstat.scale.k"),
        UPPER_K("dsEdit.vmstat.scale.K"),
        LOWER_M("dsEdit.vmstat.scale.m"),
        UPPER_M("dsEdit.vmstat.scale.M");

        public final String i18nKey;

        private OutputScale(String i18nKey) {
            this.i18nKey = i18nKey;
        }

        @Override
        public String getI18nKey() {
            return i18nKey;
        }

        public String getName() {
            return name();
        }

    }

    @Override
    public Type getType() {
        return Type.VMSTAT;
    }

    @Override
    public LocalizableMessage getConnectionDescription() {
        return new LocalizableMessageImpl("dsEdit.vmstat.dsconn", pollSeconds);
    }

    @Override
    public DataSourceRT createDataSourceRT() {
        return new VMStatDataSourceRT(this);
    }

    @Override
    public VMStatPointLocatorVO createPointLocator() {
        return new VMStatPointLocatorVO();
    }

    private int pollSeconds = 3;
    private OutputScale outputScale = OutputScale.NONE;

    public int getPollSeconds() {
        return pollSeconds;
    }

    public void setPollSeconds(int pollSeconds) {
        this.pollSeconds = pollSeconds;
    }

    public OutputScale getOutputScale() {
        return outputScale;
    }

    public void setOutputScale(OutputScale outputScale) {
        this.outputScale = outputScale;
    }

    @Override
    public void validate(DwrResponseI18n response) {
        super.validate(response);
        if (pollSeconds < 1) {
            response.addContextual("pollSeconds", "validate.greaterThanZero", pollSeconds);
        }
    }

    @Override
    protected void addPropertiesImpl(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.vmstat.pollSeconds", pollSeconds);
        AuditEventType.addPropertyMessage(list, "dsEdit.vmstat.outputScale", outputScale);
    }

    @Override
    protected void addPropertyChangesImpl(List<LocalizableMessage> list, VMStatDataSourceVO from) {
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.vmstat.pollSeconds", from.pollSeconds, pollSeconds);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.vmstat.outputScale", from.outputScale, outputScale);
    }

    //
    // /
    // / Serialization
    // /
    //
    private static final long serialVersionUID = -1;
    private static final int version = 2;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);
        out.writeInt(pollSeconds);
        out.writeUTF(outputScale.name());
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        switch (ver) {
            case 1:
                pollSeconds = in.readInt();
                final int osValue = in.readInt();
                outputScale = OutputScale.values()[osValue - 1];
                break;
            case 2:
                pollSeconds = in.readInt();
                outputScale = OutputScale.valueOf(in.readUTF());
                break;
            default:
                throw new RuntimeException(String.format("Version %d not supported", ver));
        }

    }
}
