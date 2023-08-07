#include <EvryThng.h>
#include <float.h>

DWORD Filter(LPEXCEPTION_POINTERS, LPDWORD);

int _tmain(int argc, LPTSTR argv[])
{
	DWORD ECatgry, i = 0, ix, iy = 0;
	LPDWORD pNull = NULL;
	BOOL Done = FALSE;
	DWORD FPOld, FPNew;
	FPOld = _controlfp(0, 0); /* ��������� ������ ����������� �����. */
	FPNew = FPOld & ~(EM_OVERFLOW | EM_UNDERFLOW | EM_INEXACT | EM_ZERODIVIDE | EM_DENORMAL | EM_INVALID);
	_controlfp(FPNew, MCW_EM);
	while (!Done) __try { /* ���� try-finally. */
		_tprintf(_T("������� ��� ����������: "));
		_tprintf(_T("1: Mem, 2: Int, 3: Flt, 4: User, 5: __leave"));
		_tscanf(_T("%d"), &i);
		__try { /* ���� try-except. */
			switch(i) {
				case 1: /* ���������� ��� ��������� � ������. */
					ix = *pNull;
					*pNull = 5;
					break;
				case 2: /* ���������� ��� ���������� �������������� �������� ��� ������ �������. */
					ix = ix / iy;
					break;
				case 3: /* FP-����������. */
					x = x / y;
					_tprintf(_T("x = %20e\n"), x);
					break;
				case 4: /* ���������������� ����������. */
					ReportException(_T("���������������� ����������"), 1);
					break;
				case 5: /* ������������ �������� __leave ��� ���������� ����������. */
					__leave;
				default:
					Done = TRUE;
			}
			/* ����� ����������� ����� __try */
		} __except(Filter(GetExceptionInformation(), &ECatgry)) {
			switch (ECatgry) {
				case 0:
					_tprintf(_T("����������� ����������\n"));
					break;
				case 1:
					_tprintf(_T("���������� ��� ��������� � ������\n"));
					continue;
				case 2:
					_tprintf(_T("���������� ��� ���������� �������������� �������� ��� ������ �������\n"));
					break;
				case 3:
					_tprintf(_T("FP-����������.\n"));
					_clearfp();
					break;
				case 10:
					_tprintf(_T("���������������� ����������\n"));
					break;
			} /* ����� ��������� switch. */
			/* ����� �����������. */
		} /* ����� ����� try-except. */
		/* ����� ����� while - ���� ��������� ���������� ����������. */
	} __finally { /* ��� ����� ����� while. */
		_tprintf(_T("��������� ����������?: %d\n"), AbnormalTermination());
	}
	_controlfp(FPOld, 0xFFFFFFFF); /* ������������ ������ FP-�����. */
	return 0;
}

/* ������������� ���������� � ����� ���������������� ��������. */
static DWORD Filter(LPEXCEPTION_POINTERS pExP, LPDWORD ECatgry)
{
	DWORD ExCode, ReadWrite, VirtAddr;
	ExCode = pExP->ExceptionRecord->ExceptionCode;
	_tprintf(_T("Filter. ExCode: %x\n"), ExCode);
	if ((0x20000000 | ExCode) != 0) { /* ���������������� ����������. */
		*ECatgry = 10;
		return EXCEPTION_EXECUTE_HANDLER;
	}
	switch (ExCode) {
		case EXCEPTION_ACCESS_VIOLATION:
			ReadWrite = pExP->ExceptionRecord->ExceptionInformation[0]; /* �������� ������ ��� ������ */
			VirtAddr = pExP->ExceptionRecord->ExceptionInformation[1]; /* ����� ���� � ����������� ������. */
			_tprintf(_T("��������� �������. ������/������: %d. �����: %x\n"), ReadWrite, VirtAddr);
			*ECatgry = 1;
			return EXCEPTION_EXECUTE_HANDLER;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
		case EXCEPTION_INT_OVERFLOW:
			*ECatgry = 2;
			return EXCEPTION_EXECUTE_HANDLER;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		case EXCEPTION_FLT_OVERFLOW:
			_tprintf(_T("FP-���������� - ������� ������� ��������.\n"));
			*ECatgry = 3;
			_clearfp();
			return (DWORD)EXCEPTION_EXECUTE_HANDLER;
		default:
			*ECatgry = 0;
			return EXCEPTION_CONTINUE_SEARCH;
	}
}