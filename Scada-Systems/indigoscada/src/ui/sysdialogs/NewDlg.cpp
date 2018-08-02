/**********************************************************************
--- Qt Architect generated file ---
File: NewDlg.cpp
Last generated: Mon Apr 17 17:03:48 2000
*********************************************************************/
#include <qt.h>
#include "NewDlg.h"
#define Inherited NewDlgData
#include "main.h"
#include "helper_functions.h"

NewDlg::NewDlg
(QComboBox * pL,
QWidget * parent, const char *name):Inherited (parent, name)
{
	setCaption (tr ("New Item"));
	OkButton->setEnabled (false);
	connect (Name, SIGNAL (textChanged (const QString &)),
	SLOT (Changed (const QString &)));
	//
	// Fill the the list box with the input combobox list
	//
	//
	for (unsigned i = 0; i < (unsigned)pL->count (); i++)
	{
		List->insertItem (pL->text (i));
	};
	//
	Name->setMaxLength(MAX_LENGHT_OF_STRING);
	//
	//
}
NewDlg::NewDlg
(QListBox * pL,
QWidget * parent, const char *name):Inherited (parent, name)
{
	setCaption (tr ("New Item"));
	OkButton->setEnabled (false);
	connect (Name, 
	SIGNAL(textChanged (const QString &)),
	SLOT (Changed (const QString &)));
	//
	// Fill the the list box with the input combobox list
	//
	//
	for (unsigned i = 0; i < pL->count (); i++)
	{
		List->insertItem (pL->text (i));
	};
	//
	
	Name->setMaxLength(MAX_LENGHT_OF_STRING);
	
}
NewDlg::NewDlg
(QListView * pL,
QWidget * parent, const char *name):Inherited (parent, name)
{
	setCaption (tr ("New Item"));
	OkButton->setEnabled (false);
	connect (Name, SIGNAL (textChanged (const QString &)),
	SLOT (Changed (const QString &)));
	//
	// Fill the the list box with the input list
	//
	QListViewItem *pI = pL->firstChild();
	if(pI)
	{
		for (; pI != 0; pI = pI->nextSibling())
		{
			List->insertItem (pI->text (0));
		};
	};
	//
	Name->setMaxLength(MAX_LENGHT_OF_STRING);

}
NewDlg::~NewDlg ()
{
}
void NewDlg::Changed (const QString & s)
{
	// does this string match any in the list
	bool f = s.isEmpty ();
	if (!f)
	{
		for (unsigned i = 0; i < List->count (); i++)
		{
			if (List->text (i) == s)
			{
				f = true;
				break;
			}
		};
	};
	OkButton->setEnabled (!f);
}
void NewDlg::Help ()
{
	MSG ("Help", "Help");
};

