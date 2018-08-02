/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.email;

import br.org.scadabr.utils.ImplementMeException;
import java.io.File;
import javax.mail.MessagingException;
import org.springframework.mail.javamail.MimeMessageHelper;

/**
 *
 * @author aploese
 */
public abstract class EmailAttachment {

    protected final String filename;

    public EmailAttachment(String filename) {
        this.filename = filename;
    }

    public abstract void attach(MimeMessageHelper helper) throws MessagingException;

    public static class FileAttachment extends EmailAttachment {

        private final File file;

        public FileAttachment(String filename, File file) {
            super(filename);
            this.file = file;
        }

        @Override
        public void attach(MimeMessageHelper helper) throws MessagingException {
            helper.addAttachment(filename, file);
        }
    }

}
