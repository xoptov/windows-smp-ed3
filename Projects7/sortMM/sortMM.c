/* Глава 5. Команда sortMM.
   Сортировка отображённого в памяти файла - только один файл. Опции:
   -r Сортировать в обратном порядке.
   -I Использовать индексный файл для получения отсортированного файла. */
#include "EvryThng.h"

int KeyCompare(LPCTSTR, LPCTSTR);
DWORD CreateIndexFile(DWORD, LPCTSTR, LPTSTR);
DWORD KStart, KSize; /* Начальная позиция и размер ключа(TCHAR). */
BOOL Revrs;

int _tmain(int argc, LPTSTR argv[])
{
	HANDLE hInFile, hInMap; /* Дескрипторы входного файла. */
	HANDLE hXFile, hXMap; /* Дескрипторы индексного файла. */
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	BOOL IdxExists;
	DWORD FsIn, FsX, RSize, iKey, nWrite, *pSizes;
	LPTSTR pInFile = NULL;
	LPBYTE pXFile = NULL, pX;
	TCHAR __based(pInFile) *pIn;
	TCHAR IdxFlNam[MAX_PATH], ChNewLine = TNEWLINE;
	int FlIdx = Options(argc, argv, _T("rI"), &Revrs, &IdxExists, NULL);
	/* Шаг 1: открыть и отобразить входной файл. */
	hInFile = CreateFile(argv[FlIdx], GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	hInMap = CreateFileMapping(hInFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	pInFile = MapViewOfFile(hInMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	FsIn = GetFileSize(hInFile, NULL);
	/* Шаги 2 и 3: создать имя индексного файла. */
	_stprintf(IdxFlNam, _T("%s%s"), argv[FlIdx], _T(".idx"));
	if (!IdxExists)
		RSize = CreateIndexFile(FsIn, IdxFlNam, pInFile);
	/* Шаг 4: отобразить индексный файл. */
	hXFile = CreateFile(IdxFlNam, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	hXMap = CreateFileMapping(hXFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	pXFile = MapViewOfFile(hXMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	FsX = GetFileSize(hXFile, NULL);
	pSizes = (LPDWORD)pXFile; /* Поля развера в .idx-файле. */
	KSize = *pSizes; /* Размер ключа. */
	KStart = *(pSizes + 1); /* Начальная позиция ключа в записи. */
	FsX -= 2 * sizeof(DWORD);
	/* Шаг 5: сортировать индексный файл при помощи qsort. */
	if (!IdxExists)
		qsort(pXFile + 2 * sizeof(DWORD), FsX / RSize, RSize, KeyCompare);
	/* Шаг 6: Отобразить входной файл в отсортированном виде. */
	pX = pXFile + 2 * sizeof(DWORD) + RSize - sizeof(LPTSTR);
	for (iKey = 0; iKey < FsX / RSize; iKey++) {
		WriteFile(hStdOut, &ChNewLine, TSIZE, &nWrite, NULL);
		/* Приведение типа pX, если это необходимо! */
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
	/* Шаг 2: Создать индексный файл. Не отображать уго на данной стадии. */
	hXFile = CreateFile(IdxFlNam, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
	/* Шаг 2b: получить первый ключ и определить его размер и начальную
	   позицию. Пропустить пробел и получить длину ключа. */
	KStart = (DWORD)pInScan;
	while (*pInScan != TSPACE && *pInScan != TAB)
		pInScan++; /* Найти поле первого ключа. */
	KSize = ((DWORD)pInScan - KStart) / TSIZE;
	/* Шаг 3: посмотреть весь файл, записывая ключи и указатели записей в
	   индексный файл. */
	WriteFile(hXFile, &KSize, sizeof(DWORD), &nWrite, NULL);
	WriteFile(hXFile, &KStart, sizeof(DWORD), &nWrite, NULL);
	pInScan = 0;
	while ((DWORD)pInScan < FsIn) {
		WriteFile(hXFile, pInScan + KStart, KSize * TSIZE, &nWrite, NULL);
		WriteFile(hXFile, &pInScan, sizeof(LPTSTR), &mWrite, NULL);
		while ((DWORD)pInScan < FsIn && ((*pInScan != CR) || (*(pInScan + 1) != LF)
			pInScan++; /* Пропустить до конца строки. */
		pInScan += 2; /* Пропустить CR, LF. */
	}
	CloseHandle(hXFile);
	return KSize * TSIZE + sizeof(LPTSTR); /* Размер отдельной записи. */
}