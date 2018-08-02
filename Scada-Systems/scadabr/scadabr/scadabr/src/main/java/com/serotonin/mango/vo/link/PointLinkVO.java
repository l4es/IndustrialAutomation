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
package com.serotonin.mango.vo.link;

import br.org.scadabr.ScadaBrConstants;
import br.org.scadabr.dao.DataPointDao;
import br.org.scadabr.utils.i18n.LocalizableEnum;
import java.util.List;

import com.serotonin.mango.rt.event.type.AuditEventType;
import br.org.scadabr.util.ChangeComparable;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import com.fasterxml.jackson.annotation.JsonIgnore;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;
import org.springframework.validation.Errors;
import org.springframework.validation.Validator;

/**
 * @author Matthew Lohbihler
 */
@Configurable
public class PointLinkVO implements ChangeComparable<PointLinkVO> {

    @Autowired
    private DataPointDao dataPointDao;

    @Configurable
    public static class PointLinkVoValidator implements Validator {

        @Autowired
        private DataPointDao dataPointDao;

        @Override
        public boolean supports(Class<?> clazz) {
            return PointLinkVO.class.isAssignableFrom(clazz);
        }

        @Override
        public void validate(Object target, Errors errors) {
            final PointLinkVO vo = (PointLinkVO) target;
            if (vo.sourcePointId == 0) {
                errors.rejectValue("sourcePointId", "pointLinks.validate.sourceRequired");
            }
            if (vo.targetPointId == 0) {
                errors.rejectValue("targetPointId", "pointLinks.validate.targetRequired");
            }
            if (vo.sourcePointId == vo.targetPointId) {
                errors.rejectValue("targetPointId", "pointLinks.validate.samePoint");
            }
        }

    }

    public static final String XID_PREFIX = "PL_";

    public enum EventType implements LocalizableEnum<EventType> {

        UPDATE(1, "pointLinks.event.update"),
        CHANGE(2, "pointLinks.event.change");
        private final Integer id;
        private final String i18nKey;

        private EventType(int id, String i18nKey) {
            this.id = id;
            this.i18nKey = i18nKey;
        }

        public Integer getId() {
            return id;
        }

        public static EventType fromId(int id) {
            switch (id) {
                case 1:
                    return UPDATE;
                case 2:
                    return CHANGE;
                default:
                    throw new IndexOutOfBoundsException("Cant get EventType from: " + id);
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

    private Integer id;
    private String xid;
    private int sourcePointId;
    private int targetPointId;

    private String script;
    private EventType event;

    private boolean disabled;

    @JsonIgnore
    public boolean isNew() {
        return id == null;
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

    public int getSourcePointId() {
        return sourcePointId;
    }

    public void setSourcePointId(int sourcePointId) {
        this.sourcePointId = sourcePointId;
    }

    public int getTargetPointId() {
        return targetPointId;
    }

    public void setTargetPointId(int targetPointId) {
        this.targetPointId = targetPointId;
    }

    public String getScript() {
        return script;
    }

    public void setScript(String script) {
        this.script = script;
    }

    public EventType getEvent() {
        return event;
    }

    public void setEvent(EventType event) {
        this.event = event;
    }

    public boolean isDisabled() {
        return disabled;
    }

    public void setDisabled(boolean disabled) {
        this.disabled = disabled;
    }

    @Override
    public String getTypeKey() {
        return "event.audit.pointLink";
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "common.xid", xid);
        AuditEventType.addPropertyMessage(list, "pointLinks.source", dataPointDao.getExtendedPointName(sourcePointId));
        AuditEventType.addPropertyMessage(list, "pointLinks.target", dataPointDao.getExtendedPointName(targetPointId));
        AuditEventType.addPropertyMessage(list, "pointLinks.script", script);
        AuditEventType.addPropertyMessage(list, "pointLinks.event", event);
        AuditEventType.addPropertyMessage(list, "common.disabled", disabled);
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, PointLinkVO from) {
        AuditEventType.maybeAddPropertyChangeMessage(list, "common.xid", from.xid, xid);
        AuditEventType
                .maybeAddPropertyChangeMessage(list, "pointLinks.source",
                        dataPointDao.getExtendedPointName(from.sourcePointId),
                        dataPointDao.getExtendedPointName(sourcePointId));
        AuditEventType
                .maybeAddPropertyChangeMessage(list, "pointLinks.target",
                        dataPointDao.getExtendedPointName(from.targetPointId),
                        dataPointDao.getExtendedPointName(targetPointId));
        AuditEventType.maybeAddPropertyChangeMessage(list, "pointLinks.script", from.script, script);
        AuditEventType.maybeAddPropertyChangeMessage(list, "pointLinks.event", from.event, event);
        AuditEventType.maybeAddPropertyChangeMessage(list, "common.disabled", from.disabled, disabled);
    }

}
