/*
 *   Mango - Open Source M2M - http://mango.serotoninsoftware.com
 *   Copyright (C) 2010 Arne Pl\u00f6se
 *   @author Arne Pl\u00f6se
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package br.org.scadabr.vo.datasource.fhz4j;

import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.rt.datasource.fhz4j.Fhz4JDataSourceRT;
import br.org.scadabr.vo.dataSource.PointLocatorVO;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import com.serotonin.mango.rt.dataSource.DataSourceRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.util.ExportCodes;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.event.EventTypeVO;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;
import java.util.Set;
import java.util.logging.Logger;
import net.sf.atmodem4j.spsw.SerialPortList;

import net.sf.fhz4j.Fhz1000;
import net.sf.fhz4j.FhzProtocol;

public class Fhz4JDataSourceVO extends DataSourceVO<Fhz4JDataSourceVO> {

    private final static int MAX_FHZ_ADDR = Fhz1000.parseHouseCode("9999");
    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCARABR_DS_FHZ4J);
    private static final ExportCodes EVENT_CODES = new ExportCodes();

    static {
        EVENT_CODES.addElement(Fhz4JDataSourceRT.DATA_SOURCE_EXCEPTION_EVENT, "DATA_SOURCE_EXCEPTION_EVENT");
        EVENT_CODES.addElement(Fhz4JDataSourceRT.POINT_READ_EXCEPTION_EVENT, "POINT_READ_EXCEPTION");
        EVENT_CODES.addElement(Fhz4JDataSourceRT.POINT_WRITE_EXCEPTION_EVENT, "POINT_WRITE_EXCEPTION");
    }

    private String commPort;
    private short fhzHousecode = (short) 0x1234;
    private boolean fhtMaster;

    @Override
    public Type getType() {
        return Type.FHZ_4_J;
    }

    @Override
    protected void addEventTypes(List<EventTypeVO> eventTypes) {
        eventTypes.add(createEventType(Fhz4JDataSourceRT.DATA_SOURCE_EXCEPTION_EVENT, new LocalizableMessageImpl("event.ds.dataSource")));
        eventTypes.add(createEventType(Fhz4JDataSourceRT.POINT_READ_EXCEPTION_EVENT, new LocalizableMessageImpl("event.ds.pointRead")));
        eventTypes.add(createEventType(Fhz4JDataSourceRT.POINT_WRITE_EXCEPTION_EVENT, new LocalizableMessageImpl("event.ds.pointWrite")));
    }

    @Override
    public LocalizableMessage getConnectionDescription() {
        return new LocalizableMessageImpl("common.default", commPort);
    }

    @Override
    public PointLocatorVO createPointLocator() {
        return new Fhz4JPointLocatorVO();
    }

    public Fhz4JPointLocatorVO<?> createPointLocator(FhzProtocol fhzProtocol) {
        switch (fhzProtocol) {
            case FS20:
                return new Fhz4JPointLocatorVO(new FS20PointLocator());
            case EM:
                return new Fhz4JPointLocatorVO(new EmPointLocator());
            case FHT:
                return new Fhz4JPointLocatorVO(new FhtPointLocator());
            case HMS:
                return new Fhz4JPointLocatorVO(new HmsPointLocator());
            case FHT_MULTI_MSG:
                return new Fhz4JPointLocatorVO(new FhtMultiMsgPointLocator());
            default:
                throw new RuntimeException("Unknown protocol");
        }
    }

    @Override
    public DataSourceRT createDataSourceRT() {
        LOG.severe("FHZ DS RT created");
        return new Fhz4JDataSourceRT(this);
    }

    @Override
    public ExportCodes getEventCodes() {
        return EVENT_CODES;
    }

    @Override
    protected void addPropertiesImpl(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.fhz4j.port", commPort);
        AuditEventType.addPropertyMessage(list, "dsEdit.fhz4j.fhzHousecode", fhzHousecode);
        AuditEventType.addPropertyMessage(list, "dsEdit.fhz4j.fhtMaster", fhtMaster);
    }

    @Override
    protected void addPropertyChangesImpl(List<LocalizableMessage> list, Fhz4JDataSourceVO from) {
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.fhz4j.port", from.commPort, commPort);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.fhz4j.fhzHousecode", from.fhzHousecode, fhzHousecode);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.fhz4j.fhtMaster", from.fhtMaster, fhtMaster);
    }

    public String getCommPort() {
        return commPort;
    }

    public void setCommPort(String commPort) {
        this.commPort = commPort;
    }

    @Override
    public void validate(DwrResponseI18n response) {
        super.validate(response);

        if (commPort.isEmpty()) {
            response.addContextual("commPortId", "validate.required");
        }
    }
    //
    // /
    // / Serialization
    // /
    //
    private static final long serialVersionUID = -1;
    private static final int SERIAL_VERSION = 1;

    // Serialization for saveDataSource
    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(SERIAL_VERSION);
        out.writeUTF(commPort);
        out.writeShort(fhzHousecode);
        out.writeBoolean(fhtMaster);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        switch (ver) {
            case 1:
                commPort = in.readUTF();
                fhzHousecode = in.readShort();
                fhtMaster = in.readBoolean();
                break;
            default:
                throw new RuntimeException("Cant read object from stream");
        }
    }

    public Set<String> getCommPorts() {
        return SerialPortList.getPortNames(false);
    }

    /**
     * @return the fhzHousecode
     */
    public short getFhzHousecode() {
        return fhzHousecode;
    }

    /**
     * @return the fhzHousecode as String
     */
    public String getFhzHousecodeStr() {
        return Fhz1000.houseCodeToString(fhzHousecode);
    }

    /**
     * @param fhzHousecode the housecode ot this FHZ to set
     */
    public void setFhzHousecode(short fhzHousecode) {
        this.fhzHousecode = fhzHousecode;
    }

    /**
     * @return the fhtMaster
     */
    public boolean isFhtMaster() {
        return fhtMaster;
    }

    /**
     * @param fhtMaster the fhtMaster to set
     */
    public void setFhtMaster(boolean fhtMaster) {
        this.fhtMaster = fhtMaster;
    }

    //TODO helper for JSP
    public FhzProtocol[] getFhzProtocols() {
        return FhzProtocol.values();
    }

    public void setFhzHousecode(String fhzHousecode) {
        this.fhzHousecode = Fhz1000.parseHouseCode(fhzHousecode);
    }

}
