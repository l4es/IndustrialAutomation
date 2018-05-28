/*****************************************************************************
 *
 * Testmanager - Graphical Automation and Visualisation Tool
 *
 * Copyright (C) 2018  Florian Pose <fp@igh.de>
 *
 * This file is part of Testmanager.
 *
 * Testmanager is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Testmanager is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with Testmanager. If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include "StyleDialog.h"

#include "lib/qcssparser_p.h"
#include "lib/csshighlighter_p.h"

#include <QPushButton>
#include <QToolBar>
#include <QMenu>
#include <QSignalMapper>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QDebug>

/****************************************************************************/

StyleDialog::StyleDialog(QWidget *editWidget, QWidget *parent):
    QDialog(parent),
    editWidget(editWidget),
    addResourceAction(tr("Add Resource..."), this),
    addColorAction(tr("Add Color..."), this),
    addFontAction(tr("Add Font..."), this)
{
    setupUi(this);

    textEditStyle->setTabStopWidth(fontMetrics().width(QLatin1Char(' ')) * 4);
    new qdesigner_internal::CssHighlighter(textEditStyle->document());
    connect(textEditStyle, SIGNAL(textChanged()),
            this, SLOT(validateStyleSheet()));
    textEditStyle->setPlainText(editWidget->styleSheet());

    QToolBar *toolBar = new QToolBar(this);
    verticalLayout->insertWidget(0, toolBar);

    // resources -------------------------------------------------------------

    QSignalMapper *resourceActionMapper = new QSignalMapper(this);
    resourceActionMapper->setMapping(&addResourceAction, QString());
    connect(&addResourceAction, SIGNAL(triggered()),
            resourceActionMapper, SLOT(map()));
    connect(resourceActionMapper, SIGNAL(mapped(QString)),
            this, SLOT(slotAddResource(QString)));

    const char * const resourceProperties[] = {
        "background-image",
        "border-image",
        "image",
        0
    };

    QMenu *resourceActionMenu = new QMenu(this);

    for (int resourceProperty = 0; resourceProperties[resourceProperty];
            ++resourceProperty) {
        QAction *action = resourceActionMenu->addAction(
                QLatin1String(resourceProperties[resourceProperty]));
        connect(action, SIGNAL(triggered()),
                resourceActionMapper, SLOT(map()));
        resourceActionMapper->setMapping(action,
                QLatin1String(resourceProperties[resourceProperty]));
    }

    addResourceAction.setMenu(resourceActionMenu);
    toolBar->addAction(&addResourceAction);

    // colors ----------------------------------------------------------------

    QSignalMapper *colorActionMapper = new QSignalMapper(this);
    colorActionMapper->setMapping(&addColorAction, QString());
    connect(&addColorAction, SIGNAL(triggered()),
            colorActionMapper, SLOT(map()));
    connect(colorActionMapper, SIGNAL(mapped(QString)),
            this, SLOT(slotAddColor(QString)));

    const char * const colorProperties[] = {
        "color",
        "background-color",
        "alternate-background-color",
        "border-color",
        "border-top-color",
        "border-right-color",
        "border-bottom-color",
        "border-left-color",
        "gridline-color",
        "selection-color",
        "selection-background-color",
        0
    };

    QMenu *colorActionMenu = new QMenu(this);

    for (int colorProperty = 0; colorProperties[colorProperty];
            ++colorProperty) {
        QAction *colorAction = colorActionMenu->addAction(
                QLatin1String(colorProperties[colorProperty]));
        connect(colorAction, SIGNAL(triggered()),
                colorActionMapper, SLOT(map()));
        colorActionMapper->setMapping(colorAction,
                QLatin1String(colorProperties[colorProperty]));
    }

    addColorAction.setMenu(colorActionMenu);
    toolBar->addAction(&addColorAction);

    // fonts -----------------------------------------------------------------

    connect(&addFontAction, SIGNAL(triggered()),
            this, SLOT(slotAddFont()));
    toolBar->addAction(&addFontAction);
}

/****************************************************************************/

StyleDialog::~StyleDialog()
{
}

/****************************************************************************/

