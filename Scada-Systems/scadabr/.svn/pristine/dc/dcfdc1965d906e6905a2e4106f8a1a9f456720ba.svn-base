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
package com.serotonin.mango.rt.dataSource.modbus;

import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.timer.cron.CronExpression;
import com.serotonin.io.serial.SerialParameters;
import com.serotonin.mango.vo.dataSource.modbus.ModbusSerialDataSourceVO;
import com.serotonin.mango.vo.dataSource.modbus.ModbusSerialDataSourceVO.EncodingType;
import com.serotonin.modbus4j.ModbusFactory;
import com.serotonin.modbus4j.ModbusMaster;
import java.text.ParseException;

public class ModbusSerialDataSource extends ModbusDataSource<ModbusSerialDataSourceVO> {

    public ModbusSerialDataSource(ModbusSerialDataSourceVO vo) {
        super(vo, true);
    }

    //
    // /
    // / Lifecycle
    // /
    //
    @Override
    public void initialize() {
        SerialParameters params = new SerialParameters();
        params.setCommPortId(vo.getCommPortId());
        params.setPortOwnerName("Mango Modbus Serial Data Source");
        params.setBaudRate(vo.getBaudRate());
        params.setFlowControlIn(vo.getFlowControlIn());
        params.setFlowControlOut(vo.getFlowControlOut());
        params.setDataBits(vo.getDataBits());
        params.setStopBits(vo.getStopBits());
        params.setParity(vo.getParity());

        ModbusMaster modbusMaster;
        if (vo.getEncoding() == EncodingType.ASCII) {
            modbusMaster = new ModbusFactory().createAsciiMaster(params, vo.getConcurrency());
        } else {
            modbusMaster = new ModbusFactory().createRtuMaster(params, vo.getConcurrency());
        }

        super.initialize(modbusMaster);
    }

    @Override
    protected CronExpression getCronExpression() throws ParseException {
        throw new ImplementMeException();
    }
}
