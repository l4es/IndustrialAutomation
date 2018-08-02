/**********************************************************************
--- Qt Architect generated file ---
File: SelectPrinterDlg.cpp
Last generated: Wed Feb 28 12:05:03 2001
*********************************************************************/
#include "SelectPrinterDlg.h"
#include "common.h"
#include <ctype.h>
#include <stdlib.h>
#define Inherited SelectPrinterDlgData
//
// this is where Linux keeps the printer names
// change for the appropriate operating system
//  
static void parsePrintcap( QListBox * printers )
{
	QFile printcap( QString::fromLatin1("/etc/printcap") );
	if ( !printcap.open( IO_ReadOnly ) )
	return;
	char * line = new char[1025];
	line[1024] = '\0';
	QString printerDesc;
	int lineLength = 0;
	while( !printcap.atEnd() &&
	(lineLength=printcap.readLine( line, 1024 )) > 0 ) {
		if ( *line == '#' ) {
			*line = '\0';
			lineLength = 0;
		}
		if ( lineLength >= 2 && line[lineLength-2] == '\\' ) {
			line[lineLength-2] = '\0';
			printerDesc += QString::fromLocal8Bit(line);
			} else {
			printerDesc += QString::fromLocal8Bit(line);
			printerDesc = printerDesc.simplifyWhiteSpace();
			int i = printerDesc.find( ':' );
			QString printerName, printerComment, printerHost;
			if ( i >= 0 ) {
				// have : want |
				int j = printerDesc.find( '|' );
				printerName = printerDesc.left( j > 0 ? j : i );
				if ( j > 0 ) {
					// try extracting a comment from the aliases...
					printerComment = qApp->translate( "QPrintDialog",
					"Aliases: " );
					printerComment += printerDesc.mid( j+1, i-j-1 );
					j=printerComment.length();
					while( j > 0 ) {
						j--;
						if ( printerComment[j] == '|' )
						printerComment[j] = ',';
					}
				}
				// then look for a real comment
				j = i+1;
				while( printerDesc[j].isSpace() )
				j++;
				if ( printerDesc[j] != ':' ) {
					printerComment = printerDesc.mid( i, j-i );
					printerComment = printerComment.simplifyWhiteSpace();
				}
				// look for lprng psuedo all printers entry
				i = printerDesc.find( QRegExp( ": *all *=" ) );
				if ( i >= 0 )
				printerName = "";
				// look for signs of this being a remote printer
				i = printerDesc.find(
				QRegExp( QString::fromLatin1(": *rm *=") ) );
				if ( i >= 0 ) {
					// point k at the end of remote host name
					while( printerDesc[i] != '=' )
					i++;
					while( printerDesc[i] == '=' || printerDesc[i].isSpace() )
					i++;
					j = i;
					while( j < (int)printerDesc.length() && printerDesc[j] != ':' )
					j++;
					// and stuff that into the string
					printerHost = printerDesc.mid( i, j-i );
				}
			}
			if ( printerName.length() )
			{
				printers->insertItem(printerName);
			};
			//perhapsAddPrinter( printers, printerName, printerHost, printerComment );
			// chop away the line, for processing the next one
			printerDesc = "";
		}
	}
	delete[] line;
}
#if 0
//
// solaris, not 2.6
static void parseEtcLpPrinters( QListBox * printers )
{
	QDir lp( QString::fromLatin1("/etc/lp/printers") );
	const QFileInfoList * dirs = lp.entryInfoList();
	if ( !dirs )
	return;
	QFileInfoListIterator it( *dirs );
	QFileInfo *printer;
	QString tmp;
	while ( (printer = it.current()) != 0 ) {
		++it;
		if ( printer->isDir() ) {
			tmp.sprintf( "/etc/lp/printers/%s/configuration",
			printer->fileName().ascii() );
			QFile configuration( tmp );
			char * line = new char[1025];
			QRegExp remote( QString::fromLatin1("^Remote:") );
			QRegExp contentType( QString::fromLatin1("^Content types:") );
			QString printerHost;
			bool canPrintPostscript = FALSE;
			if ( configuration.open( IO_ReadOnly ) ) {
				while( !configuration.atEnd() &&
				configuration.readLine( line, 1024 ) > 0 ) {
					if ( remote.match( QString::fromLatin1(line) ) == 0 ) {
						const char * p = line;
						while( *p != ':' )
						p++;
						p++;
						while( isspace(*p) )
						p++;
						printerHost = QString::fromLocal8Bit(p);
						printerHost = printerHost.simplifyWhiteSpace();
						} else if ( contentType.match( QString::fromLatin1(line) ) == 0 ) {
						char * p = line;
						while( *p != ':' )
						p++;
						p++;
						char * e;
						while( *p ) {
							while( isspace(*p) )
							p++;
							if ( *p ) {
								char s;
								e = p;
								while( isalnum(*e) )
								e++;
								s = *e;
								*e = '\0';
								if ( !qstrcmp( p, "postscript" ) ||
								!qstrcmp( p, "any" ) )
								canPrintPostscript = TRUE;
								*e = s;
								if ( s == ',' )
								e++;
								p = e;
							}
						}
					}
				}
				if ( canPrintPostscript )
				{
					printers->insertItem(printer->fileName());
					// perhapsAddPrinter( printers, printer->fileName(),
					//	       printerHost, QString::fromLatin1("") );
				};
			}
			delete[] line;
		}
	}
}
// solaris 2.6
static char * parsePrintersConf( QListBox * printers )
{
	QFile pc( QString::fromLatin1("/etc/printers.conf") );
	if ( !pc.open( IO_ReadOnly ) )
	return 0;
	char * line = new char[1025];
	line[1024] = '\0';
	QString printerDesc;
	int lineLength = 0;
	char * defaultPrinter = 0;
	while( !pc.atEnd() &&
	(lineLength=pc.readLine( line, 1024 )) > 0 ) {
		if ( *line == '#' ) {
			*line = '\0';
			lineLength = 0;
		}
		if ( lineLength >= 2 && line[lineLength-2] == '\\' ) {
			line[lineLength-2] = '\0';
			printerDesc += QString::fromLocal8Bit(line);
			} else {
			printerDesc += QString::fromLocal8Bit(line);
			printerDesc = printerDesc.simplifyWhiteSpace();
			int i = printerDesc.find( ':' );
			QString printerName, printerHost, printerComment;
			if ( i >= 0 ) {
				// have : want |
				int j = printerDesc.find( '|', 0 );
				if ( j >= i )
				j = -1;
				printerName = printerDesc.mid( 0, j < 0 ? i : j );
				if ( printerName == QString::fromLatin1("_default") ) {
					i = printerDesc.find(
					QRegExp( QString::fromLatin1(": *use *=") ) );
					while( printerDesc[i] != '=' )
					i++;
					while( printerDesc[i] == '=' || printerDesc[i].isSpace() )
					i++;
					j = i;
					while( j < (int)printerDesc.length() &&
					printerDesc[j] != ':' &&
					printerDesc[j] != ',' )
					j++;
					// that's our default printer
					defaultPrinter =
					qstrdup( printerDesc.mid( i, j-i ).ascii() );
					printerName = "";
					printerDesc = "";
					} else if ( printerName == QString::fromLatin1("_all") ) {
					// skip it.. any other cases we want to skip?
					printerName = "";
					printerDesc = "";
				}
				if ( j > 0 ) {
					// try extracting a comment from the aliases...
					printerComment = qApp->translate( "QPrintDialog",
					"Aliases: " );
					printerComment += printerDesc.mid( j+1, i-j-1 );
					for( j=printerComment.length(); j>-1; j-- )
					if ( printerComment[j] == '|' )
					printerComment[j] = ',';
				}
				// look for signs of this being a remote printer
				i = printerDesc.find(
				QRegExp( QString::fromLatin1(": *bsdaddr *=") ) );
				if ( i >= 0 ) {
					// point k at the end of remote host name
					while( printerDesc[i] != '=' )
					i++;
					while( printerDesc[i] == '=' || printerDesc[i].isSpace() )
					i++;
					j = i;
					while( j < (int)printerDesc.length() &&
					printerDesc[j] != ':' &&
					printerDesc[j] != ',' )
					j++;
					// and stuff that into the string
					printerHost = printerDesc.mid( i, j-i );
					// maybe stick the remote printer name into the comment
					if ( printerDesc[j] == ',' ) {
						i = ++j;
						while( printerDesc[i].isSpace() )
						i++;
						j = i;
						while( j < (int)printerDesc.length() &&
						printerDesc[j] != ':' &&
						printerDesc[j] != ',' )
						j++;
						if ( printerName != printerDesc.mid( i, j-i ) ) {
							printerComment =
							QString::fromLatin1("Remote name: ");
							printerComment += printerDesc.mid( i, j-i );
						}
					}
				}
			}
			if ( printerComment == ":" )
			printerComment = ""; // for cups
			if ( printerName.length() )
			{
				printers->insertItem(printerName);
				//	perhapsAddPrinter( printers, printerName, printerHost,
				//  printerComment );
			};
			// chop away the line, for processing the next one
			printerDesc = "";
		}
	}
	delete[] line;
	return defaultPrinter;
}
// HP-UX
static void parseEtcLpMember( QListBox * printers )
{
	QDir lp( QString::fromLatin1("/etc/lp/member") );
	if ( !lp.exists() )
	return;
	const QFileInfoList * dirs = lp.entryInfoList();
	if ( !dirs )
	return;
	QFileInfoListIterator it( *dirs );
	QFileInfo *printer;
	QString tmp;
	while ( (printer = it.current()) != 0 ) 
	{
		++it;
		// uglehack.
		// I haven't found any real documentation, so I'm guessing that
		// since lpstat uses /etc/lp/member rather than one of the
		// other directories, it's the one to use.  I did not find a
		// decent way to locate aliases and remote printers.
		if ( printer->isFile() )
		{
			printers->insertItem(printer->fileName());
			//perhapsAddPrinter( printers, printer->fileName(),
			//	       qApp->translate( "QPrintDialog","unknown"),
			//		QString::fromLatin1("") );
		};
	}
}
// IRIX 6.x
static void parseSpoolInterface( QListBox * printers )
{
	QDir lp( QString::fromLatin1("/usr/spool/lp/interface") );
	if ( !lp.exists() )
	return;
	const QFileInfoList * files = lp.entryInfoList();
	if( !files )
	return;
	QFileInfoListIterator it( *files );
	QFileInfo *printer;
	while ( (printer = it.current()) != 0) {
		++it;
		if ( !printer->isFile() )
		continue;
		// parse out some information
		QFile configFile( printer->filePath() );
		if ( !configFile.open( IO_ReadOnly ) )
		continue;
		QCString line(1025);
		QString hostName;
		QString hostPrinter;
		QString printerType;
		QRegExp typeKey(QString::fromLatin1("^TYPE="));
		QRegExp hostKey(QString::fromLatin1("^HOSTNAME="));
		QRegExp hostPrinterKey(QString::fromLatin1("^HOSTPRINTER="));
		int length;
		while( !configFile.atEnd() &&
		(configFile.readLine( line.data(), 1024 )) > 0 )
		{
			if(typeKey.match(line, 0, &length) == 0)
			{
				printerType = line.mid(length, line.length()-length);
				printerType = printerType.simplifyWhiteSpace();
			}
			if(hostKey.match(line, 0, &length) == 0) 
			{
				hostName = line.mid(length, line.length()-length);
				hostName = hostName.simplifyWhiteSpace();
			}
			if(hostPrinterKey.match(line, 0, &length) == 0) 
			{
				hostPrinter = line.mid(length, line.length()-length);
				hostPrinter = hostPrinter.simplifyWhiteSpace();
			}
		}
		configFile.close();
		printerType = printerType.stripWhiteSpace();
		if ( !printerType.isEmpty() && qstricmp( printerType.ascii(), "postscript" ))
		continue;
		if(hostName.isEmpty() || hostPrinter.isEmpty())
		{
			printers->insertItem(printer->fileName());
			//perhapsAddPrinter( printers, printer->fileName(),
			//QString::fromLatin1(""), QString::fromLatin1(""));
		} 
		else
		{
			QString comment = QString::fromLatin1("Remote name: ");
			comment += hostPrinter;
			printers->insertItem(printer->fileName());
			//perhapsAddPrinter( printers, printer->fileName(),hostName, comment);
		}
	}
}
// Every unix must have its own.  It's a standard.  Here is AIX.
static void parseQconfig( QListBox * printers )
{
	QFile qconfig( QString::fromLatin1("/etc/qconfig") );
	if ( !qconfig.open( IO_ReadOnly ) )
	return;
	QTextStream ts( &qconfig );
	QString line;
	QString stanzaName; // either a queue or a device name
	bool up = TRUE; // queue up?  default TRUE, can be FALSE
	QString remoteHost; // null if local
	QString deviceName; // null if remote
	QRegExp newStanza( QString::fromLatin1("^[0-z][0-z]*:$") );
	// our basic strategy here is to process each line, detecting new
	// stanzas.  each time we see a new stanza, we check if the
	// previous stanza was a valid queue for a) a remote printer or b)
	// a local printer.  if it wasn't, we assume that what we see is
	// the start of the first stanza, or that the previous stanza was
	// a device stanza, or that there is some syntax error (we don't
	// report those).
	do {
		line = ts.readLine();
		bool indented = line[0].isSpace();
		line = line.simplifyWhiteSpace();
		if ( indented && line.contains( '=' ) ) { // line in stanza
			int i = line.find( '=' );
			QString variable = line.left( i ).simplifyWhiteSpace();
			QString value=line.mid( i+1, line.length() ).simplifyWhiteSpace();
			if ( variable == QString::fromLatin1("device") )
			deviceName = value;
			else if ( variable == QString::fromLatin1("host") )
			remoteHost = value;
			else if ( variable == QString::fromLatin1("up") )
			up = !(value.lower() == QString::fromLatin1("false"));
			} else if ( line[0] == '*' ) { // comment
			// nothing to do
		} else if ( ts.atEnd() || // end of file, or beginning of new stanza
		( !indented &&
		line.contains( newStanza ) ) ) {
			if ( up && stanzaName.length() > 0 && stanzaName.length() < 21 ) {
				if ( remoteHost.length() ) // remote printer
				{
					printers->insertItem(stanzaName);
					//perhapsAddPrinter( printers, stanzaName, remoteHost,QString::null );
				}
				else if ( deviceName.length() ) // local printer
				{
					printers->insertItem(stanzaName);
					//perhapsAddPrinter( printers, stanzaName, QString::null,QString::null );
				};
			}
			line.truncate( line.length()-1 );
			if ( line.length() >= 1 && line.length() <= 20 )
			stanzaName = line;
			up = TRUE;
			remoteHost = QString::null;
			deviceName = QString::null;
			} else {
			// syntax error?  ignore.
		}
	} while( !ts.atEnd() );
}
#endif
SelectPrinterDlg::SelectPrinterDlg
(
QWidget* parent,
const char* name
)
:
Inherited( parent, name )
{
	setCaption(tr("Select Printer"));
	parsePrintcap(List);
	//
	if(List->count() > 0)
	{
		List->setCurrentItem(0);
	};
	//parseEtcLpMember(List);
	//parseSpoolInterface(List);
	//parseQconfig(List);
}
SelectPrinterDlg::~SelectPrinterDlg()
{
}
void SelectPrinterDlg::OkClicked()
{
	// handle the empty case
	if(List->currentText().isEmpty() || (List->count() < 1))
	{
		reject();
	}
	else
	{
		accept();
	};
}

