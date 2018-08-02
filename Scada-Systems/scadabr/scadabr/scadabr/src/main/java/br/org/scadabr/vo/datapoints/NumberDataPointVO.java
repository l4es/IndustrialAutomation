/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.datapoints;

import br.org.scadabr.json.ColorDeserializer;
import br.org.scadabr.json.ColorSerializer;
import br.org.scadabr.utils.TimePeriods;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import com.fasterxml.jackson.databind.annotation.JsonSerialize;
import com.serotonin.mango.rt.dataImage.NumberValueTime;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.vo.DataPointVO;
import java.awt.Color;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;

/**
 *
 * @author aploese
 * @param <T>
 * @param <P>
 */
public abstract class NumberDataPointVO<T extends NumberDataPointVO<T, P>, P extends NumberValueTime> extends DataPointVO<T, P> {

    private TimePeriods chartTimePeriods = TimePeriods.DAYS;
    private int numberOfChartPeriods = 1;
    
    private Color chartColor = Color.BLACK;

    public NumberDataPointVO(String valuePattern, String valueAndUnitPattern) {
        super(valuePattern, valueAndUnitPattern);
    }

    /**
     * @return the chartTimePeriods
     */
    public TimePeriods getChartTimePeriods() {
        return chartTimePeriods;
    }

    /**
     * @param chartTimePeriods the chartTimePeriods to set
     */
    public void setChartTimePeriods(TimePeriods chartTimePeriods) {
        this.chartTimePeriods = chartTimePeriods;
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        super.addProperties(list);
        AuditEventType.addPropertyMessage(list, "pointEdit.chart.timePeriods", chartTimePeriods.getPeriodDescription(numberOfChartPeriods));
        AuditEventType.addPropertyMessage(list, "pointEdit.chart.color", chartColor);
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, T from) {
        final NumberDataPointVO<T, P> numberDp = from;
        super.addPropertyChanges(list, numberDp);

        AuditEventType.maybeAddPropertyChangeMessage(list, "pointEdit.chart.timePeriods", numberDp.chartTimePeriods.getPeriodDescription(numberOfChartPeriods), chartTimePeriods.getPeriodDescription(numberOfChartPeriods));
        AuditEventType.maybeAddPropertyChangeMessage(list, "pointEdit.chart.color", numberDp.chartColor, chartColor);
    }

    //
    //
    // Serialization
    //
    private static final int version = 1;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);
        out.writeObject(chartTimePeriods);
        out.writeInt(numberOfChartPeriods);
        out.writeObject(chartColor);
    }

    private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        switch (ver) {
            case 1:
                chartTimePeriods = (TimePeriods) in.readObject();
                numberOfChartPeriods = in.readInt();
                chartColor = (Color) (in.readObject());
        }
    }

    /**
     * @return the numberOfChartPeriods
     */
    public int getNumberOfChartPeriods() {
        return numberOfChartPeriods;
    }

    /**
     * @param numberOfChartPeriods the numberOfChartPeriods to set
     */
    public void setNumberOfChartPeriods(int numberOfChartPeriods) {
        this.numberOfChartPeriods = numberOfChartPeriods;
    }

    /**
     * @return the chartColor
     */
    @JsonSerialize(using = ColorSerializer.class)
    public Color getChartColor() {
        return chartColor;
    }

    /**
     * @param chartColor the chartColor to set
     */
    @JsonDeserialize(using = ColorDeserializer.class)
    public void setChartColor(Color chartColor) {
        this.chartColor = chartColor;
    }
    
    public String getChartColorHtml() {
        return String.format("#%06x", chartColor.getRGB() & 0x00FFFFFF);
    }

    public void setChartColorHtml(String htmlHex) {
        chartColor = new Color((int)Long.parseLong(htmlHex.substring(1), 16));
    }
}
