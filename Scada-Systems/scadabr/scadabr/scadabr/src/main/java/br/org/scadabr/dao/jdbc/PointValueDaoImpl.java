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
package br.org.scadabr.dao.jdbc;

import br.org.scadabr.DataType;
import br.org.scadabr.dao.PointValueDao;
import br.org.scadabr.db.IntValuePair;
import br.org.scadabr.db.spring.IntValuePairRowMapper;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.vo.NodeType;
import br.org.scadabr.vo.VO;
import com.serotonin.mango.rt.dataImage.DoubleValueTime;
import com.serotonin.mango.rt.dataImage.PointValueAnnotation;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.bean.LongPair;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.Types;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.logging.Level;
import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import javax.inject.Inject;
import javax.inject.Named;
import org.springframework.dao.ConcurrencyFailureException;
import org.springframework.dao.DataAccessException;
import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.jdbc.core.PreparedStatementCreator;
import org.springframework.jdbc.core.RowMapper;

@Named
public class PointValueDaoImpl extends BaseDao implements PointValueDao {

    @Inject
    private BatchWriteBehind batchWriteBehind;

    private final static List<UnsavedPointValue> UNSAVED_POINT_VALUES = new ArrayList<>();

    static final String POINT_VALUE_INSERT_START = "insert into doublePointValues (dataPointId, pointValue, ts) values ";
    static final String POINT_VALUE_INSERT_VALUES = "(?,?,?)";
    static final String POINT_VALUE_INSERT = POINT_VALUE_INSERT_START
            + POINT_VALUE_INSERT_VALUES;
    static final String POINT_VALUE_ANNOTATION_INSERT = "insert into pointValueAnnotations "
            + "(pointValueId, textPointValueShort, textPointValueLong, sourceType, sourceId) values (?,?,?,?,?)";

    public PointValueDaoImpl() {
        super();
    }

    private void flushWriteBehind() {
        try {
            final Future<Integer> f = batchWriteBehind.flush(ejt);
            if (f == null) {
                return;
            }
            f.get(); // Just wait to finish
        } catch (InterruptedException | ExecutionException ex) {
            LOG.log(Level.SEVERE, null, ex);
        }
    }

    @PostConstruct
    @Override
    public void init() {
        super.init();
        // Use our ejt, so that there is no tranaction boundary... // otherwise we wont see the values written there here
        batchWriteBehind.init(daf);
    }

    /**
     * Flush anything
     */
    @PreDestroy
    public void shutdown() {
        flushWriteBehind();
    }

    /**
     * Only the PointValueCache should call this method during runtime. Do not
     * use.
     *
     * @param pointId
     */
/*    @Override
    public PointValueTime savePointValueSync(PointValueTime pointValue, VO<?> source) {
        long id = savePointValueImpl(pointValue, source, false);

        PointValueTime savedPointValue;
        int retries = 5;
        while (true) {
            try {
                savedPointValue = getPointValue(id, pointValue.getDataPointId(), pointValue.getDataType());
                break;
            } catch (ConcurrencyFailureException e) {
                if (retries <= 0) {
                    throw e;
                }
                retries--;
            }
        }

        return savedPointValue;
    }
*/
    /**
     * @inheritDoc
     */
    @Override
    public <T extends PointValueTime> void savePointValueAsync(T pointValue, VO<?> source) {
        switch (pointValue.getDataType()) {
            case DOUBLE:
                batchWriteBehind.add((DoubleValueTime) pointValue, ejt);
                break;
            default:
                throw new ImplementMeException();
        }
    }

