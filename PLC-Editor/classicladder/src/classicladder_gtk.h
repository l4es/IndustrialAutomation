#ifndef _CLASSICLADDER_GTK_H
#define _CLASSICLADDER_GTK_H

//For i18n
#define _(STRING) gettext(STRING)
#define gettext_noop(STRING) (STRING)
#define N_(STRING) gettext_noop(STRING)

// macro used to be compatible with GTK v2.16 (used on Windows), perhaps a little before big switch to GTK3...
#if ( GTK_MAJOR_VERSION>=2 && GTK_MINOR_VERSION>=18 ) || (GTK_MAJOR_VERSION>=3)
#define MY_GTK_WIDGET_VISIBLE(widget) gtk_widget_get_visible(widget)
#else
#define MY_GTK_WIDGET_VISIBLE(widget) GTK_WIDGET_VISIBLE(widget)
#endif
#if GTK_MAJOR_VERSION>=3
#define GTK_OBJECT(o) G_OBJECT(o)
#define GTK_SIGNAL_FUNC(f)	    G_CALLBACK(f)
#define gtk_signal_connect(instance, detailed_signal, c_handler, data) g_signal_connect(instance, detailed_signal, c_handler, data)
#endif
#if ( GTK_MAJOR_VERSION>=2 && GTK_MINOR_VERSION>=24 ) || (GTK_MAJOR_VERSION>=3)
#define MyGtkComboBox GtkComboBoxText
#define MY_GTK_COMBO_BOX GTK_COMBO_BOX_TEXT
#define gtk_combo_box_new_text() gtk_combo_box_text_new()
#define gtk_combo_box_append_text(c,t) gtk_combo_box_text_append_text(c,t)
#define gtk_combo_box_get_active_text(c) gtk_combo_box_text_get_active_text(c)
#define gtk_combo_box_remove_text(c,p) gtk_combo_box_text_remove(c,p)
#else
#define MyGtkComboBox GtkComboBox
#define MY_GTK_COMBO_BOX GTK_COMBO_BOX
#endif


#include "monitor_transfer.h"
void GetTheSizesForRung( void );
void UpdateVScrollBar( char AutoSelectCurrentRung );
void RefreshLabelCommentEntries( void );
void ClearLabelCommentEntries();
void SaveLabelCommentEntriesEdited();
void AdjustLabelCommentEntriesToSection( int SectionLanguage );
void DoActionSave( );
void CreateFileSelection(char * Prompt,int CreateFileSelectionType);
void DoActionConfirmNewProject( );
void DoActionLoadProject( );
void DoActionSaveAs( );
void DoActionResetAndConfirmIfRunning( );
void ButtonConfig_click();
void DoActionAboutClassicLadder();
void DoActionExportSvg( void );
void DoActionExportPng( void );
void DoActionCopyToClipboard( void );
void ShowMessageBox(const char * title, const char * text, const char * button);
void ShowMessageBoxError( const char * text );
void ShowConfirmationBoxWithChoiceOrNot(const char * title, const char * text, void * function_if_yes, char HaveTheChoice);
void ShowConfirmationBox(const char * title, const char * text, void * function_if_yes);
char ShowEnterMessageBox( const char * title, const char * text, char * StoreStringResult, int LengthStringResult );
void ShowMessageBoxInBackground( char * Title, char * Message );
void DoQuitGtkApplication( void );
void ConfirmQuit( );
gboolean MessageInStatusBar( char * msg );
void FunctionSearchCloseBox( void );
void FunctionSearchOpenBox( void );
void RedrawSignalDrawingArea( void );
gboolean FileTransferUpdateInfosGtk( void );
void MainSectionWindowTakeFocus( );
void InitGtkWindows( int argc, char *argv[] );
gboolean UpdateAllGtkWindows( void );
gboolean UpdateWindowTitleWithProjectName( void );

#endif
