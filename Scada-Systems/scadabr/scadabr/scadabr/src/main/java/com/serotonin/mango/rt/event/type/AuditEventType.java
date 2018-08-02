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
package com.serotonin.mango.rt.event.type;

import br.org.scadabr.rt.UserRT;
import java.util.ArrayList;
import java.util.List;

import br.org.scadabr.rt.event.type.EventSources;
import br.org.scadabr.vo.event.AlarmLevel;
import com.serotonin.mango.Common;
import br.org.scadabr.util.ChangeComparable;
import com.serotonin.mango.util.ExportCodes;
import br.org.scadabr.web.i18n.LocalizableI18nKey;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.event.type.AuditEventKey;
import java.util.Objects;


public class AuditEventType extends EventType<AuditEventKey> {

    public static void raiseAddedEvent(AuditEventKey auditEventType, ChangeComparable<?> o) {
        List<LocalizableMessage> list = new ArrayList<>();
        o.addProperties(list);
        raiseEvent(auditEventType, o, "event.audit.added", list.toArray());
    }

    public static <T> void raiseChangedEvent(AuditEventKey auditEventType, T from, ChangeComparable<T> to) {
        List<LocalizableMessage> changes = new ArrayList<>();
        to.addPropertyChanges(changes, from);
        if (changes.isEmpty()) // If the object wasn't in fact changed, don't raise an event.
        {
            return;
        }
        raiseEvent(auditEventType, to, "event.audit.changed", changes.toArray());
    }

    public static void raiseDeletedEvent(AuditEventKey auditEventType, ChangeComparable<?> o) {
        List<LocalizableMessage> list = new ArrayList<>();
        o.addProperties(list);
        raiseEvent(auditEventType, o, "event.audit.deleted", list.toArray());
    }

    private static void raiseEvent(AuditEventKey auditEventType, ChangeComparable<?> o, String key, Object[] props) {
        UserRT user = null; //TODO IMplement me !!! Common.getUser();
        Object username = null;
        if (user != null) {
            username = user.getName() + " (" + user.getId() + ")";
        } else {
            String descKey = Common.getBackgroundProcessDescription();
            if (descKey == null) {
                username = new LocalizableMessageImpl("common.unknown");
            } else {
                username = new LocalizableMessageImpl(descKey);
            }
        }

        LocalizableMessage message = new LocalizableMessageImpl(key, username, new LocalizableMessageImpl(o.getTypeKey()),
                o.getId(), new LocalizableMessageImpl("event.audit.propertyList." + props.length, props));

        AuditEventType type = new AuditEventType(auditEventType, o.getId(), user);

        type.fire(message);
    }

    //
    // /
    // / Utility methods for other classes
    // /
    //
    public static void addPropertyMessage(List<LocalizableMessage> list, String propertyNameKey, Object propertyValue) {
        list.add(new LocalizableMessageImpl("event.audit.property", new LocalizableMessageImpl(propertyNameKey), propertyValue));
    }

    public static void addDoubleSientificProperty(List<LocalizableMessage> list, String propertyNameKey, double propertyValue) {
        list.add(new LocalizableMessageImpl("event.audit.propertyDoubleScientific", new LocalizableMessageImpl(propertyNameKey), propertyValue));
    }

    public static void addPropertyMessage(List<LocalizableMessage> list, String propertyNameKey, boolean propertyValue) {
        list.add(new LocalizableMessageImpl("event.audit.property", new LocalizableMessageImpl(propertyNameKey),
                getBooleanMessage(propertyValue)));
    }

    public static void addExportCodeMessage(List<LocalizableMessage> list, String propertyNameKey, ExportCodes codes,
            int id) {
        list.add(new LocalizableMessageImpl("event.audit.property", new LocalizableMessageImpl(propertyNameKey),
                getExportCodeMessage(codes, id)));
    }

    public static void maybeAddPropertyChangeMessage(List<LocalizableMessage> list, String propertyNameKey,
            int fromValue, int toValue) {
        if (fromValue != toValue) {
            addPropertyChangeMessage(list, propertyNameKey, fromValue, toValue);
        }
    }

