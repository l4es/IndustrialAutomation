/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib;

import br.org.scadabr.web.util.PagingDataForm;
import java.util.Enumeration;
import java.util.List;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.jsp.tagext.TagSupport;

/**
 *
 * @author aploese
 */
public class PaginationTagSupport extends TagSupport {

    protected PagingDataForm paging;
    protected String prefix = "";

    public void setPaging(PagingDataForm paging) {
        this.paging = paging;
    }

    public void setPrefix(String prefix) {
        this.prefix = prefix;
    }

    protected String getBaseHref(List<String> excludeParamNames) {
        HttpServletRequest request = (HttpServletRequest) pageContext.getRequest();

        StringBuilder baseHref = new StringBuilder();
        baseHref.append(request.getAttribute("javax.servlet.forward.request_uri"));

        boolean first = true;
        Enumeration<String> paramNames = pageContext.getRequest().getParameterNames();
        while (paramNames.hasMoreElements()) {
            String name = paramNames.nextElement();
            if (!excludeParamNames.contains(name)) {
                if (first) {
                    baseHref.append('?');
                    first = false;
                } else {
                    baseHref.append("&amp;");
                }
                if (paging != null) {
                    if ((prefix + "page").equals(name)) {
                        baseHref.append(name).append('=').append(Integer.toString(paging.getPage()));
                    } else if ((prefix + "numberOfPages").equals(name)) {
                        baseHref.append(name).append('=').append(Integer.toString(paging.getNumberOfPages()));
                    } else if ((prefix + "offset").equals(name)) {
                        baseHref.append(name).append('=').append(Integer.toString(paging.getOffset()));
                    } else if ((prefix + "itemsPerPage").equals(name)) {
                        baseHref.append(name).append('=').append(Integer.toString(paging.getItemsPerPage()));
                    } else if ((prefix + "sortField").equals(name)) {
                        baseHref.append(name).append('=').append(paging.getSortField());
                    } else if ((prefix + "sortDesc").equals(name)) {
                        baseHref.append(name).append('=').append(Boolean.toString(paging.getSortDesc()));
                    } else {
                        baseHref.append(name).append('=').append(request.getParameter(name));
                    }
                } else {
                    baseHref.append(name).append('=').append(request.getParameter(name));
                }
            }
        }
        if (first) {
            baseHref.append('?');
        } else {
            baseHref.append("&amp;");
        }
        return baseHref.toString();
    }

    @Override
    public void release() {
        super.release();
        paging = null;
        prefix = "";
    }
}
