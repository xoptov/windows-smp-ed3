/* Глава 7. grepMP. */
/* Параллельный поиск текстового шаблона - версия, использующая несколько
   потоков. */
#include "EvryThng.h"

typedef struct { /* Структура данных потока поиска. */
	int argc;
	TCHAR targv[4][MAX_PATH];
} GREP_THREAD_ARG;

typedef GREP_THREAD_ARG *PGR_ARGS; /* создаём тип указателя на GREP_THREAD_ARG. */
static DWORD WINAPI ThGrep(PGR_ARGS pArgs);

int _tmain(int argc, LPTSTR argv[])
{
	PGR_ARGS gArg;
	HANDLE *tHandle;
	DWORD ThdIdxP, ThIdm, ExitCode;
	TCHAR CmdLine[MAX_COMMAND_LINE];
	int iThrd, ThdCnt;
	STARTUPINFO StartUp;
	PROCESS_INFORMATION ProcessInfo;
	GetStartupInfo(&StartUp);
	/* Основной поток: создаёт отдельные потоки на основе
	   функции "grep" для каждого файла. */
	tHandle = malloc((argc - 2) * sizeof(HANDLE));
	gArg = malloc((argc - 2) * sizeof(GREP_THREAD_ARG));
	for (iThrd = 0; iThrd < argc - 2; iThrd++) {
		_tcscpy(gArg[iThrd].targv[1], argv[1]); /* Pattern. */
		_tcscpy(gArg[iThrd].targv[2], argv[iThrd + 2]);
		GetTempFileName /* Имя временного файла. */
			(".", "Gre", 0, gArg[iThrd].targv[3]);
		gArg[iThrd].argc = 4;
		/* Создать рабочий поток для выполнения командной строки. */
		tHandle[iThrd] = (HANDLE)_beginthreadex(
			NULL, 0, ThGrep, &gArg[iThrd], 0, &ThId
		);
	}
	/* Перенаправить стандартный вывод для вывода списка файлов. */
	StartUp.dwFlags = STARTF_USESTDHANDLES;
	StartUp.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	/* Выполняются все рабочие потоки. Ожидать их завершения. */
	ThdCnt = argc - 2;
	while (ThdCnt > 0) {
		ThdIdxP = WaitForMultipleObjects(
			ThdCnt, tHandle, FALSE, INFINITE
		);
		iThrd = (int)ThdIdxP - (int)WAIT_OBJECT_0;
		GetExitCodeThread(tHandle[iThrd], &ExitCode);
		CloseHandle(tHandle[iThrd]);
		if (ExitCode == 0) { /* Шаблон найден. */
			if (argc > 3) {
				/* Вывести имя файла, если имеется несколько файлов. */
				_tprintf(
					_T("\n**Результаты поиска - файла: %s\n"),
					gArg[iThrd].targv[2]
				);
				fflush(stdout);
			}
			/* Использовать программу "cat" для перечисления результирующих
			   файлов. */
			_stprintf(
				CmdLine, _T("%s%s"), _T("cat "), gArg[iThrd].targv[3]
			);
			CreateProcess(
				NULL, CmdLine, NULL, NULL, TRUE, 0,
				NULL, NULL, &StartUp, &ProcessInfo
			);
			WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
			CloseHandle(ProcessInfo.hProcess);
			CloseHandle(ProcessInfo.hThread);
		}
		DeleteFile(gArg[iThrd].targv[3]);
		/* Скорректировать массивы потоков и инен файлов. */
		tHandle[iThrd] = tHandle[thdCnt - 1];
		_tcscpy(gArg[iThrd].targv[3], gArg[ThdCnt-1].targv[3]);
		_tcscpy(gArg[iThrd].targv[2], gArg[ThdCnt-1].targv[2]);
		ThdCnt--;
	}
}

/* Прототип функции контекстного поиска:
static DWORD WINAPI ThGrep(PGR_ARGS pArgs)
{
} */
