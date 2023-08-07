/* Глава 2. pwd - вывод на печать содержимого рабочего каталога. */
#include <EvryThng.h>
#define DIRNAME_LEN MAX_PATH+2

int _tmain(int argc, LPTSTR argv[])
{
	TCHAR pwdBuffer[DIRNAME_LEN];
	DWORD LenCurDir;
	LenCurDir = GetCurrentDirectory(DIRNAME_LEN, pwdBuffer);
	if (LenCurDir == 0)
		ReportError(_T("Не удалось получить путь."), 1, TRUE);
	if (LenCurDir > DIRNAME_LEN)
		ReportError(_T("Слишком длинный путь."), 2, FALSE);
	PrintMsg(GetStdHandle(STD_OUTPUT_HANDLE), pwdBuffer);
	return 0;
}