/*
 Mango - Open Source M2M - http://mango.serotoninsoftware.com
 Copyright (C) 2006-2011 Serotonin Software Technologies Inc.
 @author Matthew Lohbihler
    
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package br.org.scadabr.web.mvc.controller;

import br.org.scadabr.rt.UserRT;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.web.i18n.LocaleResolver;
import br.org.scadabr.web.mvc.form.LoginForm;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.springframework.validation.BindException;

import com.serotonin.mango.Common;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.web.UserSessionContextBean;
import com.serotonin.mango.web.integration.CrowdUtils;
import java.util.Calendar;
import java.util.Locale;
import java.util.TimeZone;
import javax.inject.Inject;
import javax.validation.Valid;
import org.springframework.context.annotation.Scope;
import org.springframework.context.i18n.SimpleTimeZoneAwareLocaleContext;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.validation.BindingResult;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

@Controller
@RequestMapping("/login")
@Scope("request")
class LoginController {

    private static final Log logger = LogFactory.getLog(LoginController.class);

    private String successUrl = "redirect:watchList";
    private String newUserUrl = "redirect:help";
    private final static String LOGIN_VIEW = "login";
    @Inject
    private RuntimeManager runtimeManager;

    @Inject
    transient private LocaleResolver localeResolver;

    @Inject
    private UserSessionContextBean userSessionContextBean;

    public LoginController() {
        super();
    }

    public void setSuccessUrl(String url) {
        successUrl = url;
    }

    public void setNewUserUrl(String newUserUrl) {
        this.newUserUrl = newUserUrl;
    }

    @RequestMapping(method = RequestMethod.GET)
    protected String showForm(Model model, HttpServletRequest request, HttpServletResponse response) throws Exception {
// Check if Crowd is enabled
        if (CrowdUtils.isCrowdEnabled()) {
            String username = CrowdUtils.getCrowdUsername(request);

            if (username != null) {
                model.addAttribute(LOGIN_VIEW, new LoginForm(username));

                // The user is logged into Crowd. Make sure the username is valid in this instance.
                UserRT user = runtimeManager.getUser(username);
                if (user == null) {
//                    ValidationUtils.rejectValue(errors, "username", "login.validation.noSuchUser");
                } else {
                    // Validate some stuff about the user.
                    if (user.isDisabled()) {
//                        ValidationUtils.reject(errors, "login.validation.accountDisabled");
                    } else {
                        if (CrowdUtils.isAuthenticated(request, response)) {
                            throw new ImplementMeException();
                            /*
                            String result = performLogin(username);
                            CrowdUtils.setCrowdAuthenticated(Common.getUser(request));
                            return result;
                            */
                        }
                    }
                }
            }
        } else {
            model.addAttribute(LOGIN_VIEW, new LoginForm());
        }
        return LOGIN_VIEW;
    }

    /*
     protected void onBindAndValidate(HttpServletRequest request, Object command, BindException errors) {
     LoginForm login = (LoginForm) command;

     // Make sure there is a username
     if (login.getUsername().isEmpty()) {
     ValidationUtils.rejectValue(errors, "username", "login.validation.noUsername");
     }

     // Make sure there is a password
     if (login.getPassword().isEmpty()) {
     ValidationUtils.rejectValue(errors, "password", "login.validation.noPassword");
     }
     }
     */
    //TODO @Valid does not work with <spring:bind ????
    @RequestMapping(method = RequestMethod.POST)
    public String onSubmit(@ModelAttribute("login") @Valid LoginForm loginForm, BindingResult bindingResult, HttpServletRequest request, HttpServletResponse response) throws BindException {
//NPE??? AspectJ konfigurieren       
       
       if (bindingResult.hasErrors()) {
            return "login";
        }

        boolean crowdAuthenticated = false;

        // Check if the user exists
        UserRT user = runtimeManager.getUser(loginForm.getUsername());
        if (user == null) {
//            ValidationUtils.rejectValue(errors, "username", "login.validation.noSuchUser");
        } else if (user.isDisabled()) {
//            ValidationUtils.reject(errors, "login.validation.accountDisabled");
        } else {
            if (CrowdUtils.isCrowdEnabled()) {
                // First attempt authentication with Crowd.
                crowdAuthenticated = CrowdUtils.authenticate(request, response, loginForm.getUsername(),
                        loginForm.getPassword());
            }

            if (!crowdAuthenticated) {
                //TODOuser.canLogin()

                // Validating the password against the database.
                if (user.comparePaswordWithHash(loginForm.getPassword())) {
//TODO                    ValidationUtils.reject(errors, "login.validation.invalidLogin");
                }
            }
        }
        /*
         if (errors.hasErrors()) {
         return "login";
         }
         */
        String result = performLogin(loginForm.getUsername());
        if (crowdAuthenticated) {
            CrowdUtils.setCrowdAuthenticated(userSessionContextBean.getUser());
        }
        fixTimeZone(request, response);
        return result;
    }

    public void fixTimeZone(HttpServletRequest request, HttpServletResponse response) {
        //TODO set client to different timezone of locale I.e. (bash: export TZ=Asia/Calcutta && firefox) ->> how to get this timezone to the server (us)? 
        final Locale locale = Locale.ENGLISH; //localeResolver.resolveLocale(request);
        TimeZone timeZone = localeResolver.resolveTimeZone(request);
        if (timeZone == null) {
            timeZone = Calendar.getInstance(locale).getTimeZone();
            localeResolver.setLocaleContext(request, response, new SimpleTimeZoneAwareLocaleContext(locale, timeZone));
        }
        userSessionContextBean.setLocale(locale);
        userSessionContextBean.setTimeZone(timeZone);
    }

    private String performLogin(String username) {
        // Check if the user is already logged in.
        UserRT user = userSessionContextBean.getUser();
        if (user != null && user.getName().equals(username)) {
            // The user is already logged in. Nothing to do.
            if (logger.isDebugEnabled()) {
                logger.debug("User is already logged in, not relogging in");
            }
        } else {
            // Get the user data from the app server.
            user = runtimeManager.getUser(username);
            if (user == null) {
                return "redirect:login";
            }
            // Update the last login time.
            runtimeManager.recordLogin(user);

            // Add the user object to the session. This indicates to the rest
            // of the application whether the user is logged in or not.
            userSessionContextBean.loginUser(user);
            if (logger.isDebugEnabled()) {
                logger.debug("User object added to session");
            }
        }

        if (user.isFirstLogin()) {
            return newUserUrl;
        }
        if (!user.getHomeUrl().isEmpty()) {
            return "redirect:" + user.getHomeUrl();
        }

        return successUrl;
    }

}
