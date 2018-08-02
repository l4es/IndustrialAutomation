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


import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.joda.time.DateTime;
import org.springframework.jdbc.core.BatchPreparedStatementSetter;
import org.springframework.transaction.TransactionStatus;
import org.springframework.transaction.support.TransactionCallbackWithoutResult;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.dao.MailingListDao;
import br.org.scadabr.utils.ImplementMeException;
import com.serotonin.mango.vo.event.RecipientListEntry;
import com.serotonin.mango.vo.mailingList.AddressEntry;
import com.serotonin.mango.vo.mailingList.EmailRecipient;
import com.serotonin.mango.vo.mailingList.MailingList;
import com.serotonin.mango.vo.mailingList.UserEntry;
import java.sql.Connection;
import java.sql.Statement;
import javax.inject.Inject;
import javax.inject.Named;
import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.jdbc.core.PreparedStatementCreator;
import org.springframework.jdbc.core.RowMapper;

/**
 * @author Matthew Lohbihler
 */
@Named
public class MailingListDaoImpl extends BaseDao implements MailingListDao {

    public MailingListDaoImpl() {
        super();
    }
    
    public String generateUniqueXid() {
        return generateUniqueXid(MailingList.XID_PREFIX, "mailingLists");
    }

    public boolean isXidUnique(String xid, int excludeId) {
        return isXidUnique(xid, excludeId, "mailingLists");
    }

    private static final String MAILING_LIST_SELECT = "select id, xid, name from mailingLists ";

    public List<MailingList> getMailingLists() {
        List<MailingList> result = ejt.query(MAILING_LIST_SELECT + "order by name", new MailingListRowMapper());
        setRelationalData(result);
        return result;
    }

    @Override
    public MailingList getMailingList(int id) {
        MailingList ml = ejt.queryForObject(MAILING_LIST_SELECT + "where id=?", new MailingListRowMapper(), id);
        setRelationalData(ml);
        return ml;
    }

