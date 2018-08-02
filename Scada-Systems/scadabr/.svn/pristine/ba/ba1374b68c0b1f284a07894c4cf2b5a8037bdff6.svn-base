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
package com.serotonin.mango.vo.event;

import br.org.scadabr.DataType;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.dao.DataPointDao;
import br.org.scadabr.dao.MailingListDao;
import br.org.scadabr.rt.UserRT;
import com.serotonin.mango.rt.event.handlers.EmailHandlerRT;
import com.serotonin.mango.rt.event.handlers.EventHandlerRT;
import com.serotonin.mango.rt.event.handlers.ProcessHandlerRT;
//import com.serotonin.mango.rt.event.handlers.ScriptHandlerRT;
import com.serotonin.mango.rt.event.handlers.SetPointHandlerRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import br.org.scadabr.util.ChangeComparable;
import com.serotonin.mango.vo.mailingList.EmailRecipient;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.utils.TimePeriods;
import br.org.scadabr.utils.i18n.LocalizableEnum;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.vo.DataPointVO;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;
import org.springframework.validation.Errors;
import org.springframework.validation.Validator;

@Configurable
public class EventHandlerVO implements Serializable, ChangeComparable<EventHandlerVO> {

    @Configurable
    public static class EventHandlerVoValidator implements Validator {

        @Autowired
        private DataPointDao dataPointDao;

        @Override
        public boolean supports(Class<?> clazz) {
            return EventHandlerVO.class.isAssignableFrom(clazz);
        }

        @Override
        public void validate(Object target, Errors errors) {
            final EventHandlerVO vo = (EventHandlerVO) target;
            switch (vo.handlerType) {
                case SET_POINT:
                    final DataPointVO dp = dataPointDao.getDataPoint(vo.targetPointId);

                    if (dp == null) {
                        errors.rejectValue("targetPointId", "eventHandlers.noTargetPoint");
                    } else {
                        final DataType dataType = dp.getDataType();

                        if (vo.activeAction == SetActionType.NONE && vo.inactiveAction == SetActionType.NONE) {
                            errors.reject("eventHandlers.noSetPointAction");
                        }

                        // Active
                        if (vo.activeAction == SetActionType.STATIC_VALUE && dataType == DataType.MULTISTATE) {
                            try {
                                Integer.parseInt(vo.activeValueToSet);
                            } catch (NumberFormatException e) {
                                errors.reject("eventHandlers.invalidActiveValue");
                            }
                        }

                        if (vo.activeAction == SetActionType.STATIC_VALUE && dataType == DataType.DOUBLE) {
                            try {
                                Double.parseDouble(vo.activeValueToSet);
                            } catch (NumberFormatException e) {
                                errors.reject("eventHandlers.invalidActiveValue");
                            }
                        }

                        if (vo.activeAction == SetActionType.POINT_VALUE) {
                            final DataPointVO dpActive = dataPointDao.getDataPoint(vo.activePointId);

                            if (dpActive == null) {
                                errors.reject("eventHandlers.invalidActiveSource");
                            } else if (dataType != dpActive.getDataType()) {
                                errors.reject("eventHandlers.invalidActiveSourceType");
                            }
                        }

                        // Inactive
                        if (vo.inactiveAction == SetActionType.STATIC_VALUE && dataType == DataType.MULTISTATE) {
                            try {
                                Integer.parseInt(vo.inactiveValueToSet);
                            } catch (NumberFormatException e) {
                                errors.reject("eventHandlers.invalidInactiveValue");
                            }
                        }

                        if (vo.inactiveAction == SetActionType.STATIC_VALUE && dataType == DataType.DOUBLE) {
                            try {
                                Double.parseDouble(vo.inactiveValueToSet);
                            } catch (NumberFormatException e) {
                                errors.reject("eventHandlers.invalidInactiveValue");
                            }
                        }

                        if (vo.inactiveAction == SetActionType.POINT_VALUE) {
                            final DataPointVO dpInactive = dataPointDao.getDataPoint(vo.inactivePointId);

                            if (dpInactive == null) {
                                errors.reject("eventHandlers.invalidInactiveSource");
                            } else if (dataType != dpInactive.getDataType()) {
                                errors.reject("eventHandlers.invalidInactiveSourceType");
                            }
                        }
                    }
                    break;
                case EMAIL:
                    if (vo.activeRecipients.isEmpty()) {
                        errors.reject("eventHandlers.noEmailRecips");
                    }

                    if (vo.sendEscalation) {
                        if (vo.escalationDelay <= 0) {
                            errors.rejectValue("escalationDelay", "eventHandlers.escalDelayError");
                        }
                        if (vo.escalationRecipients.isEmpty()) {
                            errors.reject("eventHandlers.noEscalRecips");
                        }
                    }

                    if (vo.sendInactive && vo.inactiveOverride) {
                        if (vo.inactiveRecipients.isEmpty()) {
                            errors.reject("eventHandlers.noInactiveRecips");
                        }
                    }
                    break;
                case PROCESS:
                    if (vo.activeProcessCommand == null && vo.inactiveProcessCommand == null) {
                        errors.reject("eventHandlers.invalidCommands");
                    }
                    break;
                case SCRIPT:
                    if (vo.activeScriptCommand < 1 && vo.inactiveScriptCommand < 1) {
                        errors.reject("eventHandlers.invalidScripts");
                    }
                    break;
                default:
                    throw new ShouldNeverHappenException("cant handle Type" + vo.handlerType);
            }
        }
    }
    @Autowired
    private MailingListDao mailingListDao;
    @Autowired
    private DataPointDao dataPointDao;
    @Autowired
    private RuntimeManager runtimeManager;

