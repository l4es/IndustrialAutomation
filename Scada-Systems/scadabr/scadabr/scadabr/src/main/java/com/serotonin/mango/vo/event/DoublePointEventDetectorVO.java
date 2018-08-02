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

import java.util.List;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.utils.TimePeriods;
import br.org.scadabr.vo.event.AlarmLevel;
import com.serotonin.mango.rt.event.detectors.AlphanumericStateDetectorRT;
import com.serotonin.mango.rt.event.detectors.AnalogHighLimitDetectorRT;
import com.serotonin.mango.rt.event.detectors.AnalogLowLimitDetectorRT;
import com.serotonin.mango.rt.event.detectors.BinaryStateDetectorRT;
import com.serotonin.mango.rt.event.detectors.MultistateStateDetectorRT;
import com.serotonin.mango.rt.event.detectors.NegativeCusumDetectorRT;
import com.serotonin.mango.rt.event.detectors.NoChangeDetectorRT;
import com.serotonin.mango.rt.event.detectors.NoUpdateDetectorRT;
import com.serotonin.mango.rt.event.detectors.PointChangeDetectorRT;
import com.serotonin.mango.rt.event.detectors.PointEventDetectorRT;
import com.serotonin.mango.rt.event.detectors.DoublePositiveCusumDetectorRT;
import com.serotonin.mango.rt.event.detectors.StateChangeCountDetectorRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import br.org.scadabr.util.ChangeComparable;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.event.type.DataPointDetectorKey;
import com.serotonin.mango.rt.event.type.DataPointEventType;
import com.serotonin.mango.vo.DoubleDataPointVO;

public class DoublePointEventDetectorVO implements EventDetectorVO, Cloneable, ChangeComparable<DoublePointEventDetectorVO> {

    public static final String XID_PREFIX = "PED_";

    private Integer id;
    private String xid;

    private String alias;
    private DoubleDataPointVO dataPoint;
    private DataPointDetectorKey dataPointDetectorKey;
    private AlarmLevel alarmLevel;
    private double limit;
    private int duration;
    private TimePeriods durationType = TimePeriods.SECONDS;
    private boolean binaryState;
    private int multistateState;
    private int changeCount = 2;
    private String alphanumericState;
    private double weight;
    private DataPointEventType dataPointEventType;

    public synchronized DataPointEventType getEventType() {
        if (dataPointEventType == null) {
            dataPointEventType = new DataPointEventType(this);
        }
        return dataPointEventType;
    }

    public PointEventDetectorRT createRuntime() {
        switch (dataPointDetectorKey) {
            case ANALOG_HIGH_LIMIT:
                return new AnalogHighLimitDetectorRT(this);
            case ANALOG_LOW_LIMIT:
                return new AnalogLowLimitDetectorRT(this);
            case BINARY_STATE:
                return new BinaryStateDetectorRT(this);
            case MULTISTATE_STATE:
                return new MultistateStateDetectorRT(this);
            case POINT_CHANGE:
                return new PointChangeDetectorRT(this);
            case STATE_CHANGE_COUNT:
                return new StateChangeCountDetectorRT(this);
            case NO_CHANGE:
                return new NoChangeDetectorRT(this);
            case NO_UPDATE:
                return new NoUpdateDetectorRT(this);
            case ALPHANUMERIC_STATE:
                return new AlphanumericStateDetectorRT(this);
            case POSITIVE_CUSUM:
                return new DoublePositiveCusumDetectorRT(this);
            case NEGATIVE_CUSUM:
                return new NegativeCusumDetectorRT(this);
        }
        throw new ShouldNeverHappenException("Unknown detector type: " + dataPointDetectorKey);
    }

    @Override
    public String getEventDetectorKey() {
        return EventDetectorVO.POINT_EVENT_DETECTOR_PREFIX + id;
    }

    public LocalizableMessage getDescription() {
        if (!alias.isEmpty()) {
            return new LocalizableMessageImpl("common.default", alias);
        }
        return getConfigurationDescription();
    }

