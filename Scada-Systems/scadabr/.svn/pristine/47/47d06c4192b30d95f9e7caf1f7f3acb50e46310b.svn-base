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
package com.serotonin.mango.vo;

import br.org.scadabr.DataType;
import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.utils.TimePeriods;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.vo.AbstractVO;
import br.org.scadabr.vo.IntervalLoggingTypes;
import br.org.scadabr.vo.LoggingTypes;
import br.org.scadabr.vo.NodeType;
import br.org.scadabr.vo.datapoints.DataPointNodeVO;
import br.org.scadabr.vo.datasource.PointLocatorVO;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.vo.event.DoublePointEventDetectorVO;
import java.io.Serializable;
import java.util.EnumSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import javax.validation.ValidationException;
import org.springframework.beans.factory.annotation.Configurable;
import org.springframework.validation.Errors;
import org.springframework.validation.Validator;

public abstract class DataPointVO<T extends DataPointVO<T, P>, P extends PointValueTime> 
        extends AbstractVO<T>
        implements DataPointNodeVO<T>, Serializable, Cloneable {

    public static DataPointVO create(DataType dataType) {
        switch (dataType) {
            case DOUBLE:
                return new DoubleDataPointVO();
            default:
                throw new ImplementMeException();
        }
    }
    
    public static Class<? extends DataPointVO> getVoClass(String dataType) {
        return getVoClass(DataType.valueOf(dataType));
    }
    
    public static Class<? extends DataPointVO> getVoClass(DataType dataType) {
        switch (dataType) {
            case DOUBLE:
                return DoubleDataPointVO.class;
            default:
                //TODO implement proper!
                return DoubleDataPointVO.class;
        }
    }
    

    public DataPointVO() {
    }

    public abstract DataPointRT<T, P> createRT(PointLocatorVO<P> pointLocatorVO);

    /**
     * @return the valuePattern
     */
    public String getValuePattern() {
        return valuePattern;
    }

    /**
     * @return the valuePattern for the specific PointValueTime
     */
    public String getValuePattern(P pvt) {
        return valuePattern;
    }

    /**
     * @param valuePattern the valuePattern to set
     */
    public void setValuePattern(String valuePattern) {
        this.valuePattern = valuePattern;
    }

    /**
     * @return the valueAndUnitPattern
     */
    public String getValueAndUnitPattern() {
        return valueAndUnitPattern;
    }

    /**
     * @param pvt
     * @return the valueAndUnitPattern for the specific PointValueTime. The
     * first param ('{0}') is the value, the second ('{1}') is the unit
     */
    public String getValueAndUnitPattern(P pvt) {
        return valueAndUnitPattern;
    }

    /**
     * @param valueAndUnitPattern the valueAndUnitPattern to set
     */
    public void setValueAndUnitPattern(String valueAndUnitPattern) {
        this.valueAndUnitPattern = valueAndUnitPattern;
    }

    /**
     * @return the unit
     */
    public String getUnit() {
        return unit;
    }

    /**
     * @param unit the unit to set
     */
    public void setUnit(String unit) {
        this.unit = unit;
    }

    /**
     * @return the pointLocatorId
     */
    public Integer getPointLocatorId() {
        return pointLocatorId;
    }

    /**
     * @param pointLocatorId the pointLocatorId to set
     */
    public void setPointLocatorId(Integer pointLocatorId) {
        this.pointLocatorId = pointLocatorId;
    }

    @JsonIgnore
    public boolean isEvtDetectorsEmpty() {
        return eventDetectors == null ? true : eventDetectors.isEmpty();
    }

    @Configurable
    public static class DataPointVoValidator implements Validator {

        @Override
        public boolean supports(Class<?> clazz) {
            return DataPointVO.class.isAssignableFrom(clazz);
        }

        @Override
        public void validate(Object target, Errors errors) {
            final DataPointVO vo = (DataPointVO) target;

//TODO            if (vo.name.isEmpty()) {
//                errors.rejectValue("name", "validate.required");
//            }

            if (vo.intervalLoggingPeriod <= 0) {
                errors.rejectValue("intervalLoggingPeriod", "validate.greaterThanZero");
            }

            if (vo.purgePeriod <= 0) {
                errors.rejectValue("purgePeriod", "validate.greaterThanZero");
            }

        }

    }

    public abstract DataType getDataType();
    
    public void setDataType(DataType dataType) {
        if (!getDataType().equals(dataType)) {
            throw new ValidationException("DataType mismatch");
        }
    }

    public static final Set<TimePeriods> PURGE_TYPES = EnumSet.of(TimePeriods.DAYS, TimePeriods.WEEKS, TimePeriods.MONTHS, TimePeriods.YEARS);

    //
    //
    // Properties
    //

    private LoggingTypes loggingType = LoggingTypes.ALL;
    private TimePeriods intervalLoggingPeriodType = TimePeriods.MINUTES;

    private int intervalLoggingPeriod = 15;
    private IntervalLoggingTypes intervalLoggingType = IntervalLoggingTypes.INSTANT;

    private TimePeriods _purgeType = TimePeriods.YEARS;

    private int purgePeriod = 1;
    private List<DoublePointEventDetectorVO> eventDetectors;
    private List<UserComment> comments;

    private Integer pointLocatorId;

    private String valuePattern;
    private String valueAndUnitPattern;
    private String unit;

    //
    //
    // Runtime data
    //
    /*
     * This is used by the watch list and graphic views to cache the last known value for a point to determine if the
     * browser side needs to be refreshed. Initially set to this value so that point views will update (since null
     * values in this case do in fact equal each other).
     */
    //TODO use null ...
    private P lastValue;

    public DataPointVO(String valuePattern, String valueAndUnitPattern) {
        this.valuePattern = valuePattern;
        this.valueAndUnitPattern = valueAndUnitPattern;
    }

    public void resetLastValue() {
        lastValue = null;
    }

    public P lastValue() {
        return lastValue;
    }

    public void updateLastValue(P pvt) {
        lastValue = pvt;
    }

    /*
     * This value is used by the watchlists. It is set when the watchlist is loaded to determine if the user is allowed
     * to set the point or not based upon various conditions.
     */
    private boolean settable;

    public boolean isSettable() {
        return settable;
    }

    public void setSettable(boolean settable) {
        this.settable = settable;
    }

    @JsonIgnore
    @Override
    public String getTypeKey() {
        return "event.audit.dataPoint";
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
//TODO        AuditEventType.addPropertyMessage(list, "dsEdit.points.name", name);
        AuditEventType.addPropertyMessage(list, "pointEdit.logging.type", loggingType);
        AuditEventType.addPropertyMessage(list, "pointEdit.logging.period", intervalLoggingPeriodType.getPeriodDescription(intervalLoggingPeriod));
        AuditEventType.addPropertyMessage(list, "pointEdit.logging.valueType", intervalLoggingType);
        AuditEventType.addPropertyMessage(list, "pointEdit.logging.purge", _purgeType.getPeriodDescription(purgePeriod));
        AuditEventType.addPropertyMessage(list, "pointEdit.unit", unit);
        AuditEventType.addPropertyMessage(list, "pointEdit.valuePattern", valuePattern);
        AuditEventType.addPropertyMessage(list, "pointEdit.valueAndUnitPattern", valueAndUnitPattern);
    }

//TODO    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, DataPointVO<T, P> from) {
//TODO        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.points.name", from.name, name);
        AuditEventType.maybeAddPropertyChangeMessage(list, "pointEdit.logging.type", from.loggingType, loggingType);
        AuditEventType.maybeAddPropertyChangeMessage(list, "pointEdit.logging.period",
                from.intervalLoggingPeriodType.getPeriod(from.intervalLoggingPeriod),
                intervalLoggingPeriodType.getPeriod(intervalLoggingPeriod));
        AuditEventType.maybeAddPropertyChangeMessage(list, "pointEdit.logging.valueType", from.intervalLoggingType, intervalLoggingType);
        AuditEventType.maybeAddPropertyChangeMessage(list, "pointEdit.logging.purge", from._purgeType.getPeriodDescription(from.purgePeriod), _purgeType.getPeriodDescription(purgePeriod));

        AuditEventType.maybeAddPropertyChangeMessage(list, "pointEdit.unit", from.unit, unit);
        AuditEventType.maybeAddPropertyChangeMessage(list, "pointEdit.valuePattern", from.valuePattern, valuePattern);
        AuditEventType.maybeAddPropertyChangeMessage(list, "pointEdit.valueAndUnitPattern", from.valuePattern, valuePattern);
    }

    public LoggingTypes getLoggingType() {
        return loggingType;
    }

    public void setLoggingType(LoggingTypes loggingType) {
        this.loggingType = loggingType;
    }

    public int getPurgePeriod() {
        return purgePeriod;
    }

    public void setPurgePeriod(int purgePeriod) {
        this.purgePeriod = purgePeriod;
    }

    public TimePeriods getPurgeType() {
        return _purgeType;
    }

    public void setPurgeType(TimePeriods purgeType) {
        this._purgeType = purgeType;
    }

    public List<DoublePointEventDetectorVO> getEventDetectors() {
        return eventDetectors;
    }

    public void setEventDetectors(List<DoublePointEventDetectorVO> eventDetectors) {
        this.eventDetectors = eventDetectors;
    }

    public List<UserComment> getComments() {
        return comments;
    }

    public void setComments(List<UserComment> comments) {
        this.comments = comments;
    }

    public TimePeriods getIntervalLoggingPeriodType() {
        return intervalLoggingPeriodType;
    }

    public void setIntervalLoggingPeriodType(TimePeriods intervalLoggingPeriodType) {
        this.intervalLoggingPeriodType = intervalLoggingPeriodType;
    }

    public int getIntervalLoggingPeriod() {
        return intervalLoggingPeriod;
    }

    public void setIntervalLoggingPeriod(int intervalLoggingPeriod) {
        this.intervalLoggingPeriod = intervalLoggingPeriod;
    }

    public IntervalLoggingTypes getIntervalLoggingType() {
        return intervalLoggingType;
    }

    public void setIntervalLoggingType(IntervalLoggingTypes intervalLoggingType) {
        this.intervalLoggingType = intervalLoggingType;
    }

    public DataPointVO copy() {
        try {
            return (DataPointVO) super.clone();
        } catch (CloneNotSupportedException e) {
            throw new ShouldNeverHappenException(e);
        }
    }

    @Override
    public String toString() {
        return "DataPointVO{" + "id=" + getId() + ", name=" + getName() + ", loggingType=" + loggingType + ", intervalLoggingPeriodType=" + intervalLoggingPeriodType + ", intervalLoggingPeriod=" + intervalLoggingPeriod + ", intervalLoggingType=" + intervalLoggingType + ", _purgeType=" + _purgeType + ", purgePeriod=" + purgePeriod + ", eventDetectors=" + eventDetectors + ", comments=" + comments + ", pointLocatorId=" + pointLocatorId + ", valuePattern=" + valuePattern + ", valueAndUnitPattern=" + valueAndUnitPattern + ", unit=" + unit + ", lastValue=" + lastValue + ", settable=" + settable + '}';
    }

    @Override
    public int hashCode() {
        int hash = 7;
        hash = 13 * hash + Objects.hashCode(this.getName());
        return hash;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final DataPointVO<?, ?> other = (DataPointVO<?, ?>) obj;
        if (!Objects.equals(this.getName(), other.getName())) {
            return false;
        }
        return true;
    }

    @Override
    public NodeType getNodeType() {
        return NodeType.DATA_POINT;
    }

}
