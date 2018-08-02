#include "StdAfx.h"
#include ".\applicationsettings.h"

namespace Helpers
{
	//Ключ для кодирования/раскодирования
	const BYTE _RSAKey[] =
	{
		0x6A, 0x19, 0x59, 0x33, 0xC0, 0x6A, 0x19, 0xBF, 0x08, 0x4C, 0x01, 0x01, 0xF3, 0xAB, 0x59, 0x6A,
		0x04, 0xBF, 0x70, 0x4C, 0x01, 0x01, 0x6A, 0x40, 0xF3, 0xAB, 0xFF, 0x15, 0x80, 0x10, 0x00, 0x01,
		0x5F, 0x5E, 0x3B, 0xC3, 0xA3, 0xB0, 0x4D, 0x01, 0x01, 0x5B, 0x74, 0x0C, 0x68, 0x44, 0x40, 0x01,
		0x01, 0x50, 0xFF, 0x15, 0x90, 0x10, 0x00, 0x01, 0xC3, 0x55, 0x8B, 0xEC, 0x81, 0xEC, 0x80, 0x00,
		0x00, 0x00, 0x53, 0x56, 0x33, 0xF6, 0x57, 0x89, 0x75, 0xFC, 0x89, 0x75, 0xD0, 0x89, 0x75, 0xD4,
		0x89, 0x75, 0xD8, 0x89, 0x75, 0xDC, 0xE8, 0x14, 0xFE, 0xFF, 0xFF, 0xA1, 0x2C, 0x40, 0x01, 0x01,
		0x6A, 0x05, 0x68, 0x2C, 0x40, 0x01, 0x01, 0x89, 0x45, 0xF0, 0xA1, 0x38, 0x40, 0x01, 0x01, 0x68,
		0xC0, 0x12, 0x00, 0x01, 0x89, 0x45, 0xF4, 0xA1, 0x00, 0x40, 0x01, 0x01, 0x68, 0xAC, 0x12, 0x00
	};

	CApplicationSettings::CApplicationSettings(void)
	{
		_reg.Open(_T("FreeSCADA\\Архиватор"), true, false);
	}

	CApplicationSettings::~CApplicationSettings(void)
	{
	}

	BOOL CApplicationSettings::EncodeUserPass(sUserPass SrcData, vector<BYTE> &DstData)
	{
		//Получить хэндл криптографического провайдера
		HCRYPTPROV hProv;
		if(!CryptAcquireContext( &hProv, NULL, MS_ENHANCED_PROV,
			PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
		{
			return FALSE;
		}

		//Создать хэндл хэша
		HCRYPTKEY hKey;
		HCRYPTHASH hBaseData;
		if(!CryptCreateHash(hProv, CALG_MD5, NULL, NULL, &hBaseData))
			return FALSE;

		//Создать хэш используя данные ключа
		if(!CryptHashData(hBaseData, _RSAKey, sizeof(_RSAKey), NULL))
			return FALSE;

		//Создать 168 битный ключ для кодирования
		if(!CryptDeriveKey(hProv,CALG_3DES,hBaseData,168<<16, &hKey))
			return FALSE;

		//Расчитать размер буфера для зашифрованных данных
		DWORD buf_size=sizeof(sUserPass);
		CryptEncrypt( hKey, NULL, TRUE, NULL, NULL, &buf_size, NULL);

		//Установить размер
		DstData.resize(buf_size);

		//Закодировать данные
		buf_size=sizeof(sUserPass);
		memcpy(&DstData[0],&SrcData, buf_size);
		if(!CryptEncrypt( hKey, NULL, TRUE, NULL, &DstData[0], &buf_size, (DWORD)DstData.size()))
			return FALSE;

		//Освобождаем ресурсы
		CryptDestroyKey(hKey);
		CryptDestroyHash(hBaseData);
		CryptReleaseContext(hProv,0);
		
		return TRUE;
	}

	BOOL CApplicationSettings::DecodeUserPass(vector<BYTE> &SrcData, sUserPass& DstData)
	{
		//Получить хэндл криптографического провайдера
		HCRYPTPROV hProv;
		if(!CryptAcquireContext( &hProv, NULL, MS_ENHANCED_PROV,
			PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
		{
			return FALSE;
		}

		//Создать хэндл хэша
		HCRYPTKEY hKey;
		HCRYPTHASH hBaseData;
		if(!CryptCreateHash(hProv, CALG_MD5, NULL, NULL, &hBaseData))
			return FALSE;

		//Создать хэш используя данные ключа
		if(!CryptHashData(hBaseData, _RSAKey, sizeof(_RSAKey), NULL))
			return FALSE;

		//Создать 168 битный ключ для декодирования
		if(!CryptDeriveKey(hProv,CALG_3DES,hBaseData,168<<16, &hKey))
			return FALSE;

		//Расшифровать данные
		DWORD buf_size=static_cast<DWORD>(SrcData.size());
		vector<BYTE> buffer = SrcData;
		if(!CryptDecrypt(hKey, NULL, TRUE, NULL, &buffer[0], &buf_size))
			return FALSE;

		if(buf_size != sizeof(sUserPass))
			return FALSE;

		//Освобождаем ресурсы
		CryptDestroyKey(hKey);
		CryptDestroyHash(hBaseData);
		CryptReleaseContext(hProv,0);
		
		memcpy(&DstData, &buffer[0], sizeof(sUserPass));

		return TRUE;
	}

	CString CApplicationSettings::GetDBUserName()
	{
		vector<BYTE> data;
		_reg.ReadBinary(L"UsrPsw", data);
		sUserPass usrpsw;
		DecodeUserPass(data, usrpsw);
		return usrpsw.User;
	}

	CString CApplicationSettings::GetDBUserPass()
	{
		vector<BYTE> data;
		_reg.ReadBinary(L"UsrPsw", data);
		sUserPass usrpsw;
		DecodeUserPass(data, usrpsw);
		return usrpsw.Password;
	}
}