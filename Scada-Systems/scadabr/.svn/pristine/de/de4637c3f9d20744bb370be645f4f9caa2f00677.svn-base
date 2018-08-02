/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.email;

import br.org.scadabr.utils.ImplementMeException;
import java.util.Properties;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeMessage;
import org.springframework.mail.MailException;
import org.springframework.mail.javamail.JavaMailSenderImpl;
import org.springframework.mail.javamail.MimeMessageHelper;
import org.springframework.mail.javamail.MimeMessagePreparator;

/**
 *
 * @author aploese
 */
public class EmailSender {

    private final JavaMailSenderImpl senderImpl;

    public EmailSender(String host, int port, boolean useAuth, String userName, String password, boolean tls) {
        senderImpl = new JavaMailSenderImpl();
        Properties props = new Properties();
        if (useAuth) {
            props.put("mail.smtp.auth", "true");
            senderImpl.setUsername(userName);
            senderImpl.setPassword(password);
        }
        if (tls) {
            props.put("mail.smtp.starttls.enable", "true");
        }
        senderImpl.setJavaMailProperties(props);
        senderImpl.setHost(host);
        if (port != -1) {
            senderImpl.setPort(port);
        }
    }

    public void send(final InternetAddress fromAddress, final InternetAddress[] toAddresses, final String subject, final EmailContent content) throws MailException {
        
        senderImpl.send( new MimeMessagePreparator() {
        
            @Override
            public void prepare(MimeMessage mimeMessage) throws Exception {
                MimeMessageHelper helper = new MimeMessageHelper(mimeMessage, content.isMultipart(), content.getEncoding());

                helper.setFrom(fromAddress);
                helper.setTo(toAddresses);

                String sub = subject.replaceAll("\\r", "");
                sub = sub.replaceAll("\\n", "");

                helper.setSubject(sub);
                if (content.getHtmlContent() == null) {
                    helper.setText(content.getPlainContent(), false);
                } else if (content.getPlainContent() == null) {
                    helper.setText(content.getHtmlContent(), true);
                } else {
                    helper.setText(content.getPlainContent(), content.getHtmlContent());
                }
                for (EmailAttachment att : content.getAttachments()) {
                    att.attach(helper);
                }
                for (EmailInline inline : content.getInlines()) {
                    inline.attach(helper);
                }
            }
        });
    }


}
