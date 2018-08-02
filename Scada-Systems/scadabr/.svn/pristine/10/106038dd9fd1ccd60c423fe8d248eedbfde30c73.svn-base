package br.org.scadabr;

import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.EnumSet;
import java.util.Map;

import org.jinterop.dcom.common.JIException;
import org.jinterop.dcom.core.JIVariant;
import org.openscada.opc.dcom.da.OPCSERVERSTATE;
import org.openscada.opc.dcom.da.OPCSERVERSTATUS;
import org.openscada.opc.dcom.list.ClassDetails;
import org.openscada.opc.lib.common.AlreadyConnectedException;
import org.openscada.opc.lib.common.ConnectionInformation;
import org.openscada.opc.lib.da.AddFailedException;
import org.openscada.opc.lib.da.AutoReconnectController;
import org.openscada.opc.lib.da.Group;
import org.openscada.opc.lib.da.Item;
import org.openscada.opc.lib.da.ItemState;
import org.openscada.opc.lib.da.Server;
import org.openscada.opc.lib.da.browser.Access;
import org.openscada.opc.lib.da.browser.BaseBrowser;
import org.openscada.opc.lib.list.Categories;
import org.openscada.opc.lib.list.Category;
import org.openscada.opc.lib.list.ServerList;
import org.openscada.utils.timing.Scheduler;

public class  OPCUtils {

	private Map<String, Item> mapNameItem;
	private Map<Item, ItemState> mapScada;
	private Server serverOPC;
	private AutoReconnectController reconnectServerOPC;

