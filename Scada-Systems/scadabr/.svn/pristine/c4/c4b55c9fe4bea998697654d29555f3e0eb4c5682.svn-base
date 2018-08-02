/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib;

import java.io.IOException;
import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.jsp.JspException;
import javax.servlet.jsp.JspWriter;
import javax.servlet.jsp.tagext.TagSupport;
import org.directwebremoting.Container;
import org.directwebremoting.ScriptBuffer;
import org.directwebremoting.WebContextFactory;
import org.directwebremoting.extend.ConverterManager;
import org.directwebremoting.extend.MarshallException;
import org.directwebremoting.extend.ScriptBufferUtil;

/**
 *
 * @author aploese
 */
public class DwrConvertTag extends TagSupport {

    private Object obj;

    public void setObj(Object obj) {
        this.obj = obj;
    }

    @Override
    public int doStartTag() throws JspException {
        ServletContext servletContext = pageContext.getServletContext();
        Container container = (Container) servletContext.getAttribute("DwrContainer");
        if (container == null) {
            throw new JspException("Can't find 'DwrContainer' in servlet context");
        }
        ConverterManager converterManager = (ConverterManager) container.getBean(ConverterManager.class.getName());
        ScriptBuffer scriptBuffer = new ScriptBuffer();
        scriptBuffer.appendScript("return ");
        scriptBuffer.appendData(obj);

        WebContextFactory.WebContextBuilder webContextBuilder = (WebContextFactory.WebContextBuilder) servletContext.getAttribute(WebContextFactory.WebContextBuilder.class.getName());
        try {
            webContextBuilder.set((HttpServletRequest) pageContext.getRequest(), (HttpServletResponse) pageContext.getResponse(), null, servletContext, container);

            JspWriter out = pageContext.getOut();
            out.write("function() {");
            out.write(ScriptBufferUtil.createOutput(scriptBuffer, converterManager));
            out.write(";}()");
        } catch (IOException e) {
            throw new JspException("Error writing tag content", e);
        } catch (MarshallException e) {
            throw new JspException("Error marshalling object data", e);
        } finally {
            webContextBuilder.unset();
        }
        return EVAL_PAGE;
    }

    @Override
    public void release() {
        super.release();
        obj = null;
    }
}
