package br.org.scadabr.vo.dataSource.iec101;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

import br.org.scadabr.rt.dataSource.iec101.IEC101EthernetDataSource;



import com.serotonin.mango.rt.dataSource.DataSourceRT;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;


public class IEC101EthernetDataSourceVO extends IEC101DataSourceVO<IEC101EthernetDataSourceVO> {

    public static final Type TYPE = Type.IEC101_ETHERNET;

    @Override
    public DataSourceRT createDataSourceRT() {
        return new IEC101EthernetDataSource(this);
    }

    @Override
    public LocalizableMessage getConnectionDescription() {
        return new LocalizableMessageImpl("common.default", host);
    }

    @Override
    public com.serotonin.mango.vo.dataSource.DataSourceVO.Type getType() {
        return TYPE;
    }

    
    private String host = "localhost";
    
    private int port = 2404;

    @Override
    public void validate(DwrResponseI18n response) {
        super.validate(response);
        if (host.isEmpty()) {
            response.addContextual("commPortId", "validate.required");
        }
        if (port <= 0) {
            response.addContextual("port", "validate.invalidValue");
        }
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
        SerializationHelper.writeSafeUTF(out, host);
        out.writeInt(port);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();
		// Switch on the version of the class so that version changes can be
        // elegantly handled.
        if (ver == 1) {
            host = SerializationHelper.readSafeUTF(in);
            port = in.readInt();
        }
    }

    public String getHost() {
        return host;
    }

    public void setHost(String host) {
        this.host = host;
    }

    public int getPort() {
        return port;
    }

    public void setPort(int port) {
        this.port = port;
    }

}
