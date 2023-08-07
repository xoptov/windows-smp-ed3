/* Глава 3. lsReg: Команда вывода содержимого реестра. */
/* lsReg [параметры] подраздел */

#include <EvryThng.h>

BOOL TraverseRegistry(HKEY, LPTSTR, LPTSTR, LPBOOL);
BOOL DisplayPair(LPTSTR, DWORD, LPBYTE, DWORD, LPBOOL);
BOOL DisplaySubKey(LPTSTR, LPTSTR, PFILETIME, LPBOOL);

int _tmain(int argc, LPTSTR argv[])
{
	BOOL Flags[2], ok = TRUE;
	TCHAR KeyName[MAX_PATH+1];
	LPTSTR pScan;
	DWORD i, KeyIndex;
	HKEY hKey, hNextKey;
	/* Таблица преопределённых имён и дескрипторов разделов. */
	LPTSTR PreDefKeyNames[] = {
		_T("HKEY_LOCAL_MACHINE"),
		_T("HKEY_CLASSES_ROOT"),
		_T("HKEY_CURRENT_USER"),
		_T("HKEY_CURRENT_CONFIG")
	};
	HKEY PreDefKeys[] = {
		HKEY_LOCAL_MACHINE,
		HKEY_CLASSES_ROOT,
		HKEY_CURRENT_USER,
		HKEY_CURRENT_CONFIG
	};
	KeyIndex = Options(argc, argv, _T("R1"), &Flags[0], &Flags[1], NULL);
	/* "Разобрать" шаблон поиска на "раздел" и "подраздел". */
	/* Воссоздать раздел. */
	pScan = argv[KeyIndex];
	for (i = 0; *pScan != _T('\\') && *pScan != _T('\0'); pScan++, i++)
		KeyName[i] = *pScan;
	KeyName[i] = _T('\0');
	if (*pScan == _T('\\'))
		pScan++;
	/* Преобразовать преопределённое имя раздела в соответствующий HKEY. */
	for (i = 0; PreDefKeyNames[i] != NULL && _tcscmp(PreDefKeyNames[i], KeyName) != 0; i++);
	hKey = PreDefKeys[i];
	RegOpenKeyEx(hKey, pScan, 0, KEY_READ, &hNextKey);
	hKey = hNextKey;
	ok = TraverseRegistry(hKey, argv[KeyIndex], NULL, Flags);
	return ok ? 0 : 1;
}

/* Совершить обход разделов и подразделов реестра, если задан параметр -R. */
BOOL TraverseRegistry(HKEY hKey, LPTSTR FullKeyName, LPTSTR SubKey, LPBOOL Flags)
{
	HKEY hSubK;
	BOOL Recursive = Flags[0];
	LONG Result;
	DWORD ValType, Index, NumSubKs, SubKNameLen, ValNameLen, ValLen;
	DWORD MaxSubKLen, NumVals, MaxValNameLen, MaxValLen;
	FILETIME LastWriteTime;
	LPTSTR SubKName, ValName;
	LPBYTE Val;
	TCHAR FullSubKName[MAX_PATH+1];
	/* Открыть дескриптор раздела. */
	RegOpenKeyEx(hKey, SubKey, 0, KEY_READ, &hSubK);
	/* Определить максимальный размер информации относительно раздела и распределить память. */
	RegQueryInfoKey(hSubK, NULL, NULL, NULL, &NumSubKs, &MaxSubKLen, NULL, &NumVals, &MaxValNameLen, &MaxValLen, NULL, &LastWriteTime);
	SubKName = malloc(MaxSubKLen + 1); /* Выделение памяти размером MaxSubKLen байт с учётом завершающего \0 символа. */
	ValName = malloc(MaxValNameLen + 1); /* Выделение памяти размером MaxValNameLen байт с учётом завершающего \0 символа. */
	Val = malloc(MaxValLen); /* Выделение памяти размером MaxValLen байт. */
	/* Первый проход: пары "имя-значение". */
	for (Index = 0; Index < NumVals; Index++) {
		ValNameLen = MaxValNameLen + 1; /* Устанавливаем каждый раз. */
		ValLen = MaxValLen + 1;
		RegEnumValue(hSubK, Index, ValName, &ValNameLen, NULL, &ValType, Val, &ValLen);
		DisplayPair(ValName, ValType, Val, ValLen, Flags);
	}
	/* второй проход: подразделы. */
	for (Index = 0; Index < NumSubKs; Index++) {
		SubKNameLen = MaxSubKLen + 1;
		RegEnumKeyEx(hSubK, Index, SubKName, &SubKNameLen, NULL, NULL, NULL, &LastWriteTime);
		DisplaySubKey(FullKeyName, SubKName, &LastWriteTime, Flags);
		if (Recursive) {
			_stprintf(FullSubKName, _T("%s\\%s"), FullKeyName, SubKName);
			TraverseRegistry(hSubK, FullSubKName, SubKName, Flags);
		}
	}
	_tprintf(_T("\n"));
	free(SubKName); free(ValName); free(Val);
	RegCloseKey(hSubK);
	return TRUE;
}

/* функция отображения пары "имя-значение". */
BOOL DisplayPair(LPTSTR ValueName, DWORD ValueType, LPBYTE Value, DWORD ValueLen, LPBOOL Flags)
{
	LPBYTE pV = Value;
	DWORD i;
	_tprintf(_T("\nValue: %s = "), ValueName);
	switch(ValueType) {
		case REG_FULL_RESOURCE_DESCRIPTOR: /* 9: описание оборудования. */
		case REG_BINARY: /* 3: любые двоичные данные. */
			for (i = 0; i < ValueLen; i++, pV++)
				_tprintf(_T(" %x"), *pV);
			break;
		case REG_DWORD: /* 4: 32-битовое число. */
			_tprintf(_T("%x"), (DWORD)*Value);
			break;
		case REG_MULTI_SZ: /* 7: массив строк, завершающихся нулевым символом. */
		case REG_SZ: /* 1: строка, завершающаяся нулевым символом. */
			_tprintf(_T("%s"), (LPTSTR)Value);
			break;
		/* ...Несколько других типов... */
		return TRUE;			
	}
}

BOOL DisplaySubKey(LPTSTR KeyName, LPTSTR SubKeyName, PFILETIME pLastWrite, LPBOOL Flags)
{
	BOOL Long = Flags[1];
	SYSTEMTIME SysLastWrite;
	_tprintf(_T("\nSubkey: %s"), KeyName);
	if (_tcslen(SubKeyName) > 0)
		_tprintf(_T("\\%s"), SubKeyName);
	if (Long) {
		FileTimeToSystemTime(pLastWrite, &SysLastWrite);
		_tprintf(
			_T("%02d/%02d/%04d %02d:%02d:%02d"),
			SysLastWrite.wMonth, SysLastWrite.wDay, SysLastWrite.wYear,
			SysLastWrite.wHour, SysLastWrite.wMinute, SysLastWrite.wSecond
		);
	}
	return TRUE;
}