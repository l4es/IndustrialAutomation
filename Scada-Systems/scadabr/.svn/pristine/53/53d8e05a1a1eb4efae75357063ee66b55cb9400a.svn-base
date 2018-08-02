package br.org.scadabr.tests;

import java.util.ArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.jinterop.dcom.common.JISystem;
import org.openscada.opc.lib.da.Server;

import br.org.scadabr.OPCItem;
import br.org.scadabr.OPCUtils;
import br.org.scadabr.RealOPCMaster;

public class OPCTest extends Thread{


	private String user = "rba";
	private String password = "samsung";
	private String host = "cpc-5566";
	private String domain = "";
	private String servername = "Graybox.Simulator.1";

	public void testListOPCSevers() throws Exception {

		RealOPCMaster realOPC = new RealOPCMaster();
		
		ArrayList<String> listOPCServers = new ArrayList<String>();
		
		listOPCServers = realOPC.listOPCServers(user, password, host, domain);
		
		for (int i = 0; i < listOPCServers.size(); i++) {
			System.out.println(listOPCServers.get(i));
		}
	}

	public void testbrowseOPCTags() throws Exception {
		
		RealOPCMaster realOPC = new RealOPCMaster();
		
		ArrayList<OPCItem> listOPCItem = new ArrayList<OPCItem>();
		
		listOPCItem = realOPC.browseOPCTags(user, password, host, domain, servername);
		
		System.out.println(listOPCItem.size());
		for (int i = 0; i < listOPCItem.size(); i++) {
			System.out.println(listOPCItem.get(i).getTag());
		}
	}
	
	public void testInit() throws Exception {
		
		RealOPCMaster realOPC = new RealOPCMaster();
		
		realOPC.setHost(host);
		realOPC.setDomain(domain);
		realOPC.setUser(user);
		realOPC.setPassword(password);
		realOPC.setServer(servername);
		realOPC.setDataSourceXid("Teste");
		
		realOPC.init();
	}

	public void testConfigureGroup() throws Exception {

	}

	public void testDoPoll() throws Exception {

	}

	public void testeGetValue() throws Exception {

	}
	
	public void testOPCServerInfo() throws Exception {
		RealOPCMaster realOPC = new RealOPCMaster();
		
		ArrayList<String> listOPCServers = new ArrayList<String>();
		listOPCServers = realOPC.listOPCServers(user, password, host, domain);
		for (int i = 0; i < listOPCServers.size(); i++) {
			System.out.println(listOPCServers.get(i));
		}
		Thread.sleep(2000);
		for (int i = 0; i < listOPCServers.size(); i++) {
			Server serverOPC = realOPC.getOpcUtils().connect(user, password, host, domain, listOPCServers.get(i));
			if (serverOPC == null) {
				System.out.println(i + " : " + listOPCServers.get(i));
				continue;
			}
			realOPC.getOpcUtils().verifyServerInfo(serverOPC);
			Thread.sleep(2000);
		}
		
	}
	
	// Passo completo no ScadaBR com a OPC
	public void testLoop() throws Exception{
		
		RealOPCMaster realOPC = new RealOPCMaster();
		
		realOPC.setHost(host);
		realOPC.setDomain(domain);
		realOPC.setUser(user);
		realOPC.setPassword(password);
		realOPC.setServer(servername);
		realOPC.setDataSourceXid("Teste");
		
		realOPC.init();
		
		ArrayList<OPCItem> opcItens = realOPC.browseOPCTags(user, password, host, domain, servername);
		
		ArrayList<String> opcItensName = new ArrayList<String>();
		
		for (int i = 0; i < opcItens.size(); i++) {
			opcItensName.add(opcItens.get(i).getTag());
		}
		
		opcItensName.add("alalalala");
		
		realOPC.configureGroup(opcItensName);
		
		realOPC.doPoll();
		
		for (int i = 0; i < opcItens.size(); i++) {
			System.out.println(opcItens.get(i).getTag() + " : " + realOPC.getValue(opcItens.get(i).getTag()));
		}
		
	}
	
	public void testValidateTag() throws Exception{
		OPCUtils opcUtil = new OPCUtils();
		
		String tag = "numeric.random.int8";
		
		OPCItem opcItem = opcUtil.validateTag(tag, user, password, host, domain, servername);
		
		System.out.println(tag + " : " + opcItem.isValidate());
	}

	public static void main(String[] args) throws Exception {
		OPCTest test = new OPCTest();
		
		Logger log = JISystem.getLogger();
		log.setLevel(Level.OFF);
		
//		test.testValidateTag();
//		test.testOPCServerInfo();
//		test.testLoop();
//		test.testListOPCSevers();
//		test.testbrowseOPCTags();
//		test.testInit();
	}

}
