#ifndef HELPWINDOW_H
#define HELPWINDOW_H
#include <qt.h>
#include <scada_dll.h>
#ifdef WIN32
#pragma warning (disable: 4251)
#endif
class QComboBox;
class QPopupMenu;
class QSEXPORT HelpWindow : public QMainWindow
{
	Q_OBJECT
	public:
	HelpWindow( const QString& home_,  const QString& path, QWidget* parent = 0, const char *name=0 );
	~HelpWindow();
	private slots:
	void setBackwardAvailable( bool );
	void setForwardAvailable( bool );
	void textChanged();
	void about();
	void aboutQt();
	void openFile();
	void newWindow();
	void print();
	void pathSelected( const QString & );
	void histChosen( int );
	void bookmChosen( int );
	void addBookmark();
	private:
	void readHistory();
	void readBookmarks();
	QTextBrowser* browser;
	QComboBox *pathCombo;
	int backwardId, forwardId;
	QString selectedURL;
	QDir path;
	QStringList fileList, history, bookmarks;
	QMap<int, QString> mHistory, mBookmarks;
	QPopupMenu *hist, *bookm;
};
#endif

