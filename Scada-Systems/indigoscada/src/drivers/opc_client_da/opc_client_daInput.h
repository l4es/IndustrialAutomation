/**********************************************************************
--- Qt Architect generated file ---
File: Opc_client_daInput.h
Last generated: Mon May 22 17:08:25 2000
*********************************************************************/
#ifndef Opc_client_daInput_included
#define Opc_client_daInput_included
#include "opc_client_daInputData.h"

class Opc_client_da_DriverThread;

class Opc_client_daInput : public Opc_client_daInputData
{
	Q_OBJECT
	public:
	enum  {  tConfigLoad = 1};
	Opc_client_daInput(
	QWidget *parent = NULL,
	const char *name = NULL);
	virtual ~Opc_client_daInput();

	QString Name;
	public slots:
	void Load(const QString &s);// load the configuration
	void Save(const QString &s);// save the configuration
};
#endif // Opc_client_daInput_included

