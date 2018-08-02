/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.util;

import br.org.scadabr.InvalidArgumentException;
import java.awt.Color;
import java.util.HashMap;
import java.util.Map;

/**
 *
 * @author aploese
 */
public class ColorUtils {

    public static Color toColor(String s) throws InvalidArgumentException {
        if (s.charAt(0) == '#') {
            return hexToColor(s);
        }
        Color c = colorMap.get(s.toLowerCase());
        if (c != null) {
            return c;
        }
        throw new InvalidArgumentException("Invalid color format: " + s);
    }

    public static String toHexString(String s)
            throws InvalidArgumentException {
        return toHexString(toColor(s));
    }

    public static String toHexString(Color color) {
        return String.format("#%02x%02x%02x", color.getRed(), color.getGreen(), color.getBlue());
    }

    private static Color hexToColor(String s)
            throws InvalidArgumentException {
        String rrggbb;
        if (s.length() == 4) {
            rrggbb = String.format("%0$c%0$c%1$c%1$c%2$c%2$c" , s.charAt(1), s.charAt(1), s.charAt(3));
        } else {
            if (s.length() == 7) {
                rrggbb = s.substring(1, 7);
            } else {
                throw new InvalidArgumentException("Invalid # color format: " + s);
            }
        }
        try {
            return new Color(Integer.parseInt(rrggbb, 16));
        } catch (NumberFormatException e) {
            throw new InvalidArgumentException("Error parsing color value: " + s + ", " + e.getMessage());
        }
    }

    private final static Map<String, Color> colorMap = new HashMap<>();

