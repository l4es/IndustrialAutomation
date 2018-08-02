/*
 * Copyright 2002-2014 the original author or authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package br.org.scadabr.web.taglib.dijit;

import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.utils.i18n.LocalizableEnum;
import br.org.scadabr.web.taglib.DojoTag;
import java.util.Collection;
import java.util.Map;
import java.util.Objects;
import javax.servlet.jsp.JspException;
import org.springframework.web.servlet.tags.form.TagWriter;

public class SelectTag extends DojoTag {

    public SelectTag() {
        super(SKIP_BODY, "select", "dijit/form/Select");
    }

    private String i18nLabel;
    private String i18nTitle;
    private String name;
    private Object selectedValue;
    private Object items;

    @Override
    protected void writeAttributes(TagWriter tagWriter) throws JspException {
        tagWriter.writeOptionalAttributeValue("name", name);
        if (i18nLabel != null) {
            tagWriter.writeAttribute("label", getRequestContext().getMessage(i18nLabel) + ":");
        }
        if (i18nTitle != null) {
            tagWriter.writeAttribute("title", getRequestContext().getMessage(i18nTitle));
        } else if (i18nLabel != null) {
            tagWriter.writeAttribute("title", getRequestContext().getMessage(i18nLabel));
        }
    }

    @Override
    protected int writeTagContent(TagWriter tagWriter) throws JspException {
        int result = super.writeTagContent(tagWriter);
        if (items instanceof Collection) {
            writeCollection(tagWriter);
        } else if (items instanceof Map) {
            writeMap(tagWriter);
        } else {
            throw new ImplementMeException();
        }
        return result;
    }

    private void writeCollection(TagWriter tagWriter) throws JspException {
        for (Object o : (Collection) items) {
            writeOptionTag(tagWriter, o);
        }
    }

    /**
     * @param i18nLabel the i18nLabel to set
     */
    public void setI18nLabel(String i18nLabel) {
        this.i18nLabel = i18nLabel;
    }

    /**
     * @param i18nTitle the i18nTitle to set
     */
    public void setI18nTitle(String i18nTitle) {
        this.i18nTitle = i18nTitle;
    }

    /**
     * @param name the name to set
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * @param SelectedValue the selectedValue to set
     */
    public void setSelectedValue(Object selectedValue) {
        this.selectedValue = selectedValue;
    }

    /**
     * @param items the items to set
     */
    public void setItems(Object items) {
        this.items = items;
    }

    private void writeOptionTag(TagWriter tagWriter, Object o) throws JspException {
        tagWriter.startTag("option");
        if (o instanceof LocalizableEnum) {
            final LocalizableEnum e = (LocalizableEnum) o;
            if (selectedValue == e) {
                tagWriter.writeAttribute("selected", "selected");
            }
            tagWriter.writeAttribute("value", e.getName());
            tagWriter.appendValue(getRequestContext().getMessage(e.getI18nKey()));
        } else if (o instanceof Map.Entry) {
            final Map.Entry me = (Map.Entry) o;
            if (Objects.equals(selectedValue, me.getKey())) {
                tagWriter.writeAttribute("selected", "selected");
            }
            tagWriter.writeAttribute("value", me.getKey().toString());
            tagWriter.appendValue(me.getValue().toString());
        } else {
            throw new ImplementMeException();
        }
        tagWriter.endTag();

    }

    private void writeMap(TagWriter tagWriter) throws JspException {
        for (Map.Entry o : ((Map<?, ?>) items).entrySet()) {
            writeOptionTag(tagWriter, o);
        }
    }

}
