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

import java.io.IOException;

import org.json.JSONException;
import org.restlet.data.Status;
import org.restlet.ext.json.JsonRepresentation;
import org.restlet.representation.Representation;
import org.restlet.representation.StringRepresentation;
import org.restlet.representation.Variant;
import org.restlet.resource.Get;
import org.restlet.resource.Put;
import org.restlet.resource.ServerResource;

import br.ufsc.edugraf.rose.modelo.HTTPRetrieverDataSourceImpl;

import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.DataSourceDao;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.vo.dataSource.http.HttpRetrieverDataSourceVO;

public class RecursoDataSourceHTTPRetriever extends ServerResource {
	public RuntimeManager rtm;
	private String codigoDoDataSource;
	private HttpRetrieverDataSourceVO datasource;

	@Override
	protected void doInit() {
		rtm = Common.ctx.getRuntimeManager();
		codigoDoDataSource = (String) getRequest().getAttributes().get("codigo");
		datasource = (HttpRetrieverDataSourceVO) new DataSourceDao().getDataSource(codigoDoDataSource);
	}

	@Put
	public void storeRepresentation(Representation entity) {
		try {
			datasource.setUrl(entity.getText());
		} catch (IOException e) {
			e.printStackTrace();
		}

		Common.ctx.getRuntimeManager().saveDataSource(datasource);

		getResponse().setEntity(new StringRepresentation("ok"));
	}

	@Get
	public Representation represent(Variant variant) {
		
		Representation representacao = null;

		if (datasource == null) {
			representacao = new StringRepresentation("data source inexistente");
			getResponse().setStatus(Status.CLIENT_ERROR_NOT_FOUND);
		} else {
			try {
				representacao = new JsonRepresentation(new HTTPRetrieverDataSourceImpl((HttpRetrieverDataSourceVO) datasource).comoJson());
			} catch (JSONException e) {
				e.printStackTrace();
				getResponse().setStatus(Status.SERVER_ERROR_INTERNAL);
			}

		}

		return representacao;
	}
}
