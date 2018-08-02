package br.org.scadabr;

import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import org.jinterop.dcom.common.JIException;
import org.jinterop.dcom.core.JIVariant;
import org.openscada.opc.dcom.list.ClassDetails;
import org.openscada.opc.lib.common.AlreadyConnectedException;
import org.openscada.opc.lib.common.NotConnectedException;
import org.openscada.opc.lib.da.AccessBase;
import org.openscada.opc.lib.da.Async20Access;
import org.openscada.opc.lib.da.DuplicateGroupException;
import org.openscada.opc.lib.da.Group;
import org.openscada.opc.lib.da.Item;
import org.openscada.opc.lib.da.ItemState;
import org.openscada.opc.lib.list.Categories;
import org.openscada.opc.lib.list.Category;
import org.openscada.opc.lib.list.ServerList;

import br.org.scadabr.tests.DataCallbackDumper;


public class RealOPCMaster extends Thread implements OPCMaster {

	private String host;
	private String domain;
	private String user;
	private String password;
	private String server;
	private String dataSourceXid;
	private ArrayList<Item> dataSourceItems;
	private Item dataSourceItem;
	private ArrayList<ItemState> dataSourceItemStates;
	private ItemState dataSourceItemState;
	private Group group;
	private ArrayList<String> nameOPCItems;
	private OPCUtils opcUtils;
	private ArrayList<DataCallbackDumper> listDataCallBack;
	private AccessBase access;


	public RealOPCMaster() {
		OPCUtils utils = new OPCUtils();
		setOpcUtils(utils);
	}

	/**
	 * 
	 */
	public ArrayList<String> listOPCServers(String user, String password,
			String host, String domain) throws Exception {

		ArrayList<String> listNameOPCServers = new ArrayList<String>();

		ServerList serverList = new ServerList(host, user, password, domain);

		Collection<ClassDetails> detailsList = serverList
		.listServersWithDetails(
				new Category[] { Categories.OPCDAServer20 },
				new Category[] { Categories.OPCDAServer10 });

		for (ClassDetails details : detailsList) {
			listNameOPCServers.add(details.getProgId());
		}

		return listNameOPCServers;
	}

	/**
	 * Retorna os OPCItems do Servidor
	 * 
	 * @throws Exception
	 */
	public ArrayList<OPCItem> browseOPCTags(String user, String password,
			String host, String domain, String servername) throws Exception {
		getOpcUtils().connect(user, password, host, domain, servername);


		String name = String.valueOf(Math.random()*1000);
		Group group = getOpcUtils().getServerOPC().addGroup(name);

		// <OPCItem name, Is settable ?>
		Map<String, Boolean> mapNameOPCItem = new HashMap<String, Boolean>();

		ArrayList<String> nameTotalOPCItems = getOpcUtils().getItems(user, password, host, domain,servername);

		ArrayList<String> nameSetOPCItems = getOpcUtils().getSettableItems(user, password, host, domain, servername);

		for (int i = 0; i < nameTotalOPCItems.size(); i++) {
			mapNameOPCItem.put(nameTotalOPCItems.get(i), false);
		}

		for (int i = 0; i < nameSetOPCItems.size(); i++) {
			mapNameOPCItem.put(nameSetOPCItems.get(i), true);
		}

		ArrayList<Item> listOPCItems = getOpcUtils().addItems(group, nameTotalOPCItems);

		ArrayList<ItemState> listItemState = getOpcUtils().readOPCItems(listOPCItems);

		ArrayList<JIVariant> listJIVariant = new ArrayList<JIVariant>();

		// Coloca numa lista de JIVariant
		for (int i = 0; i < listItemState.size(); i++) {
			listJIVariant.add(listItemState.get(i).getValue());
		}

		// Verifica os tipos de dados do Sevidor OPC
		ArrayList<String> listDataTypeServer = getOpcUtils().verifyDataType(listJIVariant);

		// Converte os dados para os tipos de dados do ScadaBR
		ArrayList<Integer> scadaTypes = getOpcUtils().covertTypeToScada(listDataTypeServer);

		ArrayList<OPCItem> listScadaOPCItems = new ArrayList<OPCItem>();

		// Cria uma lista de ScadaOPCItem com (Tag,Type,Settable)
		for (int i = 0; i < nameTotalOPCItems.size(); i++) {

			// Setavel
			if (mapNameOPCItem.get(nameTotalOPCItems.get(i))) {
				OPCItem opcItem = new OPCItem(nameTotalOPCItems.get(i), scadaTypes
						.get(i), true);
				listScadaOPCItems.add(opcItem);
			}
			else {
				OPCItem opcItem = new OPCItem(nameTotalOPCItems.get(i), scadaTypes
						.get(i), false);
				listScadaOPCItems.add(opcItem);
			}
		}

		return listScadaOPCItems;
	}

