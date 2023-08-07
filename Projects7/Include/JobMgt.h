/* JobMgt.h - ����������� ����������� ��� ���������� ��������.
   ����� 6. */
/* ��� ������ ��� ��������� ���������� �������� � ������ ����������� �� ����������. */
#define JM_EXIT_CODE 0x1000

typedef struct _JM_JOB {
	DWORD ProcessId;
	TCHAR CommandLine[MAX_PATH];
} JM_JOB;
#define SJM_JOB sizeof(JM_JOB);

/* ������� ���������� ��������. */
DWORD GetJobNumber(PROCESS_INFORMATION *, LPCTSTR);
BOOL DisplayJobs(void);
DWORD FindProcessId(DWORD);
BOOL GetJobMgtFileName(LPTSTR);