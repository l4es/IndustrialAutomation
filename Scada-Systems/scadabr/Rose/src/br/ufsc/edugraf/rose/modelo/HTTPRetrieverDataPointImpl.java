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

import com.serotonin.mango.DataTypes;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.dataSource.http.HttpRetrieverPointLocatorVO;

public class HTTPRetrieverDataPointImpl implements HTTPRetrieverDataPoint {
	
	private  HttpRetrieverPointLocatorVO pointLocator;
	private DataPointVO dataPointVO;

	public HTTPRetrieverDataPointImpl(DataPointVO dataPointVO) {
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
	public String getValueRegex() {
		return pointLocator.getValueRegex();
	}

	@Override
	public boolean isIgnoreIfMissing() {
		return pointLocator.isIgnoreIfMissing();
	}

	@Override
	public String getValueFormat() {
		return pointLocator.getValueFormat();
	}

	@Override
	public String getDataType() {
		return DataTypes.CODES.getCode(pointLocator.getDataTypeId());
	}

	@Override
	public String getTimeRegex() {
		return pointLocator.getTimeRegex();
	}

	@Override
	public String getTimeFormat() {
		return pointLocator.getTimeFormat();
	}

	@Override
	public JSONObject comoJson() throws JSONException {
		JSONObject jo = new JSONObject();
		jo.put("name", getName());
		jo.put("xid", getXid());
		jo.put("valueRegex", getValueRegex());
		jo.put("ignoreIfMissing", isIgnoreIfMissing());
		jo.put("valueFormat", getValueFormat());
		jo.put("dataType", getDataType());
		jo.put("timeRegex", getTimeRegex());
		jo.put("timeFormat", getTimeFormat());
		
		return jo;
	}
}