    public static final String XID_PREFIX = "EH_";

    public enum Type implements LocalizableEnum<Type> {

        SET_POINT(1, "eventHandlers.type.setPoint"),
        EMAIL(2, "eventHandlers.type.email"),
        PROCESS(3, "eventHandlers.type.process"),
        SCRIPT(4, "eventHandlers.type.script");
        private final int id;
        private final String i18nKey;

        private Type(int id, String i18nKey) {
            this.id = id;
            this.i18nKey = i18nKey;
        }

        public int getId() {
            return id;
        }

        public static Type fromId(int id) {
            switch (id) {
                case 1:
                    return SET_POINT;
                case 2:
                    return EMAIL;
                case 3:
                    return PROCESS;
                case 4:
                    return SCRIPT;
                default:
                    throw new IndexOutOfBoundsException("Cant get Type from id: " + id);
            }
        }

        @Override
        public String getName() {
            return name();
        }

        @Override
        public String getI18nKey() {
            return i18nKey;
        }

        @Override
        public Object[] getArgs() {
            return null;
        }
    }

    public enum RecipientType implements LocalizableEnum<RecipientType> {

        ACTIVE(1, "eventHandlers.recipientType.active"),
        ESCALATION(2, "eventHandlers.recipientType.escalation"),
        INACTIVE(3, "eventHandlers.recipientType.inactive");
        private final int id;
        private final String i18nKey;

        private RecipientType(int id, String i18nKey) {
            this.id = id;
            this.i18nKey = i18nKey;
        }

        public int getId() {
            return id;
        }

        public static RecipientType fromId(int id) {
            switch (id) {
                case 1:
                    return ACTIVE;
                case 2:
                    return ESCALATION;
                case 3:
                    return INACTIVE;
                default:
                    throw new IndexOutOfBoundsException("Cant get Type from id: " + id);
            }
        }

        @Override
        public String getName() {
            return name();
        }

        @Override
        public String getI18nKey() {
            return i18nKey;
        }

        @Override
        public Object[] getArgs() {
            return null;
        }
    }

    public enum SetActionType implements LocalizableEnum<SetActionType> {

        NONE(0, "eventHandlers.action.none"),
        POINT_VALUE(1, "eventHandlers.action.point"),
        STATIC_VALUE(2, "eventHandlers.action.static");
        private final int id;
        private final String i18nKey;

        private SetActionType(int id, String i18nKey) {
            this.id = id;
            this.i18nKey = i18nKey;
        }

