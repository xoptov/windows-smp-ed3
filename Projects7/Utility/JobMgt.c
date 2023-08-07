/* ��������������� ������� ���������� ��������. */
#include "EvryThng.h"
#include "JobMgt.h"
void GetJobMgtFileName(LPTSTR);

LONG GetJobNumber(PROCESS_INFORMATION *pProcessInfo, LPCTSTR Command)
/* ������� ����� ������ ��� ������ �������� � ������ ���������� � �����
   �������� � ���� ������ ������. */
{
	HANDLE hJobData, hProcess; /* ���������� ������������, ��� �� ����� ���� PVOID ����, �� ���� ���������. */
	JM_JOB JobRecord;
	DWORD JobNumber = 0, nXfer, ExitCode, FsLow, FsHigh;
	TCHAR JobMgtFileName[MAX_PATH];
	OVERLAPPED RegionStart;
	if (!GetJobMgtFileName(JobMgtFileName))
		return -1;
	/* ������������� ���������� � ���� ������ "\tmp\UserName.JobMgt" */
	hJobData = CreateFile(
		JobMgtFileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_ALWAYS | FILE_ATTRIBUTE_NORMAL, NULL
	);
	if (hJobData == INVALID_HANDLE_VALUE)
		return -1;
	/* ����������� ���� ���� ���� ���� ������ ��� ���������
	   ��������������� �������. */
	RegionStart.Offset = 0;
	RegionStart.OffsetHigh = 0;
	RegionStart.hEvent = (HANDLE)0;
	FsLow = GetFizeSize(hJobData, &FsHigh);
	LockFileEx(hJobData, LOCKFILE_EXCLUSIVE_LOCK, 0, FsLow + SJM_JOB, 0, &RegionStart);
	__try {
		/* ������ ������ ��� ���������� ������� ��������. */
		while(ReadFile(hJobData, &JobRecord, SJM_JOB, &nXfer, NULL) && nXfer > 0) {
			if (JobRecord.ProcessId == 0)
				break;
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, JobRecord.ProcessId);
			if (hProcess == NULL)
				break;
			if (GetExitCodeProcess(hProcess, &ExitCode) && (ExitCode != STILL_ACTIVE))
				break;
			JobNumber++;
		}
		/* ���� ������ ������ �������, ���� �� ��������� � ����� �����
		   � ������ ������� ����� �������. */
		if (nXfer != 0) /* �� ����� �����, �������������. */
			SetFilePointer(hJobData, -(LONG)SJM_JOB, NULL, FILE_CURRENT);
		JobRecord.ProcessId = pProcessInfo->dwProcessId;
		_tcsnccpy(JobRecord.CommandLine, Command, MAX_PATH);
		WriteFile(hJobData, &JobRecord, SJM_JOB, &nXfer, NULL);
	} /* ����� try-�����. */ __finally {
		UnlockFileEx(hJobData, 0, FsLow + SJM_JOB, 0, &RegionStart);
		CloseHandle(hJobData);
	}
	return JobNumber + 1;
}

BOOL DisplayJobs(void)
/* ���������� ���� ���� ������, �������� ������ ������. */
{
	HANDLE hJobData, hProcess;
	JM_JOB JobRecord;
	DWORD JobNumber = 0, nXfer, ExitCode, FsLow, FsHigh;
	TCHAR JobMgtFileName[MAX_PATH];
	OVERLAPPED RegionStart;
	GetJobMgtFileName(JobMgtFileName);
	hJobData = CreateFile(
		JobMgtFileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
	);
	RegionStart.Offset = 0;
	RegionStart.OffsetHigh = 0;
	RegionStart.hEvent = (HANDLE)0;
	FsLow = GetFileSize(hJobData, &FsHigh);
	LockFileEx(
		hJobData, LOCKFILE_EXCLUSIVE_LOCK,
		0, FsLow, FsHigh, &RegionStart
	);
	__try {
		while (ReadFile(hJobData, &JobRecord, SJM_JOB, &nXfer, NULL) && (nXfer > 0)) {
			JobNumber++;
			if (JobRecord.ProcessId == 0)
				continue;
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, JobRecord.ProcessId);
			if (hProcess != NULL)
				GetExitCodeProcess(hProcess, &ExitCode);
			_tprintf(_T(" [%d] "), JobNumber);
			if (hProcess == NULL)
				_tprintf(_T(" ������"));
			else if (ExitCode != STILL_ACTIVE)
				_tprintf(_T("+ ������"));
			else
				_tprintf(_T(" "));
			_tprintf(_T(" %s\n"), JobRecord.CommandLine);
			/* ������� ��������, ������� � ������� ��� �� ������������. */
			if (hProcess == NULL) { /* ��������������� ���� ������. */
				SetFilePointer(hJobData, -(LONG)nXfer, NULL, FILE_CURRENT);
				JobRecord.ProcessId = 0;
				WriteFile(hJobData, &JobRecord, SJM_JOB, &nXfer, NULL);
			}
		} /* ����� ����� while. */
	} /* ����� try-�����. */ __finally {
		UnlockFileEx(hJobData, 0, FsLow, FsHigh, &RegionStart);
		CloseHandle(hJobData);
	}
	return TRUE;
}

DWORD FindProcessId(DWORD JobNumber)
/* �������� ID �������� ��� ������ � ��������� �������. */
{
	HANDLE hJobData;
	JM_JOB JobRecord;
	DWORD nXfer;
	TCHAR JobMgtFileName[MAX_PATH];
	OVERLAPPED RegionStart;
	/* ������� ���� ���������� ��������. */
	GetJobMgtFileName(JobMgtFileName);
	hJobData = CreateFile(
		JobMgtFileName, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
	);
	if (hJobData == INVALID_HANDLE_VALUE)
		return 0;
	/* ������� � ������� ������, ��������������� ���������� ������ ������.
	   � ������ ������ ��������� �������������� �������������� ������
	   ������ (JobNumber) ����������� ��������� ��������. */
	SetFilePointer(hJobData, SJM_JOB * (JobNumber - 1), NULL, FILE_BEGIN);
	/* ���������� � ������ ������. */
	RegionStart.Offset = SJM_JOB * (JobNumber - 1);
	RegionStart.OffsetHigh = 0; /* ������������, ��� ���� "��������". */
	RegionStart.hEvent = (HANDLE)0;
	LockFileEx(hJobData, 0, 0, SJM_JOB, 0, &RegionStart);
	ReadFile(hJobData, &JobRecord, SJM_JOB, &nXfer, NULL);
	UnlockFileEx(hJobData, 0, SJM_JOB, 0, &RegionStart);
	CloseHandle(hJobData);
	return JobRecord.ProcessId;
}