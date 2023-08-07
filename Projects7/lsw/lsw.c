/* Галава 3. lsw - команда вывода списка файлов. */
/* lsw [параметры] [файлы] */

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
	/* "Разобрать" шаблон поиска на "родительскую часть" и имя файла. */
	GetCurrentDirectory(MAX_PATH, CurrPath); /* Сохранить текущий путь доступа. */
	if (argc < FileIndex + 1) /* Путь доступа не указан. Использовать текущий каталог. */
		ok = TraverseDirectory(_T("*"), MAX_OPTIONS, Flags);
	else
		for (i = FileIndex; i < argc; i++) {
			/* Обработать все пути, указанные в командной строке. */
			ok = TraverseDirectory(pFileName, MAX_OPTIONS, Flags) && ok;
			SetCurrentDirectory(CurrPath); /* Восстановить каталог. */
		}
		return ok ? 0 : 1;
}

/* Обход дерева каталогов: выполнить функцию ProcessItem для каждого случая совпадения. */
/* PathName: относительное или абсолютное имя просматриваемого каталога. */
static BOOL TraverseDirectory(LPCTSTR PathName, DWORD NumFlags, LPBOOL Flags)
{
	HANDLE SearchHandle;
	WIN32_FIND_DATA FindData;
	BOOL Recursive = Flags[0];
	DWORD FType, iPass;
	TCHAR CurrPath[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, CurrPath);
	for (iPass = 1; iPass <= 2; iPass++) {
		/* Проход 1: вывод списка файлов. */
		/* Проход 2: вывод списка каталогов (если задана опция -R). */
		SearchHandle = FindFirstFile(PathName, &FindData);
		do {
			FType = FileType(&FindData); /* Файл или каталог? */
			if (iPass == 1)
				ProcessItem(&FindData, MAX_OPTIONS, Flags);
			if (FType == TYPE_DIR && iPass == 2 && Recursive) {
				/* Обработать подкаталог. */
				_tprintf(_T("\n%s\\%s:"), CurrPath, FindData.cFileName);
				/* Подготовка к обходу каталога. */
				SetCurrentDirectory(FindData.cFileName);
				TraverseDirectory(_T("*"), NumFlags, Flags);
				/* Рекурсивный вызов. */
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
	if (Long) { /* Указан ли в командной строке параметр "-1"? */
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

/* Функция для получения типа файла, поддерживаемые типы файлов -
 TYPE_FILE: файл, TYPE_DIR: каталог, TYPE_DOT: каталоги . или .. */
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