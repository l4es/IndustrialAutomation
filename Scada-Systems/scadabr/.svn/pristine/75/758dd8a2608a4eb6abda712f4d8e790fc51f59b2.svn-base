/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.content;

import br.org.scadabr.utils.ImplementMeException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Collection;
import java.util.Locale;
import javax.servlet.ServletOutputStream;
import javax.servlet.WriteListener;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author aploese
 */
public class MockServletResponse implements HttpServletResponse {

    @Override
    public int getStatus() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public String getHeader(String string) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public Collection<String> getHeaders(String string) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public Collection<String> getHeaderNames() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void setContentLengthLong(long l) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    public static class MockServletOutputStream extends ServletOutputStream {

        
        
        @Override
        public void print(boolean b) {
        }
        
        @Override
        public void println(boolean b) {
        }
        
        
        @Override
        public void print(char c) {
        }
        
        @Override
        public void println(char c) {
        }

        @Override
        public void print(double d) {
        }
        
        @Override
        public void println(double d) {
        }
        
        
        @Override
        public void print(float f) {
        }
        
        @Override
        public void println(float f) {
        }

        
        @Override
        public void print(int i) {
        }
        
        @Override
        public void println(int i) {
        }

        
        @Override
        public void print(long l) {
        }
        
        @Override
        public void println(long l) {
        }

        @Override
        public void print(String s) {
        }
        
        @Override
        public void println(String s) {
        }
        
        @Override
        public void println() {
        }
        
        @Override
        public void write(int b) {
            // We should never reach this ...
            throw new ImplementMeException();
        }

        @Override
        public boolean isReady() {
            throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
        }

        @Override
        public void setWriteListener(WriteListener wl) {
            throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
        }
        
    }
    
    private final StringWriter writer = new StringWriter();
    private final MockServletOutputStream outputStream = new MockServletOutputStream();

    @Override
    public PrintWriter getWriter() {
        return new PrintWriter(writer);
    }

    public String getContent() {
        return writer.toString();
    }

    @Override
    public void setContentType(String type) {
    }

    @Override
    public String getContentType() {
        return null;
    }

    @Override
    public String getCharacterEncoding() {
        return null;
    }

    @Override
    public void setCharacterEncoding(String charEncoding) {
    }

    @Override
    public int getBufferSize() {
        return -1;
    }

    @Override
    public void setBufferSize(int size) {
    }

    @Override
    public void setLocale(Locale locale) {
    }

    @Override
    public Locale getLocale() {
        return null;
    }

    @Override
    public ServletOutputStream getOutputStream() {
        return outputStream;
    }

    @Override
    public boolean isCommitted() {
        return false;
    }

    @Override
    public void resetBuffer() {
    }

    @Override
    public void reset() {
    }

    @Override
    public void setContentLength(int length) {
    }

    @Override
    public void flushBuffer() {
    }

    @Override
    public void sendError(int status) {
    }

    @Override
    public String encodeURL(String url) {
        return null;
    }

    @Override
    public String encodeRedirectURL(String url) {
        return null;
    }

    @Override
    public void addCookie(Cookie cookie) {
    }

    @Override
    public String encodeUrl(String url) {
        return null;
    }

    @Override
    public void sendRedirect(String location) {
    }

    @Override
    public void addDateHeader(String name, long value) {
    }

    @Override
    public void setIntHeader(String name, int value) {
    }

    @Override
    public String encodeRedirectUrl(String url) {
        return null;
    }

    @Override
    public void setStatus(int status) {
    }

    @Override
    public void addHeader(String name, String value) {
    }

    @Override
    public boolean containsHeader(String name) {
        return false;
    }

    @Override
    public void sendError(int status, String message) {
    }

    @Override
    public void addIntHeader(String name, int value) {
    }

    @Override
    public void setDateHeader(String name, long value) {
    }

    @Override
    public void setHeader(String name, String value) {
    }

    @Override
    public void setStatus(int status, String message) {
    }
}
