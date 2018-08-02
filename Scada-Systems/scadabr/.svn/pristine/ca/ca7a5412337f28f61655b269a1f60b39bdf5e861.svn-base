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

import br.ufsc.edugraf.rose.modelo.HTTPRetrieverDataSource;
import br.ufsc.edugraf.rose.modelo.HTTPRetrieverDataSourceImpl;

import com.serotonin.mango.Common;
import com.serotonin.mango.Common.TimePeriods;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.dataSource.DataSourceVO.Type;
import com.serotonin.mango.vo.dataSource.http.HttpRetrieverDataSourceVO;
import com.serotonin.web.dwr.DwrResponseI18n;

public class RecursoDataSourcesHTTPRetriever extends ServerResource {
	
	private RuntimeManager mangoRuntime;

	@Override  
	protected void doInit() {  
		mangoRuntime = Common.ctx.getRuntimeManager();
	}
	
	@Post
	public void acceptRepresentation(Representation entity) {
		
		HttpRetrieverDataSourceVO datasource = null;
		
		try {
			datasource = obterDataSource(new Form(entity));
		}
		catch(Exception e) {
			e.printStackTrace();
			requisicaoMalFormulada();
			return;
		}
		
		DwrResponseI18n dadosDoDatasource = new DwrResponseI18n();
		datasource.validate(dadosDoDatasource);
		
		if(!dadosDoDatasource.getHasMessages()) {
			try {
				Common.ctx.getRuntimeManager().saveDataSource(datasource);
				getResponse().setStatus(Status.SUCCESS_CREATED);
				getResponse().setEntity(new StringRepresentation("HTTP Retriever Data Source " + datasource.getName() + " Created succesfully"));
			}
			catch(Exception e) {
				e.printStackTrace();
				getResponse().setStatus(Status.SERVER_ERROR_INTERNAL);
				getResponse().setEntity(new StringRepresentation("Internal Server Error"));
			}
		}
		else {
			requisicaoMalFormulada();
		}
	}
	
	private HttpRetrieverDataSourceVO obterDataSource(Form dadosDoFormulario) {
		HttpRetrieverDataSourceVO datasource = new HttpRetrieverDataSourceVO();
		datasource.setXid(dadosDoFormulario.getFirstValue("identifier"));
		datasource.setName(dadosDoFormulario.getFirstValue("name"));
		datasource.setUpdatePeriods(Integer.valueOf(dadosDoFormulario.getFirstValue("updatePeriod")));
		datasource.setUpdatePeriodType(TimePeriods.SECONDS);
		datasource.setUrl(dadosDoFormulario.getFirstValue("url"));
		datasource.setTimeoutSeconds(Integer.valueOf(dadosDoFormulario.getFirstValue("timeoutSeconds")));
		datasource.setRetries(Integer.valueOf(dadosDoFormulario.getFirstValue("retries")));
		datasource.setEnabled(Boolean.valueOf(dadosDoFormulario.getFirstValue("enabled")));
	
		return datasource;
	}

	private void requisicaoMalFormulada() {
		getResponse().setStatus(Status.CLIENT_ERROR_BAD_REQUEST);
		getResponse().setEntity(new StringRepresentation("Bad Request"));
	}

	@Get
	public JsonRepresentation represent(Variant variant) {

		List<DataSourceVO<?>> todosDatasources = mangoRuntime.getDataSources();
		
		JSONArray datasourcesHTTPRetriever = new JSONArray();
		for (DataSourceVO<?> dataSourceVO : todosDatasources) {
			
			if(dataSourceVO.getType().equals(Type.HTTP_RETRIEVER)) { 
				HTTPRetrieverDataSource datasourceHTTPRetriever = new HTTPRetrieverDataSourceImpl((HttpRetrieverDataSourceVO) dataSourceVO);
				try {
					datasourcesHTTPRetriever.put(datasourceHTTPRetriever.comoJson());
				} catch (JSONException e) {
					e.printStackTrace();
					getResponse().setStatus(Status.SERVER_ERROR_INTERNAL);
				}
				
			}
		}
		
		return new JsonRepresentation(datasourcesHTTPRetriever);
	}
}
