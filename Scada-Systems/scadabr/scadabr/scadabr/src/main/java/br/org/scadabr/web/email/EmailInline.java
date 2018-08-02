/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.email;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.InputStream;
import javax.mail.MessagingException;
import org.springframework.core.io.InputStreamSource;
import org.springframework.mail.javamail.MimeMessageHelper;

/**
 *
 * @author aploese
 */
public abstract class EmailInline {

    protected final String contentId;

    protected EmailInline(String contentId) {
        this.contentId = contentId;
    }

    public abstract void attach(MimeMessageHelper helper) throws MessagingException;

    public static class ByteArrayInline extends EmailInline implements InputStreamSource {

        private final byte[] content;
        private final String contentType;

        public ByteArrayInline(String contentId, byte[] content, String contentType) {
            super(contentId);
            this.content = content;
            this.contentType = contentType;
        }

        @Override
        public InputStream getInputStream() {
            return new ByteArrayInputStream(content);
        }

        @Override
        public void attach(MimeMessageHelper mimeMessageHelper) throws MessagingException {
            mimeMessageHelper.addInline(contentId, this, contentType);
        }
    }

    public static class FileInline extends EmailInline {

        private final File file;

        public FileInline(String contentId, File file) {
            super(contentId);
            this.file = file;
        }

        @Override
        public void attach(MimeMessageHelper mimeMessageHelper) throws MessagingException {
            mimeMessageHelper.addInline(contentId, file);
        }
    }

}
