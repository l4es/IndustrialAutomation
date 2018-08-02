package br.org.scadabr.tests;

/*
 * This file is part of the OpenSCADA project
 * Copyright (C) 2006-2007 inavare GmbH (http://inavare.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

import org.openscada.opc.lib.da.DataCallback;
import org.openscada.opc.lib.da.Item;
import org.openscada.opc.lib.da.ItemState;

import br.org.scadabr.OPCUtils;

public class DataCallbackDumper implements DataCallback {
	
	private OPCUtils opcUtil;
	private Item item;
	private ItemState itemState;
	
	public DataCallbackDumper() {
		OPCUtils oUtils = new OPCUtils();
		setOpcUtil(oUtils);
	}
	
	public DataCallbackDumper(Item it, ItemState is) {
		OPCUtils oUtils = new OPCUtils();
		setOpcUtil(oUtils);
		setItem(it);
		setItemState(is);
	}
	
	public void changed(Item item, ItemState itemState) {
//		System.out.println(String.format(
//				"Item: %s, Value: %s, Timestamp: %tc, Quality: %d", item
//						.getId(), itemState.getValue(), itemState
//						.getTimestamp(), itemState.getQuality()));
		try {
//			System.out.println(getOpcUtil().getValueOPC(itemState.getValue()));
//			System.out.println("ADICIONOU: " + item.getId());
			setItem(item);
			setItemState(itemState);
			
		} catch (Exception e) {
			e.printStackTrace();
		}

	}

	public Item getItem() {
		return item;
	}

	public void setItem(Item item) {
		this.item = item;
	}

	public ItemState getItemState() {
		return itemState;
	}

	public void setItemState(ItemState itemState) {
		this.itemState = itemState;
	}

	public OPCUtils getOpcUtil() {
		return opcUtil;
	}

	public void setOpcUtil(OPCUtils opcUtil) {
		this.opcUtil = opcUtil;
	}

	
	
	
}
