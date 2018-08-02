package br.org.scadabr.rt.dataSource.dnp3;


import java.util.Date;

import br.org.scadabr.vo.dataSource.dnp3.Dnp3SerialDataSourceVO;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;

public class Dnp3SerialDataSource extends Dnp3DataSource<Dnp3SerialDataSourceVO> {

    public Dnp3SerialDataSource(Dnp3SerialDataSourceVO vo) {
        super(vo, true);
    }

    @Override
    public void initialize() {
        // inicializa DnpMaster com os parametros seriais.
        DNP3Master dnp3Master = new DNP3Master();
        try {
            dnp3Master.initSerial(vo.getSourceAddress(),
                    vo.getSlaveAddress(), vo
                    .getCommPortId(), vo.getBaudRate(),
                    vo.getStaticPollPeriods());
        } catch (Exception e) {
            e.printStackTrace();
            raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, new Date().getTime(), true,
                    new LocalizableMessageImpl("event.exception2", vo.getName(), e.getMessage()));
        }

        super.initialize(dnp3Master);
    }

}
