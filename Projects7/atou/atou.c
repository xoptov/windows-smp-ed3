/* ����� 2. atou - ����������� ������ � ��������������� ASCII � Unicode. */
#include <EvryThng.h>

BOOL Asc2Un(LPCTSTR, LPCTSTR, BOOL);

int _tmain(int argc, LPTSTR argv[])
{
	DWORD LocFileIn, LocFileOut;
	BOOL DashI = FALSE;
	TCHAR YNResp[3] = _T("y");
	/* �������� ��������� ��������� ������ � ������ �������� �����. */
	LocFileIn = Options(argc, argv, _T("i"), &DashI, NULL);
	LocFileOut = LocFileIn + 1;
	if (DashI) { // ���������� �� �������� ����?
		if (_access(argv[LocFileOut], 0) == 0) {
			_tprintf(_T("������������ ������������ ����? [y/n]"));
			_tscanf(_T("%s"), &YNResp);
			if (lstrcmp(CharLower(YNResp), YES) != 0)
				ReportError(_T("����� �� ����������"), 4, FALSE);
		}
	}
	return 0;
}