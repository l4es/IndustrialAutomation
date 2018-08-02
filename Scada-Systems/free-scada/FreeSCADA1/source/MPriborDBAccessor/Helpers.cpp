#include "StdAfx.h"
#include ".\helpers.h"
#include <boost/tokenizer.hpp>
#include "MPriborDBAccessor.h"

using namespace boost;
#define KEYLENGTH  (168<<16)

const BYTE g_RSAKey[] = {	0xE2, 0xC9, 0x91, 0xE8, 0x0B, 0x8C, 0x3F, 0x10,
							0xC9, 0x32, 0x87, 0x5C, 0xF4, 0x0C, 0x4E, 0x92,
							0x32, 0x21, 0x13, 0x22, 0xAB, 0x01, 0x7D, 0x10,
							0x43, 0xCE, 0xBD, 0x9D, 0xDE, 0xDC, 0x3C, 0x9D,
							0x18, 0x5F, 0x97, 0x06, 0xC1, 0xDB, 0x0D, 0x61,
							0x32, 0x38, 0xB9, 0x9C, 0xF6, 0x6C, 0xE8, 0x13,
							0x79, 0x1F, 0x47, 0xAA, 0x89, 0x11, 0xCA, 0xF3,
							0xA8, 0xE7, 0x49, 0x14, 0x6A, 0xCA, 0xDA, 0xD4};
const DWORD g_KeySize = 64;

namespace Helpers
{
	CString CHelpers::LoadLongQuery(ULONG ID)
	{
		CString res;
		HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(ID), _T("SQL_QUERY"));
		if(hRes)
		{
			const DWORD Size = SizeofResource(NULL, hRes);
			HANDLE hData = LoadResource(NULL, hRes);
			if(hData && Size>0)
			{
				BYTE* ptr = (BYTE*)LockResource(hData);
				if(ptr)
				{
					string tmp;
					tmp.resize(Size);
					memcpy(&tmp[0], ptr, Size);
					res = tmp.c_str();
				}
			}
		}
		return res;
	}
	vector<CString> CHelpers::SplitQueryToShort(CString query)
	{
		for(int i=0;i<query.GetLength();i++)
		{
			switch(query[i])
			{
			case _T('\n'):
			case _T('\r'):
			case _T('\t'):
				query.SetAt(i, _T(' ')); break;
			}
		}
		vector<CString> res;

		typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;
		boost::char_separator<char> separator(";", NULL, boost::drop_empty_tokens);

		CT2CA cQuery(query);
		string tmp = cQuery;
		Tokenizer tok(tmp, separator);
		for(Tokenizer::iterator tok_iter = tok.begin(); tok_iter != tok.end(); tok_iter++)
		{
			CString s(tok_iter->c_str());
			s.TrimLeft(_T(" "));
			s.TrimRight(_T(" "));
			if(!s.IsEmpty())
			{
				s+=_T(";");
				res.push_back(s);
			}
		}

		return res;
	}

	CString CHelpers::EncodePass(CString Password)
	{
		HCRYPTPROV hProv;
		if(!CryptAcquireContext( &hProv, NULL, MS_ENHANCED_PROV,
			PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
		{
			throw CE_CANT_OPEN_CRYPT_PROV;
		}

		HCRYPTKEY hKey;
		HCRYPTHASH hBaseData;
		if(!CryptCreateHash(hProv, CALG_MD5, NULL, NULL, &hBaseData))
			throw CE_CANT_CREATE_HASH;
		
		if(!CryptHashData(hBaseData, g_RSAKey, g_KeySize, NULL))
			throw CE_CANT_CREATE_HASH;

		if(!CryptDeriveKey(hProv,CALG_3DES,hBaseData,KEYLENGTH, &hKey))
			throw CE_CANT_CREATE_KEY;

		DWORD buf_size=static_cast<DWORD>(Password.GetLength()*sizeof(TCHAR));
		CryptEncrypt( hKey, NULL, TRUE, NULL, NULL, &buf_size, NULL);

		vector<BYTE> buffer(buf_size);

		CT2A cPassword(Password);
		buf_size=static_cast<DWORD>(strlen(cPassword));
		memcpy(&buffer[0],cPassword, buf_size);

		if(!CryptEncrypt( hKey, NULL, TRUE, NULL, &buffer[0], &buf_size, (DWORD)buffer.size()))
			throw CE_CANT_ENCODE;

		CryptDestroyKey(hKey);
		CryptDestroyHash(hBaseData);
		CryptReleaseContext(hProv,0);
		CString res;
		for(vector<BYTE>::iterator i=buffer.begin();i!=buffer.end();i++)
			res += *i;
		return res;
	}

	CString CHelpers::DecodePass(CString Password)
	{
		HCRYPTPROV hProv;
		if(!CryptAcquireContext( &hProv, NULL, MS_ENHANCED_PROV,
			PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
		{
			throw CE_CANT_OPEN_CRYPT_PROV;
		}

		HCRYPTKEY hKey;
		HCRYPTHASH hBaseData;
		if(!CryptCreateHash(hProv, CALG_MD5, NULL, NULL, &hBaseData))
			throw CE_CANT_CREATE_HASH;
		
		if(!CryptHashData(hBaseData, g_RSAKey, g_KeySize, NULL))
			throw CE_CANT_CREATE_HASH;

		if(!CryptDeriveKey(hProv,CALG_3DES,hBaseData,KEYLENGTH, &hKey))
			throw CE_CANT_CREATE_KEY;

		DWORD buf_size=static_cast<DWORD>(Password.GetLength());
		vector<BYTE> buffer;
		for(int i=0;i<int(Password.GetLength());i++)
			buffer.push_back((BYTE)Password[i]);

		if(!CryptDecrypt(hKey, NULL, TRUE, NULL, &buffer[0], &buf_size))
			throw CE_CANT_DECODE;
		buffer.resize(buf_size);

		CryptDestroyKey(hKey);
		CryptDestroyHash(hBaseData);
		CryptReleaseContext(hProv,0);
		CString res;
		for(vector<BYTE>::iterator i=buffer.begin();i!=buffer.end();i++)
			res+=*i;
		return res;
	}

	bool ConvertValToBool(CString value)
	{
		if(value.IsEmpty())
			return false;

		value.MakeLower();
		if(	value == "yes" ||
			value == "1" ||
			value == "true" ||
			value == "y")
		{
			return true;
		}
		else
			return false;
	}

	CString DecodeError(DWORD Code)
	{
		CString s;
		switch(Code)
		{
		case ERR_OK:					s = "Нет ошибок";					break;
		case ERR_NOT_OPEN:				s = "Не установлено подключение";	break;
		case ERR_ALREADY_OPEN:			s = "Соединение уже установлено";	break;
		case ERR_CANT_CREATE_OBJECT:	s = "Не удалось создать объект";	break;
		case ERR_ACCESSDENIED:			s = "Отказано в доступе";			break;
		case ERR_DB_ALLREADY_PRESENT:	s = "Файл БД уже существует";		break;
		case ERR_INVALIDARG:			s = "Неправильное значение аргумента"; break;
		case ERR_CRITICAL:				s = "Критическая ошибка";			break;
		case ERR_DUBLICATE:				s = "Дублирование записи";			break;
		case ERR_NOT_FOUND:				s = "Значение не найдено";			break;
		default:						s = "Неизвестная ошибка";			break;
		}

		return s;
	}
}