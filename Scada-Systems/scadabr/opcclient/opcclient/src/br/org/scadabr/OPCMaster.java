package br.org.scadabr;

import java.util.ArrayList;

import org.openscada.opc.lib.da.AccessBase;
import org.openscada.opc.lib.da.Group;
import org.openscada.opc.lib.da.Item;
import org.openscada.opc.lib.da.ItemState;

import br.org.scadabr.tests.DataCallbackDumper;


public interface OPCMaster {
	public static final int BINARY = 1;
	public static final int MULTISTATE = 2;
	public static final int NUMERIC = 3;
	public static final int ALPHANUMERIC = 4;
	public static final int IMAGE = 5;
	public static final int Unknown_Value_Type = 6;
	public static final int BAD = 7;

	public static final short QUALITY_BAD = 0;
	public static final short QUALITY_UNCERTAIN = 0x40;
	public static final short QUALITY_NA = 0x80;
	public static final short QUALITY_GOOD = 0xC0;

	public void setHost(String host);

	public void setDomain(String domain);

	public void setUser(String user);

	public void setPassword(String password);

	public void setServer(String server);

	public void setDataSourceXid(String xid);
	
	public void setAccess(AccessBase access);
	
	public AccessBase getAccess();
	
	public ArrayList<DataCallbackDumper> getListDataCallBack();

	public void setListDataCallBack(ArrayList<DataCallbackDumper> listDataCallBack);
	
	public OPCUtils getOpcUtils();

	public void setOpcUtils(OPCUtils opcUtils);

	public ArrayList<String> getNameOPCItems();

	public void setNameOPCItems(ArrayList<String> nameOPCItems);
	
	public ArrayList<ItemState> getDataSourceItemStates();

	public void setDataSourceItemStates(
			ArrayList<ItemState> dataSourceItemStates);
	
	public Group getGroup();

	public void setGroup(Group group);

	public ArrayList<Item> getDataSourceItems();

	public void setDataSourceItems(ArrayList<Item> dataSourceItems);
	
	public ArrayList<String> listOPCServers(String user, String password,
			String host, String domain) throws Exception;

	public ArrayList<OPCItem> browseOPCTags(String user, String password,
			String host, String domain, String servername) throws Exception;

	public abstract void init() throws Exception;

	public abstract void doPoll() throws Exception;
	
	public abstract void do2Poll(int period) throws Exception;

	public abstract void terminate() throws Exception;

	public abstract void configureGroup(ArrayList<String> items)
			throws Exception;

	public abstract String getValue(String tag) throws Exception;
	
	public abstract String getValue2(String tag) throws Exception;
	
	public abstract void write(String tag, Object value) throws Exception;
	
	


}