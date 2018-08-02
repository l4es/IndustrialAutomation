/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.util;

import br.org.scadabr.utils.ImplementMeException;
import java.util.Properties;
import java.util.Random;
import java.util.regex.Pattern;

/**
 *
 * @author aploese
 */
public class StringUtils {

    @Deprecated //Use Arrays.???
    public static boolean isEmpty(int[] values) {
        throw new ImplementMeException();
    }

    public static String pad(String s, char c, int length) {
        if (s.length() >= length) {
            return s;
        }
        StringBuilder sb = new StringBuilder(length);
        final int amount = length - s.length();
        for (int i = 0; i < amount; i++) {
            sb.append(c);
        }
        sb.append(s);
        return sb.toString();
    }

    public static boolean globWhiteListMatchIgnoreCase(String[] deviceIdWhiteList, String deviceId) {
        throw new ImplementMeException();
    }

    public static boolean isBetweenInc(int slaveId, int i, int i0) {
        throw new ImplementMeException();
    }

    public static int parseInt(String substring, int width) {
        throw new ImplementMeException();
    }

    public static String truncate(final String string, final int length) {
        return string.length() > length ? string.substring(0, length) : string;
    }

    public static String replaceMacros(String dir, Properties properties) {
        throw new ImplementMeException();
    }

  public static String replaceMacro(final String s, final String name, final String replacement) {
    return s.replaceAll(Pattern.quote("${" + name + "}"), replacement);
  }

    public static Object[] truncate(String url, int i, String s) {
        throw new ImplementMeException();
    }

    public static String toHex(short s) {
        throw new ImplementMeException();
    }

    //TODO escape all which may break outer  html to unicode??? ??
    public static String escapeLT_GT(String value) {
        return value.replaceAll("<", "&lt;").replaceAll(">", "&gt;");
    }

}