    private LocalizableMessage getConfigurationDescription() {
        throw new ImplementMeException(); 
        /* TODO half localized data ... haow to handle this ????
        LocalizableMessage durationDesc = getDurationDescription();
        switch (dataPointDetectorKey) {
            case ANALOG_HIGH_LIMIT:
                if (durationDesc == null) {
                    return new LocalizableMessageImpl("event.detectorVo.highLimit", dataPoint.getTextRenderer().getText(
                            limit, TextRenderer.HINT_SPECIFIC));
                } else {
                    return new LocalizableMessageImpl("event.detectorVo.highLimitPeriod", dataPoint.getTextRenderer()
                            .getText(limit, TextRenderer.HINT_SPECIFIC), durationDesc);
                }
            case ANALOG_LOW_LIMIT:
                if (durationDesc == null) {
                    return new LocalizableMessageImpl("event.detectorVo.lowLimit", dataPoint.getTextRenderer().getText(
                            limit, TextRenderer.HINT_SPECIFIC));
                } else {
                    return new LocalizableMessageImpl("event.detectorVo.lowLimitPeriod", dataPoint.getTextRenderer()
                            .getText(limit, TextRenderer.HINT_SPECIFIC), durationDesc);
                }
            case BINARY_STATE:
                if (durationDesc == null) {
                    return new LocalizableMessageImpl("event.detectorVo.state", dataPoint.getTextRenderer().getText(
                            binaryState, TextRenderer.HINT_SPECIFIC));
                } else {
                    return new LocalizableMessageImpl("event.detectorVo.statePeriod", dataPoint.getTextRenderer().getText(
                            binaryState, TextRenderer.HINT_SPECIFIC), durationDesc);
                }
            case MULTISTATE_STATE:
                if (durationDesc == null) {
                    return new LocalizableMessageImpl("event.detectorVo.state", dataPoint.getTextRenderer().getText(
                            multistateState, TextRenderer.HINT_SPECIFIC));
                } else {
                    return new LocalizableMessageImpl("event.detectorVo.statePeriod", dataPoint.getTextRenderer().getText(
                            multistateState, TextRenderer.HINT_SPECIFIC), durationDesc);
                }
            case POINT_CHANGE:
                return new LocalizableMessageImpl("event.detectorVo.change");
            case STATE_CHANGE_COUNT:
                return new LocalizableMessageImpl("event.detectorVo.changeCount", changeCount, durationDesc);
            case NO_CHANGE:
                return new LocalizableMessageImpl("event.detectorVo.noChange", durationDesc);
            case NO_UPDATE:
                return new LocalizableMessageImpl("event.detectorVo.noUpdate", durationDesc);
            case ALPHANUMERIC_STATE:
                if (durationDesc == null) {
                    return new LocalizableMessageImpl("event.detectorVo.state", dataPoint.getTextRenderer().getText(
                            alphanumericState, TextRenderer.HINT_SPECIFIC));
                } else {
                    return new LocalizableMessageImpl("event.detectorVo.statePeriod", dataPoint.getTextRenderer().getText(
                            alphanumericState, TextRenderer.HINT_SPECIFIC), durationDesc);
                }
            case POSITIVE_CUSUM:
                if (durationDesc == null) {
                    return new LocalizableMessageImpl("event.detectorVo.posCusum", dataPoint.getTextRenderer().getText(
                            limit, TextRenderer.HINT_SPECIFIC));
                } else {
                    return new LocalizableMessageImpl("event.detectorVo.posCusumPeriod", dataPoint.getTextRenderer()
                            .getText(limit, TextRenderer.HINT_SPECIFIC), durationDesc);
                }
            case NEGATIVE_CUSUM:
                if (durationDesc == null) {
                    return new LocalizableMessageImpl("event.detectorVo.negCusum", dataPoint.getTextRenderer().getText(
                            limit, TextRenderer.HINT_SPECIFIC));
                } else {
                    return new LocalizableMessageImpl("event.detectorVo.negCusumPeriod", dataPoint.getTextRenderer()
                            .getText(limit, TextRenderer.HINT_SPECIFIC), durationDesc);
                }
            default:
                throw new ShouldNeverHappenException("Unknown detector type: " + dataPointDetectorKey);
        }
        */
    }

