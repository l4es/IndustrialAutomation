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
import com.serotonin.mango.vo.dataSource.http.HttpRetrieverDataSourceVO;

public class HTTPRetrieverDataSourceImpl implements HTTPRetrieverDataSource {
	private HttpRetrieverDataSourceVO dataSourceVO;

	public HTTPRetrieverDataSourceImpl(HttpRetrieverDataSourceVO dataSourceVO) {
		this.dataSourceVO = dataSourceVO;
	}

	@Override
	public String getName() {
		return dataSourceVO.getName();
	}

	@Override
	public String getXid() {
		return dataSourceVO.getXid();
	}

	@Override
	public boolean isEnabled() {
		return dataSourceVO.isEnabled();
	}

	@Override
	public String getUrl() {
		return dataSourceVO.getUrl();
	}
	
	@Override
	public String getUpdatePeriodsType() {
		return Common.TIME_PERIOD_CODES.getCode(dataSourceVO.getUpdatePeriodType());
	}

	@Override
	public int getUpdatePeriods() {
		return dataSourceVO.getUpdatePeriods();
	}

	@Override
	public int getTimeoutSeconds() {
		return dataSourceVO.getTimeoutSeconds();
	}

	@Override
	public int getRetries() {
		return dataSourceVO.getRetries();
	}

	@Override
	public JSONObject comoJson() throws JSONException {
		
		JSONObject jo = new JSONObject();
		jo.put("name", getName());
		jo.put("xid", getXid());
		jo.put("enabled", isEnabled());
		jo.put("url", getUrl());
		jo.put("updatePeriods", getUpdatePeriods());
		jo.put("updatePeriodType", getUpdatePeriodsType());
		jo.put("timeoutSeconds", getTimeoutSeconds());
		jo.put("retries", getRetries());
		
		return jo;
	}

}
