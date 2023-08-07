/* ����� 5. ������ atou, ������������ ����� ����������. */
#include "EvryThng.h"

int _tmain(int argc, LPTSTR argv[])
{
	/* �������� ���������� Asc2Un ��� �������. */
	BOOL (*Asc2Un)(LPCTSTR, LPCTSTR, BOOL);
	DWORD LocFileIn, LocFileOut, LocDLL, DashI;
	HINSTANCE hDLL;
	FARPROC pA2U;
	LocFileIn = Options(argc, argv, _T("i"), &DashI, NULL);
	LocFileOut = LocFileIn + 1;
	LocDLL = LocFileOut + 1;
	/* ��������� ������������� �����, � ����� ������ �� �������� DashI. */
	/* ��������� ������� �������������� ASCII � Unicode. */
	hDLL = LoadLibrary(argv[LocDLL]);
	if (hDLL == NULL)
		ReportError(_T("�� ������ ��������� DLL."), 1, TRUE);
	/* �������� ����� ����� �����. */
	pA2U = GetProcAddress(hDLL, "Asc2Un");
	if (pA2U == NULL)
		ReportError(_T("��������� ����� �����."), 2, TRUE);
	/* �������� ���� ���������. ����� ����� ������������ typedef. */
	Asc2Un = (BOOL (*)(LPCTSTR, LPCTSTR, BOOL))pA2U;
	/* ����� �������. */
	Asc2Un(argv[LocFileIn], argv[LocFileOut], FALSE);
	FreeLibrary(hDLL);
	return 0;
}