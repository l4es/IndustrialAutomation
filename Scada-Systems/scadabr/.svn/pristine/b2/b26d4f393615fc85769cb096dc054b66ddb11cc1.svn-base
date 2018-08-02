/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.json;

import com.fasterxml.jackson.core.JsonGenerator;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonSerializer;
import com.fasterxml.jackson.databind.SerializerProvider;
import java.awt.Color;
import java.io.IOException;

/**
 *
 * @author aploese
 */
public class ColorSerializer  extends JsonSerializer<Color> {

        @Override
        public Class<Color> handledType() { return Color.class; }

        @Override
        public void serialize(Color value, JsonGenerator gen, SerializerProvider serializers) throws IOException, JsonProcessingException {
            gen.writeString(String.format("#%08x", value.getRGB()));
        }

}
