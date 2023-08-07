/* SkipArg.c
   ������� ������ ��������� ��������� ������ - ������� ��������� � ������� ������������. */
#include <EvryThng.h>

#define TSPACE '\0x20'
#define TAB '\t'

LPTSTR SkipArg(LPCTSTR targv)
{
	LPTSTR p;
	p = (LPTSTR)targv;
	/* ������� � ���������� ������� ��������� � �������. */
	while (*p != '\0' && *p != TSPACE && *p != TAB) p++;
	/* ���������� ������� ��������� � ������� � ������� � ���������� ���������. */
	while (*p != '\0' && (*p != TSPACE || *p == TAB)) p++;
	return p;
}