    public static void maybeAddPropertyChangeMessage(List<LocalizableMessage> list, String propertyNameKey,
            LocalizableI18nKey fromValue, LocalizableI18nKey toValue) {
        if (!Objects.equals(fromValue, toValue)) {
            addPropertyChangeMessage(list, propertyNameKey, fromValue, toValue);
        }
    }

    public static void maybeAddPropertyChangeMessage(List<LocalizableMessage> list, String propertyNameKey,
            Object fromValue, Object toValue) {
        if (!Objects.equals(fromValue, toValue)) {
            addPropertyChangeMessage(list, propertyNameKey, fromValue, toValue);
        }
    }

    @Deprecated // Use more specific
    public static void maybeAddPropertyChangeMessage(List<LocalizableMessage> list, String propertyNameKey,
            boolean fromValue, boolean toValue) {
        if (fromValue != toValue) {
            addPropertyChangeMessage(list, propertyNameKey, getBooleanMessage(fromValue), getBooleanMessage(toValue));
        }
    }

    public static void evaluateDoubleScientific(List<LocalizableMessage> list, String propertyNameKey, String formatPattern, double fromValue, double toValue) {
//TODO user MessageFormatPattern
        if (fromValue != toValue) {
            list.add(new LocalizableMessageImpl("event.audit.changedPropertyDoubleScientific", propertyNameKey, fromValue, toValue));
        }
//throw new ImplementMeException(); //"MessageFormat");
    }

    public static void maybeAddExportCodeChangeMessage(List<LocalizableMessage> list, String propertyNameKey,
            ExportCodes exportCodes, int fromId, int toId) {
        if (fromId != toId) {
            addPropertyChangeMessage(list, propertyNameKey, getExportCodeMessage(exportCodes, fromId),
                    getExportCodeMessage(exportCodes, toId));
        }
    }

    private static LocalizableMessage getBooleanMessage(boolean value) {
        if (value) {
            return new LocalizableMessageImpl("common.true");
        }
        return new LocalizableMessageImpl("common.false");
    }

    private static LocalizableMessage getExportCodeMessage(ExportCodes exportCodes, int id) {
        String key = exportCodes.getKey(id);
        if (key == null) {
            return new LocalizableMessageImpl("common.unknown");
        }
        return new LocalizableMessageImpl(key);
    }

    public static void addPropertyChangeMessage(List<LocalizableMessage> list, String propertyNameKey, Object fromValue, Object toValue) {
        list.add(new LocalizableMessageImpl("event.audit.changedProperty", new LocalizableMessageImpl(propertyNameKey), fromValue, toValue));
    }

    //
    // /
    // / Instance stuff
    // /
    //
    private final int referenceId;
    // THis is a kind of contextual data where to put this - This looks not the place to do so....
    private final UserRT raisingUser;

    public AuditEventType(AuditEventKey auditEventType, int referenceId, UserRT raisingUser) {
        super(auditEventType);
        this.referenceId = referenceId;
        this.raisingUser = raisingUser;
    }

    @Override
    public EventSources getEventSource() {
        return EventSources.AUDIT;
    }

    @Override
    public String toString() {
        return "AuditEventType(auditType=" + eventKey + ", referenceId=" + referenceId + ")";
    }

    public int getReferenceId() {
        return referenceId;
    }

    @Override
    public boolean excludeUser(UserRT user) {
        if (raisingUser != null && !raisingUser.isReceiveOwnAuditEvents()) {
            return user.equals(raisingUser);
        }
        return false;
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + eventKey.getId();
        result = prime * result + referenceId;
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        AuditEventType other = (AuditEventType) obj;
        if (eventKey != other.eventKey) {
            return false;
        }
        return referenceId == other.referenceId;
    }

    @Override
    public AlarmLevel getAlarmLevel() {
        return eventKey.getAlarmLevel();
    }

    @Override
    public int getReferenceId1() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public int getReferenceId2() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }


}
