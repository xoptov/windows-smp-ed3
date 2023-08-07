/* ����� 4. Ctrlc.c */
/* ����������� ������� �������. */
#include <EvryThng.h>

static BOOL WINAPI Handler(DWORD CtrlEvent);
volatile static BOOL Exit = FALSE;

int _tmain(int argc, LPTSTR argv[])
{ /* ������������� ������ ��������� ������� �� ����������� ������� � ����������� ����������. */
	/* �������� ���������� �������. */
	if (!SetConsoleCtrlHandler(Handler, TRUE))
		ReportError(_T("������ ��� ��������� ����������� �������."), 1, TRUE);
	while (!Exit) {
		Sleep(5000); /* �������� �� 5 ������. */
		Beep(1000 /* ������� */, 250 /* ������������ */);
	}
	_tprintf(_T("����������� ���������� ��������� �� ����������.\n"));
	return 0;
}

BOOL WINAPI Handler(DWORD CtrlEvent)
{
	Exit = TRUE;
	switch (CtrlEvent) {
		case CTRL_C_EVENT:
			_tprintf(_T("������� ������ Ctrl-c. ����� �� ����������� ����� 10 ������.\n"));
			Sleep(4000);
			_tprintf(_T("����� �� ����������� ����� 6 ������.\n"));
			Sleep(6000);
			return TRUE;
		case CTRL_CLOSE_EVENT:
			_tprintf(_T("������� ������ Close. ����� �� ����������� ����� 10 ������.\n"));
			Sleep(4000);
			_tprintf(_T("����� �� ����������� ����� 6 ������.\n"));
			Sleep(6000);
			return TRUE;
		default:
			_tprintf(_T("�������: %d. ����� �� ����������� ����� 10 ������.\n"));
			Sleep(4000);
			_tprintf(_T("����� �� ����������� ����� 6 ������.\n"));
			Sleep(6000);
			return TRUE;
	}
}