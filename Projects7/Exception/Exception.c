#include <EvryThng.h>
#include <float.h>

DWORD Filter(LPEXCEPTION_POINTERS, LPDWORD);

int _tmain(int argc, LPTSTR argv[])
{
	DWORD ECatgry, i = 0, ix, iy = 0;
	LPDWORD pNull = NULL;
	BOOL Done = FALSE;
	DWORD FPOld, FPNew;
	FPOld = _controlfp(0, 0); /* Сохранить старую управляющую маску. */
	FPNew = FPOld & ~(EM_OVERFLOW | EM_UNDERFLOW | EM_INEXACT | EM_ZERODIVIDE | EM_DENORMAL | EM_INVALID);
	_controlfp(FPNew, MCW_EM);
	while (!Done) __try { /* Блок try-finally. */
		_tprintf(_T("Введите тип исключения: "));
		_tprintf(_T("1: Mem, 2: Int, 3: Flt, 4: User, 5: __leave"));
		_tscanf(_T("%d"), &i);
		__try { /* Блок try-except. */
			switch(i) {
				case 1: /* Исключение при обращении к памяти. */
					ix = *pNull;
					*pNull = 5;
					break;
				case 2: /* Исключение при выполнении арифметических операций над целыми числами. */
					ix = ix / iy;
					break;
				case 3: /* FP-исключение. */
					x = x / y;
					_tprintf(_T("x = %20e\n"), x);
					break;
				case 4: /* Пользовательское исключение. */
					ReportException(_T("Пользовательское исключение"), 1);
					break;
				case 5: /* Использовать оператор __leave для завершения выполнения. */
					__leave;
				default:
					Done = TRUE;
			}
			/* Конец внутреннего блока __try */
		} __except(Filter(GetExceptionInformation(), &ECatgry)) {
			switch (ECatgry) {
				case 0:
					_tprintf(_T("Неизвестное исключение\n"));
					break;
				case 1:
					_tprintf(_T("Исключение при обращении к памяти\n"));
					continue;
				case 2:
					_tprintf(_T("Исключение при выполнении арифметических операций над целыми числами\n"));
					break;
				case 3:
					_tprintf(_T("FP-исключение.\n"));
					_clearfp();
					break;
				case 10:
					_tprintf(_T("Пользовательское исключение\n"));
					break;
			} /* Конец оператора switch. */
			/* Конец обработчика. */
		} /* Конец блока try-except. */
		/* Конец цикла while - ниже находится обработчик завершения. */
	} __finally { /* Это часть цикла while. */
		_tprintf(_T("Аварийное завершение?: %d\n"), AbnormalTermination());
	}
	_controlfp(FPOld, 0xFFFFFFFF); /* Восстановить старую FP-маску. */
	return 0;
}

/* Классификация исключений и выбор соответствующего действия. */
static DWORD Filter(LPEXCEPTION_POINTERS pExP, LPDWORD ECatgry)
{
	DWORD ExCode, ReadWrite, VirtAddr;
	ExCode = pExP->ExceptionRecord->ExceptionCode;
	_tprintf(_T("Filter. ExCode: %x\n"), ExCode);
	if ((0x20000000 | ExCode) != 0) { /* Пользовательское исключение. */
		*ECatgry = 10;
		return EXCEPTION_EXECUTE_HANDLER;
	}
	switch (ExCode) {
		case EXCEPTION_ACCESS_VIOLATION:
			ReadWrite = pExP->ExceptionRecord->ExceptionInformation[0]; /* Операция чтения или записи */
			VirtAddr = pExP->ExceptionRecord->ExceptionInformation[1]; /* Адрес сбоя в виртуальной памяти. */
			_tprintf(_T("Нарушение доступа. Чтение/запись: %d. Адрес: %x\n"), ReadWrite, VirtAddr);
			*ECatgry = 1;
			return EXCEPTION_EXECUTE_HANDLER;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
		case EXCEPTION_INT_OVERFLOW:
			*ECatgry = 2;
			return EXCEPTION_EXECUTE_HANDLER;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		case EXCEPTION_FLT_OVERFLOW:
			_tprintf(_T("FP-исключение - слишком большое значение.\n"));
			*ECatgry = 3;
			_clearfp();
			return (DWORD)EXCEPTION_EXECUTE_HANDLER;
		default:
			*ECatgry = 0;
			return EXCEPTION_CONTINUE_SEARCH;
	}
}