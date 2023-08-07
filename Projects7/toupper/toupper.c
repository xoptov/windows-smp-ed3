/* ����� 4. ������� toupper */
/* �������������� ����������� ������ ��� ����� ������ � ������� ���� ���� �� ���������.
   ��� ��������� ����� ���������� �� ����� �������� ����� ����������� � ���� �������� UC_. */
#include <EvryThng.h>

int _tmain(DWORD argc, LPTSTR argv[])
{
	HANDLE hIn = INVALID_HANDLE_VALUE, hOut = INVALID_HANDLE_VALUE;
	DWORD FileSize, nXfer, iFile, j;
	CHAR OutFileName[256] = "", *pBuffer = NULL;
	OVERLAPPED ov = {0, 0, 0, 0, NULL}; /* ������������ ��� ������������ ������. */
	if (argc <= 1)
		ReportError(_T("�������������: toupper �����"), 1, FALSE);
	/* ���������� ��� �����, ��������� � ��������� ������. */
	for (iFile = 1; iFile < argc; iFile++) __try { /* ���� ����������. */
		/* ��� ����������� ������ ���������������, pBuffer = NULL,
		   � ���� OutFileName ����. 
		   ���������� ���� ������� �������������� �������������. */
		_stprintf(OutFileName, "UC_%s", argv[iFile]);
		__try { /* ���������� ���� try-finally. */
			/* ������ �� ����� ���� ����������� ����������, � ���������
			   ���� ����� ���� ����� �������������� ������ ����� "������ ������".
			   ����� ������ �� �������� ������� �� ����, � ����� ����� ���������
			   �������� ������. */
			/* ������� �������� ����(���������� � ������� ���� ���� ��� ����������). */
			hIn = CreateFile(argv[iFile], GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
			if (hIn == INVALID_HANDLE_VALUE)
				ReportException(argv[iFile], 1);
			FileSize = GetFileSize(hIn, NULL);
			hOut = CreateFile(OutFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);
			if (hOut == INVALID_HANDLE_VALUE)
				ReportException(OutFileName, 1);
			/* ������������ ������ ��� ��������� �����. */
			pBuffer = malloc(FileSize);
			if (pBuffer == NULL)
				ReportException(_T("������ ������������� �������"), 1);
			/* ����������� ��� ����� ��� ����������� ����������� �����. */
			if (!LockFileEx(hIn, LOCKFILE_FAIL_IMMEDIATELY, 0, FileSize, 0, &ov))
				ReportException(_T("������ ��� ������������ �������� �����"), 1);
			if (!LockFileEx(hOut, LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY, 0, FileSize, 0, &ov))
				ReportException(_T("������ ��� ������������ ��������� �����"), 1);
			/* ������� ������, ������������� �� � �������� � �������� ����. */
			/* ���������� ������� ��� ���������� ��������� ��� ������������� ������;
			   ���������� ��������� ����. */
			if (!ReadFile(hIn, pBuffer, FileSize, &nXfer, NULL))
				ReportException(_T("������ ��� ������ �����"), 1);
			for (j = 0; j < FileSize; j++)
				if (isalpha(pBuffer[j]))
					pBuffer[j] = toupper(pBuffer[j]);
			if (!WriteFile(hOut, pBuffer, FileSize, &nXfer, NULL))
				ReportException(_T("������ ��� ������ � ����"), 1);
		} __finally {
			/* ������������ ����������, �������� ������������ ������, ������������
			   ������ � ��������� ������������� ������������ � ���������. */
			if (pBuffer != NULL)
				free(pBuffer);
			pBuffer = NULL;
			if (hIn != INVALID_HANDLE_VALUE) {
				UnlockFileEx(hIn, 0, FileSize, 0, &ov);
				CloseHandle(hIn);
				hIn = INVALID_HANDLE_VALUE;
			}
			if (hOut != INVALID_HANDLE_VALUE) {
				UnlockFileEx(hOut, 0, FileSize, 0, &ov);
				CloseHandle(hOut);
				hOut = INVALID_HANDLE_VALUE;
			}
			_tcscpy(OutFileName, _T(""));
		}
	} /* ����� ��������� ����� ��������� ������ � ����� try. */
	__except (EXCEPTION_EXECUTE_HANDLER) { /* ���������� ��������� ��� ���� �����. */
		_tprintf(_T("������ ��� ��������� ����� %s\n"), argv[iFile]);
		DeleteFile(OutFileName);
	}
	_tprintf(_T("���������� ��� �����, ����� ��������� �����.\n"));
	return 0;
}