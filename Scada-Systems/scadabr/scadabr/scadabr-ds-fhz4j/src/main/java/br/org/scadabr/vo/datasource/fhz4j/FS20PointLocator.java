/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package br.org.scadabr.vo.datasource.fhz4j;

import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import com.serotonin.mango.rt.event.type.AuditEventType;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;
import java.util.logging.Logger;
import net.sf.fhz4j.Fhz1000;
import net.sf.fhz4j.FhzProtocol;
import net.sf.fhz4j.fs20.FS20DeviceType;

/**
 *
 * @author aploese
 */
public class FS20PointLocator extends ProtocolLocator<FS20DeviceType> {

    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCARABR_DS_FHZ4J);
    private short housecode;
    private byte offset;
    private FS20DeviceType deviceType;

    /**
     * @return the housecode
     */
    public short getHousecode() {
        return housecode;
    }

    public String defaultName() {
        return getProperty() == null ? "FS20 unknown" : String.format("%d", offset);
    }
    
    /**
     * @param housecode the housecode to set
     */
    public void setHousecode(short housecode) {
        this.housecode = housecode;
    }

    public void setHousecodeStr(String deviceHousecode) {
        this.housecode = Fhz1000.parseHouseCode(deviceHousecode);
    }

    public String getHousecodeStr() {
        return String.format("%04x", housecode);
    }

    public String getPropertyLabel() {
        return getProperty().getLabel();
    }

    public void setPropertyLabel(String label) {
        throw new ImplementMeException();
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.fhz4j.fs20.housecode", getHousecodeStr());
        AuditEventType.addPropertyMessage(list, "dsEdit.fhz4j.fs20.offset", offset);
        AuditEventType.addPropertyMessage(list, "dsEdit.fhz4j.fs20.devicetype", deviceType.getLabel());
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, ProtocolLocator<FS20DeviceType> from) {
        super.addPropertyChanges(list, from);
        final FS20PointLocator fromFs20 = (FS20PointLocator)from;
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.fhz4j.fs20.housecode", fromFs20.getHousecodeStr(), getHousecodeStr());
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.fhz4j.fs20.offset", fromFs20.getOffset(), getOffset());
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.fhz4j.fs20.devicetype", fromFs20.getDeviceType().getLabel(), getDeviceType().getLabel());
    }
    //
    // /
    // / Serialization
    // /
    //
    private static final long serialVersionUID = -1;
    private static final int SERIAL_VERSION = 1;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(SERIAL_VERSION);
        out.writeShort(housecode);
        out.writeByte(offset);
        out.writeObject(deviceType);
    }

    private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        switch (ver) {
            case 1:
                housecode = in.readShort();
                offset = in.readByte();
                deviceType = (FS20DeviceType)in.readObject();
         break;
            default:
                throw new RuntimeException("Cant handle version");
        }
    }


    @Override
    public FhzProtocol getFhzProtocol() {
        return FhzProtocol.FS20;
    }

    /**
     * @return the offset
     */
    public byte getOffset() {
        return offset;
    }

    /**
     * @param offset the offset to set
     */
    public void setOffset(byte offset) {
        this.offset = offset;
    }

    /**
     * @return the deviceType
     */
    public FS20DeviceType getDeviceType() {
        return deviceType;
    }

    /**
     * @param deviceType the deviceType to set
     */
    public void setDeviceType(FS20DeviceType deviceType) {
        this.deviceType = deviceType;
    }

}