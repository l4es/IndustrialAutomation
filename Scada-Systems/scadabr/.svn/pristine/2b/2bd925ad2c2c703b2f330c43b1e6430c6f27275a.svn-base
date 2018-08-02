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

import br.ufsc.edugraf.rose.modelo.HTTPRetrieverDataPoint;
import br.ufsc.edugraf.rose.modelo.HTTPRetrieverDataPointImpl;

import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.DataPointDao;
import com.serotonin.mango.db.dao.DataSourceDao;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.view.text.NoneRenderer;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.dataSource.http.HttpRetrieverPointLocatorVO;
import com.serotonin.mango.vo.event.PointEventDetectorVO;

public class RecursoDataPointsDeUmDataSourceHTTPRetriever extends ServerResource {

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
			
			HTTPRetrieverDataPoint dp = new HTTPRetrieverDataPointImpl(dataPointVO);
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
		DataPointVO dp = null;
		HttpRetrieverPointLocatorVO locator = (HttpRetrieverPointLocatorVO) datasource.createPointLocator();		
		Form formularioComDados = new Form(entity);
		
		try {
			locator.setValueRegex(formularioComDados.getFirstValue("valueRegex"));
			locator.setDataTypeId((Integer.valueOf(formularioComDados.getFirstValue("dataType"))));
			dp = obterDataPoint(formularioComDados);
			dp.setPointLocator(locator);
			dp.setTextRenderer(new NoneRenderer());
		}
		catch(Exception e) {
			e.printStackTrace();
			requisicaoMalFormulada("Bad Request");
			return;
		}
		
		//chamar o validate para evitar erros de validacao...
		try {
			mangoRuntime.saveDataPoint(dp);
			getResponse().setStatus(Status.SUCCESS_CREATED);
			getResponse().setEntity(new StringRepresentation("Home Systems Data point " + dp.getName() + " Created succesfully"));
		}
		catch(Exception e) {
			e.printStackTrace();
			getResponse().setStatus(Status.SERVER_ERROR_INTERNAL);
			getResponse().setEntity(new StringRepresentation("Internal Server Error"));
		}
	}
	
	private void requisicaoMalFormulada(String texto) {
		getResponse().setStatus(Status.CLIENT_ERROR_BAD_REQUEST);
		getResponse().setEntity(new StringRepresentation(texto));
	}

	private DataPointVO obterDataPoint(Form form) {
		DataPointVO dp = new DataPointVO();
		dp.setId(Common.NEW_ID);
		dp.setXid(form.getFirstValue("identifier"));
		dp.setName(form.getFirstValue("name"));
		dp.setEnabled(Boolean.valueOf(form.getFirstValue("enabled")));
		dp.setPointLocator((HttpRetrieverPointLocatorVO) datasource.createPointLocator());
		dp.setEventDetectors(new ArrayList<PointEventDetectorVO>());
		dp.setDataSourceId(datasource.getId());
		dp.setDataSourceXid(codigoDoDataSource);
		return dp;
	}
}
