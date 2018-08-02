/*
 *                  			ROSE
 *
 *         Direitos Autorais Reservados (c) 2011 EDUGRAF
 *  Laboratório de Software Educacional - http://www.edugraf.ufsc.br
 *                          INE - CTC - UFSC
 *
 * ROSE é software livre; você pode redistribuí-lo e/ou modificá-lo sob os 
 * termos da Licença Pública Geral GNU conforme publicada pela Free Software 
 * Foundation; tanto a versão 2 da Licença, como (a seu critério) qualquer 
 * versão posterior.
 *
 * Este programa é distribuído na expectativa de que seja útil, porém, SEM
 * NENHUMA GARANTIA; nem mesmo a garantia implícita de COMERCIABILIDADE OU
 * ADEQUAÇÃO A UMA FINALIDADE ESPECÍFICA. Consulte a Licença Pública Geral
 * do GNU para mais detalhes. 
 *
 * Você deve ter recebido uma cópia da Licença Pública Geral do GNU junto com
 * este programa; se não, escreva para a Free Software Foundation, Inc., no
 * endereço 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA. 
 *
 * Para nos contactar ou para maiores informações sobre ROSE,
 * veja: http://agrest.edugraf.ufsc.br/index.php/ROSE
 *
 */

package br.ufsc.edugraf.rose.modelo;

import org.json.JSONException;
import org.json.JSONObject;

import com.serotonin.mango.Common;
import com.serotonin.mango.vo.dataSource.modbus.ModbusSerialDataSourceVO;
import com.serotonin.mango.vo.dataSource.modbus.ModbusSerialDataSourceVO.EncodingType;

public class ModbusSerialDataSourceImpl implements ModbusSerialDataSource {
	
	private ModbusSerialDataSourceVO datasourceVO;

	public ModbusSerialDataSourceImpl(ModbusSerialDataSourceVO datasourceVO) {
		this.datasourceVO = datasourceVO;
	}

	@Override
	public boolean isEnabled() {
		return datasourceVO.isEnabled();
	}

	@Override
	public String getName() {
		return datasourceVO.getName();
	}

	@Override
	public String getXid() {
		return datasourceVO.getXid();
	}

	@Override
	public int getUpdatePeriods() {
		return datasourceVO.getUpdatePeriods();
	}

	@Override
	public int getTimeout() {
		return datasourceVO.getTimeout();
	}

	@Override
	public int getRetries() {
		return datasourceVO.getRetries();
	}

	@Override
	public String getUpdatePeriodsType() {
		return Common.TIME_PERIOD_CODES.getCode(datasourceVO.getUpdatePeriodType());
	}

	@Override
	public int getBaudRate() {
		return datasourceVO.getBaudRate();
	}

	@Override
	public String getCommPortId() {
		return datasourceVO.getCommPortId();
	}

	@Override
	public int getDataBits() {
		return datasourceVO.getDataBits();
	}

	@Override
	public boolean isEcho() {
		return datasourceVO.isEcho();
	}

	@Override
	public int getFlowControlIn() {
		return datasourceVO.getFlowControlIn();
	}

	@Override
	public int getFlowControlOut() {
		return datasourceVO.getFlowControlOut();
	}

	@Override
	public int getParity() {
		return datasourceVO.getParity();
	}

	@Override
	public int getStopBits() {
		return datasourceVO.getStopBits();
	}

	@Override
	public EncodingType getEncoding() {
		return datasourceVO.getEncoding();
	}

	@Override
	public boolean isContiguousBatches() {
		return datasourceVO.isContiguousBatches();
	}

	@Override
	public boolean isCreateSlaveMonitorPoints() {
		return datasourceVO.isCreateSlaveMonitorPoints();
	}
	
	@Override
	public JSONObject comoJson() throws JSONException {
		JSONObject jo = new JSONObject();
		jo.put("name", getName());
		jo.put("xid", getXid());
		jo.put("enabled", isEnabled());
		jo.put("updatePeriods", getUpdatePeriods());
		jo.put("updatePeriodType", getUpdatePeriodsType());
		jo.put("retries", getRetries());
		jo.put("timeout", getTimeout());
		jo.put("isEcho", isEcho());
		jo.put("isCreateSlaveMonitorPoints", isCreateSlaveMonitorPoints());
		jo.put("isContiguousBatches", isContiguousBatches());
		jo.put("stopBits", getStopBits());
		jo.put("parity", getParity());
		jo.put("flowControlIn", getFlowControlIn());
		jo.put("flowControlOut", getFlowControlOut());
		jo.put("dataBits", getDataBits());
		jo.put("commPortId", getCommPortId());
		jo.put("baudRate", getBaudRate());
		jo.put("encodings", getEncoding());
		return jo;
	}
}
