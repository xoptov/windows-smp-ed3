/* ����� 6. */
/* JobShell.c - ������� ���������� ��������:
   jobbg -- ��������� ������ � ������� ������.
   jobs -- ������� ������ ���� ������� �����.
   kill -- ����������� ���������� ��������� ������ �� ��������� �����.
           ���������� �����, ����������� ������������ ����������� �������
		   �������. */

#include <EvryThng.h>
#include <JobMgt.h>

int _tmain(int argc, LPSTR argv[])
{
	BOOL Exit = FALSE;
	TCHAR Command[MAX_COMMAND_LINE + 10], *pc;
	DWORD i, LocArgc; /* ��������� ��������� argc. */
	TCHAR argstr[MAX_ARG][MAX_COMMAND_LINE];
	LPTSTR pArgs[MAX_ARG];
	for (i = 0; i < MAX_ARG; i++)
		pArgs[i] = argstr[i];
	/* ������� ��������� ������������, ������� ������� � ��������� �. */
	_tprintf(_T("���������� �������� Windows\n"));
	while(!Exit) {
		_tprintf(_T("%s"), _T("JM$"));
		_fgetts(Command, MAX_COMMAND_LINE, stdin);
		pc = strchr(Command, '\n');
		*pc = '\0';
		/* ��������� �������������� ������ ������� ������ � ����� ���������
		   ��������� ������ ��� ����� ������. */
		GetArgs(Command, &LocArgs, pArgs); /* ��. ���������� �. */
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
			_tprintf(_T("����� ������� �� ����������. ��������� ����\n"));
	}
	return 0;
}

/* jobbg [���������] ��������� ������ [��������� �������� ������������������]
      -c: ������������ ������� ������ ��������.
	  -d: ����������� ����� ������� ��� �������������� ��� �������.
	  ���� ��������� �� ������, ������� ��������� ������� � jobbg. */
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
	/* ���������� ����� ���� ���������, ���� �� ������������. */
	if (argv[1][0] == '-')
		targv = SkipArg(targv);
	fCreate = Flags[0] ? CREATE_NEW_CONSOLE :
		Flags[1] ? DETACHED_PROCESS : 0;
	/* ������� ���������������� ������/�����. ����������� ����������
	   ����� ����� ������ ������. */
	CreateProcess(
		NULL, targv, NULL, NULL, TRUE,
		fCreate | CREATE_SUSPEND | CREATE_NEW_PROCESS_GROUP,
		NULL, NULL, &StartUp, &ProcessInfo
	);
	/* ������� ����� ������ � ������ ID � ���������� ��������
	   � "���� ������" ������. */
	JobNo = GetJobNumber(&ProcessInfo, targv); /* ��. "JobMgt.h" */
	if (JobNo >= 0)
		ResumeThread(ProcessInfo.hThread);
	else {
		TerminateProcess(ProcessInfo.hProcess, 3);
		CloseHandle(ProcessInfo.hProcess);
		ReportError(_T("������: �� ������� ����� � ������ �����."), 0, FALSE);
		return 5;
	}
	CloseHandle(ProcessInfo.hThread);
	CloseHandle(ProcessInfo.hProcess);
	_tprintf(_T(" [%d] %d\n"), JobNo, ProcessInfo.dwProcessId);
	return 0;
}

/* jobs: ����� ������� ���� ������������� � ������������� �����. */
int Jobs(int argc, LPTSTR argv[], LPTSTR Command)
{
	if (!DisplayJobs()) /* ��. �������� ������� ���������� ��������. */
		return 1;
	return 0;
}

/* kill [���������] ����� ������ (JobNumber)
   -b: ������������ Ctrl+Break.
   -c: ������������ Ctrl+C.
       � ��������� ������ ���������� ���������� ��������. */
int Kill(int argc, LPTSTR argv[], LPTSTR Command)
{
	DWORD ProcessId, JobNumber, iJobNo;
	HANDLE hProcess;
	BOOL CntrlC, CntrlB, Killed;
	iJobNo = Options(argc, argv, _T("bc"), &CntrlB, &CntrlC, NULL);
	/* ����� ID ��������, ���������� � ������ �������. */
	JobNumber = _ttoi(argv[iJobNo]);
	ProcessId = FindProcessId(JobNumber); /* ��. �������� ������� ���������� ��������. */
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);
	if (hProcess == NULL) { /* ID �������� ����� �� ��������������. */
		ReportError(_T("���������� �������� ��� ����������.\n"), 0, FALSE);
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
	_tprintf(_T("������ [%d] ���������� ��� �������������� \n"), JobNumber);
	return 0;
}