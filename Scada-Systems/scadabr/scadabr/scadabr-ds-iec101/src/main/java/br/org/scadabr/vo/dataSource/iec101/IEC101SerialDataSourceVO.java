package br.org.scadabr.vo.dataSource.iec101;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;

import br.org.scadabr.rt.dataSource.iec101.IEC101SerialDataSource;



import com.serotonin.mango.rt.dataSource.DataSourceRT;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.util.StringUtils;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;


public class IEC101SerialDataSourceVO extends
        IEC101DataSourceVO<IEC101SerialDataSourceVO> {

    public static final Type TYPE = Type.IEC101_SERIAL;

    @Override
    public LocalizableMessage getConnectionDescription() {
        return new LocalizableMessageImpl("common.default", commPortId);
    }

    @Override
    public com.serotonin.mango.vo.dataSource.DataSourceVO.Type getType() {
        return TYPE;
    }

    @Override
    public DataSourceRT createDataSourceRT() {
        return new IEC101SerialDataSource(this);
    }

    
    private String commPortId;
    
    private int baudRate = 9600;
    
    private int flowControlIn = 0;
    
    private int flowControlOut = 0;
    
    private int dataBits = 8;
    
    private int stopBits = 1;
    
    private int parity = 0;

    @Override
    public void validate(DwrResponseI18n response) {
        super.validate(response);
        if (commPortId.isEmpty()) {
            response.addContextual("commPortId", "validate.required");
        }
        if (baudRate <= 0) {
            response.addContextual("baudRate", "validate.invalidValue");
        }
        if (!(flowControlIn == 0 || flowControlIn == 1 || flowControlIn == 4)) {
            response.addContextual("flowControlIn", "validate.invalidValue");
        }
        if (!(flowControlOut == 0 || flowControlOut == 2 || flowControlOut == 8)) {
            response.addContextual("flowControlOut", "validate.invalidValue");
        }
        if (dataBits < 5 || dataBits > 8) {
            response.addContextual("dataBits", "validate.invalidValue");
        }
        if (stopBits < 1 || stopBits > 3) {
            response.addContextual("stopBits", "validate.invalidValue");
        }
        if (parity < 0 || parity > 4) {
            response.addContextual("parityBits", "validate.invalidValue");
        }
    }

    @Override
    protected void addPropertiesImpl(List<LocalizableMessage> list) {
        super.addPropertiesImpl(list);
    }

    @Override
    protected void addPropertyChangesImpl(List<LocalizableMessage> list,
            IEC101SerialDataSourceVO from) {
        super.addPropertyChangesImpl(list, from);
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
        SerializationHelper.writeSafeUTF(out, commPortId);
        out.writeInt(baudRate);
        out.writeInt(flowControlIn);
        out.writeInt(flowControlOut);
        out.writeInt(dataBits);
        out.writeInt(stopBits);
        out.writeInt(parity);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();
		// Switch on the version of the class so that version changes can be
        // elegantly handled.
        if (ver == 1) {
            commPortId = SerializationHelper.readSafeUTF(in);
            baudRate = in.readInt();
            flowControlIn = in.readInt();
            flowControlOut = in.readInt();
            dataBits = in.readInt();
            stopBits = in.readInt();
            parity = in.readInt();
        }
    }

    public String getCommPortId() {
        return commPortId;
    }

    public void setCommPortId(String commPortId) {
        this.commPortId = commPortId;
    }

    public int getBaudRate() {
        return baudRate;
    }

    public void setBaudRate(int baudRate) {
        this.baudRate = baudRate;
    }

    public int getFlowControlIn() {
        return flowControlIn;
    }

    public void setFlowControlIn(int flowControlIn) {
        this.flowControlIn = flowControlIn;
    }

    public int getFlowControlOut() {
        return flowControlOut;
    }

    public void setFlowControlOut(int flowControlOut) {
        this.flowControlOut = flowControlOut;
    }

    public int getDataBits() {
        return dataBits;
    }

    public void setDataBits(int dataBits) {
        this.dataBits = dataBits;
    }

    public int getStopBits() {
        return stopBits;
    }

    public void setStopBits(int stopBits) {
        this.stopBits = stopBits;
    }

    public int getParity() {
        return parity;
    }

    public void setParity(int parity) {
        this.parity = parity;
    }

}
