/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.content;

import br.org.scadabr.logger.LogUtils;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.MissingResourceException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;

/**
 *
 * @author aploese
 */
public class ContentGenerator {

    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_WEB);
    private final HttpServletRequest request;
    private final String contentJsp;
    private final Map<String, Object> model;
    

    public ContentGenerator(HttpServletRequest request, String contentJsp, Map<String, Object> model) {
        this.request = request;
        this.contentJsp = contentJsp;
        this.model = model;
    }

    public String generateContent() throws ServletException, IOException {
        return generateContent(request, contentJsp, model);
    }

    public static String generateContent(HttpServletRequest request, String contentJsp, Map<String, Object> model) throws ServletException, IOException {
        MockServletResponse response = new MockServletResponse();
        Map<String, Object> oldValues = new HashMap<>();
        if (model != null) {
            for (String key : model.keySet()) {
                Object oldValue = request.getAttribute(key);
                if (oldValue != null) {
                    oldValues.put(key, oldValue);
                }
                request.setAttribute(key, model.get(key));
            }
        }
        try {
            request.getRequestDispatcher(contentJsp).forward(request, response);
        } catch (MissingResourceException e) {
            LOG.log(Level.SEVERE, "Missing resource {0}", contentJsp);
            return "Resource " + contentJsp + " not found";
        } catch (NullPointerException npe) {
            LOG.log(Level.SEVERE, "NullPointerException {0}", contentJsp);
            return "Got NullPointerException: " + contentJsp;
        } catch (Throwable t) {
            LOG.log(Level.SEVERE, "Throwable {0} {1}", new Object[] {t.getClass(), contentJsp});
            return "Got Throwable: " + contentJsp;
        } finally {
            if (model != null) {
                for (String key : model.keySet()) {
                    request.removeAttribute(key);
                }
                for (String key : oldValues.keySet()) {
                    request.setAttribute(key, oldValues.get(key));
                }
            }
        }
        return response.getContent();
    }
}
