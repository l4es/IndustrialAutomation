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

import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.dataSource.modbus.ModbusPointLocatorVO;

public class ModbusSerialDatapointImpl implements ModbusSerialDataPoint {
	
	private ModbusPointLocatorVO pointLocator;
	private DataPointVO dataPointVO;

	public ModbusSerialDatapointImpl(DataPointVO dataPointVO) {
		this.pointLocator = dataPointVO.getPointLocator();
		this.dataPointVO = dataPointVO;
	}

	@Override
	public String getName() {
		return dataPointVO.getName();
	}

	@Override
	public String getXid() {
		return dataPointVO.getXid();
	}

	@Override
	public double getAdditive() {
		return pointLocator.getAdditive();
	}

	@Override
	public byte getBit() {
		return pointLocator.getBit();
	}

	@Override
	public boolean isSettableOverride() {
		return pointLocator.isSettableOverride();
	}

	@Override
	public int getRange() {
		return pointLocator.getRange();
	}

	@Override
	public int getModbusDataType() {
		return pointLocator.getModbusDataType();
	}

	@Override
	public double getMultiplier() {
		return pointLocator.getMultiplier();
	}

	@Override
	public int getOffset() {
		return pointLocator.getOffset();
	}

	@Override
	public int getSlaveId() {
		return pointLocator.getSlaveId();
	}

	@Override
	public boolean isSlaveMonitor() {
		return pointLocator.isSlaveMonitor();
	}
	
	@Override
	public JSONObject comoJson() throws JSONException {
		JSONObject jo = new JSONObject();
		jo.put("name", getName());
		jo.put("xid", getXid());
		jo.put("addictive", getAdditive());
		jo.put("bit", getBit());
		jo.put("modbusDatatype", getModbusDataType());
		jo.put("multiplier", getMultiplier());
		jo.put("offset", getOffset());
		jo.put("range", getRange());
		jo.put("slaveId", getSlaveId());
		jo.put("settableOverride", isSettableOverride());
		jo.put("slaveMonitor", isSlaveMonitor());
		return jo;
	}
}
