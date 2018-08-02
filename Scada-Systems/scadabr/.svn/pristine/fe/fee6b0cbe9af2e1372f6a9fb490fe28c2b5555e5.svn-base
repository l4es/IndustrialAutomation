package com.serotonin.mango.web.email;

import br.org.scadabr.dao.SystemSettingsDao;
import br.org.scadabr.web.email.EmailContent;
import java.io.IOException;
import java.util.Map;
import java.util.ResourceBundle;

import com.serotonin.mango.Common;

import freemarker.template.Template;
import freemarker.template.TemplateException;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

@Configurable
public class MangoEmailContent extends EmailContent {

    @Autowired
    private SystemSettingsDao systemSettingsDao;

    public enum ContentType {

        BOTH(0, true, true),
        HTML(1, true, false),
        TEXT(2, false, true);
        private final int id;
        private boolean renderHtml;
        private boolean renderText;

        private ContentType(int id, boolean renderHtml, boolean renderText) {
            this.id = id;
            this.renderHtml = renderHtml;
            this.renderText = renderText;
        }

        public boolean isRenderHtml() {
            return renderHtml;
        }

        public boolean isRenderText() {
            return renderText;
        }
    }

    private final String defaultSubject;
    private final SubjectDirective subjectDirective;

    public MangoEmailContent(String templateName, Map<String, Object> model, ResourceBundle bundle,
            String defaultSubject) throws TemplateException, IOException {
        super(null, null);

        // This will raise a NÜE but compiles :)
        ContentType type = systemSettingsDao.getEmail().getContentType();

        this.defaultSubject = defaultSubject;
        this.subjectDirective = new SubjectDirective(bundle);

        model.put("fmt", new MessageFormatDirective(bundle));
        model.put("subject", subjectDirective);

        if (type.isRenderHtml()) {
            htmlContent = processHtmlTemplate(getHTMLTemplate(templateName), model);
        }

        if (type.isRenderText()) {
            plainContent = processPlainTemplate(getPlainTemplate(templateName), model);
        }
    }

    public String getSubject() {
        String subject = subjectDirective.getSubject();
        if (subject == null) {
            return defaultSubject;
        }
        return subject;
    }

    private Template getPlainTemplate(String name) throws IOException {
        return Common.ctx.getFreemarkerConfig().getTemplate(String.format("text/%s.ftl", name));
    }

    private Template getHTMLTemplate(String name) throws IOException {
        return Common.ctx.getFreemarkerConfig().getTemplate(String.format("html/%s.ftl", name));
    }
}
