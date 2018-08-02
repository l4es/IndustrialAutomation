package br.org.scadabr;

import br.org.scadabr.utils.i18n.LocalizableEnum;

public enum DataType implements LocalizableEnum<DataType> {

    UNKNOWN("common.unknown"),
    BOOLEAN("common.dataTypes.boolean"),
    MULTISTATE("common.dataTypes.multistate"),
    DOUBLE("common.dataTypes.double"),
    ALPHANUMERIC("common.dataTypes.alphanumeric"),
    IMAGE("common.dataTypes.image");
    private final String i18nKey;
    
    private DataType(String i18nKey) {
        this.i18nKey = i18nKey;
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
