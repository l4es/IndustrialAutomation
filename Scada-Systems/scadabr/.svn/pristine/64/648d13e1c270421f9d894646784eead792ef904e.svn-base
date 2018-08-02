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

package br.ufsc.edugraf.rose.recursos;


import java.util.List;

import org.json.JSONArray;
import org.json.JSONException;
import org.restlet.data.Form;
import org.restlet.data.Status;
import org.restlet.ext.json.JsonRepresentation;
import org.restlet.representation.Representation;
import org.restlet.representation.StringRepresentation;
import org.restlet.representation.Variant;
import org.restlet.resource.Get;
import org.restlet.resource.Post;
import org.restlet.resource.ServerResource;

import br.ufsc.edugraf.rose.modelo.ModbusSerialDataSource;
import br.ufsc.edugraf.rose.modelo.ModbusSerialDataSourceImpl;

import com.serotonin.mango.Common;
import com.serotonin.mango.Common.TimePeriods;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.dataSource.DataSourceVO.Type;
import com.serotonin.mango.vo.dataSource.modbus.ModbusSerialDataSourceVO;

public class RecursoDataSourcesModbusSerial extends ServerResource {

	private RuntimeManager mangoRuntime;

	@Override  
	protected void doInit() {  
		mangoRuntime = Common.ctx.getRuntimeManager();
	}

	@Post
	public void acceptRepresentation(Representation entity) {
		ModbusSerialDataSourceVO datasource = (ModbusSerialDataSourceVO) DataSourceVO.createDataSourceVO(DataSourceVO.Type.MODBUS_SERIAL.getId());

		try {
			Form formularioComDados = new Form(entity);
			datasource.setXid(formularioComDados.getFirstValue("xid"));
			datasource.setName(formularioComDados.getFirstValue("name"));
			datasource.setUpdatePeriods(Integer.valueOf(formularioComDados.getFirstValue("updatePeriod")));
			datasource.setUpdatePeriodType(TimePeriods.MILLISECONDS);
			datasource.setRetries(Integer.valueOf(formularioComDados.getFirstValue("retries")));
			datasource.setTimeout(Integer.valueOf(formularioComDados.getFirstValue("timeout")));
			datasource.setContiguousBatches(Boolean.valueOf(formularioComDados.getFirstValue("contiguousBatches")));
			datasource.setCreateSlaveMonitorPoints(Boolean.valueOf(formularioComDados.getFirstValue("createSlaveMonitorPoints")));
			datasource.setCommPortId(formularioComDados.getFirstValue("commPortId"));
			datasource.setBaudRate(Integer.valueOf(formularioComDados.getFirstValue("baudRate")));
			datasource.setFlowControlIn(Integer.valueOf(formularioComDados.getFirstValue("flowControlIn")));
			datasource.setFlowControlOut(Integer.valueOf(formularioComDados.getFirstValue("flowControlOut")));
			datasource.setDataBits(Integer.valueOf(formularioComDados.getFirstValue("dataBits")));
			datasource.setStopBits(Integer.valueOf(formularioComDados.getFirstValue("stopBits")));
			datasource.setParity(Integer.valueOf(formularioComDados.getFirstValue("parity")));
			datasource.setEncodingStr(formularioComDados.getFirstValue("encoding"));
			datasource.setEcho(Boolean.valueOf(formularioComDados.getFirstValue("echo")));
			datasource.setEnabled(Boolean.valueOf(formularioComDados.getFirstValue("enabled")));
		}
		catch(Exception e) {
			e.printStackTrace();
			getResponse().setStatus(Status.CLIENT_ERROR_BAD_REQUEST);
			getResponse().setEntity(new StringRepresentation("Bad Request"));
			return;
		}
		
		
		try {
			Common.ctx.getRuntimeManager().saveDataSource(datasource);
			getResponse().setEntity(new StringRepresentation("ok"));
			getResponse().setStatus(Status.SUCCESS_CREATED);
			getResponse().setEntity(new StringRepresentation("Serial Modbus Data Source " + datasource.getName() + " Created succesfully"));
		}
		catch(Exception e) {
			e.printStackTrace();
			getResponse().setStatus(Status.SERVER_ERROR_INTERNAL);
			getResponse().setEntity(new StringRepresentation("Internal Server Error"));
		}
	}

	@Get
	public JsonRepresentation represent(Variant variant) {

		List<DataSourceVO<?>> todosDatasources = mangoRuntime.getDataSources();
		
		JSONArray datasourcesModbusSerial = new JSONArray();
		for (DataSourceVO<?> dataSourceVO : todosDatasources) {
			
			if(dataSourceVO.getType().equals(Type.MODBUS_SERIAL)) { 
				ModbusSerialDataSource modbusDataSource = new ModbusSerialDataSourceImpl((ModbusSerialDataSourceVO) dataSourceVO);
				try {
					datasourcesModbusSerial.put(modbusDataSource.comoJson());
				} catch (JSONException e) {
					e.printStackTrace();
					getResponse().setStatus(Status.SERVER_ERROR_INTERNAL);
				}
				
			}
		}
		
		return new JsonRepresentation(datasourcesModbusSerial);
	}
}