	/**
	 * 
	 */
	public void init() throws Exception {
		try {

			getOpcUtils().connect(getUser(), getPassword(), getHost(), getDomain(),
					getServer());
			setGroup(getOpcUtils().getServerOPC().addGroup(dataSourceXid));

			Map<String, Item> mapNameItem = new HashMap<String, Item>();
			Map<Item, ItemState> mapScada = new HashMap<Item, ItemState>();

			getOpcUtils().setMapNameItem(mapNameItem);
			getOpcUtils().setMapScada(mapScada);

		} catch (Exception e) {
			treatException(e);
		}

	}

	private void treatException(Exception e) throws Exception {
		if (e instanceof IllegalArgumentException)
			throw new Exception("Connection failure: Illegal Argument!");
		else if (e instanceof AlreadyConnectedException)
			throw new Exception("Connection failure: Already Connected!");
		else if (e instanceof UnknownHostException)
			throw new Exception(
			"Connection failure: Unknow Host - Check your OPC Server");
		else if (e instanceof NotConnectedException)
			throw new Exception("Connection failure: Not Connected!");
		else if (e instanceof DuplicateGroupException)
			throw new Exception("Connection failure: Duplicate Group!");
		else
			throw new Exception("Connection failure: Unknown Error!");
	}

	/**
	 * 
	 */
	public void do2Poll(int period) throws Exception {

		try {
			// add sync access
			setAccess(new Async20Access(getOpcUtils().getServerOPC(), period, false));

			setListDataCallBack(new ArrayList<DataCallbackDumper>());

			for (int i = 0; i < getNameOPCItems().size(); i++) {
				DataCallbackDumper dcd = new DataCallbackDumper();
				getListDataCallBack().add(dcd);
			}

			for (int i = 0; i < getNameOPCItems().size(); i++) {
				getAccess().addItem(getNameOPCItems().get(i), getListDataCallBack().get(i) );
			}

			getAccess().bind();

		} catch (JIException e) {
			System.out.println(String.format("%08X: %s", e.getErrorCode(),
					getOpcUtils().getServerOPC().getErrorMessage(e.getErrorCode())));
		}

	}

	/**
	 * Realiza um synch read do grupo do datasource
	 */
	public void doPoll() throws Exception {

		getOpcUtils().readOPCItemsMap(getNameOPCItems());

	}

	/**
	 * Termina a conexão com o servidor
	 */
	public void terminate() throws Exception {
		getAccess().clear();
		getAccess().unbind();
		getOpcUtils().getServerOPC().disconnect();
	}

	/**
	 * Adiciona os OPCItems em determinado Grupo no Servidor
	 */
	public void configureGroup(ArrayList<String> itemsName) throws Exception{

		getGroup().clear();
		
		ArrayList<Item> OPCItemsEnabled = getOpcUtils().addItemsMap(getGroup(), itemsName);
		setNameOPCItems(itemsName);
		setDataSourceItems(OPCItemsEnabled);
	}

	public void write(String tag, Object value) throws Exception {
		for (Item item : getDataSourceItems()) {
			if (item.getId().equals(tag)) {
				item.write(new JIVariant(value));
			}
		}
	}

