/**********************************************************************
--- Qt Architect generated file ---
File: NewPassword.cpp
Last generated: Mon Jun 12 09:45:42 2000
*********************************************************************/
#include <qt.h>
#include "NewPassword.h"
#define Inherited NewPasswordData
NewPassword::NewPassword
(
QWidget* parent,
const char* name
)
:
Inherited( parent, name )
{
	setCaption(tr("New Password"));
	connect (Password1, SIGNAL (textChanged (const QString &)),
	SLOT (Changed (const QString &)));
	connect (Password2, SIGNAL (textChanged (const QString &)),
	SLOT (Changed (const QString &)));
	OkButton->setEnabled(false);
	Password1->setFocus();
	Password1->setEchoMode( QLineEdit::Password );
	Password2->setEchoMode( QLineEdit::Password );
}
NewPassword::~NewPassword()
{
}
void NewPassword::Changed(const QString &)
{
	if(!Password1->text().isEmpty())
	{
		if(Password1->text().length() > 5)
		{
			//cerr << "|" << (const char *)Password1->text() << "|" << (const char *)Password2->text() << "|" << endl;  
			if(Password1->text().stripWhiteSpace() == Password2->text().stripWhiteSpace())
			{
				OkButton->setEnabled(true);
				return;
			};
		};
	};
	OkButton->setEnabled(false);
};
void NewPassword::OkClicked()
{
	accept();
}

