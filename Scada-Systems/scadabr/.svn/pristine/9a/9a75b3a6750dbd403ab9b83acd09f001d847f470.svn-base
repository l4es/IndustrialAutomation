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

import com.serotonin.mango.vo.dataSource.modbus.ModbusSerialDataSourceVO.EncodingType;

public interface ModbusSerialDataSource extends DataSource {

	public int getUpdatePeriods();

	public int getTimeout();

	public int getRetries();
	
	public String getUpdatePeriodsType();
	
	public int getBaudRate();

    public String getCommPortId();
    
    public int getDataBits();

    public boolean isEcho();

    public int getFlowControlIn();

    public int getFlowControlOut();

    public int getParity();

    public int getStopBits();
    
    public EncodingType getEncoding();
    
    public boolean isContiguousBatches();

    public boolean isCreateSlaveMonitorPoints();
}