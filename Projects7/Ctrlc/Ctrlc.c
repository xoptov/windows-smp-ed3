/* Глава 4. Ctrlc.c */
/* Перехватчик событий консоли. */
#include <EvryThng.h>

static BOOL WINAPI Handler(DWORD CtrlEvent);
volatile static BOOL Exit = FALSE;

int _tmain(int argc, LPTSTR argv[])
{ /* Переодическая подача звукового сигнала до поступления сигнала о прикращении выполнении. */
	/* Добавить обработчик события. */
	if (!SetConsoleCtrlHandler(Handler, TRUE))
		ReportError(_T("Ошибка при установке обработчика событий."), 1, TRUE);
	while (!Exit) {
		Sleep(5000); /* Засыпаем на 5 секунд. */
		Beep(1000 /* Частота */, 250 /* Длительность */);
	}
	_tprintf(_T("Прекращение выполнения программы по требованию.\n"));
	return 0;
}

BOOL WINAPI Handler(DWORD CtrlEvent)
{
	Exit = TRUE;
	switch (CtrlEvent) {
		case CTRL_C_EVENT:
			_tprintf(_T("Получен сигнал Ctrl-c. Выход из обработчика через 10 секунд.\n"));
			Sleep(4000);
			_tprintf(_T("Выход из обработчика через 6 секунд.\n"));
			Sleep(6000);
			return TRUE;
		case CTRL_CLOSE_EVENT:
			_tprintf(_T("Получен сигнал Close. Выход из обработчика через 10 секунд.\n"));
			Sleep(4000);
			_tprintf(_T("Выход из обработчика через 6 секунд.\n"));
			Sleep(6000);
			return TRUE;
		default:
			_tprintf(_T("Событие: %d. Выход из обработчика через 10 секунд.\n"));
			Sleep(4000);
			_tprintf(_T("Выход из обработчика через 6 секунд.\n"));
			Sleep(6000);
			return TRUE;
	}
}