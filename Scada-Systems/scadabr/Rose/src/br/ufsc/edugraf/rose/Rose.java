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

package br.ufsc.edugraf.rose;

import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.logging.SimpleFormatter;

import org.restlet.Application;
import org.restlet.Context;
import org.restlet.Restlet;
import org.restlet.engine.log.AccessLogFileHandler;
import org.restlet.resource.Directory;
import org.restlet.routing.Router;

import br.ufsc.edugraf.rose.recursos.RecursoAlarmesAtivos;
import br.ufsc.edugraf.rose.recursos.RecursoDataPointsDeUmDataSourceHTTPRetriever;
import br.ufsc.edugraf.rose.recursos.RecursoDataPointsDeUmDataSourceModbusSerial;
import br.ufsc.edugraf.rose.recursos.RecursoDataSourceHTTPRetriever;
import br.ufsc.edugraf.rose.recursos.RecursoDataSourcesHTTPRetriever;
import br.ufsc.edugraf.rose.recursos.RecursoDataSourcesModbusSerial;
import br.ufsc.edugraf.rose.recursos.RecursoRaizDoRose;
import br.ufsc.edugraf.rose.recursos.RecursoUltimaLeituraDeUmDataPoint;

public class Rose extends Application {
	public static Configuracoes config = new Configuracoes();

	public static final String RAIZ_DOS_EXEMPLOS = Configuracoes
			.obterComoTexto("raizDosExemplos");

	public Rose() {
		configureLogger();
	}

	public Rose(Context contexto) {
		super(contexto);
		configureLogger();
	}

	private void configureLogger() {
		Logger root = Logger.getLogger("");
		root.removeHandler(root.getHandlers()[0]);

		int limit = Configuracoes.obterComoNumero(
				"org.restlet.engine.log.AccessLogFileHandler.limit", "0");
		int count = Configuracoes.obterComoNumero(
				"org.restlet.engine.log.AccessLogFileHandler.count", "0");
		boolean append = Configuracoes.obterComoBooleano(
				"org.restlet.util.AccessLogFileHandler.append", false);
		String level = Configuracoes.obterComoTexto(
				"org.restlet.engine.log.AccessLogFileHandler.level", "ALL");
		Level lev = Level.parse(level);
		try {
			AccessLogFileHandler fileHandler = new AccessLogFileHandler(
					"logs/rose.log", limit, count, append);
			fileHandler.setFormatter(new SimpleFormatter());
			fileHandler.setLevel(lev);

			getLogger().addHandler(fileHandler);
		} catch (SecurityException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	@Override
	public Restlet createRoot() {
		Router router = new Router(getContext());

		router.attach("/", RecursoRaizDoRose.class);
		router.attach("/examples", new Directory(getContext(),
				RAIZ_DOS_EXEMPLOS));

		router.attach("/datapoint/{codigo}",
				RecursoUltimaLeituraDeUmDataPoint.class);
		router.attach("/datasources/httpRetriever",
				RecursoDataSourcesHTTPRetriever.class);
		router.attach("/datasource/httpRetriever/{codigo}",
				RecursoDataSourceHTTPRetriever.class);
		router.attach("/datasource/httpRetriever/{codigo}/datapoints",
				RecursoDataPointsDeUmDataSourceHTTPRetriever.class);
		router.attach("/datasources/modbusSerial",
				RecursoDataSourcesModbusSerial.class);
		router.attach("/datasource/modbusSerial/{codigo}/datapoints",
				RecursoDataPointsDeUmDataSourceModbusSerial.class);
		router.attach("/alarmesAtivos", RecursoAlarmesAtivos.class);

		return router;
	}
}
