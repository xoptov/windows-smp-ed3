/* Глава 5. Версия atou, использующая явное связывание. */
#include "EvryThng.h"

int _tmain(int argc, LPTSTR argv[])
{
	/* Объявить переменную Asc2Un как функцию. */
	BOOL (*Asc2Un)(LPCTSTR, LPCTSTR, BOOL);
	DWORD LocFileIn, LocFileOut, LocDLL, DashI;
	HINSTANCE hDLL;
	FARPROC pA2U;
	LocFileIn = Options(argc, argv, _T("i"), &DashI, NULL);
	LocFileOut = LocFileIn + 1;
	LocDLL = LocFileOut + 1;
	/* Проверить существование файла, а также опущен ли параметр DashI. */
	/* Загрузить функцию преобразования ASCII в Unicode. */
	hDLL = LoadLibrary(argv[LocDLL]);
	if (hDLL == NULL)
		ReportError(_T("Не удаётся загрузить DLL."), 1, TRUE);
	/* Получить адрес точки входа. */
	pA2U = GetProcAddress(hDLL, "Asc2Un");
	if (pA2U == NULL)
		ReportError(_T("Ненайдена точка входа."), 2, TRUE);
	/* Привести тиип указателя. Здесь можно использовать typedef. */
	Asc2Un = (BOOL (*)(LPCTSTR, LPCTSTR, BOOL))pA2U;
	/* Вызов функции. */
	Asc2Un(argv[LocFileIn], argv[LocFileOut], FALSE);
	FreeLibrary(hDLL);
	return 0;
}