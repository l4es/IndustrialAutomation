/*
 *                  			ROSE
 *
 *         Direitos Autorais Reservados (c) 2011 EDUGRAF
 *  LaboratÃ³rio de Software Educacional - http://www.edugraf.ufsc.br
 *                          INE - CTC - UFSC
 *
 * ROSE Ã© software livre; vocÃª pode redistribuÃ­-lo e/ou modificÃ¡-lo sob os 
 * termos da LicenÃ§a PÃºblica Geral GNU conforme publicada pela Free Software 
 * Foundation; tanto a versÃ£o 2 da LicenÃ§a, como (a seu critÃ©rio) qualquer 
 * versÃ£o posterior.
 *
 * Este programa Ã© distribuÃ­do na expectativa de que seja Ãºtil, porÃ©m, SEM
 * NENHUMA GARANTIA; nem mesmo a garantia implÃ­cita de COMERCIABILIDADE OU
 * ADEQUAÃ‡ÃƒO A UMA FINALIDADE ESPECÃ�FICA. Consulte a LicenÃ§a PÃºblica Geral
 * do GNU para mais detalhes. 
 *
 * VocÃª deve ter recebido uma cÃ³pia da LicenÃ§a PÃºblica Geral do GNU junto com
 * este programa; se nÃ£o, escreva para a Free Software Foundation, Inc., no
 * endereÃ§o 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA. 
 *
 * Para nos contactar ou para maiores informaÃ§Ãµes sobre ROSE,
 * veja: http://agrest.edugraf.ufsc.br/index.php/ROSE
 *
 */

package br.ufsc.edugraf.rose.recursos;

import java.util.ArrayList;
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

import br.ufsc.edugraf.rose.modelo.ModbusSerialDataPoint;
import br.ufsc.edugraf.rose.modelo.ModbusSerialDatapointImpl;

import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.DataPointDao;
import com.serotonin.mango.db.dao.DataSourceDao;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.dataSource.modbus.ModbusPointLocatorVO;
import com.serotonin.mango.vo.event.PointEventDetectorVO;

public class RecursoDataPointsDeUmDataSourceModbusSerial extends ServerResource {

	private RuntimeManager mangoRuntime;
	private String codigoDoDataSource;
	private DataSourceVO<?> datasource;

	@Override  
	protected void doInit() {  
		codigoDoDataSource = (String) getRequest().getAttributes().get("codigo");
		mangoRuntime = Common.ctx.getRuntimeManager();
		datasource = new DataSourceDao().getDataSource(codigoDoDataSource);
	}

	@Get
	public Representation represent(Variant variant) {
		Representation representacao = null;
		
		List<DataPointVO> dps = new DataPointDao().getDataPoints(datasource.getId(), null);
		
		JSONArray datapoints = new JSONArray();
		for (DataPointVO dataPointVO : dps) {
			
			ModbusSerialDataPoint dp = new ModbusSerialDatapointImpl(dataPointVO);
			try {
				datapoints.put(dp.comoJson());
				representacao = new JsonRepresentation(datapoints);
			} catch (JSONException e) {
				e.printStackTrace();
				getResponse().setStatus(Status.SERVER_ERROR_INTERNAL);
			}
		}

		return representacao;
	}

	@Post
	public void acceptRepresentation(Representation entity) {
		DataPointVO dataPoint = new DataPointVO();

		ModbusPointLocatorVO locator = (ModbusPointLocatorVO) datasource.createPointLocator();

		try {
			Form formularioComDados = new Form(entity);
			
			locator.setSlaveId(Integer.valueOf(formularioComDados.getFirstValue("slaveId")));
			locator.setRange(Integer.valueOf(formularioComDados.getFirstValue("range")));
			locator.setModbusDataType(Integer.valueOf(formularioComDados.getFirstValue("modbusDataType")));
			locator.setOffset(Integer.valueOf(formularioComDados.getFirstValue("offset")));
			locator.setBit(Byte.valueOf(formularioComDados.getFirstValue("bit")));
			locator.setSettableOverride(Boolean.valueOf(formularioComDados.getFirstValue("settableOverride")));
			locator.setMultiplier(Double.valueOf(formularioComDados.getFirstValue("multiplier")));
			locator.setAdditive(Double.valueOf(formularioComDados.getFirstValue("additive")));
			dataPoint.setEnabled(Boolean.valueOf(formularioComDados.getFirstValue("enabled")));
			
			dataPoint.setId(Common.NEW_ID);
			dataPoint.setXid(formularioComDados.getFirstValue("xid"));
			dataPoint.setName(formularioComDados.getFirstValue("name"));
			dataPoint.setPointLocator(locator);
			dataPoint.setEventDetectors(new ArrayList<PointEventDetectorVO>());
			dataPoint.setDataSourceId(datasource.getId());
			
		}
		catch(Exception e) {
			e.printStackTrace();
			getResponse().setStatus(Status.CLIENT_ERROR_BAD_REQUEST);
			getResponse().setEntity(new StringRepresentation("Bad Request"));
			return;
		}
        
        try {
        	mangoRuntime.saveDataPoint(dataPoint);
			getResponse().setStatus(Status.SUCCESS_CREATED);
			getResponse().setEntity(new StringRepresentation("Modbus Serial Data point " + dataPoint.getName() + " Created succesfully"));
		}
		catch(Exception e) {
			e.printStackTrace();
			getResponse().setStatus(Status.SERVER_ERROR_INTERNAL);
			getResponse().setEntity(new StringRepresentation("Internal Server Error"));
		}
	}
}