	public void verifyServerInfo(Server serverOPC) {
		
		OPCSERVERSTATUS opcServerStatus = serverOPC.getServerState();
		System.out.println(opcServerStatus.getVendorInfo());
		System.out.println(opcServerStatus.getGroupCount());
		
		OPCSERVERSTATE opcServerState = opcServerStatus.getServerState();
		System.out.println(opcServerState.name());
		
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public OPCItem validateTag(String tag, String user, String password, String host, String domain, String servername) {
		
		OPCItem opcItem = new OPCItem(tag, 0, true);
		
		try {
			Server server = connect(user, password, host, domain, servername);

			String name = String.valueOf(Math.random()*1000);
			Group group = server.addGroup(name);
			
			Item it = addItem(group, tag);
			
			ItemState is = it.read(true);
			
			JIVariant ji = is.getValue();
			
			String opcServerType = verifyDataType(ji);
			
			int scadaType = covertTypeToScada(opcServerType);
			
			opcItem.setDataType(scadaType);
			opcItem.setValidate(true);
			
			return opcItem;
		} catch (Exception e) {
			e.printStackTrace();
			opcItem.setValidate(false);
			opcItem.setDataType(OPCMaster.Unknown_Value_Type);
			return opcItem;
		}
	}
	
	/**
	 * Adiciona items ao grupo fornecido no Servidor e retorna os Items inseridos
	 */
	public ArrayList<Item> addItems(Group group, ArrayList<String> nameOPCItems)
	throws JIException, AddFailedException {

		ArrayList<Item> listOPCItems = new ArrayList<Item>();
		
		try {
			for (int i = 0; i < nameOPCItems.size(); i++) {
				listOPCItems.add(group.addItem(nameOPCItems.get(i)));
			}
			
			return listOPCItems;
		} catch (Exception e) {
			e.printStackTrace();
			return listOPCItems;
		}

	}

	/**
	 * Adiciona item ao grupo fornecido no Servidor
	 */
	public Item addItem(Group group, String nameOPCItem)
	throws JIException, AddFailedException {

		return group.addItem(nameOPCItem);
	}

	/**
	 * Adiciona items ao grupo fornecido no Servidor, coloca em um HashMap e retorna os Items inseridos
	 */
	public ArrayList<Item> addItemsMap(Group group, ArrayList<String> nameOPCItems)
	throws JIException, AddFailedException {

		ArrayList<Item> listOPCItems = new ArrayList<Item>();
		
			for (int i = 0; i < nameOPCItems.size(); i++) {
					listOPCItems.add(group.addItem(nameOPCItems.get(i)));
					getMapNameItem().put(nameOPCItems.get(i),listOPCItems.get(i));
			}
			
			return listOPCItems;

	}

	/**
	 * Le os items fornecidos do Servidor
	 */
	public ArrayList<ItemState> readOPCItems(ArrayList<Item> listOPCItems)
	throws JIException {

		ArrayList<ItemState> listItemState = new ArrayList<ItemState>();

		for (int i = 0; i < listOPCItems.size(); i++) {
			try {
				listItemState.add(listOPCItems.get(i).read(true));
			} catch (Exception e) {
				ItemState is = new ItemState();
				JIVariant ji = new JIVariant("u64/?");
				is.setValue(ji);
				listItemState.add(is);
			}
		}

		return listItemState;
	}

	/**
	 * Le os items fornecidos do Servidor e coloca em um HashMap
	 */
	public void readOPCItemsMap(ArrayList<String> nameOPCItem)
	throws Exception {

		for (int i = 0; i < nameOPCItem.size(); i++) {
			try {
					getMapScada().put(getMapNameItem().get(nameOPCItem.get(i)), getMapNameItem().get(nameOPCItem.get(i)).read(true));
			} catch (Exception e) {
				ItemState is = new ItemState();
				JIVariant ji = new JIVariant("u64/?");
				is.setValue(ji);
				getMapScada().put(getMapNameItem().get(nameOPCItem.get(i)), is);

				throw new Exception("[OPC] READ OPC ITEMS MAP Failed!");
			}
		}
	}

	/**
	 * Retorna todos os Items do Servidor
	 */
	public ArrayList<String> getItems(String user, String password,
			String host, String domain, String servername)
			throws IllegalArgumentException, UnknownHostException, JIException {

		ArrayList<String> nameOPCItems = new ArrayList<String>();

		// Browse flat
		BaseBrowser flatBrowser = getServerOPC().getFlatBrowser();
		
		if (flatBrowser != null) {
			for (String item : getServerOPC().getFlatBrowser().browse("")) {
				nameOPCItems.add(item);
			}
		}

		return nameOPCItems;
	}

	/**
	 * Retorna todos os Items Setaveis do Servidor
	 */
	public ArrayList<String> getSettableItems(String user, String password,
			String host, String domain, String servername) throws IllegalArgumentException, UnknownHostException, JIException {

		ArrayList<String> writtableItems = (ArrayList<String>) getServerOPC()
		.getFlatBrowser().browse(EnumSet.of(Access.WRITE));

		return writtableItems;
	}

	/**
	 * 
	 */
	public ArrayList<String> verifyDataType(ArrayList<JIVariant> listJIVariant)
	throws JIException {

		ArrayList<String> typeDataOPCItem = new ArrayList<String>();

		for (int i = 0; i < listJIVariant.size(); i++) {
			Object obj = listJIVariant.get(i).getObject();

			if (obj.getClass().equals(Double.class)) {
				typeDataOPCItem.add("Double");
			} else if (obj.getClass().equals(Float.class)) {
				typeDataOPCItem.add("Float");
			} else if (obj.getClass().equals(Byte.class)) {
				typeDataOPCItem.add("Byte");
			} else if (obj.getClass().equals(Character.class)) {
				typeDataOPCItem.add("Character");
			} else if (obj.getClass().equals(Integer.class)) {
				typeDataOPCItem.add("Integer");
			} else if (obj.getClass().equals(Long.class)) {
				typeDataOPCItem.add("Long");
			} else if (obj.getClass().equals(Boolean.class)) {
				typeDataOPCItem.add("Boolean");
			} else if (obj.getClass().equals(org.jinterop.dcom.core.JIUnsignedByte.class)) {
				typeDataOPCItem.add("JIUnsignedByte");
			} else if (obj.getClass().equals(Short.class)) {
				typeDataOPCItem.add("Short");
			} else if (obj.getClass().equals(org.jinterop.dcom.core.JIUnsignedShort.class)) {
				typeDataOPCItem.add("JIUnsignedShort");
			} else if (obj.getClass().equals(org.jinterop.dcom.core.JIUnsignedInteger.class)) {
				typeDataOPCItem.add("JIUnsignedInteger");
			} else if (obj.getClass().equals(org.jinterop.dcom.core.JIString.class)) {
				typeDataOPCItem.add("JIString");
			} else if (obj.getClass().equals(org.jinterop.dcom.core.JICurrency.class)) {
				typeDataOPCItem.add("JICurrency");
			} else if (obj.getClass().equals(Date.class)) {
				typeDataOPCItem.add("Date");
			} else {
				typeDataOPCItem.add("Unknown value type");
			}
		}

		return typeDataOPCItem;
	}

	public String verifyDataType(JIVariant jIVariant)
	throws JIException {

		String typeDataOPCItem = "";

		Object obj = jIVariant.getObject();
		
		if (obj.getClass().equals(Double.class)) {
			typeDataOPCItem = "Double";
		} else if (obj.getClass().equals(Float.class)) {
			typeDataOPCItem = "Float";
		} else if (obj.getClass().equals(Byte.class)) {
			typeDataOPCItem = "Byte";
		} else if (obj.getClass().equals(Character.class)) {
			typeDataOPCItem = "Character";
		} else if (obj.getClass().equals(Integer.class)) {
			typeDataOPCItem = "Integer";
		} else if (obj.getClass().equals(Long.class)) {
			typeDataOPCItem = "Long";
		} else if (obj.getClass().equals(Boolean.class)) {
			typeDataOPCItem = "Boolean";
		} else if (obj.getClass().equals(org.jinterop.dcom.core.JIUnsignedByte.class)) {
			typeDataOPCItem = "JIUnsignedByte";
		} else if (obj.getClass().equals(Short.class)) {
			typeDataOPCItem = "Short";
		} else if (obj.getClass().equals(org.jinterop.dcom.core.JIUnsignedShort.class)) {
			typeDataOPCItem = "JIUnsignedShort";
		} else if (obj.getClass().equals(org.jinterop.dcom.core.JIUnsignedInteger.class)) {
			typeDataOPCItem = "JIUnsignedInteger";
		} else if (obj.getClass().equals(org.jinterop.dcom.core.JIString.class)) {
			typeDataOPCItem = "JIString";
		} else if (obj.getClass().equals(org.jinterop.dcom.core.JICurrency.class)) {
			typeDataOPCItem = "JICurrency";
		} else if (obj.getClass().equals(Date.class)) {
			typeDataOPCItem = "Date";
		} else {
			typeDataOPCItem = "Unknown value type" ;
		}

		return typeDataOPCItem;
	}

	/**
	 * 
	 */
	public String getValueOPC(JIVariant jIVariant) throws JIException {

		String valueOPCItem = "";

		Object obj = jIVariant.getObject();
		
		String type = verifyDataType(jIVariant);
		
		if (type.equals("Double")) {
			valueOPCItem = String.valueOf(((Double) obj).doubleValue());
		} else if (type.equals("Float")) {
			valueOPCItem = String.valueOf(((Float) obj).floatValue());
		} else if (type.equals("Byte")) {
			valueOPCItem = String.valueOf(((Byte) obj).byteValue());
		} else if (type.equals("Character")) {
			Character c = ((Character) obj).charValue();
			valueOPCItem = String.valueOf(c.toString().getBytes()[0]);
		} else if (type.equals("Integer")) {
			valueOPCItem = String.valueOf(((Integer) obj).intValue());
		} else if (type.equals("Long")) {
			valueOPCItem = String.valueOf(((Long) obj).longValue());
		} else if (type.equals("Boolean")) {
			valueOPCItem = String.valueOf(((Boolean) obj).booleanValue());
		} else if (type.equals("JIUnsignedByte")) {
			valueOPCItem = String
			.valueOf(((org.jinterop.dcom.core.JIUnsignedByte) obj)
					.getValue());
		} else if (type.equals("Short")) {
			valueOPCItem = String.valueOf(((Short) obj).shortValue());
		} else if (type.equals("JIUnsignedShort")) {
			valueOPCItem = String
			.valueOf(((org.jinterop.dcom.core.JIUnsignedShort) obj)
					.getValue());
		} else if (type.equals("JIUnsignedInteger")) {
			valueOPCItem = String
			.valueOf(((org.jinterop.dcom.core.JIUnsignedInteger) obj)
					.getValue());
		} else if (type.equals("JIString")) {
			valueOPCItem = jIVariant.getObjectAsString2();
		} else if (type.equals("JICurrency")) {
			valueOPCItem = String
			.valueOf(((org.jinterop.dcom.core.JICurrency) obj)
					.getUnits());
		} else if (type.equals("Date")) {
			Date date = new Date(((Date) obj).getTime());
			valueOPCItem = date.toString();
		} else {
			valueOPCItem = "Unknown value type";
		}

		return valueOPCItem;
	}


	/**
	 * 
	 */
	public ArrayList<Integer> covertTypeToScada(ArrayList<String> typeDataOPCItem) {

		ArrayList<Integer> typeDataScada = new ArrayList<Integer>();

		for (int i = 0; i < typeDataOPCItem.size(); i++) {
			if (typeDataOPCItem.get(i).equals("Double")
					|| typeDataOPCItem.get(i).equals("Float")
					|| typeDataOPCItem.get(i).equals("Integer")
					|| typeDataOPCItem.get(i).equals("Long")
					|| typeDataOPCItem.get(i).equals("JIUnsignedByte")
					|| typeDataOPCItem.get(i).equals("Byte")
					|| typeDataOPCItem.get(i).equals("JIUnsignedShort")
					|| typeDataOPCItem.get(i).equals("JIUnsignedInteger")
					|| typeDataOPCItem.get(i).equals("JICurrency")
					|| typeDataOPCItem.get(i).equals("Short")) {
				typeDataScada.add(OPCMaster.NUMERIC);
			} else if (typeDataOPCItem.get(i).equals("Boolean")) {
				typeDataScada.add(OPCMaster.BINARY);
			} else if (typeDataOPCItem.get(i).equals("Date") 
					|| typeDataOPCItem.get(i).equals("Character")
					|| typeDataOPCItem.get(i).equals("JIString")) {
				typeDataScada.add(OPCMaster.ALPHANUMERIC);
			} else if (typeDataOPCItem.get(i).equals("Unknown value type")) {
				typeDataScada.add(OPCMaster.Unknown_Value_Type);
			} else {
				typeDataScada.add(OPCMaster.BAD);
			}
		}

		return typeDataScada;
	}

	public Integer covertTypeToScada(String typeDataOPCItem) {

		int typeDataScada = 0;

		if (typeDataOPCItem.equals("Double")
				|| typeDataOPCItem.equals("Float")
				|| typeDataOPCItem.equals("Integer")
				|| typeDataOPCItem.equals("Long")
				|| typeDataOPCItem.equals("JIUnsignedByte")
				|| typeDataOPCItem.equals("Byte")
				|| typeDataOPCItem.equals("JIUnsignedShort")
				|| typeDataOPCItem.equals("JIUnsignedInteger")
				|| typeDataOPCItem.equals("JICurrency")
				|| typeDataOPCItem.equals("Short")) {
			typeDataScada = OPCMaster.NUMERIC;
		} else if (typeDataOPCItem.equals("Boolean")) {
			typeDataScada = OPCMaster.BINARY;
		} else if (typeDataOPCItem.equals("Date") 
				|| typeDataOPCItem.equals("Character")
				|| typeDataOPCItem.equals("JIString")) {
			typeDataScada = OPCMaster.ALPHANUMERIC;
		} else if (typeDataOPCItem.equals("Unknown value type")) {
			typeDataScada = OPCMaster.Unknown_Value_Type;
		} else {
			typeDataScada = OPCMaster.BAD;
		}

		return typeDataScada;
	}

	/**
	 * @throws AlreadyConnectedException
	 * 
	 */
	public Server connect(String user, String password, String host,
			String domain, String servername) throws IllegalArgumentException,
			UnknownHostException, JIException, AlreadyConnectedException {

		Server serverOPC = null ;
		
		try {
			String clsid = "";
			
			ServerList serverListOPC = new ServerList(host, user, password, domain);
			
			Collection<ClassDetails> detailsList = serverListOPC
			.listServersWithDetails(
					new Category[] { Categories.OPCDAServer20 },
					new Category[] { Categories.OPCDAServer10});
			
			// Pega o CLSID conforme o nome do servidor //
			for (ClassDetails classDetails : detailsList) {
				if (servername.equals(classDetails.getProgId())) {
					clsid = classDetails.getClsId();
				}
			}
			// Configura as informacoes de conexao //
			ConnectionInformation ci = new ConnectionInformation();
			ci.setHost(host);
			ci.setDomain(domain);
			ci.setUser(user);
			ci.setPassword(password);
			ci.setClsid(clsid);
			
			// Cria um novo servidor //
			serverOPC = new Server(ci, new Scheduler(true));
			serverOPC.connect();
			setServerOPC(serverOPC);
			return serverOPC;
			
		} catch (Exception e) {
			e.printStackTrace();
			serverOPC = null;
			return serverOPC;
		}
		
	}

	public void enableReconnect(){
		AutoReconnectController autoReconnectController = new AutoReconnectController(
				getServerOPC());

		try {
			autoReconnectController.connect();
			setReconnectServerOPC(autoReconnectController);
		} catch (Exception e) {
			// TODO: handle exception
		}

	}


	public AutoReconnectController getReconnectServerOPC() {
		return reconnectServerOPC;
	}

	public void setReconnectServerOPC(AutoReconnectController reconnectServerOPC) {
		this.reconnectServerOPC = reconnectServerOPC;
	}

	public Server getServerOPC() {
		return serverOPC;
	}

	public void setServerOPC(Server serverOPC) {
		this.serverOPC = serverOPC;
	}

	public Map<Item, ItemState> getMapScada() {
		return mapScada;
	}

	public void setMapScada(Map<Item, ItemState> mapScada) {
		this.mapScada = mapScada;
	}

	public Map<String, Item> getMapNameItem() {
		return mapNameItem;
	}

	public void setMapNameItem(Map<String, Item> mapNameItem) {
		this.mapNameItem = mapNameItem;
	}



}
