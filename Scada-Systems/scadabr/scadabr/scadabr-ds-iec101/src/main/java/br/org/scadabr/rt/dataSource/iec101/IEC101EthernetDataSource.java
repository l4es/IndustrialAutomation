package br.org.scadabr.rt.dataSource.iec101;

import br.org.scadabr.protocol.iec101.session.IECConfig;
import br.org.scadabr.vo.dataSource.iec101.IEC101EthernetDataSourceVO;

public class IEC101EthernetDataSource extends IEC101DataSource<IEC101EthernetDataSourceVO> {

    public IEC101EthernetDataSource(IEC101EthernetDataSourceVO vo) {
        super(vo, true);
    }

    @Override
    public void initialize() {
        IECConfig config = new IECConfig(vo.getHost(), vo.getPort());
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
