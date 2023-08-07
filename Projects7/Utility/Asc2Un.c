#include <EvryThng.h>
#define BUF_SIZE 256

/* Функция копирования файлов с преобразованием из ASCII в Unicode.
   Функция построена на основе функции CopyFile. */
BOOL Asc2Un(LPCTSTR fIn, LPCTSTR fOut, BOOL bFailIfExists)
{
	HANDLE hIn, hOut;
	DWORD dwOut, nIn, nOut, iCopy;
	CHAR aBuffer[BUF_SIZE];
	WCHAR uBuffer[BUF_SIZE];
	BOOL WriteOK = TRUE;
	hIn = CreateFile(fIn, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	/* Определить поводение функции CreateFile, если выходной файл уже существует. */
	dwOut = bFailIfExists ? CREATE_NEW : CREATE_ALWAYS;
	hOut = CreateFile(fOut, GENERIC_WRITE, 0, NULL, dwOut, FILE_ATTRIBUTE_NORMAL, NULL);
	while (ReadFile(hIn, aBuffer, BUF_SIZE, &nIn, NULL) && nIn > 0 && WriteOK) {
		for (iCopy = 0; iCopy < nIn; iCopy++)
			uBuffer[iCopy] = (WCHAR)aBuffer[iCopy];
		WriteOK = WriteFile(hOut, uBuffer, 2 * nIn, &nOut, NULL);
	}
	CloseHandle(hIn);
	CloseHandle(hOut);
	return WriteOK;
}