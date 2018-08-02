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

import br.org.scadabr.DataType;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;
import java.util.Map;

import br.org.scadabr.json.JsonException;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonReader;


import br.org.scadabr.json.JsonSerializable;
import com.serotonin.mango.rt.dataSource.PointLocatorRT;
import br.org.scadabr.rt.datasource.vmstat.VMStatPointLocatorRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.vo.dataSource.AbstractPointLocatorVO;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;

/**
 * @author Matthew Lohbihler
 */

public class VMStatPointLocatorVO extends AbstractPointLocatorVO implements JsonSerializable {

    
    private Attribute attribute = Attribute.CPU_ID;

    @Override
    public boolean isSettable() {
        return false;
    }

    @Override
    public PointLocatorRT createRuntime() {
        return new VMStatPointLocatorRT(this);
    }

    @Override
    public LocalizableMessage getConfigurationDescription() {
        return new LocalizableMessageImpl(attribute.getI18nKey());
    }

    @Override
    public DataType getDataType() {
        return DataType.NUMERIC;
    }

    public Attribute getAttribute() {
        return attribute;
    }

    public void setAttribute(Attribute attribute) {
        this.attribute = attribute;
    }

    @Override
    public void validate(DwrResponseI18n response) {
        //NoOp
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.vmstat.attribute", attribute);
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, Object o) {
        VMStatPointLocatorVO from = (VMStatPointLocatorVO) o;
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.vmstat.attribute", from.attribute, attribute);
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
        out.writeUTF(attribute.name());
    }

    private void readObject(ObjectInputStream in) throws IOException {
        final int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        switch (ver) {
            case 1:
                final int attributeId = in.readInt();
                attribute = Attribute.values()[attributeId - 1];
                break;
            case 2:
                attribute = Attribute.valueOf(in.readUTF());
                break;
            default:
                throw new RuntimeException(String.format("Version %d not supported", ver));
        }
    }

    @Override
    public void jsonDeserialize(JsonReader reader, JsonObject json) throws JsonException {
        /*TODO
         String text = json.getString("attributeId");
         if (text == null) {
         throw new LocalizableJsonException("emport.error.missing", "attributeId", ATTRIBUTE_CODES.getCodeList());
         }
     
         attributeId = ATTRIBUTE_CODES.getId(text);
         if (!ATTRIBUTE_CODES.isValidId(attributeId)) {
         throw new LocalizableJsonException("emport.error.invalid", "attributeId", text, ATTRIBUTE_CODES
         .getCodeList());
         }
         */
    }

    @Override
    public void jsonSerialize(Map<String, Object> map) {
        //TODO map.put("attributeId", ATTRIBUTE_CODES.getCode(attributeId));
    }
}
