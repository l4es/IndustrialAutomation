/*
 Mango - Open Source M2M - http://mango.serotoninsoftware.com
 Copyright (C) 2006-2011 Serotonin Software Technologies Inc.
 @author Matthew Lohbihler
    
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package com.serotonin.mango.vo.report;

import br.org.scadabr.l10n.AbstractLocalizer;
import java.io.PrintWriter;
import java.util.ResourceBundle;

import org.joda.time.DateTime;
import org.joda.time.format.DateTimeFormat;
import org.joda.time.format.DateTimeFormatter;

import com.serotonin.mango.view.export.CsvWriter;
import com.serotonin.mango.view.text.TextRenderer;
import br.org.scadabr.web.i18n.I18NUtils;
import br.org.scadabr.l10n.Localizer;

/**
 * @author Matthew Lohbihler
 */
public class ReportCsvStreamer implements ReportDataStreamHandler {

    private final PrintWriter out;

    // Working fields
    private TextRenderer textRenderer;
    private final String[] data = new String[5];
    private final DateTimeFormatter dtf = DateTimeFormat.forPattern("yyyy/MM/dd HH:mm:ss");
    private final CsvWriter csvWriter = new CsvWriter();

    public ReportCsvStreamer(PrintWriter out, ResourceBundle bundle) {
        this.out = out;

        // Write the headers.
        data[0] = AbstractLocalizer.localizeI18nKey("reports.pointName", bundle);
        data[1] = AbstractLocalizer.localizeI18nKey("common.time", bundle);
        data[2] = AbstractLocalizer.localizeI18nKey("common.value", bundle);
        data[3] = AbstractLocalizer.localizeI18nKey("reports.rendered", bundle);
        data[4] = AbstractLocalizer.localizeI18nKey("common.annotation", bundle);
        out.write(csvWriter.encodeRow(data));
    }

    @Override
    public void startPoint(ReportPointInfo pointInfo) {
        data[0] = pointInfo.getExtendedName();
        textRenderer = pointInfo.getTextRenderer();
    }

    @Override
    public void pointData(ReportDataValue rdv) {
        data[1] = dtf.print(new DateTime(rdv.getTime()));

        if (rdv.getValue() == null) {
            data[2] = data[3] = null;
        } else {
            data[2] = rdv.getValue().toString();
            data[3] = textRenderer.getText(rdv.getValue(), TextRenderer.HINT_FULL);
        }

        data[4] = rdv.getAnnotation();

        out.write(csvWriter.encodeRow(data));
    }

    @Override
    public void done() {
        out.flush();
        out.close();
    }
}
