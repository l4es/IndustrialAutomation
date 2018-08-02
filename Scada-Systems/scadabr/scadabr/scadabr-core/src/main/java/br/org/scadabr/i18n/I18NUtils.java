/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.i18n;

import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import java.util.LinkedList;

/**
 *
 * @author aploese
 */
public class I18NUtils {

    public static String serialize(LocalizableMessage msg) {
        StringBuilder sb = new StringBuilder();
        sb.append(msg.getI18nKey()).append('|');
        if (msg.getArgs() == null) {
            sb.append('|');
            return sb.toString();
        }
        for (Object arg : msg.getArgs()) {
            if (arg != null) {
                if (arg instanceof String) {
                    writeString(sb, (String) arg);
                } else if (arg instanceof LocalizableMessage) {
                    sb.append('[');
                    sb.append(serialize((LocalizableMessage) arg));
                    sb.append(']');
                } else if (arg instanceof Boolean) {
                    sb.append("\\Z");
                    sb.append(arg.toString());
                } else if (arg instanceof Byte) {
                    sb.append("\\B");
                    sb.append(arg.toString());
                } else if (arg instanceof Short) {
                    sb.append("\\S");
                    sb.append(arg.toString());
                } else if (arg instanceof Integer) {
                    sb.append("\\I");
                    sb.append(arg.toString());
                } else if (arg instanceof Long) {
                    sb.append("\\J");
                    sb.append(arg.toString());
                } else if (arg instanceof Float) {
                    sb.append("\\F");
                    sb.append(arg.toString());
                } else if (arg instanceof Double) {
                    sb.append("\\D");
                    sb.append(arg.toString());
                } else {
                    writeString(sb, arg.toString());
                }
            }
            sb.append('|');
        }

        return sb.toString();
    }

    private static void writeString(StringBuilder sb, String s) {
        for (int i = 0; i < s.length(); i++) {
            switch (s.charAt(i)) {
                case '\\':
                    sb.append("\\\\");
                    break;
                case '[':
                    sb.append("\\[");
                    break;
                case ']':
                    sb.append("\\]");
                    break;
                case '|':
                    sb.append("\\|");
                    break;
                default:
                    sb.append(s.charAt(i));
            }
        }

    }

    enum DecoderState {

        PARSE_KEY,
        PARSE_ARG,
        PARSE_ARG_NEXT_CHAR_ESCAPED,
        LOCALIZED_MESSAGE_PARSED;
    }

    static class Decoder {

        LinkedList<String> i18nKeyStack = new LinkedList<>();
        LinkedList<LinkedList<Object>> argsStack = new LinkedList<>();
        LocalizableMessage message;
        char TypeChar = ' ';

        DecoderState state;
        StringBuilder sb = new StringBuilder();

        public Decoder() {
            init();
        }

        final void init() {
            i18nKeyStack.clear();
            argsStack.clear();
            state = DecoderState.PARSE_KEY;
            sb.delete(0, sb.length());
            TypeChar = ' ';
        }

