package br.org.scadabr.util;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

/**
 * TODO wrap Object*Stream and read write fields like O*S "name dataType value"...???
 * 
 * @author aploese
 */
@Deprecated
public class SerializationHelper {

    public static Object readObject(InputStream is) {
        try (ObjectInputStream ois = new ScadaBrObjectInputStream(is)) {
            return ois.readObject();
        } catch (IOException | ClassNotFoundException e) {
            throw new RuntimeException(e);
        }
    }

    public static InputStream writeObject(Object data) {
        return new ByteArrayInputStream(writeObjectToArray(data));
    }

    @Deprecated // Write String as Object ...
    public static void writeSafeUTF(ObjectOutputStream out, String str) throws IOException {
        if (str == null) {
            out.writeBoolean(false);
        } else {
            out.writeBoolean(true);
            out.writeUTF(str);
        }
    }

    @Deprecated // Write String as Object ...
    public static String readSafeUTF(ObjectInputStream in) throws IOException {
        if (in.readBoolean()) {
            return in.readUTF();
        } else {
            return null;
        }
    }

    public static byte[] writeObjectToArray(Object o) {
        try (ByteArrayOutputStream bos = new ByteArrayOutputStream(4096)) {
            try (ObjectOutputStream oos = new ObjectOutputStream(bos)) {
                oos.writeObject(o);
                return bos.toByteArray();
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

}
