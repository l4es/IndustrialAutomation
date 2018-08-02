package br.org.scadabr.web.mvc.controller.jsonrpc;

import br.org.scadabr.dao.EventDao;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.l10n.Localizer;
import br.org.scadabr.rt.UserRT;
//import com.googlecode.jsonrpc4j.JsonRpcService;
import com.serotonin.mango.rt.event.EventInstance;
import com.serotonin.mango.web.UserSessionContextBean;
import java.util.Collection;
import java.util.logging.Logger;
import javax.inject.Inject;
import javax.inject.Named;
import org.springframework.context.annotation.Scope;
import org.springframework.context.annotation.ScopedProxyMode;

@Named
//@JsonRpcService("/rpc/events/")
@Scope(value = "request", proxyMode = ScopedProxyMode.TARGET_CLASS)
public class EventsService {

    private static final Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_WEB);

    @Inject
    private Localizer localizer;
    @Inject
    private UserSessionContextBean userSessionContextBean;
    @Inject
    private EventDao eventDao;

    public Collection<JsonEventInstance> acknowledgeAllPendingEvents() {
        final UserRT user = userSessionContextBean.getUser();
        if (user != null) {
            long now = System.currentTimeMillis();
            for (EventInstance evt : eventDao.getPendingEvents(user)) {
                eventDao.ackEvent(evt.getId(), now, user, null);
            }
        }
        return JsonEventInstance.wrap(eventDao.getPendingEvents(user), localizer);

    }

    public Collection<JsonEventInstance> acknowledgePendingEvent(int eventId) {
        final UserRT user = userSessionContextBean.getUser();
        if (user != null) {
            long now = System.currentTimeMillis();
            eventDao.ackEvent(eventId, now, user, null);
//TODO impl            MiscDWR.resetLastAlarmLevelChange();
        }
        return JsonEventInstance.wrap(eventDao.getPendingEvents(user), localizer);

    }
}