    private long savePointValueImpl(final PointValueTime pointValue, final VO<?> source, boolean async) {
        throw new ImplementMeException();
        /*
         MangoValue value = pointValue.getMangoValue();
         final DataType dataType = value.getDataType();
         double dvalue = 0;
         String svalue = null;

         switch (dataType) {
         case IMAGE:
         ImageValue imageValue = (ImageValue) value;
         dvalue = imageValue.getType();
         if (!imageValue.isNew()) {
         svalue = Long.toString(imageValue.getId());
         }
         break;
         case BOOLEAN:
         case MULTISTATE:
         case DOUBLE:
         dvalue = ((DoubleValue) value).getDoubleValue();
         break;
         case ALPHANUMERIC:
         svalue = ((AlphanumericValue) value).getValue();
         break;
         default:
         throw new ImplementMeException();
         }

         // Check if we need to create an annotation.
         long id;
         try {
         if (svalue != null || source != null || dataType == DataType.IMAGE) {
         final double dvalueFinal = dvalue;
         final String svalueFinal = svalue;

         // Create a transaction within which to do the insert.
         id = getTransactionTemplate().execute(
         new TransactionCallback<Long>() {
         @Override
         public Long doInTransaction(TransactionStatus status) {
         return savePointValue(pointValue.getDataPointId(), dataType,
         dvalueFinal, pointValue.getTimestamp(),
         svalueFinal, source, false);
         }
         });
         } else {
         // Single sql call, so no transaction required.
         id = savePointValue(pointValue.getDataPointId(), dataType, dvalue,
         pointValue.getTimestamp(), svalue, source, async);
         }
         } catch (ConcurrencyFailureException e) {
         // Still failed to insert after all of the retries. Store the data
         synchronized (UNSAVED_POINT_VALUES) {
         UNSAVED_POINT_VALUES.add(new UnsavedPointValue(pointValue.getDataPointId(),
         pointValue, source));
         }
         return -1;
         }

         // Check if we need to save an image
         if (dataType == DataType.IMAGE) {
         ImageValue imageValue = (ImageValue) value;
         if (imageValue.isNew()) {
         imageValue.setId(id);

         File file = new File(common.getFiledataPath(),
         imageValue.getFilename());

         // Write the file.
         FileOutputStream out = null;
         try {
         out = new FileOutputStream(file);
         StreamUtils
         .transfer(
         new ByteArrayInputStream(imageValue
         .getData()), out);
         } catch (IOException e) {
         // Rethrow as an RTE
         throw new ImageSaveException(e);
         } finally {
         try {
         if (out != null) {
         out.close();
         }
         } catch (IOException e) {
         // no op
         }
         }

         // Allow the data to be GC'ed
         imageValue.setData(null);
         }
         }

         return id;
         */
    }

    private void clearUnsavedPointValues() {
        if (!UNSAVED_POINT_VALUES.isEmpty()) {
            synchronized (UNSAVED_POINT_VALUES) {
                while (!UNSAVED_POINT_VALUES.isEmpty()) {
                    UnsavedPointValue data = UNSAVED_POINT_VALUES.remove(0);
                    savePointValueImpl(data.getPointValue(), data.getSource(), false);
                }
            }
        }
    }

    public void savePointValue(PointValueTime pointValue) {
throw new ImplementMeException();
//savePointValueImpl(pointValue, new AnonymousUserRT(), true);
    }

    private long savePointValue(final int pointId, final DataType dataType, double dvalue,
            final long time, final String svalue, final VO<?> source,
            boolean async) {
        // Apply database specific bounds on double values.

        int retries = 5;
        while (true) {
            try {
                return savePointValueImpl(pointId, dataType, dvalue, time,
                        svalue, source);
            } catch (ConcurrencyFailureException e) {
                if (retries <= 0) {
                    throw e;
                }
                retries--;
            } catch (RuntimeException e) {
                throw new RuntimeException(
                        "Error saving point value: dataType=" + dataType
                        + ", dvalue=" + dvalue, e);
            }
        }
    }

    private long savePointValueImpl(final int pointId, final DataType dataType, final double dvalue,
            final long time, String svalue, VO<?> source) {
        long id = doInsertLong(new PreparedStatementCreator() {

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(POINT_VALUE_INSERT, Statement.RETURN_GENERATED_KEYS);
                ps.setInt(1, pointId);
//                ps.setInt(2, dataType.mangoDbId);
                ps.setDouble(3, dvalue);
                ps.setLong(4, time);
                return ps;
            }
        });

        if (svalue == null && dataType == DataType.IMAGE) {
            svalue = Long.toString(id);
        }

