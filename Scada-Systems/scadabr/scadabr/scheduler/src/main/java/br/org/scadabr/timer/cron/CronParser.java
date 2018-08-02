/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.timer.cron;

import java.text.ParseException;
import java.util.TimeZone;

/**
 *
 * @author aploese
 */
public class CronParser {

    private int sbToValue() {
        switch (currentField) {
            case DAY_OF_WEEK:
                switch (sb.toString()) {
                    case "SUN":
                        return 0;
                    case "MON":
                        return 1;
                    case "THE":
                        return 2;
                    case "WED":
                        return 3;
                    case "THU":
                        return 4;
                    case "FRI":
                        return 5;
                    case "SAT":
                        return 6;
                    default:
                        throw new RuntimeException("Cant convert to day of week: " + sb.toString());
                }
            case MONTH:
                switch (sb.toString()) {
                    case "JAN":
                        return 1;
                    case "FEB":
                        return 2;
                    case "MAR":
                        return 3;
                    case "APR":
                        return 4;
                    case "MAY":
                        return 5;
                    case "JUN":
                        return 6;
                    case "JUL":
                        return 7;
                    case "AUG":
                        return 8;
                    case "SEP":
                        return 9;
                    case "OCT":
                        return 10;
                    case "NOV":
                        return 11;
                    case "DEC":
                        return 12;
                    default:
                        throw new RuntimeException("Cant convert to month: " + sb.toString());
                }
            default:
                return Integer.parseInt(sb.toString());
        }
    }

    enum ParseState {

        NONE,
        COLLECTING,
        ANY,
        WAIT_FOR_RANGE_END,
        WAIT_FOR_INCREMENT;
    }

    private CronExpression result;
    private StringBuilder sb;
    private CronFieldType currentField;
    private ParseState state;
    private CronField currentCombinedField;

    public CronExpression parse(final String cron, TimeZone tz) throws ParseException {
        result = new CronExpression(tz);
        sb = new StringBuilder();
        currentField = CronFieldType.MILLISECOND;
        state = ParseState.NONE;
        CronField currentBasicField = null;
        currentCombinedField = null;
        int pos = -1;
        for (char c : cron.toCharArray()) {
            pos++;
            switch (Character.toUpperCase(c)) {
                case '*':
                    state = ParseState.ANY;
                    break;
                case ',':
                    switch (state) {
                        case WAIT_FOR_RANGE_END:
                            currentBasicField.setEndRange(sbToValue());
                            sb.setLength(0);
                            if (currentCombinedField == null) {
                                currentCombinedField = new CronField(currentBasicField);
                            } else {
                                currentCombinedField.addChild(currentBasicField);
                            }
                            currentBasicField = null;
                            break;
                        case WAIT_FOR_INCREMENT:
                            currentBasicField.setIncrement(Integer.parseInt(sb.toString()));
                            sb.setLength(0);
                            sb.setLength(0);
                            if (currentCombinedField == null) {
                                currentCombinedField = new CronField(currentBasicField);
                            } else {
                                currentCombinedField.addChild(currentBasicField);
                            }
                            currentBasicField = null;
                            break;
                        case COLLECTING:
                            currentCombinedField = new CronField(new CronField(CronField.Type.VALUE).setValue(sbToValue()));
                            sb.setLength(0);
                            break;
                        default:
                            throw new RuntimeException();
                    }
                    state = ParseState.NONE;
                    break;
                case '-':
                    if (state != ParseState.COLLECTING) {
                        throw new RuntimeException();
                    }
                    if (currentBasicField != null) {
                        throw new RuntimeException();
                    }
                    currentBasicField = new CronField(CronField.Type.RANGE_INCREMENT).setStartRange(sbToValue());
                    sb.setLength(0);
                    state = ParseState.WAIT_FOR_RANGE_END;
                    break;
                case '/':
                    switch (state) {
                        case ANY:
                            currentBasicField = new CronField(CronField.Type.RANGE_INCREMENT).setStartRange(currentField.floor).setEndRange(currentField.ceil);
                            state = ParseState.WAIT_FOR_INCREMENT;
                            break;
                        case WAIT_FOR_RANGE_END:
                            currentBasicField.setEndRange(sbToValue());
                            sb.setLength(0);
                            state = ParseState.WAIT_FOR_INCREMENT;
                            break;
                        default:
                            throw new RuntimeException();
                    }
                    break;
                case ' ':
                case '\t':
                    fieldParsed(currentBasicField);
                    currentBasicField = null;
                    switch (currentField) {
                        case MILLISECOND:
                            currentField = CronFieldType.SECOND;
                            break;
                        case SECOND:
                            currentField = CronFieldType.MINUTE;
                            break;
                        case MINUTE:
                            currentField = CronFieldType.HOUR_OF_DAY;
                            break;
                        case HOUR_OF_DAY:
                            currentField = CronFieldType.DAY_OF_MONTH;
                            break;
                        case DAY_OF_MONTH:
                            currentField = CronFieldType.MONTH;
                            break;
                        case MONTH:
                            currentField = CronFieldType.DAY_OF_WEEK;
                            break;
                        case DAY_OF_WEEK:
                            currentField = CronFieldType.YEAR;
                            break;
                        case YEAR:
                            throw new RuntimeException("Cron expression too long");
                        default:
                            throw new RuntimeException();
                    }
                    break;
                default:
                    if (state == ParseState.NONE) {
                        state = ParseState.COLLECTING;
                    }
                    sb.append(c);

            }
        }

        if (currentField != CronFieldType.YEAR) {
            throw new RuntimeException("Expression too short, last field: " + currentField);
        }
        fieldParsed(currentBasicField);
        currentBasicField = null;
        return result;
    }

    private void fieldParsed(CronField cronField) throws RuntimeException {
        switch (state) {
            case WAIT_FOR_RANGE_END:
                cronField.setEndRange(sbToValue());
                sb.setLength(0);
                addToResult(cronField);
                break;
            case WAIT_FOR_INCREMENT:
                cronField.setIncrement(Integer.parseInt(sb.toString()));
                sb.setLength(0);
                addToResult(cronField);
                break;
            case COLLECTING:
                CronField cf = new CronField(CronField.Type.VALUE).setValue(sbToValue());
                sb.setLength(0);
                addToResult(cf);
                break;
            case ANY:
                if ((currentCombinedField != null) || (cronField != null)) {
                    throw new RuntimeException();
                }
                result.setField(currentField, new CronField(CronField.Type.ANY));
                break;
            default:
                throw new RuntimeException("Field parsed, and state = " + state + " Dont know where to go");
        }
        state = ParseState.NONE;
    }

    private void addToResult(CronField cronField) {
        if (currentCombinedField != null) {
            currentCombinedField.addChild(cronField);
            result.setField(currentField, currentCombinedField);
            currentCombinedField = null;
        } else {
            result.setField(currentField, cronField);
        }
    }

}
