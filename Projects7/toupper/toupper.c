/* Глава 4. Команда toupper */
/* Преобразование содержимого одного или более файлов с заменой всех букв на прописные.
   Имя выходного файла получается из имени входного файла добавлением к непу префикса UC_. */
#include <EvryThng.h>

int _tmain(DWORD argc, LPTSTR argv[])
{
	HANDLE hIn = INVALID_HANDLE_VALUE, hOut = INVALID_HANDLE_VALUE;
	DWORD FileSize, nXfer, iFile, j;
	CHAR OutFileName[256] = "", *pBuffer = NULL;
	OVERLAPPED ov = {0, 0, 0, 0, NULL}; /* Используется для блокирования файлов. */
	if (argc <= 1)
		ReportError(_T("Использование: toupper файлы"), 1, FALSE);
	/* Обработать все файлы, указанные в командной строке. */
	for (iFile = 1; iFile < argc; iFile++) __try { /* Блок исключений. */
		/* Все дескрипторы файлов недействительны, pBuffer = NULL,
		   а файл OutFileName пуст. 
		   Выполнение этих условий обеспечивается обработчиками. */
		_stprintf(OutFileName, "UC_%s", argv[iFile]);
		__try { /* Внутренний блок try-finally. */
			/* Ошибка на любом шаге сгенерирует исключение, и следующий
			   файл будет файл будет обрабатываться только после "уборки мусора".
			   Объём работы по отчистки зависит от того, в каком месте программы
			   возникла ошибка. */
			/* Создать выходной файл(завершится с ошибкой если файл уже существует). */
			hIn = CreateFile(argv[iFile], GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
			if (hIn == INVALID_HANDLE_VALUE)
				ReportException(argv[iFile], 1);
			FileSize = GetFileSize(hIn, NULL);
			hOut = CreateFile(OutFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);
			if (hOut == INVALID_HANDLE_VALUE)
				ReportException(OutFileName, 1);
			/* Распределить память под содержмое файла. */
			pBuffer = malloc(FileSize);
			if (pBuffer == NULL)
				ReportException(_T("Ошибка распределения памятии"), 1);
			/* Блокировать оба файла для обеспечения целостности копии. */
			if (!LockFileEx(hIn, LOCKFILE_FAIL_IMMEDIATELY, 0, FileSize, 0, &ov))
				ReportException(_T("Ошибка при блокировании входного файла"), 1);
			if (!LockFileEx(hOut, LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY, 0, FileSize, 0, &ov))
				ReportException(_T("Ошибка при блокировании выходного файла"), 1);
			/* Считать данные, приобразовать их и записать в выходной файл. */
			/* Освободить ресурсы при завершении обработки или возникновении ошибки;
			   обработать следующий файл. */
			if (!ReadFile(hIn, pBuffer, FileSize, &nXfer, NULL))
				ReportException(_T("Ошибка при чтении файла"), 1);
			for (j = 0; j < FileSize; j++)
				if (isalpha(pBuffer[j]))
					pBuffer[j] = toupper(pBuffer[j]);
			if (!WriteFile(hOut, pBuffer, FileSize, &nXfer, NULL))
				ReportException(_T("Ошибка при записи в файл"), 1);
		} __finally {
			/* Освобождение блокировок, закрытие дескрипторов файлов, освобождение
			   памяти и повторная инициализация дескрипторов и указателя. */
			if (pBuffer != NULL)
				free(pBuffer);
			pBuffer = NULL;
			if (hIn != INVALID_HANDLE_VALUE) {
				UnlockFileEx(hIn, 0, FileSize, 0, &ov);
				CloseHandle(hIn);
				hIn = INVALID_HANDLE_VALUE;
			}
			if (hOut != INVALID_HANDLE_VALUE) {
				UnlockFileEx(hOut, 0, FileSize, 0, &ov);
				CloseHandle(hOut);
				hOut = INVALID_HANDLE_VALUE;
			}
			_tcscpy(OutFileName, _T(""));
		}
	} /* Конец основного цикла обработки файлов и блока try. */
	__except (EXCEPTION_EXECUTE_HANDLER) { /* Обработчик исключеня для тела цикла. */
		_tprintf(_T("Ошибка при обработке файла %s\n"), argv[iFile]);
		DeleteFile(OutFileName);
	}
	_tprintf(_T("Обработаны все файлы, кроме указанных высше.\n"));
	return 0;
}