/* ����� 5. ������� sortMM.
   ���������� ������������ � ������ ����� - ������ ���� ����. �����:
   -r ����������� � �������� �������.
   -I ������������ ��������� ���� ��� ��������� ���������������� �����. */
#include "EvryThng.h"

int KeyCompare(LPCTSTR, LPCTSTR);
DWORD CreateIndexFile(DWORD, LPCTSTR, LPTSTR);
DWORD KStart, KSize; /* ��������� ������� � ������ �����(TCHAR). */
BOOL Revrs;

int _tmain(int argc, LPTSTR argv[])
{
	HANDLE hInFile, hInMap; /* ����������� �������� �����. */
	HANDLE hXFile, hXMap; /* ����������� ���������� �����. */
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	BOOL IdxExists;
	DWORD FsIn, FsX, RSize, iKey, nWrite, *pSizes;
	LPTSTR pInFile = NULL;
	LPBYTE pXFile = NULL, pX;
	TCHAR __based(pInFile) *pIn;
	TCHAR IdxFlNam[MAX_PATH], ChNewLine = TNEWLINE;
	int FlIdx = Options(argc, argv, _T("rI"), &Revrs, &IdxExists, NULL);
	/* ��� 1: ������� � ���������� ������� ����. */
	hInFile = CreateFile(argv[FlIdx], GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	hInMap = CreateFileMapping(hInFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	pInFile = MapViewOfFile(hInMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	FsIn = GetFileSize(hInFile, NULL);
	/* ���� 2 � 3: ������� ��� ���������� �����. */
	_stprintf(IdxFlNam, _T("%s%s"), argv[FlIdx], _T(".idx"));
	if (!IdxExists)
		RSize = CreateIndexFile(FsIn, IdxFlNam, pInFile);
	/* ��� 4: ���������� ��������� ����. */
	hXFile = CreateFile(IdxFlNam, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	hXMap = CreateFileMapping(hXFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	pXFile = MapViewOfFile(hXMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	FsX = GetFileSize(hXFile, NULL);
	pSizes = (LPDWORD)pXFile; /* ���� ������� � .idx-�����. */
	KSize = *pSizes; /* ������ �����. */
	KStart = *(pSizes + 1); /* ��������� ������� ����� � ������. */
	FsX -= 2 * sizeof(DWORD);
	/* ��� 5: ����������� ��������� ���� ��� ������ qsort. */
	if (!IdxExists)
		qsort(pXFile + 2 * sizeof(DWORD), FsX / RSize, RSize, KeyCompare);
	/* ��� 6: ���������� ������� ���� � ��������������� ����. */
	pX = pXFile + 2 * sizeof(DWORD) + RSize - sizeof(LPTSTR);
	for (iKey = 0; iKey < FsX / RSize; iKey++) {
		WriteFile(hStdOut, &ChNewLine, TSIZE, &nWrite, NULL);
		/* ���������� ���� pX, ���� ��� ����������! */
		pIn = (TCHAR __based(pInFile)*)*(LPDWORD)pX;
		while ((*pIn != CR || *(pIn + 1) != LF) && (DWORD)pIn < FsIn) {
			WriteFile(hStdOut, pIn, TSIZE, &nWrite, NULL);
			pIn++;
		}
		pX += RSize;
	}
	UnmapViewOfFile(pInFile);
	CloseHandle(hInMap);
	CloseHandle(hInFile);
	UnmapViewOfFile(pXFile);
	CloseHandle(hXMap);
	CloseHandle(hXFile);
	return 0;
}

DWORD CreateIndexFile(DWORD FsIn, LPCTSTR IdxFlNam, LPTSTR pInFile)
{
	HANDLE hXFile;
	TCHAR __based(pInFile) *pInScan = 0;
	DWORD nWrite;
	/* ��� 2: ������� ��������� ����. �� ���������� ��� �� ������ ������. */
	hXFile = CreateFile(IdxFlNam, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
	/* ��� 2b: �������� ������ ���� � ���������� ��� ������ � ���������
	   �������. ���������� ������ � �������� ����� �����. */
	KStart = (DWORD)pInScan;
	while (*pInScan != TSPACE && *pInScan != TAB)
		pInScan++; /* ����� ���� ������� �����. */
	KSize = ((DWORD)pInScan - KStart) / TSIZE;
	/* ��� 3: ���������� ���� ����, ��������� ����� � ��������� ������� �
	   ��������� ����. */
	WriteFile(hXFile, &KSize, sizeof(DWORD), &nWrite, NULL);
	WriteFile(hXFile, &KStart, sizeof(DWORD), &nWrite, NULL);
	pInScan = 0;
	while ((DWORD)pInScan < FsIn) {
		WriteFile(hXFile, pInScan + KStart, KSize * TSIZE, &nWrite, NULL);
		WriteFile(hXFile, &pInScan, sizeof(LPTSTR), &mWrite, NULL);
		while ((DWORD)pInScan < FsIn && ((*pInScan != CR) || (*(pInScan + 1) != LF)
			pInScan++; /* ���������� �� ����� ������. */
		pInScan += 2; /* ���������� CR, LF. */
	}
	CloseHandle(hXFile);
	return KSize * TSIZE + sizeof(LPTSTR); /* ������ ��������� ������. */
}