package br.org.scadabr.rt.dataSource.dnp3;

import java.util.Date;

import br.org.scadabr.vo.dataSource.dnp3.Dnp3IpDataSourceVO;

import br.org.scadabr.utils.i18n.LocalizableMessageImpl;

public class Dnp3IpDataSource extends Dnp3DataSource<Dnp3IpDataSourceVO> {

    public Dnp3IpDataSource(Dnp3IpDataSourceVO vo) {
        super(vo, true);
    }

    @Override
    public void initialize() {
        // inicializa DnpMaster com os parametros IP.
        DNP3Master dnp3Master = new DNP3Master();
        try {
            dnp3Master.initEthernet(vo.getSourceAddress(),
                    vo.getSlaveAddress(), vo.getHost(),
                    vo.getPort(), vo.getStaticPollPeriods());
        } catch (Exception e) {
            e.printStackTrace();
            raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, new Date().getTime(), true,
                    new LocalizableMessageImpl("event.exception2", vo
                            .getName(), e.getMessage()));
        }

        super.initialize(dnp3Master);
    }
}
