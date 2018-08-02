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
package com.serotonin.mango.vo.publish;

import br.org.scadabr.ScadaBrConstants;
import br.org.scadabr.dao.PublisherDao;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

import br.org.scadabr.rt.event.type.EventSources;
import com.serotonin.mango.rt.publish.PublisherRT;
import com.serotonin.mango.util.ExportCodes;
import com.serotonin.mango.vo.event.EventTypeVO;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.utils.TimePeriods;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.event.type.PublisherEventKey;
import com.fasterxml.jackson.annotation.JsonIgnore;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;
import org.springframework.validation.Errors;
import org.springframework.validation.Validator;

/**
 * @author Matthew Lohbihler
 * @param <T>
 */
abstract public class PublisherVO<T extends PublishedPointVO> implements Serializable {

    @Configurable
    public static class PublisherVoValidator implements Validator {

        @Autowired
        private PublisherDao publisherDao;

        @Override
        public boolean supports(Class<?> clazz) {
            return PublisherVO.class.isAssignableFrom(clazz);
        }

        @Override
        public void validate(Object target, Errors errors) {
            final PublisherVO vo = (PublisherVO) target;
            if (vo.name.isEmpty()) {
                errors.rejectValue("name", "validate.required");
            }
            if (vo.name.length() > 40) {
                errors.rejectValue("name", "validate.nameTooLong");
            }

            if (vo.xid.isEmpty()) {
                errors.rejectValue("xid", "validate.required");
            } else if (!publisherDao.isXidUnique(vo.xid, vo.id)) {
                errors.rejectValue("xid", "validate.xidUsed");
            } else if (vo.xid.length() > 50) {
                errors.rejectValue("xid", "validate.notLongerThan", new Object[]{50}, "validate.notLongerThan");
            }

            if (vo.sendSnapshot) {
                if (vo.snapshotSendPeriods <= 0) {
                    errors.rejectValue("snapshotSendPeriods", "validate.greaterThanZero");
                }
            }

            if (vo.cacheWarningSize < 1) {
                errors.rejectValue("cacheWarningSize", "validate.greaterThanZero");
            }

        }

    }

    public enum Type {

        HTTP_SENDER(1, "publisherEdit.httpSender") {
                    @Override
                    public PublisherVO<?> createPublisherVO() {
                        throw new ImplementMeException(); // return new HttpSenderVO();
                    }
                },
        PACHUBE(2, "publisherEdit.pachube") {
                    @Override
                    public PublisherVO<?> createPublisherVO() {
                        throw new ImplementMeException(); // return new PachubeSenderVO();
                    }
                },
        PERSISTENT(3, "publisherEdit.persistent") {
                    @Override
                    public PublisherVO<?> createPublisherVO() {
                        throw new ImplementMeException(); //return new PersistentSenderVO();
                    }
                };

        private Type(int id, String key) {
            this.id = id;
            this.key = key;
        }

        private final int id;
        private final String key;

        public int getId() {
            return id;
        }

        public String getKey() {
            return key;
        }

        public abstract PublisherVO<?> createPublisherVO();

        public static Type valueOf(int id) {
            for (Type type : values()) {
                if (type.id == id) {
                    return type;
                }
            }
            return null;
        }

        public static Type valueOfIgnoreCase(String text) {
            for (Type type : values()) {
                if (type.name().equalsIgnoreCase(text)) {
                    return type;
                }
            }
            return null;
        }

        public static List<String> getTypeList() {
            List<String> result = new ArrayList<>();
            for (Type type : values()) {
                result.add(type.name());
            }
            return result;
        }
    }

    public static final String XID_PREFIX = "PUB_";

    public static PublisherVO<? extends PublishedPointVO> createPublisherVO(int typeId) {
        return Type.valueOf(typeId).createPublisherVO();
    }

    public PublisherEventKey getPublisherEventKey() {
        throw new ImplementMeException();
    }

    abstract public Type getType();

    abstract public LocalizableMessage getConfigDescription();

    abstract public PublisherRT<T> createPublisherRT();

    public LocalizableMessage getTypeMessage() {
        return new LocalizableMessageImpl(getType().getKey());
    }

