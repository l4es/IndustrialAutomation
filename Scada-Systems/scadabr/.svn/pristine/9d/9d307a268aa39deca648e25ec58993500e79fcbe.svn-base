/*
 Mango - Open Source M2M - http://mango.serotoninsoftware.com
 Copyright (C) 2006-2011 Serotonin Software Technologies Inc.
 @author Matthew Lohbihler
    
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package com.serotonin.mango.util;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import com.serotonin.mango.Common;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.NodeList;

/**
 * @author Matthew Lohbihler
 */
@Deprecated //APL hanle this in the html files directly ...
public class DocumentationManifest {

    public List<Element> getElementsByTagName(Element parent, String name) {
        List<Element> result = new ArrayList<>();

        final NodeList list = parent.getElementsByTagName(name);
        for (int i = 0; i < list.getLength(); i++) {
            result.add((Element) list.item(i));
        }
        return result;
    }

    private final List<DocumentationItem> items = new ArrayList<>();

    public DocumentationManifest() throws Exception {
        // Read the documentation manifest file.
        final DocumentBuilder builder;
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        builder = factory.newDocumentBuilder();

        Document document = builder.parse(new File(Common.getDocPath() + "manifest.xml"));

        Element root = document.getDocumentElement();

        final NodeList itemList = root.getElementsByTagName("item");
        for (int nodeIndex = 0; nodeIndex < itemList.getLength(); nodeIndex++) {
            final Element item = ((Element) itemList.item(nodeIndex));
            DocumentationItem di = new DocumentationItem(item.getAttribute("id"));

            final NodeList relationList = item.getElementsByTagName("relation");
            for (int relationIndex = 0; relationIndex < relationList.getLength(); relationIndex++) {
                di.addRelated(((Element) relationList.item(relationIndex)).getAttribute("id"));
            }
            items.add(di);
        }
    }

    public DocumentationItem getItem(String id) {
        for (DocumentationItem di : items) {
            if (id.equals(di.getId())) {
                return di;
            }
        }
        return null;
    }
}