        public int getId() {
            return id;
        }

        public static SetActionType fromId(int id) {
            switch (id) {
                case 0:
                    return NONE;
                case 1:
                    return POINT_VALUE;
                case 2:
                    return STATIC_VALUE;
                default:
                    throw new IndexOutOfBoundsException("Cant get Type from id: " + id);
            }
        }

        @Override
        public String getName() {
            return name();
        }

        @Override
        public String getI18nKey() {
            return i18nKey;
        }

        @Override
        public Object[] getArgs() {
            return null;
        }
    }

    // Common fields
    private Integer id;
    private String xid;

    private String alias;
    private Type handlerType;

    private boolean disabled;

    // Set point handler fields.
    private int targetPointId;
    private SetActionType activeAction;
    /*
    * TODO INtroduce generic
    */
    private String activeValueToSet;
    private int activePointId;
    private SetActionType inactiveAction;
    /*
    * TODO INtroduce generic
    */
    private String inactiveValueToSet;
    private int inactivePointId;

    // Email handler fields.
    private List<RecipientListEntry> activeRecipients;
    private boolean sendEscalation;
    private TimePeriods escalationDelayType;
    private int escalationDelay;
    private List<RecipientListEntry> escalationRecipients;
    private boolean sendInactive;
    private boolean inactiveOverride;
    private List<RecipientListEntry> inactiveRecipients;

    // Process handler fields.
    private String activeProcessCommand;
    private String inactiveProcessCommand;

    // script fields
    private int activeScriptCommand;
    private int inactiveScriptCommand;

    public EventHandlerRT createRuntime() {
        switch (handlerType) {
            case SET_POINT:
                return new SetPointHandlerRT(this);
            case EMAIL:
                return new EmailHandlerRT(this);
            case PROCESS:
                return new ProcessHandlerRT(this);
            case SCRIPT:
                throw new ImplementMeException(); //return new ScriptHandlerRT(this);
            default:
                throw new ShouldNeverHappenException("Unknown handler type: " + handlerType);
        }
    }

    public LocalizableMessage getMessage() {
        if (!alias.isEmpty()) {
            return new LocalizableMessageImpl("common.default", alias);
        }
        return handlerType;
    }

    public int getTargetPointId() {
        return targetPointId;
    }

    public void setTargetPointId(int targetPointId) {
        this.targetPointId = targetPointId;
    }

    @Override
    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public String getXid() {
        return xid;
    }

    public void setXid(String xid) {
        this.xid = xid;
    }

    public String getAlias() {
        return alias;
    }

    public void setAlias(String alias) {
        this.alias = alias;
    }

    public Type getHandlerType() {
        return handlerType;
    }

    public void setHandlerType(Type handlerType) {
        this.handlerType = handlerType;
    }

    public boolean isDisabled() {
        return disabled;
    }

    public void setDisabled(boolean disabled) {
        this.disabled = disabled;
    }

    public SetActionType getActiveAction() {
        return activeAction;
    }

    public void setActiveAction(SetActionType activeAction) {
        this.activeAction = activeAction;
    }

    public SetActionType getInactiveAction() {
        return inactiveAction;
    }

    public void setInactiveAction(SetActionType inactiveAction) {
        this.inactiveAction = inactiveAction;
    }

    public String getActiveValueToSet() {
        return activeValueToSet;
    }

    public void setActiveValueToSet(String activeValueToSet) {
        this.activeValueToSet = activeValueToSet;
    }

    public int getActivePointId() {
        return activePointId;
    }

    public void setActivePointId(int activePointId) {
        this.activePointId = activePointId;
    }

    public String getInactiveValueToSet() {
        return inactiveValueToSet;
    }

    public void setInactiveValueToSet(String inactiveValueToSet) {
        this.inactiveValueToSet = inactiveValueToSet;
    }

    public int getInactivePointId() {
        return inactivePointId;
    }

    public void setInactivePointId(int inactivePointId) {
        this.inactivePointId = inactivePointId;
    }

