package br.org.scadabr.dao.jdbc;

import br.org.scadabr.jdbc.DatabaseAccessFactory;
import br.org.scadabr.jdbc.DatabaseType;
import br.org.scadabr.logger.LogUtils;
import java.util.Collection;
import java.util.List;
import java.util.logging.Logger;
import javax.annotation.PostConstruct;
import javax.inject.Inject;
import javax.sql.DataSource;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.jdbc.core.PreparedStatementCreator;
import org.springframework.jdbc.datasource.DataSourceTransactionManager;
import org.springframework.jdbc.support.GeneratedKeyHolder;
import org.springframework.transaction.support.TransactionTemplate;

public class DaoUtils {

    protected final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_DAO);
    @Inject
    protected DatabaseAccessFactory daf;
    protected DataSource dataSource;
    protected JdbcTemplate ejt;
    protected DataSourceTransactionManager tm;

    public DaoUtils() {
        
    }
    
    @Deprecated
    protected DaoUtils(DataSource dataSource) {
        this.dataSource = dataSource;
        init();
    }

    @PostConstruct
    public void init() {
        if (dataSource == null) {
            dataSource = daf.getDataSource();
        }
        ejt = new JdbcTemplate(dataSource);
    }
    
    protected String createDelimitedList(Collection<?> values, String delimeter) {
        final StringBuilder sb = new StringBuilder();
        boolean isFirst = true;
        for (Object o : values) {
            if (isFirst) {
                isFirst = false;
            } else {
                sb.append(delimeter);
            }
            sb.append(o);
        }
        return sb.toString();
    }

    protected String createDelimitedList(List<?> values, int from, int to, String delimeter, String quote) {
        final StringBuilder sb = new StringBuilder();
        sb.append(quote).append(values.get(from)).append(quote);
        for (int i = from + 1; i < to; i++) {
            sb.append(delimeter).append(quote).append(values.get(i)).append(quote);
        }
        return sb.toString();
    }

    protected int doInsert(PreparedStatementCreator psc) {
        final GeneratedKeyHolder gkh = new GeneratedKeyHolder();
        ejt.update(psc, gkh);
        return gkh.getKey().intValue();
    }

    protected long doInsertLong(PreparedStatementCreator psc) {
        final GeneratedKeyHolder gkh = new GeneratedKeyHolder();
        ejt.update(psc, gkh);
        return gkh.getKey().longValue();
    }

    protected DataSourceTransactionManager getTransactionManager() {
        if (tm == null) {
            tm = new DataSourceTransactionManager(dataSource);
        }
        return tm;
    }

    protected TransactionTemplate getTransactionTemplate() {
        return new TransactionTemplate(getTransactionManager());
    }
    
    public void compressTables() {
        daf.executeCompress(ejt);
    }
    
    public DatabaseType getDataBaseType() {
        return daf.getDatabaseType();
    }


}
