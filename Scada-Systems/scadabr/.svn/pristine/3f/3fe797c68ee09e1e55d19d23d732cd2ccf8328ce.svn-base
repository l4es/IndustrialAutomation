package br.org.scadabr.rt.dataSource.iec101;

import br.org.scadabr.protocol.iec101.session.IECConfig;
import br.org.scadabr.protocol.iec101.session.IECConfig.COMM;
import br.org.scadabr.vo.dataSource.iec101.IEC101SerialDataSourceVO;

public class IEC101SerialDataSource extends IEC101DataSource<IEC101SerialDataSourceVO> {

    public IEC101SerialDataSource(IEC101SerialDataSourceVO vo) {
        super(vo, true);
    }

    @Override
    public void initialize() {
        IECConfig config = new IECConfig();
        config.setCommType(COMM.SERIAL);
        config.setBaudrate(vo.getBaudRate());
        config.setSerialPort(vo.getCommPortId());
        config.setLinkAddress(vo.getLinkLayerAddress());
        config.setLinkAddressSize((byte) vo.getLinkLayerAddressSize());
        config.setObjectAddressSize((byte) vo.getObjectAddressSize());
        config.setAsduAddressSize((byte) vo.getAsduAddressSize());
        config.setCotSize((byte) vo.getCotSize());

        IEC101Master iec101Master = new IEC101Master();
        iec101Master.setConfiguration(config);
        super.initialize(iec101Master);
    }

}
