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

import br.org.scadabr.DataType;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.json.JsonException;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonReader;
import br.org.scadabr.json.JsonSerializable;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.rt.datasource.fhz4j.Fhz4JPointLocatorRT;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import com.serotonin.mango.rt.dataSource.PointLocatorRT;
import com.serotonin.mango.vo.dataSource.AbstractPointLocatorVO;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;
import java.util.Map;
import java.util.logging.Logger;
import net.sf.fhz4j.FhzProtocol;

import net.sf.fhz4j.scada.ScadaProperty;

// Container to move data with json and ajax so ony basic datatypes
public class Fhz4JPointLocatorVO<T extends ScadaProperty> extends AbstractPointLocatorVO implements JsonSerializable {

    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCARABR_DS_FHZ4J);
    private ProtocolLocator<T> protocolLocator;

    Fhz4JPointLocatorVO() {
        super();
    }

    Fhz4JPointLocatorVO(ProtocolLocator<T> protocolLocator) {
        this.protocolLocator = protocolLocator;
    }

    public String defaultName() {
        return getProperty() == null ? "Fhz4J dataPoint" : getProperty().getLabel();
    }

    @Override
    public DataType getDataType() {
        if (getProperty() == null) {
            return DataType.UNKNOWN;
        }
        switch (getProperty().getDataType()) {
            case BOOLEAN:
                return DataType.BINARY;
            case BYTE:
                return DataType.MULTISTATE;
            case CHAR:
                return DataType.ALPHANUMERIC;
            case DOUBLE:
                return DataType.NUMERIC;
            case FLOAT:
                return DataType.NUMERIC;
            case LONG:
                return DataType.MULTISTATE;
            case INT:
                return DataType.MULTISTATE;
            case SHORT:
                return DataType.MULTISTATE;
            case STRING:
                return DataType.ALPHANUMERIC;
            case TIME:
                return DataType.ALPHANUMERIC;
            default:
                throw new RuntimeException("Cant find datatype of " + getProperty());
        }

    }

    @Override
    public LocalizableMessage getConfigurationDescription() {
        return new LocalizableMessageImpl("dsEdit.openv4j", "Something", "I dont know");
    }

    @Override
    public boolean isSettable() {
        return protocolLocator.isSettable();
    }

    @Override
    public PointLocatorRT createRuntime() {
        return new Fhz4JPointLocatorRT(this);
    }

    @Override
    public void validate(DwrResponseI18n response) {
        // no op
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        protocolLocator.addProperties(list);
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, Object o) {
        protocolLocator.addPropertyChanges(list, ((Fhz4JPointLocatorVO) o).protocolLocator);
    }
    //
    // /
    // / Serialization
    // /
    //
    private static final long serialVersionUID = -1;
    private static final int serialVersion = 1;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(serialVersion);
        out.writeObject(protocolLocator);
    }

    private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        switch (ver) {
            case 1:
                protocolLocator = (ProtocolLocator<T>) in.readObject();
                break;
            default:
                throw new RuntimeException("Cant handle version");
        }
    }

    public FhzProtocol getFhzProtocol() {
        return protocolLocator.getFhzProtocol();
    }

    /**
     * @return the property
     */
    public T getProperty() {
        return protocolLocator.getProperty();
    }

    /**
     * @return the protocolLocator
     */
    public ProtocolLocator<T> getProtocolLocator() {
        return protocolLocator;
    }

    @Override
    public void jsonSerialize(Map<String, Object> paramMap) {
        throw new ImplementMeException();
    }

    @Override
    public void jsonDeserialize(JsonReader paramJsonReader, JsonObject paramJsonObject) throws JsonException {
        throw new ImplementMeException();
    }
}
