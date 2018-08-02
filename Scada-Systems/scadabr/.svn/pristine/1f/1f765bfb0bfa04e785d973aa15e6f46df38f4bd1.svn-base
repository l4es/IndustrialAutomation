/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.timer.cron;

import java.util.Arrays;

/**
 *
 * @author aploese
 */
public class CronField {

    CronField setValue(int value) {
        values[0] = value;
        return this;
    }

    int getValue() {
        return values[0];
    }

    int getStart() {
        return values[0];
    }

    int getEnd() {
        return values[1];
    }

    int getIncrement() {
        return values[2];
    }

    CronField setStartRange(int value) {
        values[0] = value;
        return this;
    }

    CronField setEndRange(int value) {
        values[1] = value;
        return this;
    }

    CronField setIncrement(int value) {
        values[2] = value;
        return this;
    }

    void addChild(CronField cf) {
        if (childFields[1] == null) {
            childFields[1] = cf;
            return;
        }
        childFields = Arrays.copyOf(childFields, childFields.length + 1);
        childFields[childFields.length - 1] = cf;
    }

    CronField[] getChildren() {
        return childFields;
    }

    String toDayOfMonth(int i) {
        switch (i) {
            case 1:
                return "JAN";
            case 2:
                return "FEB";
            case 3:
                return "MAR";
            case 4:
                return "APR";
            case 5:
                return "MAY";
            case 6:
                return "JUN";
            case 7:
                return "JUL";
            case 8:
                return "AUG";
            case 9:
                return "SEP";
            case 10:
                return "OCT";
            case 11:
                return "NOV";
            case 12:
                return "DEC";
            default:
                throw new RuntimeException();
        }
    }

    String getMonthValue() {
        return toDayOfMonth(getValue());
    }

    String getMonthStart() {
        return toDayOfMonth(getStart());
    }

    String getMonthEnd() {
        return toDayOfMonth(getEnd());
    }

    String getDayOfWeekValue() {
        return toDayOfWeek(getValue());
    }

    String getDayOfWeekStart() {
        return toDayOfWeek(getStart());
    }

    String getDayOfWeekEnd() {
        return toDayOfWeek(getEnd());
    }

    private String toDayOfWeek(int value) {
        switch (value) {
            case 0:
                return "SUN";
            case 1:
                return "MON";
            case 2:
                return "TUE";
            case 3:
                return "WED";
            case 4:
                return "THU";
            case 5:
                return "FRI";
            case 6:
                return "SAT";
            default:
                throw new RuntimeException();
        }
    }

    enum Type {

        ANY,
        RANGE_INCREMENT,
        VALUE,
        CHILD_FIELDS;

    }

    final Type type;
    private int[] values;
    private CronField[] childFields;

    public CronField(CronField cf) {
        this(Type.CHILD_FIELDS);
        childFields[0] = cf;
    }

    public CronField(Type type) {
        this.type = type;
        switch (type) {
            case ANY:
                values = null;
                return;
            case RANGE_INCREMENT:
                values = new int[3];
                values[2] = 1;
                return;
            case VALUE:
                values = new int[1];
                return;
            case CHILD_FIELDS:
                childFields = new CronField[2];
                return;
            default:
                throw new RuntimeException();
        }
    }

}
