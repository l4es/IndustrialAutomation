/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.json;

import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.DeserializationContext;
import com.fasterxml.jackson.databind.JsonDeserializer;
import java.awt.Color;
import java.io.IOException;

/**
 *
 * @author aploese
 */
public class ColorDeserializer  extends JsonDeserializer<Color> {

        @Override
        public Class<Color> handledType() { return Color.class; }

        @Override
        public Color deserialize(JsonParser p, DeserializationContext ctxt) throws IOException, JsonProcessingException {
            String value = p.getValueAsString();
            if (value.charAt(0) == '#') {
                return new Color((int)Long.parseLong(value.substring(1), 16), true);
            } else {
                return new Color(Integer.parseInt(value), true);
            }
        }

    
}
