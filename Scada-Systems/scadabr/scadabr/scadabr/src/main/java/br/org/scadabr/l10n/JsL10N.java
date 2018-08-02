/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.l10n;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.Writer;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.LinkedList;
import java.util.List;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author aploese
 */
public class JsL10N {

    private final List<String> locales = new LinkedList<>();
    private boolean prependSemi;

    public void initBase(File nlsDir) throws FileNotFoundException, IOException, URISyntaxException {
        final Properties messages = new Properties();
        final File msg = new File(JsL10N.class.getResource("/messages.properties").toURI());
        final File localeFile = new File(nlsDir, "messages.js");
        if (msg.lastModified() < localeFile.lastModified()) {
            return;
        }

        try (InputStream is = new FileInputStream(msg)) {
            messages.load(is);
            try (Writer fw = new FileWriter(localeFile, false)) {
                fw.write("define({\n");
                fw.write("root: {\n");
                prependSemi = false;
                messages.forEach((k, v) -> {
//                System.out.printf("%s => %s ", k, v);
                    try {
                        if (prependSemi) {
                            fw.write(",\n\"");
                        } else {
                            fw.write('\"');
                            prependSemi = true;
                        }
                        fw.write((String) k);
                        fw.write("\":\"");
                        fw.write(((String) v).replaceAll("\"", "\\\\\""));
                        fw.write('\"');
                    } catch (IOException ex) {
                        throw new RuntimeException(ex);
                    }
                });

                fw.write("\n},\n");
                prependSemi = false;
                for (String s : locales) {
                    try {
                        if (prependSemi) {
                            fw.write(",\n");
                        } else {
                            prependSemi = true;
                        }
                        fw.write(s);
                        fw.write(":true");
                    } catch (IOException ioe) {
                        throw new RuntimeException(ioe);
                    };
                }
                fw.write("\n});");
            }
        }
    }

    public void init(File nlsDir, String localeName) throws IOException, URISyntaxException {
        final Properties messages = new Properties();
        final File msg = new File(JsL10N.class.getResource("/messages_" + localeName + ".properties").toURI());
        final File localeDir = new File(nlsDir, localeName);
        localeDir.mkdir();
        final File localeFile = new File(localeDir, "messages.js");
        if (msg.lastModified() < localeFile.lastModified()) {
            return;
        }

        try (InputStream is = new FileInputStream(msg)) {
            messages.load(is);
            try (Writer fw = new FileWriter(localeFile, false)) {
                fw.write("define({\n");
                prependSemi = false;
                messages.forEach((k, v) -> {
//                System.out.printf("%s => %s ", k, v);
                    try {
                        if (prependSemi) {
                            fw.write(",\n\"");
                        } else {
                            fw.write('\"');
                            prependSemi = true;
                        }
                        fw.write((String) k);
                        fw.write("\":\"");
                        fw.write(((String) v).replaceAll("\"", "\\\\\""));
                        fw.write('\"');
                    } catch (IOException ex) {
                        throw new RuntimeException(ex);
                    }
                });
                fw.write("\n});");
                locales.add(localeName);
            }
        }
    }

    public void init(URL url) throws FileNotFoundException, IOException {
        final File basedir;
        try {
            basedir = new File(url.toURI());
        } catch (URISyntaxException ex) {
            throw new RuntimeException(ex);
        }
        final File resourcesDir = new File(basedir, "resources");
        final File scadabrResDir = new File(resourcesDir, "scadabr");
        final File nlsDir = new File(scadabrResDir, "nls");
        nlsDir.mkdir();
        final Properties i18nPropertiers = new Properties();
        i18nPropertiers.load(JsL10N.class.getResourceAsStream("/i18n.properties"));
        i18nPropertiers.forEach((k, v) -> {
            try {
                init(nlsDir, (String) k);
            } catch (IOException | URISyntaxException ex) {
                throw new RuntimeException(ex);
            }
        });
        try {
            initBase(nlsDir);
        } catch (URISyntaxException ex) {
            throw new RuntimeException(ex);
        }
    }

}