        // Check if we need to create an annotation.
        if (svalue != null || source != null) {
            NodeType sourceType = null;
            Integer sourceId = null;
            if (source != null) {
                sourceType = source.getNodeType();
                sourceId = source.getId();
            }

            String shortString = null;
            String longString = null;
            if (svalue != null) {
                if (svalue.length() > 128) {
                    longString = svalue;
                } else {
                    shortString = svalue;
                }
            }

            ejt.update(POINT_VALUE_ANNOTATION_INSERT, new Object[]{id,
                shortString, longString, sourceType, sourceId}, new int[]{
                Types.INTEGER, Types.VARCHAR, Types.CLOB, Types.SMALLINT,
                Types.INTEGER});
        }

        return id;
    }

    private static final String POINT_VALUE_SELECT
            = "select\n"
            + " pv.ts, pv.pointValue\n"
            + "from\n"
            + " doublePointValues pv\n";

    @Override
    public <T extends PointValueTime> List<T> getPointValues(final DataPointVO<?, T> vo, final long since) {
        flushWriteBehind();
        List<T> result = ejt.query(new PreparedStatementCreator() {

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareCall(
                        POINT_VALUE_SELECT
                        + "where\n"
                        + " pv.dataPointId=? and pv.ts >= ?\n"
                        + "order\n"
                        + " by ts");
                ps.setInt(1, vo.getId());
                ps.setLong(2, since);
                return ps;

            }
        }, new PointValueRowMapper(vo));
        updateAnnotations(result);
        return result;
    }

    @Override
    public <T extends PointValueTime> Iterable<T> getPointValuesBetween(final DataPointVO<?, T> vo, final long from, final long to) {
        flushWriteBehind();
        List<T> result = ejt.query(new PreparedStatementCreator() {

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareCall(POINT_VALUE_SELECT
                        + " where pv.dataPointId=? and pv.ts >= ? and pv.ts<? order by ts");
                ps.setInt(1, vo.getId());
                ps.setLong(2, from);
                ps.setLong(3, to);
                return ps;

            }
        }, new PointValueRowMapper(vo));
        updateAnnotations(result);
        return result;
    }

    //TODO replace with queryforObject
    @Override
    public <T extends PointValueTime> T getLatestPointValue(final DataPointVO<?, T> vo) {
        flushWriteBehind();
        //TODO optimaze into one hit of the db???
        final Long maxTs = ejt.queryForObject("select max(ts) from doublePointValues where dataPointId=?", Long.class, vo.getId());
        if (maxTs == null) {
            return null;
        }
        List<T> result = ejt.query(new PreparedStatementCreator() {

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareCall(POINT_VALUE_SELECT
                        + " where pv.dataPointId=? and pv.ts=?");
                ps.setInt(1, vo.getId());
                ps.setLong(2, maxTs);
                ps.setMaxRows(1);
                return ps;

            }
        }, new PointValueRowMapper<T>(vo));
        updateAnnotations(result);
        return result.get(0);
    }

    private <T extends PointValueTime> T getPointValue(final long id, final int dataPointId, DataType dataType) {
        flushWriteBehind();
        List<T> result = ejt.query(new PreparedStatementCreator() {

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareCall(POINT_VALUE_SELECT
                        + " where pv.id=?");
                ps.setLong(1, id);
                ps.setMaxRows(1);
                return ps;

            }
        }, new PointValueRowMapper(dataPointId, dataType));
        updateAnnotations(result);
        return result.get(0);
    }

    @Override
    public <T extends PointValueTime> T getPointValueBefore(DataPointVO<?, T> vo, long time) {
        flushWriteBehind();
        try {
            final Long valueTime = ejt.queryForObject(
                    "select max(ts) from doublePointValues where dataPointId=? and ts<?",
                    Long.class, vo.getId(), time);
            return valueTime == null ? null : getPointValueAt(vo, valueTime);
        } catch (DataAccessException e) {
            return null;
        }
    }

    public <T extends PointValueTime> T getPointValueAt(final DataPointVO<?, T> vo, final long time) {
        flushWriteBehind();
        List<T> result = ejt.query(new PreparedStatementCreator() {

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareCall(POINT_VALUE_SELECT
                        + " where pv.dataPointId=? and pv.ts=?");
                ps.setInt(1, vo.getId());
                ps.setLong(2, time);
                ps.setMaxRows(1);
                return ps;

            }
        }, new PointValueRowMapper(vo));
        updateAnnotations(result);
        return result.get(0);
    }

    public <T extends PointValueTime> T getPointValueAt(final int dpId, DataType dataType, final long time) {
        flushWriteBehind();
        List<T> result = ejt.query(new PreparedStatementCreator() {

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareCall(POINT_VALUE_SELECT
                        + " where pv.dataPointId=? and pv.ts=?");
                ps.setInt(1, dpId);
                ps.setLong(2, time);
                ps.setMaxRows(1);
                return ps;

            }
        }, new PointValueRowMapper(dpId, dataType));
        updateAnnotations(result);
        return result.get(0);
    }

    @Override
    public PointValueTime getPointValueBefore(int id, DataType dataType, long time) {
        flushWriteBehind();
        try {
            final Long valueTime = ejt.queryForObject(
                    "select max(ts) from doublePointValues where dataPointId=? and ts<?",
                    Long.class, id, time);
            return valueTime == null ? null : getPointValueAt(id, dataType, valueTime);
        } catch (DataAccessException e) {
            return null;
        }
    }

    @Override
    public <T extends PointValueTime> T savePointValueSync(T newValue, VO<?> source) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    class PointValueRowMapper<T extends PointValueTime> implements RowMapper<T> {

        private final int pointId;
        private final DataType dataType;

        public PointValueRowMapper(int pointId, DataType dataType) {
            this.pointId = pointId;
            this.dataType = dataType;
        }

        public PointValueRowMapper(DataPointVO<?, T> vo) {
            this.pointId = vo.getId();
            this.dataType = vo.getDataType();
        }

        @Override
        public T mapRow(ResultSet rs, int rowNum) throws SQLException {
            T result;
            switch (dataType) {
                case DOUBLE:
                    return (T) new DoubleValueTime(rs.getDouble(2), pointId, rs.getLong(1));
                default:
                    throw new ImplementMeException();
            }
        }

    }

    @Deprecated // TODO reimplement
    private <T extends PointValueTime> void updateAnnotations(List<T> values) {
        Map<Integer, List<PointValueAnnotation>> userIds = new HashMap<>();
        List<PointValueAnnotation> alist;

        // Look for annotated point values.
        PointValueAnnotation apv;

        // Get the usernames from the database.
        if (userIds.size() > 0) {
            updateAnnotations("select id, username from users where id in ",
                    userIds);
        }
    }

    private void updateAnnotations(String sql,
            Map<Integer, List<PointValueAnnotation>> idMap) {
        // Get the description information from the database.
        List<IntValuePair> data = ejt.query(
                sql + "(" + createDelimitedList(idMap.keySet(), ",")
                + ")", new IntValuePairRowMapper());

        // Collate the data with the id map, and set the values in the
        // annotations
        List<PointValueAnnotation> annos;
        for (IntValuePair ivp : data) {
            annos = idMap.get(ivp.getKey());
            for (PointValueAnnotation avp : annos) {
                avp.setSourceDescriptionArgument(ivp.getValue());
            }
        }
    }

    //
    //
    // Multiple-point callback for point history replays
    //
    private static final String POINT_ID_VALUE_SELECT = "select pv.dataPointId, pv.dataType, pv.pointValue, " //
            + "pva.textPointValueShort, pva.textPointValueLong, pv.ts "
            + "from pointValues pv "
            + "  left join pointValueAnnotations pva on pv.id = pva.pointValueId";

    //
    //
    // Point value deletions
    //
    public long deletePointValuesBefore(int dataPointId, long time) {
        return deletePointValues(
                "delete from pointValues where dataPointId=? and ts<?",
                new Object[]{dataPointId, time});
    }

    public long deletePointValues(int dataPointId) {
        return deletePointValues("delete from pointValues where dataPointId=?",
                new Object[]{dataPointId});
    }

    public long deleteAllPointData() {
        return deletePointValues("delete from pointValues", null);
    }

    @Deprecated
    public long deletePointValuesWithMismatchedType(int dataPointId, DataType dataType) {
        throw new RuntimeException("Deprecated");
        /*
        return deletePointValues(
                "delete from pointValues where dataPointId=? and dataType<>?",
                new Object[]{dataPointId, dataType.mangoDbId});
        */
    }

    private long deletePointValues(String sql, Object[] params) {
        int cnt;
        if (params == null) {
            cnt = ejt.update(sql);
        } else {
            cnt = ejt.update(sql, params);
        }
        clearUnsavedPointValues();
        return cnt;
    }

    public long dateRangeCount(int dataPointId, long from, long to) {
        flushWriteBehind();
        return ejt.queryForObject(
                "select count(*) from pointValues where dataPointId=? and ts>=? and ts<=?",
                Long.class,
                dataPointId, from, to);
    }

    @Override
    public long getInceptionDate(DataPointVO vo) {
        flushWriteBehind();
        try {
            return ejt.queryForObject("select min(ts) from pointValues where dataPointId=?",
                    Long.class,
                    vo.getId());
        } catch (NullPointerException e) {
            return -1;
        }
    }

    public long getStartTime(List<Integer> dataPointIds) {
        flushWriteBehind();
        if (dataPointIds.isEmpty()) {
            return -1;
        }
        return ejt.queryForObject(
                "select min(ts) from pointValues where dataPointId in ("
                        + createDelimitedList(dataPointIds, ",") + ")",
                Long.class);
    }

    public long getEndTime(List<Integer> dataPointIds) {
        flushWriteBehind();
        if (dataPointIds.isEmpty()) {
            return -1;
        }
        return ejt.queryForObject(
                "select max(ts) from pointValues where dataPointId in ("
                        + createDelimitedList(dataPointIds, ",") + ")",
                Long.class);
    }

    public LongPair getStartAndEndTime(List<Integer> dataPointIds) {
        flushWriteBehind();
        if (dataPointIds.isEmpty()) {
            return null;
        }
        try {
            return ejt.queryForObject(
                    "select min(ts), max(ts) from pointValues where dataPointId in ("
                    + createDelimitedList(dataPointIds, ",") + ")",
                    null, new RowMapper<LongPair>() {
                @Override
                public LongPair mapRow(ResultSet rs, int index)
                        throws SQLException {
                    long l = rs.getLong(1);
                    if (rs.wasNull()) {
                        return null;
                    }
                    return new LongPair(l, rs.getLong(2));
                }
            });
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    public List<Long> getFiledataIds() {
        flushWriteBehind();
        final StringBuilder sb = new StringBuilder();
        sb.append("select distinct id from ( ");
        sb.append("  select id as id from pointValues where dataType=");
//        sb.append(DataType.IMAGE.mangoDbId);
        sb.append("  union");
        sb.append("  select d.pointValueId as id from reportInstanceData d ");
        sb.append("    join reportInstancePoints p on d.reportInstancePointId=p.id");
        sb.append("  where p.dataType=");
  //      sb.append(DataType.IMAGE.mangoDbId);
        sb.append(") a order by 1");
        return ejt.queryForList(sb.toString(), Long.class);
    }

    /**
     * Class that stored point value data when it could not be saved to the
     * database due to concurrency errors.
     *
     * @author Matthew Lohbihler
     */
    class UnsavedPointValue {

        private final int pointId;
        private final PointValueTime pointValue;
        private final VO<?> source;

        public UnsavedPointValue(int pointId, PointValueTime pointValue,
                VO<?> source) {
            this.pointId = pointId;
            this.pointValue = pointValue;
            this.source = source;
        }

        public int getPointId() {
            return pointId;
        }

        public PointValueTime getPointValue() {
            return pointValue;
        }

        public VO<?> getSource() {
            return source;
        }
    }

}
