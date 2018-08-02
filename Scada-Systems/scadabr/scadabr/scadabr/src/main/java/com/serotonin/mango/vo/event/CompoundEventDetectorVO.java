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

import br.org.scadabr.dao.DataPointDao;
import br.org.scadabr.utils.ImplementMeException;
import java.util.List;
import com.serotonin.mango.rt.event.compound.CompoundEventDetectorRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import br.org.scadabr.util.ChangeComparable;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.vo.event.type.CompoundEventKey;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.serotonin.mango.rt.event.type.CompoundDetectorEventType;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;
import org.springframework.validation.Errors;
import org.springframework.validation.Validator;

/**
 * @author Matthew Lohbihler
 */
public class CompoundEventDetectorVO implements ChangeComparable<CompoundEventDetectorVO> {

    @Configurable
    public static class CompoundEventDetectorVoValidator implements Validator {
        
    @Autowired
    private DataPointDao dataPointDao;

        
        
    @Override
    public boolean supports(Class<?> clazz) {
        return CompoundEventDetectorVO.class.isAssignableFrom(clazz);
    }

    @Override
    public void validate(Object target, Errors errors) {
        throw new ImplementMeException();
        /*
        final CompoundEventDetectorVO vo = (CompoundEventDetectorVO) target;
        
        if (vo.name.isEmpty()) {
            errors.rejectValue("name", "compoundDetectors.validation.nameRequired");
        }
        try {
            User user = Common.getUser();
            Permissions.ensureDataSourcePermission(user);

            LogicalOperator l = CompoundEventDetectorRT.parseConditionStatement(vo.condition);
            List<String> keys = l.getDetectorKeys();

            // Get all of the point event detectors.
            for (String key : keys) {
                if (!key.startsWith(EventDetectorVO.POINT_EVENT_DETECTOR_PREFIX)) {
                    continue;
                }

                boolean found = false;
                for (DataPointVO dp : dataPointDao.getDataPoints(true)) {
                    if (!Permissions.hasDataSourcePermission(user, dp.getDataSourceId())) {
                        continue;
                    }

                    for (PointEventDetectorVO ped : dp.getEventDetectors()) {
                        if (ped.getEventDetectorKey().equals(key) && ped.isStateful()) {
                            found = true;
                            break;
                        }
                    }

                    if (found) {
                        break;
                    }
                }

                if (!found) {
                    throw new ConditionParseException("compoundDetectors.validation.invalidKey");
                }
            }
        } catch (ConditionParseException e) {
            throw new ImplementMeException();
            /*
            response.addContextual("condition", e);
            if (e.isRange()) {
                response.addData("range", true);
                response.addData("from", e.getFrom());
                response.addData("to", e.getTo());
            }
            * /
        }
        */
    }


    }
    
    public static final String XID_PREFIX = "CED_";

    private Integer id;
    private String xid;

    private String name;
    private AlarmLevel alarmLevel = AlarmLevel.NONE;

    private boolean stateful = true;

    private boolean disabled = false;

    private String condition;
    private CompoundDetectorEventType compoundEventType;
    private CompoundEventKey compoundEventKey;

    public synchronized CompoundDetectorEventType getEventType() {
        if (compoundEventType == null) {
            compoundEventType = new CompoundDetectorEventType(this);
        }
        return compoundEventType;
    }

    @JsonIgnore
    public boolean isNew() {
        return id == null;
    }

    @Override
    public String getTypeKey() {
        return "event.audit.compoundEventDetector";
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "common.xid", xid);
        AuditEventType.addPropertyMessage(list, "compoundDetectors.name", name);
        AuditEventType.addPropertyMessage(list, "common.alarmLevel", alarmLevel.getI18nKey());
        AuditEventType.addPropertyMessage(list, "common.rtn", stateful);
        AuditEventType.addPropertyMessage(list, "common.disabled", disabled);
        AuditEventType.addPropertyMessage(list, "compoundDetectors.condition", condition);
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, CompoundEventDetectorVO from) {
        AuditEventType.maybeAddPropertyChangeMessage(list, "common.xid", from.xid, xid);
        AuditEventType.maybeAddPropertyChangeMessage(list, "compoundDetectors.name", from.name, name);
        AuditEventType.maybeAddPropertyChangeMessage(list, "common.alarmLevel", from.alarmLevel, alarmLevel);
        AuditEventType.maybeAddPropertyChangeMessage(list, "common.rtn", from.stateful, stateful);
        AuditEventType.maybeAddPropertyChangeMessage(list, "common.disabled", from.disabled, disabled);
        AuditEventType.maybeAddPropertyChangeMessage(list, "compoundDetectors.condition", from.condition, condition);
    }

    public CompoundEventDetectorRT createRuntime() {
        return new CompoundEventDetectorRT(this);
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

    public AlarmLevel getAlarmLevel() {
        return alarmLevel;
    }

    public void setAlarmLevel(AlarmLevel alarmLevel) {
        this.alarmLevel = alarmLevel;
    }

    public boolean isStateful() {
        return stateful;
    }

    public void setStateful(boolean stateful) {
        this.stateful = stateful;
    }

    public boolean isDisabled() {
        return disabled;
    }

    public void setDisabled(boolean disabled) {
        this.disabled = disabled;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getCondition() {
        return condition;
    }

    public void setCondition(String condition) {
        this.condition = condition;
    }

    /**
     * @return the compoundEventKey
     */
    public CompoundEventKey getCompoundEventKey() {
        return compoundEventKey;
    }

    /**
     * @param compoundEventKey the compoundEventKey to set
     */
    public void setCompoundEventKey(CompoundEventKey compoundEventKey) {
        this.compoundEventKey = compoundEventKey;
    }

}
