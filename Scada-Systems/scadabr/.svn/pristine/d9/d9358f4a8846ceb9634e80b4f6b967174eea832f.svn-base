package br.org.scadabr.web.dwr;

import java.lang.reflect.Method;
import java.util.Locale;
import java.util.ResourceBundle;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.jsp.jstl.core.Config;
import javax.servlet.jsp.jstl.fmt.LocalizationContext;
import org.directwebremoting.AjaxFilterChain;
import org.directwebremoting.WebContext;
import org.directwebremoting.WebContextFactory;
import org.springframework.web.context.WebApplicationContext;
import org.springframework.web.context.support.WebApplicationContextUtils;
import org.springframework.web.servlet.LocaleResolver;

public class LocalizationFilter implements org.directwebremoting.AjaxFilter {

    private String localeResolverName = "localeResolver";
    private String bundleBaseName = "messages";

    public String getLocaleResolverName() {
        return localeResolverName;
    }

    public void setLocaleResolverName(String localeResolverName) {
        this.localeResolverName = localeResolverName;
    }

    public String getBundleBaseName() {
        return bundleBaseName;
    }

    public void setBundleBaseName(String bundleBaseName) {
        this.bundleBaseName = bundleBaseName;
    }

    public static void prepareRequest(final HttpServletRequest request, final String localeResolverKey, final String bundleBaseName) {
        WebApplicationContext webApplicationContext = WebApplicationContextUtils.getRequiredWebApplicationContext(request.getSession().getServletContext());

        LocaleResolver localeResolver = (LocaleResolver) webApplicationContext.getBean(localeResolverKey);
        Locale locale = localeResolver.resolveLocale(request);
        ResourceBundle resourceBundle = ResourceBundle.getBundle(bundleBaseName, locale);
        LocalizationContext localizationContext = new LocalizationContext(resourceBundle, locale);
        Config.set(request, Config.FMT_LOCALIZATION_CONTEXT, localizationContext);
    }

    @Override
    public Object doFilter(Object obj, Method method, Object[] params, AjaxFilterChain chain) throws Exception {
        WebContext webContext = WebContextFactory.get();
        prepareRequest(webContext.getHttpServletRequest(), localeResolverName, bundleBaseName);
        return chain.doFilter(obj, method, params);
    }

}
