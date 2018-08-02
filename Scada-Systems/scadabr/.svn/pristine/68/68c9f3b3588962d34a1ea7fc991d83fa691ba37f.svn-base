/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.email;

import freemarker.template.Template;
import freemarker.template.TemplateException;
import java.io.IOException;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Pattern;

/**
 *
 * @author aploese
 */
public class EmailContent {
    
    private final static Pattern PATTERN_TAG_BR = Pattern.compile("<br\\s*/>");
    private final static String TAG_BR_SUBSTITUTE ="\r\n";
    
    private final static Pattern PATTERN_NON_BREAKING_SPACE = Pattern.compile("&nbsp;");
    private final static String NON_BREAKING_SPACE_SUBSTITUTE = " ";

    protected final String processPlainTemplate(Template plainTpl, Object model) throws TemplateException, IOException {
        if (plainTpl != null) {
            StringWriter plain = new StringWriter();
            plainTpl.process(model, plain);
            String result = PATTERN_TAG_BR.matcher(plain.getBuffer()).replaceAll(TAG_BR_SUBSTITUTE);
            return PATTERN_NON_BREAKING_SPACE.matcher(result).replaceAll(NON_BREAKING_SPACE_SUBSTITUTE);
        }
        return null;
    }

    protected final String processHtmlTemplate(Template htmlTpl, Object model) throws TemplateException, IOException {
        if (htmlTpl != null) {
            StringWriter html = new StringWriter();
            htmlTpl.process(model, html);
            return html.toString();
        }
        return null;
    }


    private final String encoding;
    protected String plainContent;
    protected String htmlContent;
    private final List<EmailAttachment> attachments = new ArrayList<>();
    private final List<EmailInline> inlineParts = new ArrayList();

    /**
     * 
     * Set default encoding to UTF-8
     * 
     * @param plainContent
     * @param htmlContent 
     */
    public EmailContent(String plainContent, String htmlContent) {
        this.plainContent = plainContent;
        this.htmlContent = htmlContent;
        this.encoding = "UTF-8";
    }

    public void addAttachment(EmailAttachment attachment) {
        attachments.add(attachment);
    }

    public void addInline(EmailInline inline) {
        inlineParts.add(inline);
    }
    
    public String getEncoding() {
        return encoding;
    }
    
    public boolean isMultipart()  {
    return ((plainContent != null) && (htmlContent != null)) || (!attachments.isEmpty()) || (!inlineParts.isEmpty());
  }

    public String getHtmlContent() {
        return htmlContent;
    }

    public String getPlainContent() {
        return plainContent;
    }

    public Iterable<EmailAttachment> getAttachments() {
        return attachments;
    }

    public Iterable<EmailInline> getInlines() {
        return inlineParts;
    }
  
}
