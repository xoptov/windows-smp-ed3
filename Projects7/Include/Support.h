#if defined(UTILITY_3_0_EXPORTS)
#define LIBSPEC __declspec (dllexport)
#elif defined (__cplusplus)
#define LIBSPEC extern "C" __declspec (dllimport)
#else
#define LIBSPEC __declspec (dllimport)
#endif

#define EMPTY _T("")
#define YES _T("y")
#define NO _T("n")
#define CR 0x0D
#define LF 0x0A
#define TSIZE sizeof(TCHAR)

/* Предельные значение я константы */
#define TYPE_FILE 1
#define TYPE_DIR 2
#define TYPE_DOT 3
#define MAX_OPTIONS 20
#define MAX_ARG 1000
#define MAX_COMMAND_LINE MAX_PATH+50

/* Часто ипользуемые функции */
LIBSPEC BOOL ConsolePrompt(LPCTSTR, LPTSTR, DWORD, BOOL);
LIBSPEC BOOL PrintStrings(HANDLE, ...);
LIBSPEC BOOL PrintMsg(HANDLE, LPCTSTR);
LIBSPEC VOID ReportError(LPCTSTR, DWORD, BOOL);
LIBSPEC VOID ReportException(LPCTSTR, DWORD);
LIBSPEC DWORD Options(int, LPCTSTR *, LPCTSTR, ...);
LIBSPEC LPTSTR SkipArg(LPCTSTR);
LIBSPEC VOID GetArgs(LPCTSTR, int *, LPTSTR *);

/* Набор функций для работы с обобщёнными строками */
LIBSPEC LPCTSTR wmemchr(LPCTSTR, TCHAR, DWORD);

#ifdef _UNICODE
#define _strrchr wcsrchr
#else
#define _strrchr strrchr
#endif

#ifdef _UNICODE
#define _memtchr wmemchr
#else
#define _memtchr memchr
#endif

/* Функции безопасности */
LPSECURITY_ATTRIBUTES InitializeUnixSA(DWORD, LPTSTR, LPTSTR, LPDWORD, LPHANDLE);
LPSECURITY_ATTRIBUTES InitializeAccessOnlySA(DWORD, LPTSTR, LPTSTR, LPDWORD, LPHANDLE);
DWORD ReadFilePermissions(LPTSTR, LPTSTR, LPTSTR);
BOOL ChangeFilePermissions(DWORD, LPTSTR, LPDWORD, LPDWORD);

/* Констаноты которые требуются для функций безопасности */
#define LUSIZE 1024
#define ACCT_NAME_SIZE LUSIZE
