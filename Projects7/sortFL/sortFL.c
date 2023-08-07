/* ����� 5. sortFL. ���������� ������. ������ ����� ������������� ������. */
/* �������������: sortFL ���� */
#include "EvryThng.h"

typedef struct _RECORD {
	TCHAR Key[KEY_SIZE];
	TCHAR Data[DATALEN];
} RECORD;

#define RECSIZE sizeof(RECORD)

int _tmain(int argc, LPTSTR argv[])
{
	HANDLE hFile = INVALID_HANDLE_VALUE, hMap = NULL;
	LPVOID pFile = NULL;
	DWORD FsLow, Result = 2;
	TCHAR TempFile[MAX_PATH];
	LPTSTR pTFile;
	/* ������� ��� ���������� �����, ���������������� ��� �������� �����
	   ������������ �����, ������� � ������������ ����������. */
	/* ����� ����������� ��-�������, ������� ���� � �������� ���������
	   �������� ����������� ������. */
	_stprintf(TempFile, _T("%s%s"), argv[1], _T(".tmp"));
	CopyFile(argv[1], TempFile, TRUE);
	Result = 1; /* ��������� ���� �������� ����� ��������� � ������ ���� �����. */
	/* ���������� ��������� ����� ��������� ��� ���������� � ������. */
	hFile = CreateFile(TempFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	FsLow = GetFileSize(hFile, NULL);
	hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, FsLow + TSIZE, NULL);
	pFile = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0 /* FsLow + TSIZE */, 0);
	qsort(pFile, FsLow / RECSIZE, RECSIZE, KeyCompare); /* KeyCompare - ��� � ��������� 5.2. */
	/* ���������� ��������������� ����. */
	pTFile = (LPTSTR)pFile;
	pTFile[FsLow/TSIZE] = '\0';
	_tprintf(_T("%s"), pFile);
	UnmapViewOfFile(pFile);
	CloseHandle(hMap);
	CloseHandle(hFile);
	DeleteFile(TempFile);
	return 0;
}