package br.org.scadabr.vo.dataSource.asciiFile;

import br.org.scadabr.DataType;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang3.StringUtils;

import br.org.scadabr.rt.dataSource.asciiFile.ASCIIFilePointLocatorRT;

import br.org.scadabr.json.JsonException;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonReader;


import br.org.scadabr.json.JsonSerializable;
import com.serotonin.mango.rt.dataSource.PointLocatorRT;
import com.serotonin.mango.vo.dataSource.AbstractPointLocatorVO;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;


public class ASCIIFilePointLocatorVO extends AbstractPointLocatorVO implements
        JsonSerializable {

    
    private String valueRegex = "";
    
    private boolean customTimestamp;
    
    private String timestampFormat = "";
    
    private String timestampRegex = "";
    
    private DataType dataType = DataType.BINARY;
    
    private boolean settable;

    @Override
    public void validate(DwrResponseI18n response) {
        if (StringUtils.isEmpty(valueRegex)) {
            response.addContextual("valueRegex", "validate.required");
        }
        if (customTimestamp) {
            if (StringUtils.isEmpty(timestampFormat)) {
                response.addContextual("timestampFormat", "validate.required");
            }
            if (StringUtils.isEmpty(timestampRegex)) {
                response.addContextual("timestampRegex", "validate.required");
            }
        }
    }

    public String getValueRegex() {
        return valueRegex;
    }

    public void setValueRegex(String valueRegex) {
        this.valueRegex = valueRegex;
    }

    public String getTimestampRegex() {
        return timestampRegex;
    }

    public void setTimestampRegex(String timestampRegex) {
        this.timestampRegex = timestampRegex;
    }

    @Override
    public DataType getDataType() {
        return dataType;
    }

    public void setDataType(DataType dataType) {
        this.dataType = dataType;
    }

    public void setSettable(boolean settable) {
        this.settable = settable;
    }

    @Override
    public PointLocatorRT createRuntime() {
        return new ASCIIFilePointLocatorRT(this);
    }

    @Override
    public LocalizableMessage getConfigurationDescription() {
        return null;
    }

    private static final long serialVersionUID = -1;
    private static final int version = 1;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);
        SerializationHelper.writeSafeUTF(out, valueRegex);
        SerializationHelper.writeSafeUTF(out, timestampFormat);
        SerializationHelper.writeSafeUTF(out, timestampRegex);
        out.writeInt(dataType.mangoDbId);
        out.writeBoolean(settable);
        out.writeBoolean(customTimestamp);

    }

    private void readObject(ObjectInputStream in) throws IOException,
            ClassNotFoundException {
        int ver = in.readInt();
        if (ver == 1) {
            valueRegex = SerializationHelper.readSafeUTF(in);
            timestampFormat = SerializationHelper.readSafeUTF(in);
            timestampRegex = SerializationHelper.readSafeUTF(in);
            dataType = DataType.fromMangoDbId(in.readInt());
            settable = in.readBoolean();
            customTimestamp = in.readBoolean();
        }
    }

    @Override
    public void jsonDeserialize(JsonReader arg0, JsonObject arg1)
            throws JsonException {

    }

    @Override
    public void jsonSerialize(Map<String, Object> arg0) {

    }

    @Override
    public boolean isSettable() {
        return settable;
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {

    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, Object o) {

    }

    public void setCustomTimestamp(boolean customTimestamp) {
        this.customTimestamp = customTimestamp;
    }

    public boolean isCustomTimestamp() {
        return customTimestamp;
    }

    public void setTimestampFormat(String timestampFormat) {
        this.timestampFormat = timestampFormat;
    }

    public String getTimestampFormat() {
        return timestampFormat;
    }

}