    public List<RecipientListEntry> getActiveRecipients() {
        return activeRecipients;
    }

    public void setActiveRecipients(
            List<RecipientListEntry> activeRecipients) {
        this.activeRecipients = activeRecipients;
    }

    public int getEscalationDelay() {
        return escalationDelay;
    }

    public void setEscalationDelay(int escalationDelay) {
        this.escalationDelay = escalationDelay;
    }

    public TimePeriods getEscalationDelayType() {
        return escalationDelayType;
    }

    public void setEscalationDelayType(TimePeriods escalationDelayType) {
        this.escalationDelayType = escalationDelayType;
    }

    public List<RecipientListEntry> getEscalationRecipients() {
        return escalationRecipients;
    }

    public void setEscalationRecipients(
            List<RecipientListEntry> escalationRecipients) {
        this.escalationRecipients = escalationRecipients;
    }

    public boolean isSendEscalation() {
        return sendEscalation;
    }

    public void setSendEscalation(boolean sendEscalation) {
        this.sendEscalation = sendEscalation;
    }

    public boolean isSendInactive() {
        return sendInactive;
    }

    public void setSendInactive(boolean sendInactive) {
        this.sendInactive = sendInactive;
    }

    public boolean isInactiveOverride() {
        return inactiveOverride;
    }

    public void setInactiveOverride(boolean inactiveOverride) {
        this.inactiveOverride = inactiveOverride;
    }

    public List<RecipientListEntry> getInactiveRecipients() {
        return inactiveRecipients;
    }

    public void setInactiveRecipients(
            List<RecipientListEntry> inactiveRecipients) {
        this.inactiveRecipients = inactiveRecipients;
    }

    public String getActiveProcessCommand() {
        return activeProcessCommand;
    }

    public void setActiveProcessCommand(String activeProcessCommand) {
        this.activeProcessCommand = activeProcessCommand;
    }

    public String getInactiveProcessCommand() {
        return inactiveProcessCommand;
    }

    public void setInactiveProcessCommand(String inactiveProcessCommand) {
        this.inactiveProcessCommand = inactiveProcessCommand;
    }

