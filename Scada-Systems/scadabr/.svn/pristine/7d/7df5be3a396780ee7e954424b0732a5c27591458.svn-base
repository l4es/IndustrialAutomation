/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import javax.servlet.jsp.JspException;

/**
 *
 * @author aploese
 */
public class PaginationUrlTag extends PaginationTagSupport {

    private String excludeParams;

    public void setExcludeParams(String excludeParams) {
        this.excludeParams = excludeParams;
    }

    @Override
    public int doStartTag() throws JspException {
        List<String> exclude = new ArrayList<>();
        if (excludeParams != null) {
            addExcludeParams(exclude);
        }
        try {
            pageContext.getOut().write(getBaseHref(exclude));
        } catch (IOException e) {
            throw new JspException("Error writing page info", e);
        }
        return SKIP_BODY;
    }

    protected void addExcludeParams(List<String> excludeList) {
        if (excludeParams != null) {
            excludeList.addAll(Arrays.asList(excludeParams.split(",")));
        }
    }

    @Override
    public void release() {
        super.release();
        excludeParams = null;
    }
}
