/**********************************************************************
--- Qt Architect generated file ---
File: NewPassword.h
Last generated: Mon Jun 12 09:45:42 2000
*********************************************************************/
#ifndef NewPassword_included
#define NewPassword_included
#include "NewPasswordData.h"
class NewPassword : public NewPasswordData
{
	Q_OBJECT
	public:
	NewPassword(
	QWidget *parent = NULL,
	const char *name = NULL );
	virtual ~NewPassword();
	protected slots:
	virtual void OkClicked();
	void Changed(const QString &);
	public:
	friend class LoginDlg;
};
#endif // NewPassword_included

