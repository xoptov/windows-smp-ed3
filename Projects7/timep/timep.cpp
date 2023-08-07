/* ����� 6. timep. */
#include <EvryThng.h>

int _tmain(int argc, LPTSTR argv[])
{
	STARTUPINFO StartUp;
	PROCESS_INFORMATION ProcInfo;
	/* ��� ��������� ������������ ��� ���������� �������������� �������� � 
	   �������� ��������� ����������. */
	union {
		LONGLONG li;
		FILETIME ft;
	} CreateTime, ExitTime, ElapsedTime;

	FILETIME KernelTime, UserTime;
	SYSTEMTIME ElTiSys, KeTiSys, UsTiSys, StartTimeSys, ExitTimeSys;
	LPTSTR targv = SkipArg(GetCommandLine());
	OSVERSIONINFO OSVer;
	BOOL IsNT;
	HANDLE hProc;

	OSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&OSVer);
	IsNT = (OSVer.dwPlatformId == VER_PLATFORM_WIN32_NT);
	/* NT (��� ������) ���������� VER_PLATFORM_WIN32_NT. */
	GetStartupInfo(&StartUp);
	GetSystemTime(&StartTimeSys);

	/* ��������� ��������� ������; ��������� ���������� ��������.  */
	CreateProcess(NULL, targv, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &StartUp, &ProcInfo);
	/* ��������� � ������� ���� ����������� ���� ������� � ��������. */
	DuplicateHandle(
		GetCurrentProcess(), 
		ProcInfo.hProcess,
		GetCurrentProcess(),
		&hProc,
		PROCESS_QUERY_INFORMATION | SYNCHRONIZE,
		FALSE, 0
	);
	WaitForSingleObject(hProc, INFINITE);
	GetSystemTime(&ExitTimeSys);
	if (IsNT) {
		/* Windows NT. ��� �������� ����������� �������� �����, �����
		   ���������� � ������ ���� � ����� ���������� � ����������������
		   ������. */
		GetProcessTimes(
			hProc, &CreateTime.ft, &ExitTime.ft, &KernelTime, &UserTime
		);
		ElapsedTime.li = ExitTime.li - CreateTime.li;
		FileTimeToSystemTime(&ElapsedTime.ft, &ElTiSys);
		FileTimeToSystemTime(&KernelTime, &KeTiSys);
		FileTimeToSystemTime(&UserTime, &UsTiSys);
		_tprintf(
			_T("�������� �����: %02d:%02d:%02d:%03d\n"),
			ElTiSys.wHour, ElTiSys.wMinute, ElTiSys.wSecond, ElTiSys.wMilliseconds
		);
		_tprintf(
			_T("���������������� �����: %02d:%02d:%02d:%03d\n"),
			UsTiSys.wHour, UsTiSys.wMinute, UsTiSys.wSecond, UsTiSys.wMilliseconds
		);
		_tprintf(
			_T("��������� �����: %02d:%02d:%02d:%03d\n"),
			KeTiSys.wHour, KeTiSys.wMinute, KeTiSys.wSecond, KeTiSys.wMilliseconds
		);
	} else {
		/* Windows 9x � CE. ����������� ���� �������� �����. */
	}
	CloseHandle(ProcInfo.hThread);
	CloseHandle(ProcInfo.hProcess);
	CloseHandle(hProc);
	return 0;
}