	/**
	 * Retorna o Valor da Tag fornecida
	 */
	public String getValue(String tag) throws Exception {

		String value = "Unknown Tag";

		Item opcItem = getOpcUtils().getMapNameItem().get(tag);

		short quality = 0;

		try {
			if (opcItem != null) {


				if (getOpcUtils().getMapScada().get(opcItem).getQuality() != null) {
					quality = getOpcUtils().getMapScada().get(opcItem).getQuality();
				}

				if (quality != QUALITY_GOOD) {
					throw new Exception("Not GOOD Quality Received: "
							+ toStringQuality(
									quality));
				}

				value = getOpcUtils().getValueOPC(getOpcUtils().getMapScada().get(opcItem).getValue());
			}
		} catch (Exception e) {
			value = toStringQuality(quality);
		}

		return value;

	}

	public String getValue2(String tag) throws Exception {

		String value = "Unknown Tag";

		for (int i = 0; i < getListDataCallBack().size(); i++) {
			if(getListDataCallBack().get(i).getItem().getId().equals(tag)) {
				Item opcItem = getListDataCallBack().get(i).getItem();

				if (opcItem != null) {
					short quality = getListDataCallBack().get(i).getItemState().getQuality();

					if (quality != QUALITY_GOOD) {
						throw new Exception("Not GOOD Quality Received: "
								+ toStringQuality(quality));
					}

					value = getOpcUtils().getValueOPC(getListDataCallBack().get(i).getItemState().getValue());
				}
			}
		}

		return value;
	}

	private String toStringQuality(short quality) {

		switch (quality) {
		case QUALITY_BAD:
			return "BAD_QUALITY";
		case QUALITY_UNCERTAIN:
			return "UNCERTAIN_QUALITY";
		case QUALITY_NA:
			return "NA_QUALITY";
		}

		return "BAD_QUALITY";
	}



	public Item getDataSourceItem() {
		return dataSourceItem;
	}

	public void setDataSourceItem(Item dataSourceItem) {
		this.dataSourceItem = dataSourceItem;
	}

	public ItemState getDataSourceItemState() {
		return dataSourceItemState;
	}

	public void setDataSourceItemState(ItemState dataSourceItemState) {
		this.dataSourceItemState = dataSourceItemState;
	}

	public AccessBase getAccess() {
		return access;
	}

	public void setAccess(AccessBase access) {
		this.access = access;
	}

	public ArrayList<DataCallbackDumper> getListDataCallBack() {
		return listDataCallBack;
	}

	public void setListDataCallBack(ArrayList<DataCallbackDumper> listDataCallBack) {
		this.listDataCallBack = listDataCallBack;
	}

	public OPCUtils getOpcUtils() {
		return opcUtils;
	}

	public void setOpcUtils(OPCUtils opcUtils) {
		this.opcUtils = opcUtils;
	}

	public ArrayList<String> getNameOPCItems() {
		return nameOPCItems;
	}

	public void setNameOPCItems(ArrayList<String> nameOPCItems) {
		this.nameOPCItems = nameOPCItems;
	}

	public ArrayList<ItemState> getDataSourceItemStates() {
		return dataSourceItemStates;
	}

	public void setDataSourceItemStates(
			ArrayList<ItemState> dataSourceItemStates) {
		this.dataSourceItemStates = dataSourceItemStates;
	}

	public Group getGroup() {
		return group;
	}

	public void setGroup(Group group) {
		this.group = group;
	}

	public ArrayList<Item> getDataSourceItems() {
		return dataSourceItems;
	}

	public void setDataSourceItems(ArrayList<Item> dataSourceItems) {
		this.dataSourceItems = dataSourceItems;
	}

	public String getHost() {
		return host;
	}

	public void setHost(String host) {
		this.host = host;
	}

	public String getDomain() {
		return domain;
	}

	public void setDomain(String domain) {
		this.domain = domain;
	}

	public String getUser() {
		return user;
	}

	public void setUser(String user) {
		this.user = user;
	}

	public String getPassword() {
		return password;
	}

	public void setPassword(String password) {
		this.password = password;
	}

	public String getServer() {
		return server;
	}

	public void setServer(String server) {
		this.server = server;
	}

	@Override
	public void setDataSourceXid(String xid) {
		this.dataSourceXid = xid;

	}

}
