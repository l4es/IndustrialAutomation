/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.event.type;

import br.org.scadabr.DataType;
import br.org.scadabr.rt.event.type.DuplicateHandling;
import br.org.scadabr.vo.event.AlarmLevel;
import java.util.EnumSet;

/**
 *
 * @author aploese
 */
public enum DataPointDetectorKey implements EventKey<DataPointDetectorKey> {

    ANALOG_HIGH_LIMIT(1, "pointEdit.detectors.highLimit", EnumSet.of(DataType.DOUBLE)),
    ANALOG_LOW_LIMIT(2, "pointEdit.detectors.lowLimit", EnumSet.of(DataType.DOUBLE)),
    BINARY_STATE(3, "pointEdit.detectors.state", EnumSet.of(DataType.BOOLEAN)),
    MULTISTATE_STATE(4, "pointEdit.detectors.state", EnumSet.of(DataType.MULTISTATE)),
    POINT_CHANGE(5, "pointEdit.detectors.change", EnumSet.of(DataType.BOOLEAN, DataType.MULTISTATE, DataType.DOUBLE, DataType.ALPHANUMERIC)) {
                @Override
                public boolean isStateful() {
                    return false;
                }

                @Override
                public DuplicateHandling getDuplicateHandling() {
                    return DuplicateHandling.ALLOW;
                }

            },
    STATE_CHANGE_COUNT(6, "pointEdit.detectors.changeCount", EnumSet.of(DataType.BOOLEAN, DataType.MULTISTATE, DataType.ALPHANUMERIC)),
    NO_CHANGE(7, "pointEdit.detectors.noChange", EnumSet.of(DataType.BOOLEAN, DataType.MULTISTATE, DataType.DOUBLE, DataType.ALPHANUMERIC)),
    NO_UPDATE(8, "pointEdit.detectors.noUpdate", EnumSet.of(DataType.BOOLEAN, DataType.MULTISTATE, DataType.DOUBLE, DataType.ALPHANUMERIC, DataType.IMAGE)),
    ALPHANUMERIC_STATE(9, "pointEdit.detectors.state", EnumSet.of(DataType.ALPHANUMERIC)),
    POSITIVE_CUSUM(10, "pointEdit.detectors.posCusum", EnumSet.of(DataType.DOUBLE)),
    NEGATIVE_CUSUM(11, "pointEdit.detectors.negCusum", EnumSet.of(DataType.DOUBLE));

    public static DataPointDetectorKey fromId(int id) {
        switch (id) {
            case 1:
                return ANALOG_HIGH_LIMIT;
            case 2:
                return ANALOG_LOW_LIMIT;
            case 3:
                return BINARY_STATE;
            case 4:
                return MULTISTATE_STATE;
            case 5:
                return POINT_CHANGE;
            case 6:
                return STATE_CHANGE_COUNT;
            case 7:
                return NO_CHANGE;
            case 8:
                return NO_UPDATE;
            case 9:
                return ALPHANUMERIC_STATE;
            case 10:
                return POSITIVE_CUSUM;
            case 11:
                return NEGATIVE_CUSUM;
            default:
                throw new IndexOutOfBoundsException("No such id: " + id);
        }
    }
    private final int id;
    private final String i18nKey;
    private final EnumSet<DataType> dataTypes;

    private DataPointDetectorKey(int id, String i18nKey, EnumSet<DataType> dataTypes) {
        this.id = id;
        this.i18nKey = i18nKey;
        this.dataTypes = dataTypes;
    }

    @Override
    public int getId() {
        return id;
    }

    @Override
    public DuplicateHandling getDuplicateHandling() {
        return DuplicateHandling.IGNORE;
    }

    @Override
    public AlarmLevel getDefaultAlarmLevel() {
        return AlarmLevel.NONE;
    }

    @Override
    public boolean isStateful() {
        return true;
    }

    @Override
    public String getI18nKey() {
        return i18nKey;
    }

    @Override
    public Object[] getArgs() {
        return null;
    }

    public boolean supports(DataType dataType) {
        return dataTypes.contains(dataType);
    }

    @Override
    public String getName() {
        return name();
    }

}
