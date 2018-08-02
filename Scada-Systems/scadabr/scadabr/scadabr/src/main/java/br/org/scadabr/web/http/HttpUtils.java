/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.http;

import br.org.scadabr.utils.ImplementMeException;
import java.io.IOException;
import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpMethodBase;
import org.apache.commons.httpclient.methods.GetMethod;
import org.apache.commons.httpclient.methods.PostMethod;
import org.apache.commons.httpclient.methods.PutMethod;

/**
 *
 * @author aploese
 */
public class HttpUtils {

    public static String readResponseBody(HttpMethodBase method, int i) {
        throw new ImplementMeException();
    }

    public static String readResponseBody(PostMethod postMethod) {
        throw new ImplementMeException();
    }

    public static String readResponseBody(GetMethod postMethod) {
        throw new ImplementMeException();
    }

    public static void execute(HttpClient httpClient, PutMethod method) throws IOException {
        throw new ImplementMeException();
    }
}
