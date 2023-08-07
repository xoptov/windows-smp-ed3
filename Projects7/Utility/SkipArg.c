/* SkipArg.c
   ѕропуск одного аргумента командной строки - символы табул€ции и пробела пропускаютс€. */
#include <EvryThng.h>

#define TSPACE '\0x20'
#define TAB '\t'

LPTSTR SkipArg(LPCTSTR targv)
{
	LPTSTR p;
	p = (LPTSTR)targv;
	/* ѕерейти к следующему символу табул€ции и пробела. */
	while (*p != '\0' && *p != TSPACE && *p != TAB) p++;
	/* ѕропустить символы табул€ции и пробела и перейти к следующему аргументу. */
	while (*p != '\0' && (*p != TSPACE || *p == TAB)) p++;
	return p;
}
