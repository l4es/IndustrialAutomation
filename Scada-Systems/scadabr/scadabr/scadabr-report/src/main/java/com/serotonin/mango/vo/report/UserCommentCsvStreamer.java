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
import java.util.List;
import java.util.ResourceBundle;

import com.serotonin.mango.view.export.CsvWriter;
import com.serotonin.mango.vo.UserComment;
import br.org.scadabr.web.i18n.I18NUtils;
import br.org.scadabr.l10n.Localizer;

/**
 * @author Matthew Lohbihler
 */
public class UserCommentCsvStreamer {

    public UserCommentCsvStreamer(PrintWriter out, List<ReportUserComment> comments, ResourceBundle bundle) {
        CsvWriter csvWriter = new CsvWriter();
        String[] data = new String[5];

        // Write the headers.
        data[0] = AbstractLocalizer.localizeI18nKey("users.username", bundle);
        data[1] = AbstractLocalizer.localizeI18nKey("reports.commentList.type", bundle);
        data[2] = AbstractLocalizer.localizeI18nKey("reports.commentList.typeKey", bundle);
        data[3] = AbstractLocalizer.localizeI18nKey("reports.commentList.time", bundle);
        data[4] = AbstractLocalizer.localizeI18nKey("notes.note", bundle);
        out.write(csvWriter.encodeRow(data));

        for (ReportUserComment comment : comments) {
            data[0] = comment.getUsername();
            if (data[0] == null) {
                data[0] = AbstractLocalizer.localizeI18nKey("common.deleted", bundle);
            }
            if (comment.getCommentType() == UserComment.TYPE_EVENT) {
                data[1] = AbstractLocalizer.localizeI18nKey("reports.commentList.type.event", bundle);
                data[2] = Integer.toString(comment.getTypeKey());
            } else if (comment.getCommentType() == UserComment.TYPE_POINT) {
                data[1] = AbstractLocalizer.localizeI18nKey("reports.commentList.type.point", bundle);
                data[2] = comment.getPointName();
            } else {
                data[1] = AbstractLocalizer.localizeI18nKey("common.unknown", bundle);
                data[2] = "";
            }

            data[3] = AbstractLocalizer.localizeTime(bundle.getLocale(), comment.getTs());
            data[4] = comment.getComment();

            out.write(csvWriter.encodeRow(data));
        }

        out.flush();
        out.close();
    }
}
