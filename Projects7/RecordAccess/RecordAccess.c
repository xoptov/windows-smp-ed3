#include <EvryThng.h>
#define STRING_SIZE 256

typedef struct _RECORD { /* Структура записи в файле. */
	DWORD ReferenceCount; /* 0 означает пустую запись. */
	SYSTEMTIME RecordCreationTime; /* Время создания записи. */
	SYSTEMTIME RecordLastReferenceTime; /* Время последней ссылки на запись. */
	SYSTEMTIME RecordUpdateTime; /* Время обновления записи. */
	TCHAR DataString[STRING_SIZE]; /* Данные в виде строки. */
} RECORD;

typedef struct _HEADER { /* Структура заголовка файла. */
	DWORD NumRecords; /* Кол-во записей. */
	DWORD NumNonEmptyRecords; /* Кол-во не пустых записей. */
} HEADER;

int _tmain(int argc, LPTSTR argv[]) /* Главная функция программы. */
{
	HANDLE hFile; /* Определение локальной переменной типа структуры HANDLE. */
	LARGE_INTEGER CurPtr; /* Определение локальной переменной CurPtr типа объединения(union) LARGE_INTEGER (64bit). */
	DWORD FPos, OpenOption, nXref, RecNo;
	RECORD Record /* Определение локальной переменной типа структуры RECORD. */;
	TCHAR String[STRING_SIZE], Command, Extra;
	OVERLAPPED ov = {0, 0, 0, 0, NULL}, ovZero = {0, 0, 0, 0, NULL};
	HEADER Header = {0, 0};
	SYSTEMTIME CurrentTime;
	BOOLEAN HeaderChange, RecordChange;
	OpenOption = (argc == 2) ? OPEN_EXISTING : CREATE_ALWAYS;
	hFile = CreateFile(argv[1], GENERIC_READ | GENERIC_WRITE, 0, NULL, OpenOption, FILE_ATTRIBUTE_NORMAL, NULL);
	if (argc >= 3) { /* Записить заголовок и заранее установить размер нового файла. */
		Header.NumRecords = atoi(argv[2]);
		WriteFile(hFile, &Header, sizeof(Header), &nXref, &ovZero);
		CurPtr.QuadPart = sizeof(RECORD) * atoi(argv[2]) + sizeof(HEADER);
		FPos = SetFilePointer(hFile, CurPtr.LowPart, &CurPtr.HighPart, FILE_BEGIN);
		if (FPos == 0xFFFFFFFF && GetLastError() != NO_ERROR)
			ReportError(_T("Ошибка указателя."), 4, TRUE);
		SetEndOfFile(hFile);
	}
	/* Считать заголовок файла: определить кол-во записей и кол-во не пустых записей. */
	ReadFile(hFile, &Header, sizeof(HEADER), &nXref, &ovZero);
	/* Предложить пользователю считать или записать запись с определённым номером. */
	while(TRUE) {
		HeaderChange = FALSE;
		RecordChange = FALSE;
		_tprintf(_T("Введите r(ead)/w(rite)/d(elete)/q Запись #\n"));
		_tscanf(_T("%c" "%d" "%c"), &Command, &RecNo, &Extra);
		if (Command == 'q')
			break;
		CurPtr.QuadPart = RecNo * sizeof(RECORD) + sizeof(HEADER);
		ov.Offset = CurPtr.LowPart;
		ov.OffsetHigh = CurPtr.HighPart;
		ReadFile(hFile, &Record, sizeof(RECORD), &nXref, &ov);
		GetSystemTime(&CurrentTime);
		Record.RecordLastReferenceTime = CurrentTime;
		if (Command == 'r' || Command == 'd') {
			if (Record.ReferenceCount == 0) {
				_tprintf(_T("Запись номер %d - пустая.\n"), RecNo);
				continue;
			} else {
				_tprintf(_T("Запись номер %d. Значение счётчика: %d\n"), RecNo, Record.ReferenceCount);
				_tprintf(_T("Данные: %s\n"), Record.DataString);
				/* Упражнение: вывести метки времени . См. следующий пример. */
				RecordChange = TRUE;
			}
			if (Command == 'd') { /* Удалить запись */
				Record.ReferenceCount = 0;
				Header.NumNonEmptyRecords--;
				HeaderChange = TRUE;
				RecordChange = TRUE;
			}
		} else if (Command == 'w') {
			_tprintf(_T("Введите новую строку для записи.\n"));
			_getts(String);
			if (Record.ReferenceCount == 0) {
				Record.RecordCreationTime = CurrentTime;
				Header.NumNonEmptyRecords++;
				HeaderChange = TRUE;
			}
			Record.RecordUpdateTime = CurrentTime;
			Record.ReferenceCount++;
			_tcsncpy(Record.DataString, String, STRING_SIZE-1);
			RecordChange = TRUE;
		} else {
			_tprintf(_T("Допустимые команды: r, w и d. Повторите ввод\n."));
		}
		/* Обновить запись на месте, если её содержимое изменилось. */
		if (RecordChange)
			WriteFile(hFile, &Record, sizeof(RECORD), &nXref, &ov);
		if (HeaderChange)
			WriteFile(hFile, &Header, sizeof(Header), &nXref, &ovZero);
	}
	_tprintf(_T("Вычесленное кол-во не пустых записей: %d\n"), Header.NumNonEmptyRecords);
	CloseHandle(hFile);
	return 0;
}
