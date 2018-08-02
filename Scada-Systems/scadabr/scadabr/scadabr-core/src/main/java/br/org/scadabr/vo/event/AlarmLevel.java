package br.org.scadabr.vo.event;

import br.org.scadabr.utils.i18n.LocalizableEnum;

public enum AlarmLevel implements LocalizableEnum<AlarmLevel> {

    //do not reorder!!
    NONE("common.alarmLevel.none"),
    INFORMATION("common.alarmLevel.info"),
    URGENT("common.alarmLevel.urgent"),
    CRITICAL("common.alarmLevel.critical"),
    LIFE_SAFETY("common.alarmLevel.lifeSafety");
    private final String i18nKey;

    private AlarmLevel(String i18nKey) {
        this.i18nKey = i18nKey;
    }

    @Deprecated //For JSON Export ...
    public static String nameValues() {
        final AlarmLevel[] values = values();
        StringBuilder result = new StringBuilder();
        for (AlarmLevel value : values) {
            result.append(value.getName());
            result.append(" ");
        }
        return result.toString();
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

    public boolean otherIsHigher(AlarmLevel o) {
        return this.ordinal() < o.ordinal();
    }

    public boolean otherIsLower(AlarmLevel o) {
        return this.ordinal() > o.ordinal();
    }

    public boolean meIsHigher(AlarmLevel o) {
        return this.ordinal() > o.ordinal();
    }

    public boolean meIsLower(AlarmLevel o) {
        return this.ordinal() < o.ordinal();
    }

}
