/* Глава 6. grepMP. */
/* Версия команды grep, использующая несколько процессов. */

#include <EvryThng.h>
#include <stdlib.h>
#include <malloc.h>

int _tmain(DWORD argc, LPTSTR argv[])
/* Для выполнения поиска в каждом из файлов, указанных в командной строке,
   создаётся отдельный процесс. Каждому процессу предоставляется временный
   файл в текущем каталоге, в котором сохраняются результаты. */
{
	HANDLE hTempFile;
	SECURITY_ATTRIBUTES StdOutSA = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE}; /* Атрибуты защиты для наследуемого дескриптора. */
	
	TCHAR CommandLine[MAX_PATH+100];
	STARTUPINFO StartUpSearch, StartUp;
	PROCESS_INFORMATION ProcessInfo;
	DWORD iProc, ExCode;
	HANDLE *hProc; /* Указатель на массив дескрипторов процессов. */
	
	typedef struct {TCHAR TempFile[MAX_PATH];} PROCFILE;
	PROCFILE *ProcFile; /* Указатель на массив имен временных файлов. */
	
	GetStartupInfo(&StartUpSearch);
	GetStartupInfo(&StartUp);
	ProcFile = (PROCFILE*)malloc((argc - 2) * sizeof(PROCFILE));
	hProc = (HANDLE*)malloc((argc - 2) * sizeof(HANDLE));

	/* Создать для каждого файла отдельный процесс "grep". */
	for (iProc = 0; iProc < argc - 2; iProc++) {
		_stprintf(CommandLine, _T("%s%s %s"), _T("grep "), argv[1], argv[iProc + 2]);
		GetTempFileName(_T("."), _T("gtm"), 0, ProcFile[iProc].TempFile); /* Для хранения результатов поиска. */
		hTempFile = CreateFile(
			ProcFile[iProc].TempFile,
			GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			&StdOutSA,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		); /* Этот дискриптор является наследуемым. */
		StartUpSearch.dwFlags = STARTF_USESTDHANDLES;
		StartUpSearch.hStdOutput = hTempFile;
		StartUpSearch.hStdError = hTempFile;
		StartUpSearch.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		/* Создать процесс для выполнения командной строки. */
		CreateProcess(NULL, CommandLine, NULL, NULL, TRUE, 0, NULL, NULL, &StartUpSearch, &ProcessInfo);
		/* Закрыть ненужные дескрипторы. */
		CloseHandle(hTempFile);
		CloseHandle(ProcessInfo.hThread);
		hProc[iProc] = ProcessInfo.hProcess;
	}

	/* Выполнить процессы и дождаться завершения каждого из них. */
	for (iProc = 0; iProc < argc - 2; iProc += MAXIMUM_WAIT_OBJECTS)
		/* Разрешить использовать достаточно большое кол-во процессов. */
		WaitForMultipleObjects(min(MAXIMUM_WAIT_OBJECTS, argc - 2 - iProc), &hProc[iProc], TRUE, INFINITE);
	
	/* Переслать результирующие файлы на стандартный вывод с использованием утилиты cat. */
	for (iProc = 0; iProc < argc - 2; iProc++) {
		if (GetExitCodeProcess(hProc[iProc], &ExCode) && ExCode == 0) {
			/* Обнаружен шаблон - Вывести результаты. */
			if (argc > 3)
				_tprintf(_T("%s:\n"), argv[iProc + 2]);
			fflush(stdout); /* Использование стандартного вывода несколькими процессами. */
			_stprintf(CommandLine, _T("%s%s"), _T("cat "), ProcFile[iProc].TempFile);
			CreateProcess(NULL, CommandLine, NULL, NULL, TRUE, 0, NULL, NULL, &StartUp, &ProcessInfo);
			WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
			CloseHandle(ProcessInfo.hProcess);
			CloseHandle(ProcessInfo.hThread);
		}
		CloseHandle(hProc[iProc]);
		CloseHandle(ProcFile[iProc].TempFile);
	}
	free(ProcFile);
	free(hProc);
	return 0;
}