    static {
        colorMap.put("aliceblue", new Color(0xF0F8FF));
        colorMap.put("antiquewhite", new Color(0xFAEBD7));
        colorMap.put("aqua", new Color(0x00FFFF));
        colorMap.put("aquamarine", new Color(0x7FFFD4));
        colorMap.put("azure", new Color(0xF0FFFF));
        colorMap.put("beige", new Color(0xF5F5DC));
        colorMap.put("bisque", new Color(0xFFE4C4));
        colorMap.put("black", new Color(0x000000));
        colorMap.put("blanchedalmond", new Color(0xFFEBCD));
        colorMap.put("blue", new Color(0x0000FF));
        colorMap.put("blueviolet", new Color(0x8A2BE2));
        colorMap.put("brown", new Color(0xA52A2A));
        colorMap.put("burlywood", new Color(0xDEB887));
        colorMap.put("cadetblue", new Color(0x5F9EA0));
        colorMap.put("chartreuse", new Color(0x7FFF00));
        colorMap.put("chocolate", new Color(0xD2691E));
        colorMap.put("coral", new Color(0xFF7F50));
        colorMap.put("cornflowerblue", new Color(0x6495ED));
        colorMap.put("cornsilk", new Color(0xFFF8DC));
        colorMap.put("crimson", new Color(0xDC143C));
        colorMap.put("cyan", new Color(0x00FFFF));
        colorMap.put("darkblue", new Color(0x00008B));
        colorMap.put("darkcyan", new Color(0x008B8B));
        colorMap.put("darkgoldenrod", new Color(0xB8860B));
        colorMap.put("darkgray", new Color(0xA9A9A9));
        colorMap.put("darkgreen", new Color(0x006400));
        colorMap.put("darkkhaki", new Color(0xBDB76B));
        colorMap.put("darkmagenta", new Color(0x8B008B));
        colorMap.put("darkolivegreen", new Color(0x556B2F));
        colorMap.put("darkorange", new Color(0xFF8C00));
        colorMap.put("darkorchid", new Color(0x9932CC));
        colorMap.put("darkred", new Color(0x8B0000));
        colorMap.put("darksalmon", new Color(0xE9967A));
        colorMap.put("darkseagreen", new Color(0x8FBC8F));
        colorMap.put("darkslateblue", new Color(0x483D8B));
        colorMap.put("darkslategray", new Color(0x2F4F4F));
        colorMap.put("darkturquoise", new Color(0x00CED1));
        colorMap.put("darkviolet", new Color(0x9400D3));
        colorMap.put("deeppink", new Color(0xFF1493));
        colorMap.put("deepskyblue", new Color(0x00BFFF));
        colorMap.put("dimgray", new Color(0x696969));
        colorMap.put("dodgerblue", new Color(0x1E90FF));
        colorMap.put("firebrick", new Color(0xB22222));
        colorMap.put("floralwhite", new Color(0xFFFAF0));
        colorMap.put("forestgreen", new Color(0x228B22));
        colorMap.put("fuchsia", new Color(0xFF00FF));
        colorMap.put("gainsboro", new Color(0xDCDCDC));
        colorMap.put("ghostwhite", new Color(0xF8F8FF));
        colorMap.put("gold", new Color(0xFFD700));
        colorMap.put("goldenrod", new Color(0xDAA520));
        colorMap.put("gray", new Color(0x808080));
        colorMap.put("green", new Color(0x008000));
        colorMap.put("greenyellow", new Color(0xADFF2F));
        colorMap.put("honeydew", new Color(0xF0FFF0));
        colorMap.put("hotpink", new Color(0xFF69B4));
        colorMap.put("indianred ", new Color(0xCD5C5C));
        colorMap.put("indigo  ", new Color(0x4B0082));
        colorMap.put("ivory", new Color(0xFFFFF0));
        colorMap.put("khaki", new Color(0xF0E68C));
        colorMap.put("lavender", new Color(0xE6E6FA));
        colorMap.put("lavenderblush", new Color(0xFFF0F5));
        colorMap.put("lawngreen", new Color(0x7CFC00));
        colorMap.put("lemonchiffon", new Color(0xFFFACD));
        colorMap.put("lightblue", new Color(0xADD8E6));
        colorMap.put("lightcoral", new Color(0xF08080));
        colorMap.put("lightcyan", new Color(0xE0FFFF));
        colorMap.put("lightgoldenrodyellow", new Color(0xFAFAD2));
        colorMap.put("lightgrey", new Color(0xD3D3D3));
        colorMap.put("lightgreen", new Color(0x90EE90));
        colorMap.put("lightpink", new Color(0xFFB6C1));
        colorMap.put("lightsalmon", new Color(0xFFA07A));
        colorMap.put("lightseagreen", new Color(0x20B2AA));
        colorMap.put("lightskyblue", new Color(0x87CEFA));
        colorMap.put("lightslategray", new Color(0x778899));
        colorMap.put("lightsteelblue", new Color(0xB0C4DE));
        colorMap.put("lightyellow", new Color(0xFFFFE0));
        colorMap.put("lime", new Color(0x00FF00));
        colorMap.put("limegreen", new Color(0x32CD32));
        colorMap.put("linen", new Color(0xFAF0E6));
        colorMap.put("magenta", new Color(0xFF00FF));
        colorMap.put("maroon", new Color(0x800000));
        colorMap.put("mediumaquamarine", new Color(0x66CDAA));
        colorMap.put("mediumblue", new Color(0x0000CD));
        colorMap.put("mediumorchid", new Color(0xBA55D3));
        colorMap.put("mediumpurple", new Color(0x9370D8));
        colorMap.put("mediumseagreen", new Color(0x3CB371));
        colorMap.put("mediumslateblue", new Color(0x7B68EE));
        colorMap.put("mediumspringgreen", new Color(0x00FA9A));
        colorMap.put("mediumturquoise", new Color(0x48D1CC));
        colorMap.put("mediumvioletred", new Color(0xC71585));
        colorMap.put("midnightblue", new Color(0x191970));
        colorMap.put("mintcream", new Color(0xF5FFFA));
        colorMap.put("mistyrose", new Color(0xFFE4E1));
        colorMap.put("moccasin", new Color(0xFFE4B5));
        colorMap.put("navajowhite", new Color(0xFFDEAD));
        colorMap.put("navy", new Color(0x000080));
        colorMap.put("oldlace", new Color(0xFDF5E6));
        colorMap.put("olive", new Color(0x808000));
        colorMap.put("olivedrab", new Color(0x6B8E23));
        colorMap.put("orange", new Color(0xFFA500));
        colorMap.put("orangered", new Color(0xFF4500));
        colorMap.put("orchid", new Color(0xDA70D6));
        colorMap.put("palegoldenrod", new Color(0xEEE8AA));
        colorMap.put("palegreen", new Color(0x98FB98));
        colorMap.put("paleturquoise", new Color(0xAFEEEE));
        colorMap.put("palevioletred", new Color(0xD87093));
        colorMap.put("papayawhip", new Color(0xFFEFD5));
        colorMap.put("peachpuff", new Color(0xFFDAB9));
        colorMap.put("peru", new Color(0xCD853F));
        colorMap.put("pink", new Color(0xFFC0CB));
        colorMap.put("plum", new Color(0xDDA0DD));
        colorMap.put("powderblue", new Color(0xB0E0E6));
        colorMap.put("purple", new Color(0x800080));
        colorMap.put("red", new Color(0xFF0000));
        colorMap.put("rosybrown", new Color(0xBC8F8F));
        colorMap.put("royalblue", new Color(0x4169E1));
        colorMap.put("saddlebrown", new Color(0x8B4513));
        colorMap.put("salmon", new Color(0xFA8072));
        colorMap.put("sandybrown", new Color(0xF4A460));
        colorMap.put("seagreen", new Color(0x2E8B57));
        colorMap.put("seashell", new Color(0xFFF5EE));
        colorMap.put("sienna", new Color(0xA0522D));
        colorMap.put("silver", new Color(0xC0C0C0));
        colorMap.put("skyblue", new Color(0x87CEEB));
        colorMap.put("slateblue", new Color(0x6A5ACD));
        colorMap.put("slategray", new Color(0x708090));
        colorMap.put("snow", new Color(0xFFFAFA));
        colorMap.put("springgreen", new Color(0x00FF7F));
        colorMap.put("steelblue", new Color(0x4682B4));
        colorMap.put("tan", new Color(0xD2B48C));
        colorMap.put("teal", new Color(0x008080));
        colorMap.put("thistle", new Color(0xD8BFD8));
        colorMap.put("tomato", new Color(0xFF6347));
        colorMap.put("turquoise", new Color(0x40E0D0));
        colorMap.put("violet", new Color(0xEE82EE));
        colorMap.put("wheat", new Color(0xF5DEB3));
        colorMap.put("white", new Color(0xFFFFFF));
        colorMap.put("whitesmoke", new Color(0xF5F5F5));
        colorMap.put("yellow", new Color(0xFFFF00));
        colorMap.put("yellowgreen", new Color(0x9ACD32));
    }
}
