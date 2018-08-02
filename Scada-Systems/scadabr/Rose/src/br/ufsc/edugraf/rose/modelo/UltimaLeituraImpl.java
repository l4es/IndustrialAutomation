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

import java.text.SimpleDateFormat;
import java.util.Date;

import org.json.JSONException;
import org.json.JSONObject;

import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.vo.DataPointVO;

public class UltimaLeituraImpl implements UltimaLeitura {
	private DataPointVO datapointvo;
	private DataPointRT datapointrt;
	
	public UltimaLeituraImpl(DataPointVO datapointvo, DataPointRT datapointrt) {
		this.datapointvo = datapointvo;
		this.datapointrt = datapointrt;
	}

	@Override
	public String getName() {
		return datapointvo.getName();
	}

	@Override
	public String getXid() {
		return datapointvo.getXid();
	}

	@Override
	public String getValue() {
		return datapointrt.getPointValue().getValue().toString();
	}

	@Override
	public String getTimeStamp() {
		return new SimpleDateFormat("dd/MM/yyyy HH:mm:ss").format(new Date(datapointrt.getPointValue().getTime()));
	}

	@Override
	public JSONObject comoJson() throws JSONException {
		JSONObject jo = new JSONObject();
		jo.put("name", getName());
		jo.put("value", getValue());
		jo.put("timestamp", getTimeStamp());
		return jo;
	}

}
