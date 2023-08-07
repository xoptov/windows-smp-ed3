/* ����� 7. grepMP. */
/* ������������ ����� ���������� ������� - ������, ������������ ���������
   �������. */
#include "EvryThng.h"

typedef struct { /* ��������� ������ ������ ������. */
	int argc;
	TCHAR targv[4][MAX_PATH];
} GREP_THREAD_ARG;

typedef GREP_THREAD_ARG *PGR_ARGS; /* ������ ��� ��������� �� GREP_THREAD_ARG. */
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
	/* �������� �����: ������ ��������� ������ �� ������
	   ������� "grep" ��� ������� �����. */
	tHandle = malloc((argc - 2) * sizeof(HANDLE));
	gArg = malloc((argc - 2) * sizeof(GREP_THREAD_ARG));
	for (iThrd = 0; iThrd < argc - 2; iThrd++) {
		_tcscpy(gArg[iThrd].targv[1], argv[1]); /* Pattern. */
		_tcscpy(gArg[iThrd].targv[2], argv[iThrd + 2]);
		GetTempFileName /* ��� ���������� �����. */
			(".", "Gre", 0, gArg[iThrd].targv[3]);
		gArg[iThrd].argc = 4;
		/* ������� ������� ����� ��� ���������� ��������� ������. */
		tHandle[iThrd] = (HANDLE)_beginthreadex(
			NULL, 0, ThGrep, &gArg[iThrd], 0, &ThId
		);
	}
	/* ������������� ����������� ����� ��� ������ ������ ������. */
	StartUp.dwFlags = STARTF_USESTDHANDLES;
	StartUp.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	/* ����������� ��� ������� ������. ������� �� ����������. */
	ThdCnt = argc - 2;
	while (ThdCnt > 0) {
		ThdIdxP = WaitForMultipleObjects(
			ThdCnt, tHandle, FALSE, INFINITE
		);
		iThrd = (int)ThdIdxP - (int)WAIT_OBJECT_0;
		GetExitCodeThread(tHandle[iThrd], &ExitCode);
		CloseHandle(tHandle[iThrd]);
		if (ExitCode == 0) { /* ������ ������. */
			if (argc > 3) {
				/* ������� ��� �����, ���� ������� ��������� ������. */
				_tprintf(
					_T("\n**���������� ������ - �����: %s\n"),
					gArg[iThrd].targv[2]
				);
				fflush(stdout);
			}
			/* ������������ ��������� "cat" ��� ������������ ��������������
			   ������. */
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
		/* ��������������� ������� ������� � ���� ������. */
		tHandle[iThrd] = tHandle[thdCnt - 1];
		_tcscpy(gArg[iThrd].targv[3], gArg[ThdCnt-1].targv[3]);
		_tcscpy(gArg[iThrd].targv[2], gArg[ThdCnt-1].targv[2]);
		ThdCnt--;
	}
}

/* �������� ������� ������������ ������:
static DWORD WINAPI ThGrep(PGR_ARGS pArgs)
{
} */