    public MailingList getMailingList(String xid) {
        try {
            MailingList ml = ejt.queryForObject(MAILING_LIST_SELECT + "where xid=?", new MailingListRowMapper(), xid);
            setRelationalData(ml);
            return ml;
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    class MailingListRowMapper implements RowMapper<MailingList> {

        @Override
        public MailingList mapRow(ResultSet rs, int rowNum) throws SQLException {
            MailingList ml = new MailingList();
            ml.setId(rs.getInt(1));
            ml.setXid(rs.getString(2));
            ml.setName(rs.getString(3));
            return ml;
        }
    }

    private void setRelationalData(List<MailingList> mls) {
        for (MailingList ml : mls) {
            setRelationalData(ml);
        }
    }

    private static final String MAILING_LIST_INACTIVE_SELECT = "select inactiveInterval from mailingListInactive where mailingListId=?";
    private static final String MAILING_LIST_ENTRIES_SELECT = "select typeId, userId, address, '' from mailingListMembers where mailingListId=?";

    private void setRelationalData(MailingList ml) {
        ml.getInactiveIntervals().addAll(
                ejt.query(MAILING_LIST_INACTIVE_SELECT, new MailingListScheduleInactiveMapper(), ml.getId()));

        ml.setEntries(ejt.query(MAILING_LIST_ENTRIES_SELECT, new EmailRecipientRowMapper(), ml.getId()));

        // Update the user type entries with their respective user objects.
        populateEntrySubclasses(ml.getEntries());
    }

    class MailingListScheduleInactiveMapper implements RowMapper<Integer> {

        @Override
        public Integer mapRow(ResultSet rs, int rowNum) throws SQLException {
            return rs.getInt(1);
        }
    }

    class EmailRecipientRowMapper implements RowMapper<EmailRecipient> {

        @Override
        public EmailRecipient mapRow(ResultSet rs, int rowNum) throws SQLException {
            int type = rs.getInt(1);
            switch (type) {
                case EmailRecipient.TYPE_MAILING_LIST:
                    MailingList ml = new MailingList();
                    ml.setId(rs.getInt(2));
                    ml.setName(rs.getString(4));
                    return ml;
                case EmailRecipient.TYPE_USER:
                    UserEntry ue = new UserEntry();
                    ue.setUserId(rs.getInt(2));
                    return ue;
                case EmailRecipient.TYPE_ADDRESS:
                    AddressEntry ae = new AddressEntry();
                    ae.setAddress(rs.getString(3));
                    return ae;
            }
            throw new ShouldNeverHappenException("Unknown mailing list entry type: " + type);
        }
    }

    @Override
    public Set<String> getRecipientAddresses(List<RecipientListEntry> beans, DateTime sendTime) {
        List<EmailRecipient> entries = new ArrayList<>(beans.size());
        for (RecipientListEntry bean : beans) {
            entries.add(bean.createEmailRecipient());
        }
        populateEntrySubclasses(entries);
        Set<String> addresses = new HashSet<>();
        for (EmailRecipient entry : entries) {
            entry.appendAddresses(addresses, sendTime);
        }
        return addresses;
    }

    public void populateEntrySubclasses(List<EmailRecipient> entries) {
throw new ImplementMeException();
/*
// Update the user type entries with their respective user objects.
        for (EmailRecipient e : entries) {
            if (e instanceof MailingList) // NOTE: this does not set the mailing list name.
            {
                setRelationalData((MailingList) e);
            } else if (e instanceof UserEntry) {
                UserEntry ue = (UserEntry) e;
                ue.setUser(userDao.getUser(ue.getUserId()));
            }
        }
        */
    }

    private static final String MAILING_LIST_UPDATE = "update mailingLists set xid=?, name=? where id=?";

    public void saveMailingList(final MailingList ml) {
        final JdbcTemplate ejt2 = ejt;
        getTransactionTemplate().execute(new TransactionCallbackWithoutResult() {
            @SuppressWarnings("synthetic-access")
            @Override
            protected void doInTransactionWithoutResult(TransactionStatus status) {
                if (ml.isNew()) {
                    final int id = doInsert(new PreparedStatementCreator() {

                        final static String SQL_INSERT = "insert into mailingLists (xid, name) values (?,?)";

                        @Override
                        public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                            PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                            ps.setString(1, ml.getXid());
                            ps.setString(2, ml.getName());
                            return ps;
                        }
                    });
                    ml.setId(id);
                } else {
                    ejt2.update(MAILING_LIST_UPDATE, new Object[]{ml.getXid(), ml.getName(), ml.getId()});
                }
                saveRelationalData(ml);
            }
        });
    }

    private static final String MAILING_LIST_INACTIVE_INSERT = "insert into mailingListInactive (mailingListId, inactiveInterval) values (?,?)";
    private static final String MAILING_LIST_ENTRY_INSERT = "insert into mailingListMembers (mailingListId, typeId, userId, address) values (?,?,?,?)";

    void saveRelationalData(final MailingList ml) {
        // Save the inactive intervals.
        ejt.update("delete from mailingListInactive where mailingListId=?", new Object[]{ml.getId()});

        // Save what is in the mailing list object.
        final List<Integer> intervalIds = new ArrayList<>(ml.getInactiveIntervals());
        ejt.batchUpdate(MAILING_LIST_INACTIVE_INSERT, new BatchPreparedStatementSetter() {
            @Override
            public int getBatchSize() {
                return intervalIds.size();
            }

            @Override
            public void setValues(PreparedStatement ps, int i) throws SQLException {
                ps.setInt(1, ml.getId());
                ps.setInt(2, intervalIds.get(i));
            }
        });

        // Delete existing entries
        ejt.update("delete from mailingListMembers where mailingListId=?", new Object[]{ml.getId()});

        // Save what is in the mailing list object.
        final List<EmailRecipient> entries = ml.getEntries();
        ejt.batchUpdate(MAILING_LIST_ENTRY_INSERT, new BatchPreparedStatementSetter() {
            @Override
            public int getBatchSize() {
                return entries.size();
            }

            @Override
            public void setValues(PreparedStatement ps, int i) throws SQLException {
                EmailRecipient e = entries.get(i);
                ps.setInt(1, ml.getId());
                ps.setInt(2, e.getRecipientType());
                ps.setInt(3, e.getReferenceId());
                ps.setString(4, e.getReferenceAddress());
            }
        });

    }

    public void deleteMailingList(final int mailingListId) {
        ejt.update("delete from mailingLists where id=?", new Object[]{mailingListId});
    }
}
