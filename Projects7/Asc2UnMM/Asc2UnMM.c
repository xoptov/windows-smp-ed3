/* Глава 5. Asc2UnMM.c: Реализация использующая отображение файлов. */
#include <EvryThng.h>

BOOL Asc2Un(LPCTSTR fIn, LPCTSTR fOut, BOOL bFailIfExists)
{
	HANDLE hIn, hOut, hInMap, hOutMap;
	LPSTR pIn, pInFile;
	LPWSTR pOut, pOutFile;
	DWORD FsLow, dwOut;
	/* Открыть и отобразить входной файл. */
	hIn = CreateFile(fIn, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	hInMap = CreateFileMapping(hIn, NULL, PAGE_READONLY, 0, 0, NULL);
	pInFile = MapViewOfFile(hInMap, FILE_MAP_READ, 0, 0, 0);
	dwOut = bFailIfExists ? CREATE_NEW : CREATE_ALWAYS;
	/* Открыть и отобразить выходной файл. */
	hOut = CreateFile(fOut, GENERIC_READ | GENERIC_WRITE, 0, NULL, dwOut, FILE_ATTRIBUTE_NORMAL, NULL);
	FsLow = GetFileSize(hIn, NULL); /* Установить размер отображения. */
	hOutMap = CreateFileMapping(hOut, NULL, PAGE_READWRITE, 0, 2 * FsLow, NULL);
	pOutFile = MapViewOfFile(hOutMap, FILE_MAP_WRITE, 0, 0, (SIZE_T)(2 * FsLow));
	/* Преобразовать данные отображенного файла из ASCII в Unicode. */
	pIn = pInFile;
	pOut = pOutFile;
	while (pIn < pInFile + FsLow) {
		*pOut = (WCHAR)*pIn;
		pIn++;
		pOut++;
	}
	UnmapViewOfFile(pOutFile);
	UnmapViewOfFile(pInFile);
	CloseHandle(hOutMap);
	CloseHandle(hInMap);
	CloseHnalde(hIn);
	CloseHandle(hOut);
	return TRUE;
}