    public LocalizableMessage getDurationDescription() {
        if (duration == 0) {
            return null;
        }
        return durationType.getPeriodDescription(duration);
    }

    public DoublePointEventDetectorVO copy() {
        try {
            return (DoublePointEventDetectorVO) super.clone();
        } catch (CloneNotSupportedException e) {
            throw new ShouldNeverHappenException(e);
        }
    }

    @Override
    public String getTypeKey() {
        return "event.audit.pointEventDetector";
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "common.xid", xid);
        AuditEventType.addPropertyMessage(list, "pointEdit.detectors.alias", alias);
        AuditEventType.addPropertyMessage(list, "pointEdit.detectors.type", dataPointDetectorKey);
        AuditEventType.addPropertyMessage(list, "common.alarmLevel", alarmLevel.getI18nKey());
        AuditEventType.addPropertyMessage(list, "common.configuration", getConfigurationDescription());
        AuditEventType.addPropertyMessage(list, "pointEdit.detectors.weight", weight);
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, DoublePointEventDetectorVO from) {
        AuditEventType.maybeAddPropertyChangeMessage(list, "common.xid", from.xid, xid);
        AuditEventType.maybeAddPropertyChangeMessage(list, "pointEdit.detectors.alias", from.alias, alias);
        if (from.dataPointDetectorKey != dataPointDetectorKey) {
            AuditEventType.addPropertyChangeMessage(list, "pointEdit.detectors.type", from.dataPointDetectorKey,
                    dataPointDetectorKey);
        }
        AuditEventType.maybeAddPropertyChangeMessage(list, "common.alarmLevel", from.alarmLevel, alarmLevel);
        if (from.limit != limit || from.duration != duration || from.durationType != durationType
                || from.binaryState != binaryState || from.multistateState != multistateState
                || from.changeCount != changeCount || from.alphanumericState != alphanumericState) {
            AuditEventType.maybeAddPropertyChangeMessage(list, "common.configuration", from
                    .getConfigurationDescription(), getConfigurationDescription());
        }
        AuditEventType.maybeAddPropertyChangeMessage(list, "pointEdit.detectors.weight", from.weight, weight);
    }

    public DoubleDataPointVO njbGetDataPoint() {
        return dataPoint;
    }

    public void njbSetDataPoint(DoubleDataPointVO dataPoint) {
        this.dataPoint = dataPoint;
    }

    public AlarmLevel getAlarmLevel() {
        return alarmLevel;
    }

    public void setAlarmLevel(AlarmLevel alarmLevel) {
        this.alarmLevel = alarmLevel;
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

    public boolean isBinaryState() {
        return binaryState;
    }

    public void setBinaryState(boolean binaryState) {
        this.binaryState = binaryState;
    }

    public int getChangeCount() {
        return changeCount;
    }

    public void setChangeCount(int changeCount) {
        this.changeCount = changeCount;
    }

    public int getDuration() {
        return duration;
    }

    public void setDuration(int duration) {
        this.duration = duration;
    }

    public TimePeriods getDurationType() {
        return durationType;
    }

    public void setDurationType(TimePeriods durationType) {
        this.durationType = durationType;
    }

    public double getLimit() {
        return limit;
    }

    public void setLimit(double limit) {
        this.limit = limit;
    }

    public int getMultistateState() {
        return multistateState;
    }

    public void setMultistateState(int multistateState) {
        this.multistateState = multistateState;
    }

    public String getAlphanumericState() {
        return alphanumericState;
    }

    public void setAlphanumericState(String alphanumericState) {
        this.alphanumericState = alphanumericState;
    }

    public double getWeight() {
        return weight;
    }

    public void setWeight(double weight) {
        this.weight = weight;
    }

    /**
     * @return the dataPointDetectorKey
     */
    public DataPointDetectorKey getDataPointDetectorKey() {
        return dataPointDetectorKey;
    }

    /**
     * @param dataPointDetectorKey the dataPointDetectorKey to set
     */
    public void setDataPointDetectorKey(DataPointDetectorKey dataPointDetectorKey) {
        this.dataPointDetectorKey = dataPointDetectorKey;
    }

    boolean isStateful() {
        throw new ImplementMeException();
    }

}
