/* ReportError.c */
#include "EvryThng.h"

/* Запись строк в буфер экрана консоли. */
BOOL PrintStrings(HANDLE hOut, ...)
{
	DWORD MsgLen, Count;
	LPCTSTR pMsg;
	va_list pMsgList; /* Строка текущего сообщения */
	va_start(pMsgList, hOut); /* Начать обработку сообщения */
	while ((pMsg = va_arg(pMsgList, LPCTSTR)) != NULL) {
		MsgLen = _tcslen(pMsg);
		if (!WriteConsole(hOut, pMsg, MsgLen, &Count, NULL)
			&& !WriteFile(hOut, pMsg, MsgLen * sizeof(TCHAR), &Count, NULL)
		) return FALSE;
	}
	va_end(pMsgList);
	return TRUE;
}

/* Версия PrintStrings для вывода одиночного сообщения */
BOOL PrintMsg(HANDLE hOut, LPCTSTR pMsg)
{
	return PrintStrings(hOut, pMsg, NULL);
}

/* Вывести на консоль подсказку для пользователя и получить от него ответ */
BOOL ConsolePrompt(LPCTSTR pPromptMsg, LPTSTR pResponse, DWORD MaxTchar, BOOL echo)
{
	HANDLE hStdIn, hStdOut;
	DWORD TcharIn, EchoFlag;
	BOOL Success;
	hStdIn = CreateFile(
		_T("CONIN$"),
		GENERIC_READ | GENERIC_WRITE, 0,
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL
	);
	hStdOut = CreateFile(
		_T("CONOUT$"),
		GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL
	);
	EchoFlag = echo ? ENABLE_ECHO_INPUT : 0;
	Success = SetConsoleMode(hStdIn, ENABLE_LINE_INPUT | EchoFlag | ENABLE_PROCESSED_INPUT)
		&& SetConsoleMode(hStdOut, ENABLE_WRAP_AT_EOL_OUTPUT | ENABLE_PROCESSED_OUTPUT)
		&& PrintStrings(hStdOut, pPromptMsg, NULL)
		&& ReadConsole(hStdIn, pResponse, MaxTchar, &TcharIn, NULL);
	if (Success)
		pResponse[TcharIn - 2] = '\0';
	CloseHandle(hStdIn);
	CloseHandle(hStdOut);
	return Success;
}

/* Универсальная функция для вывода сообщений о системных ошибках */
VOID ReportError(LPCTSTR UserMessage, DWORD ExitCode, BOOL PrintErrorMsg)
{
  DWORD eMsgLen, LastErr = GetLastError();
  LPTSTR lpvSysMsg;
  HANDLE hStdErr = GetStdHandle(STD_ERROR_HANDLE);
  PrintMsg(hStdErr, UserMessage);
  if (PrintErrorMsg) {
    eMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, LastErr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpvSysMsg, 0, NULL
	);
	PrintStrings(hStdErr, _T("\n"), lpvSysMsg, _T("\n"), NULL);
	HeapFree(GetProcessHeap(), 0, lpvSysMsg);
  }
  if (ExitCode > 0)
	ExitProcess(ExitCode);
  else
	return;
}

/* Расширение функции ReportError для генерации формируемого приложением кода
   исключения вместо прикращения выполнения процесса. */
/* Вывести сообщение о некретической ошибке. */
VOID ReportException(LPCTSTR UserMessage, DWORD ExceptionCode)
{
	ReportError(UserMessage, 0, TRUE);
	/* Если ошибка критическая, сгенерировать исключение. */
	if (ExceptionCode != 0)
		RaiseException((0x0FFFFFFF & ExceptionCode) | 0xE0000000, 0, 0, NULL);
	return;
}