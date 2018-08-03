// stdafx.cpp : fichier source incluant simplement les fichiers Include standard
// LDmicroCompiler.pch représente l'en-tête précompilé
// stdafx.obj contient les informations de type précompilées

#include "stdafx.h"


//-----------------------------------------------------------------------------
// Is an expression that could be either a variable name or a number a number?
//-----------------------------------------------------------------------------
bool IsNumber(String^ str)
{
	bool result;
	try {
		Convert::ToInt32(str);
	    result = true;
	} catch (...)
	{result = false;}
	return result;
}

String^ BoolToString(bool value, String^ ifTrue, String^ ifFalse)
{
	if (value) return ifTrue; else return ifFalse;
}

int IndexOfClosingParenthesis(String^ str)
{
	int n, i;
	n = 0;
	i = 0;
	while ( i < str->Length ) {
		switch (str[i]) {
			case '(' : 
				n++; break;
			case ')' :
				n--;
				if (n == 0) return i;
				break;
		} // switch
		i++;
	}
	return i;
}

String^ RemoveDollar(String^ str)
{
	if (str->Length < 1) return str;
	if (str[0] == '$') return str->Remove(0, 1);
	return str;
}
