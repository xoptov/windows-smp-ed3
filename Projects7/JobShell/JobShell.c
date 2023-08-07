/* Глава 6. */
/* JobShell.c - команды управления задачами:
   jobbg -- Выполнить задачу в фоновом режиме.
   jobs -- Вывести список всех фоновых задач.
   kill -- Прекратиить выполнение указанной задачи из семейства задач.
           Существует опция, позволяющая генерировать управляющие сигналы
		   консоли. */

#include <EvryThng.h>
#include <JobMgt.h>

int _tmain(int argc, LPSTR argv[])
{
	BOOL Exit = FALSE;
	TCHAR Command[MAX_COMMAND_LINE + 10], *pc;
	DWORD i, LocArgc; /* Локальный пароаметр argc. */
	TCHAR argstr[MAX_ARG][MAX_COMMAND_LINE];
	LPTSTR pArgs[MAX_ARG];
	for (i = 0; i < MAX_ARG; i++)
		pArgs[i] = argstr[i];
	/* Вывести подсказку пользователю, считать команду и выполнить её. */
	_tprintf(_T("Управление задачами Windows\n"));
	while(!Exit) {
		_tprintf(_T("%s"), _T("JM$"));
		_fgetts(Command, MAX_COMMAND_LINE, stdin);
		pc = strchr(Command, '\n');
		*pc = '\0';
		/* Выполнить синтаксический разбор входных данных с целью получения
		   командной строки для новой задачи. */
		GetArgs(Command, &LocArgs, pArgs); /* См. Приложение А. */
		CharLower(argstr[0]);
		if (_tcscmp(argstr[0], _T("jobbg")) == 0)
			Jobbg(LocArgs, pArgs, Command);
		else if (_tcscmp(argstr[0], _T("jobs")) == 0)
			Jobs(LocArgs, pArgs, Command);
		else if (_tcscmp(argstr[0], _T("kill")) == 0)
			Kill(LocArgs, pArgs, Command);
		else if (_tcscmp(argstr[0], _T("quit")) == 0)
			Exit = TRUE;
		else
			_tprintf(_T("Такой команды не существует. Проверьте ввод\n"));
	}
	return 0;
}

/* jobbg [параметры] командная строка [параметры являются взаимоисключающими]
      -c: Предоставить консоль новому процессу.
	  -d: Отсоединить новый процесс без предоставления ему консоли.
	  Если параметры не заданы, процесс разделяет консоль с jobbg. */
int Jobbg(int argc, LPTSTR argv[], LPTSTR Command)
{
	DWORD fCreate;
	LONG JobNo;
	BOOL Flags[2];
	STARTUPINFO StartUp;
	PROCESS_INFORMATION ProcessInfo;
	LPTSTR targv = SkipArg(Command);
	GetStartupInfo(&StartUp);
	Options(argc, argv, _T("cd"), &Flags[0], &Flags[1], NULL);
	/* Пропустить также поле параметра, если он присутствует. */
	if (argv[1][0] == '-')
		targv = SkipArg(targv);
	fCreate = Flags[0] ? CREATE_NEW_CONSOLE :
		Flags[1] ? DETACHED_PROCESS : 0;
	/* Создать приостановленную задачу/поток. Возобновить выполнение
	   после ввода номера задачи. */
	CreateProcess(
		NULL, targv, NULL, NULL, TRUE,
		fCreate | CREATE_SUSPEND | CREATE_NEW_PROCESS_GROUP,
		NULL, NULL, &StartUp, &ProcessInfo
	);
	/* Создать номер задачи и ввести ID и дескриптор процесса
	   в "базу данных" задачи. */
	JobNo = GetJobNumber(&ProcessInfo, targv); /* См. "JobMgt.h" */
	if (JobNo >= 0)
		ResumeThread(ProcessInfo.hThread);
	else {
		TerminateProcess(ProcessInfo.hProcess, 3);
		CloseHandle(ProcessInfo.hProcess);
		ReportError(_T("Ошибка: Не хватает места в списке задач."), 0, FALSE);
		return 5;
	}
	CloseHandle(ProcessInfo.hThread);
	CloseHandle(ProcessInfo.hProcess);
	_tprintf(_T(" [%d] %d\n"), JobNo, ProcessInfo.dwProcessId);
	return 0;
}

/* jobs: вывод спииска всех выполняющихся и остановленных задач. */
int Jobs(int argc, LPTSTR argv[], LPTSTR Command)
{
	if (!DisplayJobs()) /* См. описание функции управления задачами. */
		return 1;
	return 0;
}

/* kill [переметры] Номер задачи (JobNumber)
   -b: Генерировать Ctrl+Break.
   -c: Генерировать Ctrl+C.
       В противном случае прекратить выполнение процесса. */
int Kill(int argc, LPTSTR argv[], LPTSTR Command)
{
	DWORD ProcessId, JobNumber, iJobNo;
	HANDLE hProcess;
	BOOL CntrlC, CntrlB, Killed;
	iJobNo = Options(argc, argv, _T("bc"), &CntrlB, &CntrlC, NULL);
	/* Найти ID процесса, связанного с данной задачей. */
	JobNumber = _ttoi(argv[iJobNo]);
	ProcessId = FindProcessId(JobNumber); /* См. описание функций управления задачами. */
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);
	if (hProcess == NULL) { /* ID процесса может не использоваться. */
		ReportError(_T("Выполнение процесса уже прекращено.\n"), 0, FALSE);
		return 2;
	}
	if (CntrlB)
		GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, ProcessId);
	else if (CntrlC)
		GenerateConsoleCtrlEvent(CTRL_C_EVENT, ProcessId);
	else
		TerminateProcess(hProcess, JM_EXIT_CODE);
	WaitForSingleObject(hProcess, 5000);
	CloseHandle(hProcess);
	_tprintf(_T("Задача [%d] прекращена или приостановлена \n"), JobNumber);
	return 0;
}