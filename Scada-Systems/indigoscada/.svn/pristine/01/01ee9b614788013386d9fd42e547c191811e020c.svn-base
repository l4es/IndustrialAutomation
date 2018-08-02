/**********************************************************************
--- Qt Architect generated file ---
File: LoginDlg.cpp
Last generated: Wed Apr 12 12:39:27 2000
*********************************************************************/
#include "LoginDlg.h"
#define Inherited LoginDlgData
#include "dbase.h"
#include "main.h"
#include <qt.h>
#include "NewPassword.h"
#include "IndentedTrace.h"
#include "utilities.h"
#include "helper_functions.h"

LoginDlg::LoginDlg
(
QWidget* parent,
const char* name
)
:
Inherited( parent, name )
{
	IT_IT("LoginDlg::LoginDlg");
	
	setCaption(tr("Login"));
	Username->setFocus();
	QWidget::setTabOrder(Username,Password);
	//
	// connect the database to the transaction handler
	// 
	connect(GetConfigureDb(),SIGNAL(TransactionDone(QObject *,const QString &, int, QObject*)),
	this,SLOT(QueryResponse(QObject *,const QString &,int, QObject*)));
	//
	Password->setEchoMode( QLineEdit::Password );

	#ifdef NO_LOGIN
	Username->setText("Developer");
	Password->setText("qwerty");
	#endif
}
LoginDlg::~LoginDlg()
{
	IT_IT("LoginDlg::~LoginDlg");
};
void LoginDlg::NameChanged(const QString &s)
{
	IT_IT("LoginDlg::NameChanged");
	OkButton->setEnabled(!s.isEmpty());
}
void LoginDlg::Okclicked()
{
	IT_IT("LoginDlg::Okclicked");
	QString cmd = QString("select * from USERS where NAME='") + Username->text().stripWhiteSpace() + "';";
	if(GetConfigureDb()->DoExec(this,cmd,tGet)) // post the command
	{
		OkButton->setEnabled(false); // disable while we do the transaction
	}

	Username->setFocus();
}
void LoginDlg::QueryResponse(QObject *p,const QString &,int id, QObject*caller) // response to database search
{
	IT_IT("LoginDlg::QueryResponse");

	if((p == this) && (id == tGet))
	{
		if(GetConfigureDb()->GetNumberResults() == 1)
		{
			QString str = GetConfigureDb()->GetString("PSSWORD");

			if(str == tr(NONE_STR)) // blank password handling - on start up
			{
				NewPassword dlg(this); // change the password
				if(dlg.exec())
				{
					QString cmd = "update USERS set PSSWORD='"+EncodePassword(dlg.Password1->text().stripWhiteSpace())+
					"',UPDTIME=" +  DATETIME_NOW + " where NAME='"+
					Username->text().stripWhiteSpace()+"';";
					GetConfigureDb()->DoExec(0,cmd,0); // update the database
					//
					#ifdef UNIX
					char hn[32];
					hn[0] = 0;
					gethostname(hn,sizeof(hn));
					QString msg = Username->text() + tr(" has changed password ") + 
					QString(hn) + " display=" + QString(getenv("DISPLAY"));
					#else

					char computer_name[ MAX_PATH ];
					ZeroMemory( computer_name, sizeof( computer_name ) );
					DWORD size = MAX_PATH;
					::GetComputerName( computer_name, &size );

					QString msg = Username->text() + tr(" has changed password ") + QString(computer_name); 
					#endif
					QSLogEvent("HMI",msg);
					//DOAUDIT(tr("Password Changed"));
					//
					Username->setText("");
					Password->setText("");
					return; // exit for re entry
				}
			}

			QString entered_pwd = EncodePassword(Password->text());
			QString stored_pwd = GetConfigureDb()->GetString("PSSWORD");
			
			if(entered_pwd == stored_pwd)
			{
				UserDetails &u = GetUserDetails();
				u.Name = Username->text().stripWhiteSpace(); // user name
				u.Language = GetConfigureDb()->GetString("LANGUAGE"); // what language do we speak
				u.AmSystem = false; // are we system admin - not to start with
				u.privs = GetConfigureDb()->GetString("PRIVS").toLong(); // privelge mask
				if(u.privs & PRIVS_AGE_PASSWORD)
				{
					QDateTime t = GetConfigureDb()->GetDateTime("UPDTIME"); // get the update time - from this we determine if the password is 
					//
					// out of date
					// 
					t = t.addDays(GetConfigureDb()->GetInt("EXPIREDAYS"));
					//
					if(t < QDateTime::currentDateTime()) // is the password out of date
					{
						NewPassword dlg(this); // change the password
						//
						//
						if(!dlg.exec())
						{       
							Username->setText("");
							Password->setText("");
							return;
						}
						else
						{
							QString cmd = "update USERS set PSSWORD='"+EncodePassword(dlg.Password1->text().stripWhiteSpace())
							+"',UPDTIME=" +  DATETIME_NOW + " where NAME='"+
							u.Name+"';";
							GetConfigureDb()->DoExec(0,cmd,0); // update the database
							//
							char hn[32];
							hn[0] = 0;
							//
							#ifdef UNIX
							gethostname(hn,sizeof(hn));
							QString msg = u.Name + tr(" has changed password ") + QString(hn) + " display=" + QString(getenv("DISPLAY"));
							#else
							
							char computer_name[ MAX_PATH ];
							ZeroMemory( computer_name, sizeof( computer_name ) );
							DWORD size = MAX_PATH;
							::GetComputerName( computer_name, &size );

							QString msg = u.Name + tr(" has changed password ") + QString(computer_name);
							#endif
							QSLogEvent("HMI",msg);
							//DOAUDIT(tr("Password Changed"));
							//
							Username->setText("");
							Password->setText("");
						}
					}
				}
				//    
			#ifdef UNIX
				char hn[32];
				hn[0] = 0;
				//
				gethostname(hn,sizeof(hn));
				// this is for X-Windows - DISPLAY tell us the IP address of the other end - possibly a flash git with 
				// multiple monitors
				QString msg = u.Name + tr(" has logged in from computer ") + QString(hn) + " display=" + QString(getenv("DISPLAY"));
				QSLogEvent("HMI",msg);
			#else

				char computer_name[ MAX_PATH ];
				ZeroMemory( computer_name, sizeof( computer_name ) );
				DWORD size = MAX_PATH;
				::GetComputerName( computer_name, &size );

				QString msg = u.Name + tr(" has logged in from computer ") + QString(computer_name) + QString(" - ") + QString(winver());
				QSLogEvent("HMI",msg);

				msg = QString(""SYSTEM_NAME"") + QString(" - ")
				#ifdef NDEBUG //Release build
				+ QString("Release build: ") 
				#else
				+ QString("Debug build: ") 
				#endif
				+ tr("Build Date:") + QString(GetScadaDateBuild()) + " " + QString(GetScadaTimeBuild());
				QSLogEvent("HMI",msg);
			#endif
				accept();
				return;
			}
		}
		 
			char hn[32];
			hn[0] = 0;
			#ifdef UNIX
			gethostname(hn,sizeof(hn));
			QString msg = tr(" Login Failed from Computer ") + QString(hn) + " display=" + QString(getenv("DISPLAY"));
			QSLogEvent("HMI",msg);  
			#else

			char computer_name[ MAX_PATH ];
			ZeroMemory( computer_name, sizeof( computer_name ) );
			DWORD size = MAX_PATH;
			::GetComputerName( computer_name, &size );

			QString msg = tr(" Login Failed from Computer ") + QString(computer_name);
			QSLogEvent("HMI",msg);  
			#endif
			//
			QMessageBox::warning(this,tr("Login Failed"),tr("Username or Password Not Recognised"));
			OkButton->setEnabled(true); // disable while we do the transaction
			//
			Password->setText("");
		
	}
}