        private void addChar(final char c) {
            switch (c) {
                case '\\':
                    switch (state) {
                        case PARSE_KEY:
                            throw new RuntimeException("i18nKey contains \\  giving up");
                        case PARSE_ARG:
                            state = DecoderState.PARSE_ARG_NEXT_CHAR_ESCAPED;
                            break;
                        case PARSE_ARG_NEXT_CHAR_ESCAPED:
                            sb.append(c);
                            state = DecoderState.PARSE_ARG;
                            break;
                        default:
                            sb.append(c);
                    }
                    break;
                case '|':
                    switch (state) {
                        case PARSE_KEY:
                            i18nKeyStack.add(sb.toString());
                            argsStack.add(new LinkedList<>());
                            sb.delete(0, sb.length());
                            state = DecoderState.PARSE_ARG;
                            break;
                        case PARSE_ARG_NEXT_CHAR_ESCAPED:
                            sb.append(c);
                            state = DecoderState.PARSE_ARG;
                            break;
                        case PARSE_ARG:
                            if (sb.length() == 0) {
                                argsStack.getLast().add(null);
                            } else {
                                argsStack.getLast().add(toObject(sb.toString()));
                                TypeChar = ' ';
                                sb.delete(0, sb.length());
                            }
                            break;
                        case LOCALIZED_MESSAGE_PARSED:
                            state = DecoderState.PARSE_ARG;
                            break;
                        default:
                            throw new RuntimeException("cant handle parsing state");
                    }
                    break;
                case 'Z':
                    switch (state) {
                        case PARSE_KEY:
                            sb.append(c);
                            break;
                        case PARSE_ARG_NEXT_CHAR_ESCAPED:
                            if (sb.length() == 0) {
                                TypeChar = c;
                            } else {
                                sb.append(c);
                            }
                            state = DecoderState.PARSE_ARG;
                            break;
                        case PARSE_ARG:
                            sb.append(c);
                            break;
                        default:
                            throw new RuntimeException();
                    }
                    break;
                case 'B':
                    switch (state) {
                        case PARSE_KEY:
                            sb.append(c);
                            break;
                        case PARSE_ARG_NEXT_CHAR_ESCAPED:
                            if (sb.length() == 0) {
                                TypeChar = c;
                            } else {
                                sb.append(c);
                            }
                            state = DecoderState.PARSE_ARG;
                            break;
                        case PARSE_ARG:
                            sb.append(c);
                            break;
                        default:
                            throw new RuntimeException();
                    }
                    break;
                case 'S':
                    switch (state) {
                        case PARSE_KEY:
                            sb.append(c);
                            break;
                        case PARSE_ARG_NEXT_CHAR_ESCAPED:
                            if (sb.length() == 0) {
                                TypeChar = c;
                            } else {
                                sb.append(c);
                            }
                            state = DecoderState.PARSE_ARG;
                            break;
                        case PARSE_ARG:
                            sb.append(c);
                            break;
                        default:
                            throw new RuntimeException();
                    }
                    break;
                case 'I':
                    switch (state) {
                        case PARSE_KEY:
                            sb.append(c);
                            break;
                        case PARSE_ARG_NEXT_CHAR_ESCAPED:
                            if (sb.length() == 0) {
                                TypeChar = c;
                            } else {
                                sb.append(c);
                            }
                            state = DecoderState.PARSE_ARG;
                            break;
                        case PARSE_ARG:
                            sb.append(c);
                            break;
                        default:
                            throw new RuntimeException();
                    }
                    break;
                case 'J':
                    switch (state) {
                        case PARSE_KEY:
                            sb.append(c);
                            break;
                        case PARSE_ARG_NEXT_CHAR_ESCAPED:
                            if (sb.length() == 0) {
                                TypeChar = c;
                            } else {
                                sb.append(c);
                            }
                            state = DecoderState.PARSE_ARG;
                            break;
                        case PARSE_ARG:
                            sb.append(c);
                            break;
                        default:
                            throw new RuntimeException();
                    }
                    break;
                case 'F':
                    switch (state) {
                        case PARSE_KEY:
                            sb.append(c);
                            break;
                        case PARSE_ARG_NEXT_CHAR_ESCAPED:
                            if (sb.length() == 0) {
                                TypeChar = c;
                            } else {
                                sb.append(c);
                            }
                            state = DecoderState.PARSE_ARG;
                            break;
                        case PARSE_ARG:
                            sb.append(c);
                            break;
                        default:
                            throw new RuntimeException();
                    }
                    break;
                case 'D':
                    switch (state) {
                        case PARSE_KEY:
                            sb.append(c);
                            break;
                        case PARSE_ARG_NEXT_CHAR_ESCAPED:
                            if (sb.length() == 0) {
                                TypeChar = c;
                            } else {
                                sb.append(c);
                            }
                            state = DecoderState.PARSE_ARG;
                            break;
                        case PARSE_ARG:
                            sb.append(c);
                            break;
                        default:
                            throw new RuntimeException();
                    }
                    break;
                case '[':
                    switch (state) {
                        case PARSE_KEY:
                            throw new RuntimeException("i18nKey contains \\  giving up");
                        case PARSE_ARG_NEXT_CHAR_ESCAPED:
                            sb.append(c);
                            state = DecoderState.PARSE_ARG;
                            break;
                        case PARSE_ARG:
                            state = DecoderState.PARSE_KEY;
                            break;
                        default:
                            throw new RuntimeException();
                    }
                    break;
                case ']':
                    switch (state) {
                        case PARSE_KEY:
                            throw new RuntimeException("i18nKey contains \\  giving up");
                        case PARSE_ARG_NEXT_CHAR_ESCAPED:
                            sb.append(c);
                            state = DecoderState.PARSE_ARG;
                            break;
                        case PARSE_ARG:
                            LocalizableMessage l = new LocalizableMessageImpl(i18nKeyStack.removeLast(), argsStack.removeLast().toArray());
                            argsStack.getLast().add(l);
                            state = DecoderState.LOCALIZED_MESSAGE_PARSED;
                            break;
                        default:
                            throw new RuntimeException();
                    }
                    break;

                default:
                    sb.append(c);
            }
        }

        private Object toObject(String arg) {
            switch (TypeChar) {
                case ' ':
                    return arg;
                case 'Z':
                    return Boolean.valueOf(arg);
                case 'B':
                    try {
                        return Byte.parseByte(arg);
                    } catch (NumberFormatException nfe) {
                        return arg;
                    }
                case 'S':
                    try {
                        return Short.parseShort(arg);
                    } catch (NumberFormatException nfe) {
                        return arg;
                    }
                case 'I':
                    try {
                        return Integer.parseInt(arg);
                    } catch (NumberFormatException nfe) {
                        return arg;
                    }
                case 'J':
                    try {
                        return Long.parseLong(arg);
                    } catch (NumberFormatException nfe) {
                        return arg;
                    }
                case 'F':
                    try {
                        return Float.parseFloat(arg);
                    } catch (NumberFormatException nfe) {
                        return arg;
                    }
                case 'D':
                    try {
                        return Double.parseDouble(arg);
                    } catch (NumberFormatException nfe) {
                        return arg;
                    }
                default:
                    return arg;
            }
        }

        public LocalizableMessage getMessage() {
            if (message == null) {
                if (i18nKeyStack.size() != 1) {
                    throw new RuntimeException();
                }
                if (argsStack.size() != 1) {
                    throw new RuntimeException();
                }
                message = new LocalizableMessageImpl(i18nKeyStack.getFirst(), argsStack.getFirst().toArray());
            }
            return message;
        }
    }

    public static LocalizableMessage deserialize(String serialized) throws LocalizableMessageParseException {
        Decoder d = new Decoder();

        for (int i = 0; i < serialized.length(); i++) {
            d.addChar(serialized.charAt(i));
        }
        return d.getMessage();
    }

}
