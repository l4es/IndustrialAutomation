/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.io;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.Reader;
import java.io.Writer;

/**
 *
 * @author aploese
 */
public class StreamUtils {

    public static void transfer(InputStream in, OutputStream out) throws IOException {
        final byte[] buf = new byte[2048];
        int bytesReaded;

        while ((bytesReaded = in.read(buf)) > 0) {
            out.write(buf, 0, bytesReaded);
        }

        out.flush();
    }

    public static void transfer(InputStream in, OutputStream out, long limit) throws IOException {
        final byte[] buf = new byte[2048];
        long bytesWritten = 0;
        int bytesReaded;

        while ((bytesReaded = in.read(buf)) > 0 && bytesWritten < limit) {
            out.write(buf, 0, bytesReaded);
            bytesWritten += bytesReaded;
        }

        out.flush();
    }

    public static void transfer(Reader r, Writer w) throws IOException {
        final char[] buf = new char[2048];
        int bytesReaded;

        while ((bytesReaded = r.read(buf)) > 0) {
            w.write(buf, 0, bytesReaded);
        }

        w.flush();
    }

    public static void transfer(InputStream is, Writer w) throws IOException {
        try (InputStreamReader r = new InputStreamReader(is)) {
            transfer(r, w);
        }
    }

    public static int read4ByteSigned(InputStream in) throws IOException {
        byte[] buf = new byte[4];
        if (in.read(buf) < 4) {
            throw new IOException("Stream closed");
        }
        return buf[0] | buf[1] << 8 | buf[2] << 16 | buf[3] << 24;
    }

    public static byte readByte(InputStream in) throws IOException {
        final int result = in.read();
        if (result < 0) {
            throw new IOException("Stream closed");
        }
        return (byte) result;
    }

    public static void write4ByteSigned(OutputStream out, int value) throws IOException {
        final byte[] buf = new byte[4];
        buf[0] = (byte) (value & 0x000000FF);
        buf[1] = (byte) ((value & 0x0000FF00) >> 8);
        buf[1] = (byte) ((value & 0x00FF0000) >> 16);
        buf[1] = (byte) ((value & 0xFF000000) >> 24);
        out.write(buf);
    }

    public static void writeByte(OutputStream out, byte b) throws IOException {
        out.write(b);
    }

}