    @Override
    public String getTypeKey() {
        return "event.audit.eventHandler";
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "common.xid", xid);
        AuditEventType.addPropertyMessage(list, "eventHandlers.alias", alias);
        AuditEventType.addPropertyMessage(list, "eventHandlers.type", handlerType);
        AuditEventType.addPropertyMessage(list, "common.disabled", disabled);
        switch (handlerType) {
            case SET_POINT:
                AuditEventType.addPropertyMessage(list, "eventHandlers.target",
                        dataPointDao.getExtendedPointName(targetPointId));
                AuditEventType.addPropertyMessage(list, "eventHandlers.activeAction", activeAction);
                switch (activeAction) {
                    case POINT_VALUE:
                        AuditEventType.addPropertyMessage(list,
                                "eventHandlers.action.point", dataPointDao
                                .getExtendedPointName(activePointId));
                        break;
                    case STATIC_VALUE:
                        AuditEventType.addPropertyMessage(list,
                                "eventHandlers.action.static", activeValueToSet);
                }

                AuditEventType.addPropertyMessage(list, "eventHandlers.inactiveAction", inactiveAction);
                switch (inactiveAction) {
                    case POINT_VALUE:
                        AuditEventType.addPropertyMessage(list,
                                "eventHandlers.action.point", dataPointDao
                                .getExtendedPointName(inactivePointId));
                        break;
                    case STATIC_VALUE:
                        AuditEventType.addPropertyMessage(list,
                                "eventHandlers.action.static", inactiveValueToSet);
                }
                break;
            case EMAIL:
                AuditEventType.addPropertyMessage(list,
                        "eventHandlers.emailRecipients",
                        createRecipientMessage(activeRecipients));
                AuditEventType.addPropertyMessage(list, "eventHandlers.escal",
                        sendEscalation);
                if (sendEscalation) {
                    AuditEventType.addPropertyMessage(list,
                            "eventHandlers.escalPeriod", escalationDelayType.getPeriodDescription(escalationDelay));
                    AuditEventType.addPropertyMessage(list,
                            "eventHandlers.escalRecipients",
                            createRecipientMessage(escalationRecipients));
                }
                AuditEventType.addPropertyMessage(list,
                        "eventHandlers.inactiveNotif", sendInactive);
                if (sendInactive) {
                    AuditEventType.addPropertyMessage(list,
                            "eventHandlers.inactiveOverride", inactiveOverride);
                    if (inactiveOverride) {
                        AuditEventType.addPropertyMessage(list,
                                "eventHandlers.inactiveRecipients",
                                createRecipientMessage(inactiveRecipients));
                    }
                }
                break;
            case PROCESS:
                AuditEventType.addPropertyMessage(list,
                        "eventHandlers.activeCommand", activeProcessCommand);
                AuditEventType.addPropertyMessage(list,
                        "eventHandlers.inactiveCommand", inactiveProcessCommand);
                break;
            case SCRIPT:
                AuditEventType.addPropertyMessage(list,
                        "eventHandlers.activeCommand", activeScriptCommand);
                AuditEventType.addPropertyMessage(list,
                        "eventHandlers.inactiveCommand", inactiveScriptCommand);
        }
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list,
            EventHandlerVO from) {
        AuditEventType.maybeAddPropertyChangeMessage(list, "common.xid",
                from.xid, xid);
        AuditEventType.maybeAddPropertyChangeMessage(list,
                "eventHandlers.alias", from.alias, alias);
        AuditEventType.maybeAddPropertyChangeMessage(list, "common.disabled",
                from.disabled, disabled);
        switch (handlerType) {
            case SET_POINT:
                AuditEventType.maybeAddPropertyChangeMessage(list,
                        "eventHandlers.target", dataPointDao
                        .getExtendedPointName(from.targetPointId),
                        dataPointDao.getExtendedPointName(targetPointId));
                AuditEventType.maybeAddPropertyChangeMessage(list,
                        "eventHandlers.activeAction", from.activeAction, activeAction);
                AuditEventType.maybeAddPropertyChangeMessage(list,
                        "eventHandlers.action.point", dataPointDao
                        .getExtendedPointName(from.activePointId),
                        dataPointDao.getExtendedPointName(activePointId));
                AuditEventType.maybeAddPropertyChangeMessage(list,
                        "eventHandlers.action.static", from.activeValueToSet,
                        activeValueToSet);

                AuditEventType.maybeAddPropertyChangeMessage(list,
                        "eventHandlers.inactiveAction", from.inactiveAction, inactiveAction);
                AuditEventType.maybeAddPropertyChangeMessage(list,
                        "eventHandlers.action.point", dataPointDao
                        .getExtendedPointName(from.inactivePointId),
                        dataPointDao.getExtendedPointName(inactivePointId));
                AuditEventType.maybeAddPropertyChangeMessage(list,
                        "eventHandlers.action.static", from.inactiveValueToSet,
                        inactiveValueToSet);
                break;
            case EMAIL:
                AuditEventType.maybeAddPropertyChangeMessage(list,
                        "eventHandlers.emailRecipients",
                        createRecipientMessage(from.activeRecipients),
                        createRecipientMessage(activeRecipients));
                AuditEventType.maybeAddPropertyChangeMessage(list,
                        "eventHandlers.escal", from.sendEscalation, sendEscalation);
                AuditEventType.maybeAddPropertyChangeMessage(list,
                        "eventHandlers.escalPeriod",
                        from.escalationDelayType.getPeriodDescription(from.escalationDelay),
                        escalationDelayType.getPeriodDescription(escalationDelay));
                AuditEventType.maybeAddPropertyChangeMessage(list,
                        "eventHandlers.escalRecipients",
                        createRecipientMessage(from.escalationRecipients),
                        createRecipientMessage(escalationRecipients));
                AuditEventType.maybeAddPropertyChangeMessage(list,
                        "eventHandlers.inactiveNotif", from.sendInactive,
                        sendInactive);
                AuditEventType.maybeAddPropertyChangeMessage(list,
                        "eventHandlers.inactiveOverride", from.inactiveOverride,
                        inactiveOverride);
                AuditEventType.maybeAddPropertyChangeMessage(list,
                        "eventHandlers.inactiveRecipients",
                        createRecipientMessage(from.inactiveRecipients),
                        createRecipientMessage(inactiveRecipients));
                break;
            case PROCESS:
                AuditEventType.maybeAddPropertyChangeMessage(list,
                        "eventHandlers.activeCommand", from.activeProcessCommand,
                        activeProcessCommand);
                AuditEventType.maybeAddPropertyChangeMessage(list,
                        "eventHandlers.inactiveCommand",
                        from.inactiveProcessCommand, inactiveProcessCommand);
                break;
            case SCRIPT:
                AuditEventType.maybeAddPropertyChangeMessage(list,
                        "eventHandlers.activeCommand", from.activeScriptCommand,
                        activeScriptCommand);
                AuditEventType.maybeAddPropertyChangeMessage(list,
                        "eventHandlers.inactiveCommand",
                        from.inactiveScriptCommand, inactiveScriptCommand);
        }
    }

    private LocalizableMessage createRecipientMessage(
            List<RecipientListEntry> recipients) {
        ArrayList<LocalizableMessage> params = new ArrayList<>();
        for (RecipientListEntry recip : recipients) {
            LocalizableMessage msg;
            if (recip.getRecipientType() == EmailRecipient.TYPE_MAILING_LIST) {
                msg = new LocalizableMessageImpl("event.audit.recip.mailingList",
                        mailingListDao.getMailingList(recip.getReferenceId())
                        .getName());
            } else if (recip.getRecipientType() == EmailRecipient.TYPE_USER) {
                msg = new LocalizableMessageImpl("event.audit.recip.user", runtimeManager
                        .getNode(recip.getReferenceId()).getName(), UserRT.class);
            } else {
                msg = new LocalizableMessageImpl("event.audit.recip.address", recip
                        .getReferenceAddress());
            }
            params.add(msg);
        }

        return new LocalizableMessageImpl("event.audit.recip.list." + params.size(), params.toArray());
    }

    //
    // /
    // / Serialization
    // /
    //
    private static final long serialVersionUID = -1;
    private static final int version = 3;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);
        out.writeInt(handlerType.getId());
        out.writeBoolean(disabled);
        switch (handlerType) {
            case SET_POINT:
                out.writeInt(targetPointId);
                out.writeInt(activeAction.getId());
                SerializationHelper.writeSafeUTF(out, activeValueToSet);
                out.writeInt(activePointId);
                out.writeInt(inactiveAction.getId());
                SerializationHelper.writeSafeUTF(out, inactiveValueToSet);
                out.writeInt(inactivePointId);
                break;
            case EMAIL:
                out.writeObject(activeRecipients);
                out.writeBoolean(sendEscalation);
                out.writeInt(escalationDelayType.getId());
                out.writeInt(escalationDelay);
                out.writeObject(escalationRecipients);
                out.writeBoolean(sendInactive);
                out.writeBoolean(inactiveOverride);
                out.writeObject(inactiveRecipients);
                break;
            case PROCESS:
                SerializationHelper.writeSafeUTF(out, activeProcessCommand);
                SerializationHelper.writeSafeUTF(out, inactiveProcessCommand);
                break;
            case SCRIPT:
                out.writeInt(activeScriptCommand);
                out.writeInt(inactiveScriptCommand);
        }
    }

    @SuppressWarnings("unchecked")
    private void readObject(ObjectInputStream in) throws IOException,
            ClassNotFoundException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be
        // elegantly handled.
        switch (ver) {
            case 1:
                handlerType = Type.fromId(in.readInt());
                disabled = false;
                switch (handlerType) {
                    case SET_POINT:
                        targetPointId = in.readInt();
                        activeAction = SetActionType.fromId(in.readInt());
                        activeValueToSet = SerializationHelper.readSafeUTF(in);
                        activePointId = in.readInt();
                        inactiveAction = SetActionType.fromId(in.readInt());
                        inactiveValueToSet = SerializationHelper.readSafeUTF(in);
                        inactivePointId = in.readInt();
                        break;
                    case EMAIL:
                        activeRecipients = (List<RecipientListEntry>) in
                                .readObject();
                        sendEscalation = in.readBoolean();
                        escalationDelayType = TimePeriods.fromId(in.readInt());
                        escalationDelay = in.readInt();
                        escalationRecipients = (List<RecipientListEntry>) in
                                .readObject();
                        sendInactive = in.readBoolean();
                        inactiveOverride = false;
                        inactiveRecipients = new ArrayList<>();
                        break;
                    case PROCESS:
                        activeProcessCommand = SerializationHelper.readSafeUTF(in);
                        inactiveProcessCommand = SerializationHelper.readSafeUTF(in);
                }
                break;
            case 2:
                handlerType = Type.fromId(in.readInt());
                disabled = false;
                switch (handlerType) {
                    case SET_POINT:
                        targetPointId = in.readInt();
                        activeAction = SetActionType.fromId(in.readInt());
                        activeValueToSet = SerializationHelper.readSafeUTF(in);
                        activePointId = in.readInt();
                        inactiveAction = SetActionType.fromId(in.readInt());
                        inactiveValueToSet = SerializationHelper.readSafeUTF(in);
                        inactivePointId = in.readInt();
                        break;
                    case EMAIL:
                        activeRecipients = (List<RecipientListEntry>) in
                                .readObject();
                        sendEscalation = in.readBoolean();
                        escalationDelayType = TimePeriods.fromId(in.readInt());
                        escalationDelay = in.readInt();
                        escalationRecipients = (List<RecipientListEntry>) in
                                .readObject();
                        sendInactive = in.readBoolean();
                        inactiveOverride = in.readBoolean();
                        inactiveRecipients = (List<RecipientListEntry>) in
                                .readObject();
                        break;
                    case PROCESS:
                        activeProcessCommand = SerializationHelper.readSafeUTF(in);
                        inactiveProcessCommand = SerializationHelper.readSafeUTF(in);
                }
                break;
            case 3:
                handlerType = Type.fromId(in.readInt());
                disabled = in.readBoolean();
                switch (handlerType) {
                    case SET_POINT:
                        targetPointId = in.readInt();
                        activeAction = SetActionType.fromId(in.readInt());
                        activeValueToSet = SerializationHelper.readSafeUTF(in);
                        activePointId = in.readInt();
                        inactiveAction = SetActionType.fromId(in.readInt());
                        inactiveValueToSet = SerializationHelper.readSafeUTF(in);
                        inactivePointId = in.readInt();
                        break;
                    case EMAIL:
                        activeRecipients = (List<RecipientListEntry>) in
                                .readObject();
                        sendEscalation = in.readBoolean();
                        escalationDelayType = TimePeriods.fromId(in.readInt());
                        escalationDelay = in.readInt();
                        escalationRecipients = (List<RecipientListEntry>) in
                                .readObject();
                        sendInactive = in.readBoolean();
                        inactiveOverride = in.readBoolean();
                        inactiveRecipients = (List<RecipientListEntry>) in
                                .readObject();
                        break;
                    case PROCESS:
                        activeProcessCommand = SerializationHelper.readSafeUTF(in);
                        inactiveProcessCommand = SerializationHelper.readSafeUTF(in);
                        break;
                    case SCRIPT:
                        activeScriptCommand = in.readInt();
                        inactiveScriptCommand = in.readInt();
                }
        }
    }

    public void setActiveScriptCommand(int activeScriptCommand) {
        this.activeScriptCommand = activeScriptCommand;
    }

    public int getActiveScriptCommand() {
        return activeScriptCommand;
    }

    public void setInactiveScriptCommand(int inactiveScriptCommand) {
        this.inactiveScriptCommand = inactiveScriptCommand;
    }

    public int getInactiveScriptCommand() {
        return inactiveScriptCommand;
    }

    public boolean isNew() {
        return id == null;
    }
}
