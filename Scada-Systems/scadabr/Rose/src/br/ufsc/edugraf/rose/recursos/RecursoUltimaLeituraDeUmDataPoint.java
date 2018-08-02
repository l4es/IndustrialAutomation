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

import org.json.JSONException;
import org.restlet.data.Status;
import org.restlet.ext.json.JsonRepresentation;
import org.restlet.representation.Representation;
import org.restlet.representation.StringRepresentation;
import org.restlet.representation.Variant;
import org.restlet.resource.Get;
import org.restlet.resource.ServerResource;

import br.ufsc.edugraf.rose.modelo.UltimaLeitura;
import br.ufsc.edugraf.rose.modelo.UltimaLeituraImpl;

import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.DataPointDao;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.vo.DataPointVO;

public class RecursoUltimaLeituraDeUmDataPoint extends ServerResource {
	private RuntimeManager rtm;
	private String codigoDoDatapoint;
	private DataPointVO datapointvo;

	@Override  
	protected void doInit() {  
        rtm = Common.ctx.getRuntimeManager();
        codigoDoDatapoint = (String) getRequest().getAttributes().get("codigo");
        datapointvo = new DataPointDao().getDataPoint(codigoDoDatapoint);
    }

    @Get
    public Representation represent(Variant variant) {
    	DataPointRT rt = null;
    	Representation representacao = null;
    	
    	if(datapointvo != null) {
    		rt = rtm.getDataPoint(datapointvo.getId());
    		if(rt == null) {
    			representacao = new StringRepresentation("datapoint desabilitado");
    			getResponse().setStatus(Status.CLIENT_ERROR_NOT_FOUND);
    		}
    	}
    	else {
    		representacao = new StringRepresentation("data point inexistente");
    		getResponse().setStatus(Status.CLIENT_ERROR_NOT_FOUND);    		
    	}
    	
    	if(rt != null) {
    		try {
    			UltimaLeitura ultimaLeitura = new UltimaLeituraImpl(datapointvo, rt);
				representacao = new JsonRepresentation(ultimaLeitura.comoJson());
			} catch (JSONException e) {
				e.printStackTrace();
			}
    	}
        return representacao;
    }
}