    public List<EventTypeVO> getEventTypes() {
        List<EventTypeVO> eventTypes = new ArrayList<>();
        eventTypes.add(new EventTypeVO(EventSources.PUBLISHER, getId(), PublisherRT.POINT_DISABLED_EVENT,
                new LocalizableMessageImpl("event.pb.pointMissing"), AlarmLevel.URGENT));
        eventTypes.add(new EventTypeVO(EventSources.PUBLISHER, getId(), PublisherRT.QUEUE_SIZE_WARNING_EVENT,
                new LocalizableMessageImpl("event.pb.queueSize"), AlarmLevel.URGENT));

        getEventTypesImpl(eventTypes);

        return eventTypes;
    }

    protected static void addDefaultEventCodes(ExportCodes codes) {
        codes.addElement(PublisherRT.POINT_DISABLED_EVENT, "POINT_DISABLED_EVENT");
        codes.addElement(PublisherRT.QUEUE_SIZE_WARNING_EVENT, "QUEUE_SIZE_WARNING_EVENT");
    }

    abstract public ExportCodes getEventCodes();

    abstract protected void getEventTypesImpl(List<EventTypeVO> eventTypes);

    abstract protected T createPublishedPointInstance();

    @JsonIgnore
    public boolean isNew() {
        return id == ScadaBrConstants.NEW_ID;
    }

    private int id = ScadaBrConstants.NEW_ID;
    private String xid;

    private String name;

    private boolean enabled;
    protected List<T> points = new ArrayList<>();

    private boolean changesOnly;

    private int cacheWarningSize = 100;

    private boolean sendSnapshot;
    private TimePeriods snapshotSendPeriodType = TimePeriods.MINUTES;

    private int snapshotSendPeriods = 5;

    public boolean isEnabled() {
        return enabled;
    }

    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getXid() {
        return xid;
    }

    public void setXid(String xid) {
        this.xid = xid;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public List<T> getPoints() {
        return points;
    }

    public void setPoints(List<T> points) {
        this.points = points;
    }

    public boolean isChangesOnly() {
        return changesOnly;
    }

    public void setChangesOnly(boolean changesOnly) {
        this.changesOnly = changesOnly;
    }

    public int getCacheWarningSize() {
        return cacheWarningSize;
    }

    public void setCacheWarningSize(int cacheWarningSize) {
        this.cacheWarningSize = cacheWarningSize;
    }

    public boolean isSendSnapshot() {
        return sendSnapshot;
    }

    public void setSendSnapshot(boolean sendSnapshot) {
        this.sendSnapshot = sendSnapshot;
    }

    public TimePeriods getSnapshotSendPeriodType() {
        return snapshotSendPeriodType;
    }

    public void setSnapshotSendPeriodType(TimePeriods snapshotSendPeriodType) {
        this.snapshotSendPeriodType = snapshotSendPeriodType;
    }

    public int getSnapshotSendPeriods() {
        return snapshotSendPeriods;
    }

    public void setSnapshotSendPeriods(int snapshotSendPeriods) {
        this.snapshotSendPeriods = snapshotSendPeriods;
    }

    //
    // /
    // / Serialization
    // /
    //
    private static final long serialVersionUID = -1;
    private static final int version = 2;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);
        SerializationHelper.writeSafeUTF(out, name);
        out.writeBoolean(enabled);
        out.writeObject(points);
        out.writeBoolean(changesOnly);
        out.writeInt(cacheWarningSize);
        out.writeBoolean(sendSnapshot);
        out.writeInt(snapshotSendPeriodType.getId());
        out.writeInt(snapshotSendPeriods);
    }

    @SuppressWarnings("unchecked")
    private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            name = SerializationHelper.readSafeUTF(in);
            enabled = in.readBoolean();
            points = (List<T>) in.readObject();
            changesOnly = in.readBoolean();
            cacheWarningSize = in.readInt();
            sendSnapshot = false;
            snapshotSendPeriodType = TimePeriods.MINUTES;
            snapshotSendPeriods = 5;
        } else if (ver == 2) {
            name = SerializationHelper.readSafeUTF(in);
            enabled = in.readBoolean();
            points = (List<T>) in.readObject();
            changesOnly = in.readBoolean();
            cacheWarningSize = in.readInt();
            sendSnapshot = in.readBoolean();
            snapshotSendPeriodType = TimePeriods.fromId(in.readInt());
            snapshotSendPeriods = in.readInt();
        }
    }

}
