/* Глава 3. команда touch. */
/* touch [параметры] [файлы] */

#include "EvryThng.h"

int _tmain(int argc, LPSTR argv[])
{
	SYSTEMTIME SysTime;
	FILETIME NewFileTime;
	LPFILETIME pAccessTime = NULL, pModifyTime = NULL;
	HANDLE hFile;
	BOOL Flags[MAX_OPTIONS], SetAccessTime, SetModTime, CreateNew;
	DWORD CreateFlag;
	int i, FileIndex;
	FileIndex = Options(argc, argv, _T("amc"), &Flags[0], &Flags[1], &Flags[2], NULL);
	SetAccessTime = !Flags[0];
	SetModTime = !Flags[1];
	CreateNew = !Flags[2];
	CreateFlag = CreateNew ? OPEN_ALWAYS : OPEN_EXISTING;
	for (i = FileIndex; i < argc; i++) {
		hFile = CreateFile(argv[i], GENERIC_READ | GENERIC_WRITE, 0, NULL, CreateFlag, FILE_ATTRIBUTE_NORMAL, NULL);
		GetSystemTime(&SysTime);
		SystemTimeToFileTime(&SysTime, &NewFileTime);
		if (SetAccessTime)
			pAccessTime = &NewFileTime;
		if (SetModTime)
			pModifyTime = &NewFileTime;
		SetFileTime(hFile, NULL, pAccessTime, pModifyTime);
		CloseHandle(hFile);
	}
	return 0;
}