void StyleDialog::insertCssProperty(
        const QString &name,
        const QString &value
        )
{
    if (value.isEmpty()) {
        return;
    }

    QTextCursor cursor = textEditStyle->textCursor();
    if (name.isEmpty()) {
        cursor.insertText(value);
        return;
    }

    cursor.beginEditBlock();
    cursor.removeSelectedText();
    cursor.movePosition(QTextCursor::EndOfLine);

    // Simple check to see if we're in a selector scope
    const QTextDocument *doc = textEditStyle->document();
    const QTextCursor closing = doc->find(QLatin1String("}"),
            cursor, QTextDocument::FindBackward);
    const QTextCursor opening = doc->find(QLatin1String("{"),
            cursor, QTextDocument::FindBackward);
    const bool inSelector =
        !opening.isNull() && (closing.isNull() ||
                closing.position() < opening.position());
    QString insertion;
    if (textEditStyle->textCursor().block().length() != 1) {
        insertion += QLatin1Char('\n');
    }
    if (inSelector) {
        insertion += QLatin1Char('\t');
    }
    insertion += name;
    insertion += QLatin1String(": ");
    insertion += value;
    insertion += QLatin1Char(';');
    cursor.insertText(insertion);
    cursor.endEditBlock();
}

/****************************************************************************/

/** see Qt tools/designer/src/lib/shared/stylesheeteditor.cpp
 */
bool StyleDialog::isStyleSheetValid(const QString &styleSheet)
{
    QCss::Parser parser(styleSheet);
    QCss::StyleSheet sheet;
    if (parser.parse(&sheet)) {
        return true;
    }
    QString fullSheet = QLatin1String("* { ");
    fullSheet += styleSheet;
    fullSheet += QLatin1Char('}');
    QCss::Parser parser2(fullSheet);
    return parser2.parse(&sheet);
}

/****************************************************************************/

void StyleDialog::validateStyleSheet()
{
    const bool valid = isStyleSheetValid(textEditStyle->toPlainText());
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
    if (valid) {
        labelValid->setText(tr("Valid stylesheet"));
        labelValid->setStyleSheet(QLatin1String("color: green"));
    } else {
        labelValid->setText(tr("Invalid stylesheet"));
        labelValid->setStyleSheet(QLatin1String("color: red"));
    }
}

/****************************************************************************/

void StyleDialog::on_buttonBox_accepted()
{
    editWidget->setStyleSheet(textEditStyle->toPlainText());
    accept();
}

/****************************************************************************/

void StyleDialog::slotAddResource(const QString &property)
{
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    QStringList filters;
    filters
        << tr("Image files (*.png *.svg *.bmp)")
        << tr("Any files (*)");

    dialog.setNameFilters(filters);

    QStringList searchPaths(QDir::searchPaths("layout"));
    if (!searchPaths.isEmpty()) {
        QDir layoutDir(searchPaths[0]);
        dialog.setDirectory(layoutDir);
    }

    int ret = dialog.exec();

    if (ret != QDialog::Accepted) {
        return;
    }

    QString path = dialog.selectedFiles()[0];

    QString urlStr;
    if (searchPaths.isEmpty()) {
        QString relPath(QDir::current().relativeFilePath(path));
        urlStr = "url(" + relPath + ")";
    }
    else {
        QDir layoutDir(searchPaths[0]);
        QString relPath(QDir::cleanPath(layoutDir.relativeFilePath(path)));
        urlStr = "url(layout:" + relPath + ")";
    }

    insertCssProperty(property, urlStr);
}

/****************************************************************************/

void StyleDialog::slotAddColor(const QString &property)
{
    const QColor color =
        QColorDialog::getColor(0xffffffff, this, QString(),
                QColorDialog::ShowAlphaChannel);
    if (!color.isValid()) {
        return;
    }

    QString colorStr;

    if (color.alpha() == 255) {
        colorStr = QString(QLatin1String("rgb(%1, %2, %3)")).arg(
                color.red()).arg(color.green()).arg(color.blue());
    } else {
        colorStr = QString(QLatin1String("rgba(%1, %2, %3, %4)")).arg(
                color.red()).arg(color.green()).arg(color.blue()).arg(
                color.alpha());
    }

    insertCssProperty(property, colorStr);
}

/****************************************************************************/

void StyleDialog::slotAddFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (!ok) {
        return;
    }

    QString fontStr;
    if (font.weight() != QFont::Normal) {
        fontStr += QString::number(font.weight());
        fontStr += QLatin1Char(' ');
    }

    switch (font.style()) {
    case QFont::StyleItalic:
        fontStr += QLatin1String("italic ");
        break;
    case QFont::StyleOblique:
        fontStr += QLatin1String("oblique ");
        break;
    default:
        break;
    }
    fontStr += QString::number(font.pointSize());
    fontStr += QLatin1String("pt \"");
    fontStr += font.family();
    fontStr += QLatin1Char('"');

    insertCssProperty(QLatin1String("font"), fontStr);
    QString decoration;
    if (font.underline())
        decoration += QLatin1String("underline");
    if (font.strikeOut()) {
        if (!decoration.isEmpty())
            decoration += QLatin1Char(' ');
        decoration += QLatin1String("line-through");
    }
    insertCssProperty(QLatin1String("text-decoration"), decoration);
}

/****************************************************************************/
