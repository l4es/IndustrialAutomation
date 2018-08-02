package br.org.scadabr.web.mvc.controller.jsonrpc;

import br.org.scadabr.dao.UserDao;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.rt.UserRT;
import br.org.scadabr.utils.ImplementMeException;
import com.serotonin.mango.web.UserSessionContextBean;
import java.util.logging.Logger;
import javax.inject.Inject;
import javax.inject.Named;
import javax.servlet.http.HttpServletRequest;
import org.springframework.context.annotation.Scope;
import org.springframework.context.annotation.ScopedProxyMode;

@Deprecated //TODO use REST
@Named
//@JsonRpcService("/rpc/users")
@Scope(value = "request", proxyMode = ScopedProxyMode.TARGET_CLASS)
public class UsersService {

    private static final Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_WEB);

    @Inject
    private UserSessionContextBean userSessionContextBean;
    @Inject
    private UserDao userDao;
    @Inject
    HttpServletRequest request;

    public String setHomeUrl(String homeUrl) {
        final UserRT user = userSessionContextBean.getUser();
                // Remove the scheme.
        String url = homeUrl.substring(request.getScheme().length() + 3);

        // Remove the domain.
        url = url.substring(request.getServerName().length());

        // Remove the port
        if (url.charAt(0) == ':') {
            url = url.substring(Integer.toString(request.getServerPort()).length() + 1);
        }

        // Remove the context
        url = url.substring(request.getContextPath().length());

        // Remove any leading /
        if (url.charAt(0) == '/') {
            url = url.substring(1);
        }
        throw new ImplementMeException();
/*TODO        user.setHomeUrl(url);
        userDao.saveHomeUrl(user.getId(), url);
        return userSessionContextBean.getUserHomeUrl();
*/
    }
}