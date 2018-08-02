/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.mvc.controller;

import br.org.scadabr.logger.LogUtils;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

/**
 * Handle misc stuff from page.tag
 * @author aploese
 */
@Controller
@Scope("request")
public class MainPageController {
    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_WEB);
        
    @RequestMapping(value = "/javaScriptError", method = RequestMethod.GET)
    public void jsError(String desc, String page, String line, String browserName, String browserVersion,
            String osName, String location) {
        LOG.log(Level.WARNING,"Javascript error\n\tDescription: {0}\n\tPage: {1}\n\tLine: {2}\n\tBrowser name: {3}\n\tBrowser version: {4}\n\tosName: {5}\n\tlocation: {6}", new Object[]{desc, page, line, browserName, browserVersion, osName, location});
    }

}
