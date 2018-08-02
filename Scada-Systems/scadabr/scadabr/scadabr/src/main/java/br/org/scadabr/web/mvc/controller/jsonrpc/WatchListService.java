package br.org.scadabr.web.mvc.controller.jsonrpc;
import br.org.scadabr.dao.DataPointDao;
import br.org.scadabr.dao.PointValueDao;
import br.org.scadabr.dao.WatchListDao;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.rt.UserRT;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.view.SharedUserAcess;
import br.org.scadabr.web.l10n.RequestContextAwareLocalizer;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.WatchList;
import com.serotonin.mango.vo.permission.Permissions;
import com.serotonin.mango.web.UserSessionContextBean;
import java.io.Serializable;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.inject.Inject;
import javax.inject.Named;
import javax.servlet.http.HttpServletRequest;
import org.springframework.context.annotation.Scope;
import org.springframework.context.annotation.ScopedProxyMode;

//TODO custom Watchlist scope???
@Named
//@JsonRpcService("/rpc/watchlists.json")
@Scope(value = "session", proxyMode = ScopedProxyMode.TARGET_CLASS)
public class WatchListService implements Serializable {

    private static final Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_WEB);

    @Inject
    private transient PointValueDao pointValueDao;
    @Inject
    private transient DataPointDao dataPointDao;
    @Inject
    private transient WatchListDao watchListDao;
    @Inject
    private transient RuntimeManager runtimeManager;
    @Inject
    private transient UserSessionContextBean userSessionContextBean;
    @Inject
    private transient RequestContextAwareLocalizer localizer;

    public JsonWatchList getWatchList(int watchlistId, HttpServletRequest request) {
        return new JsonWatchList(watchListDao.getWatchList(watchlistId), dataPointDao, runtimeManager, localizer);
    }

    public JsonWatchList getSelectedWatchlist(HttpServletRequest request) {
        throw new ImplementMeException();
//        return new JsonWatchList(watchListDao.getWatchList(userSessionContextBean.getUser().getSelectedWatchList()), dataPointDao, runtimeManager, localizer);
    }

    public JsonWatchList addPointToWatchlist(int watchlistId, int index, int dataPointId, HttpServletRequest request) {
        LOG.warning("ENTER addPointToWatchlist");
        final UserRT user = userSessionContextBean.getUser();
        DataPointVO point = dataPointDao.getDataPoint(dataPointId);
        if (point == null) {
            return null;
        }
        WatchList watchList = watchListDao.getWatchList(watchlistId);

        // Check permissions.
        Permissions.ensureDataPointReadPermission(user, point);
   //TODO     Permissions.ensureWatchListEditPermission(user, watchList);

        // Add it to the watch list.
        watchList.getPointList().add(index, point);
        watchListDao.saveWatchList(watchList);
   //TODO     updateSetPermission(point, watchList.getUserAccess(user), userDao.getUser(watchList.getUserId()));
        LOG.log(Level.WARNING, "ENTER addPointToWatchlist {0}", watchListDao.getWatchList(watchlistId).getName());
        return new JsonWatchList(watchListDao.getWatchList(watchlistId), dataPointDao, runtimeManager, localizer);
    }

    private void updateSetPermission(DataPointVO point, SharedUserAcess access, UserRT owner) {
        // Point isn't settable
        if (!point.isSettable()) {
            return;
        }

        // Read-only access
        if (access != SharedUserAcess.OWNER && access != SharedUserAcess.SET) {
            return;
        }

        // Watch list owner doesn't have set permission
        if (!Permissions.hasDataPointSetPermission(owner, point)) {
            return;
        }

        // All good.
        point.setSettable(true);
    }

    public JsonWatchList deletePointFromWatchlist(int watchlistId, int dataPointId, HttpServletRequest request) {
        LOG.warning("ENTER deletePointFromWatchlist");
        final UserRT user = userSessionContextBean.getUser();
        DataPointVO point = dataPointDao.getDataPoint(dataPointId);
        if (point == null) {
            return null;
        }
        WatchList watchList = watchListDao.getWatchList(watchlistId);

        // Check permissions.
        Permissions.ensureDataPointReadPermission(user, point);
//TODO        Permissions.ensureWatchListEditPermission(user, watchList);

        //remove
        for (DataPointVO dp : watchList) {
            if (dp.getId() == dataPointId) {
                watchList.getPointList().remove(dp);
                break;
            }
        }
        watchListDao.saveWatchList(watchList);
   //TODO     updateSetPermission(point, watchList.getUserAccess(user), userDao.getUser(watchList.getUserId()));
        LOG.log(Level.WARNING, "Exit deletePointFromWatchlist {0}", watchListDao.getWatchList(watchlistId).getName());
        return new JsonWatchList(watchListDao.getWatchList(watchlistId), dataPointDao, runtimeManager, localizer);
    }

    public JsonChartDataSet getChartDataSet(int dataPointId) {
        throw new ImplementMeException();
        /* TODO half localized stuff
        DataPointVO dp = dataPointDao.getDataPoint(dataPointId);
        if (dp.getChartRenderer() instanceof TimePeriodChartRenderer) {
            final TimePeriodChartRenderer tpcr = (TimePeriodChartRenderer)dp.getChartRenderer();
            final long timeStamp = System.currentTimeMillis();
            final long from = tpcr.getStartTime(timeStamp);
            final long to = tpcr.getEndTime(timeStamp);
            Iterable<PointValueTime> pvt = pointValueDao.getPointValuesBetween(dp, from, to);
            return new JsonChartDataSet(from, to, dp, pvt);
        }
        return new JsonChartDataSet();
*/
    }

    
}