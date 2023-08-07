/* ������ 3. lsw - ������� ������ ������ ������. */
/* lsw [���������] [�����] */

#include "EvryThng.h"

BOOL TraverseDirectory(LPCTSTR, DWORD, LPBOOL);
DWORD FileType(LPWIN32_FIND_DATA);
BOOL ProcessItem(LPWIN32_FIND_DATA, DWORD, LPBOOL);

int _tmain(int argc, LPTSTR argv[])
{
	BOOL Flags[MAX_OPTIONS], ok = TRUE;
	TCHAR PathName[MAX_PATH+1], CurrPath[MAX_PATH+1];
	LPTSTR pSlash, pFileName;
	int i, FileIndex;
	FileIndex = Options(argc, argv, _T("R1"), &Flags[0], &Flags[1], NULL);
	/* "���������" ������ ������ �� "������������ �����" � ��� �����. */
	GetCurrentDirectory(MAX_PATH, CurrPath); /* ��������� ������� ���� �������. */
	if (argc < FileIndex + 1) /* ���� ������� �� ������. ������������ ������� �������. */
		ok = TraverseDirectory(_T("*"), MAX_OPTIONS, Flags);
	else
		for (i = FileIndex; i < argc; i++) {
			/* ���������� ��� ����, ��������� � ��������� ������. */
			ok = TraverseDirectory(pFileName, MAX_OPTIONS, Flags) && ok;
			SetCurrentDirectory(CurrPath); /* ������������ �������. */
		}
		return ok ? 0 : 1;
}

/* ����� ������ ���������: ��������� ������� ProcessItem ��� ������� ������ ����������. */
/* PathName: ������������� ��� ���������� ��� ���������������� ��������. */
static BOOL TraverseDirectory(LPCTSTR PathName, DWORD NumFlags, LPBOOL Flags)
{
	HANDLE SearchHandle;
	WIN32_FIND_DATA FindData;
	BOOL Recursive = Flags[0];
	DWORD FType, iPass;
	TCHAR CurrPath[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, CurrPath);
	for (iPass = 1; iPass <= 2; iPass++) {
		/* ������ 1: ����� ������ ������. */
		/* ������ 2: ����� ������ ��������� (���� ������ ����� -R). */
		SearchHandle = FindFirstFile(PathName, &FindData);
		do {
			FType = FileType(&FindData); /* ���� ��� �������? */
			if (iPass == 1)
				ProcessItem(&FindData, MAX_OPTIONS, Flags);
			if (FType == TYPE_DIR && iPass == 2 && Recursive) {
				/* ���������� ����������. */
				_tprintf(_T("\n%s\\%s:"), CurrPath, FindData.cFileName);
				/* ���������� � ������ ��������. */
				SetCurrentDirectory(FindData.cFileName);
				TraverseDirectory(_T("*"), NumFlags, Flags);
				/* ����������� �����. */
				SetCurrentDirectory(_T(".."));
			}
		} while (FindNextFile(SearchHandle, &FindData));
		FindClose(SearchHandle);
	}
	return TRUE;
}

static BOOL ProcessItem(LPWIN32_FIND_DATA pFileData, DWORD NumFlags, LPBOOL Flags)
{
	const TCHAR FileTypeChar[] = {' ', 'd'};
	DWORD FType = FileType(pFileData);
	BOOL Long = Flags[1];
	SYSTEMTIME LastWrite;
	if (FType != TYPE_FILE && FType != TYPE_DIR)
		return FALSE;
	_tprintf(_T("\n"));
	if (Long) { /* ������ �� � ��������� ������ �������� "-1"? */
		_tprintf(_T("%c"), FileTypeChar[FType - 1]);
		_tprintf(_T("%10d"), pFileData->nFileSizeLow);
		FileTimeToSystemTime(&(pFileData->ftLastWriteTime), &LastWrite);
		_tprintf(
			_T(" %02d/%02d/%04d %02d:%02d:%02d"),
			LastWrite.wDay, LastWrite.wMonth, LastWrite.wYear,
			LastWrite.wHour, LastWrite.wMinute, LastWrite.wSecond
		);
	}
	_tprintf(_T("%s"), pFileData->cFileName);
	return TRUE;
}

/* ������� ��� ��������� ���� �����, �������������� ���� ������ -
 TYPE_FILE: ����, TYPE_DIR: �������, TYPE_DOT: �������� . ��� .. */
static DWORD FileType(LPWIN32_FIND_DATA pFileData)
{
	BOOL IsDir;
	DWORD FType;
	FType = TYPE_FILE;
	IsDir = (pFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	if (IsDir)
		if (lstrcmp(pFileData->cFileName, _T(".")) == 0 || lstrcmp(pFileData->cFileName, _T("..")) == 0)
			FType = TYPE_DOT;
		else
			FType = TYPE_DIR;
	return FType;
}