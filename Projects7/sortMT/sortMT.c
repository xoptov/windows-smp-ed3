/* Глава 7. sortMT.
   Сортировка файлов с использованием нескольких потоков (рабочая группа).
   sortMT [параметры] число_задач файл */

#include "EvryThng.h"
#define DATALEN 56 /* Данные: 56 байт; ключ: 8 байт. */
#define KEYLEN 8

typedef struct _RECORD {
	CHAR Key[KEYLEN];
	TCHAR Data[DATALEN];
} RECORD;

#define RECSIZE sizeof (RECORD)
typedef RECORD *LPRECORD;

typedef struct _THREADARG { /* Аргумент потока. */
	DWORD iTh; /* Номер потока: 0, 1, 2, ... */
	LPRECORD LowRec; /* Младшая часть указателя записи. */
	LPRECORD HighRec; /* Старшая часть указателя записи. */
} THREADARG, *PTHREADARG;

static int KeyCompare(LPCTSTR, LPCTSTR);
static DWORD WINAPI ThSort(PTHREADARG pThArg);
static DWORD nRec; /* Общее число записей, подлежащих сортировке. */
static HANDLE *ThreadHandle; /* Указатель на дексриптор потока. */

int _tmain(int argc, LPTSTR argv[])
{
	HANDLE hFile;
	LPRECORD pRecords = NULL;
	DWORD FsLow, nRead, LowRecNo, nRecTh, NPrm ThId, iTh;
	BOOL NoPrint;
	int iFF, iNP;
	PTHREADARG ThArg;
	LPTSTR StringEnd;
	iNP = Options(argc, argv, _T("n"), &NoPrint, NULL);
	iFF = iNP + 1;
	NPr = _ttoi(argv[iNP]); /* Количество потоков. */
	hFile = CreateFile(
		argv[iFF], GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, 0, NULL
	);
	FsLow = GetFileSize(hFile, NULL);
	nRec = FsLow / RECSIZE; /* Общее число записей. */
	nRecTh = nRec / NPr; /* Кол-во записей на один поток. */
	/* Распределить память для аргументов потока и массива дескрипторов
	   и выделить в памяти место для файла. Считать весь файл. */
	ThArg = malloc(NPr * sizeof(THREADARG)); /* Аргументы потоков. */
	ThreadHandle = malloc(NPr * sizeof(HANDLE));
	pRecords = malloc(FsLow + sizeof(TCHAR));
	ReadFile(hFile, pRecords, FsLow, &nRead, NULL);
	CloseHandle(hFile);
	LowRecNo = 0; /* Создать потоки, выполняющие сортировку. */
	for (iTh = 0; iTh < NPr; iTh++) {
		ThArg[iTh].iTh = iTh;
		ThArg[iTh].LowRec = pRecords + LowRecNo;
		ThArg[iTh].HighRec = pRecords + (LowRecNo + nRecTh);
		LowRecNo += nRecTh;
		ThreadHandle[iTh] = (HANDLE)_beginthreadex(
			NULL, 0, ThSort, &ThArg[iTh], CREATE_SUSPENDED, &ThId
		);
	}
	for (iTh = 0; iTh < NPr; iTh++) /* Запустить все потоки сортировки. */
		ResumeThread(ThreadHandle[iTh]);
	WaitForSingleObject(ThreadHandle[0], INFINITE);
	for (iTh = 0; iTh < NPr; iTh++)
		CloseHandle(ThreadHandle[iTh]);
	StringEnd = (LPTSTR)pRecords + FsLow;
	*StringEnd = '\0';
	if (!NoPrint)
		printf("\n%s", (LPCTSTR)pRecords);
	free(pRecords);
	free(ThArg);
	free(ThreadHandle);
	return 0;
}

static VOID MergeArrays(LPRECORD, LPRECORD);

DWORD WINAPI ThSort(PTHREADARG pThArg)
{
	DWORD GrpSize = 2, RecsInGrp, MyNumber, TwoToI = 1;
	LPRECORD First;
	MyNumber = pThArg->iTh;
	First = pThArg->LowRec;
	RecsInGrp = pThArg->HighRec - First;
	qsort(First, RecsInGrp, RECSIZE, KeyCompare);
	while ((MyNumber % GrpSize) == 0 && REcsInGrp < nRec) {
		/* Объединить слиянием отсортированные массивы. */
		WaitForSingleObject(
			ThreadHandle[MyNumber + TwoToI], INFINITE
		);
		MergeArrays(First, First + RecsInGrp);
		RecsInGrp *= 2;
		GrpSize *= 2;
		TwoToI *= 2;
	}
	_endthreadex(0);
	return 0; /* Подавить вывод предупреждающих сообщений. */
}

static VOID MergeArrays(LPRECORD p1, LPRECORD p2)
{
	DWORD iRec = 0, nRecs, i1 = 0, i2 = 0;
	LPRECORD pDest, p1Hold, pDestHold;
	nRecs = p2 - p1;
	pDest = pDestHold = malloc(2 * nRecs * RECSIZE);
	p1Hold = p1;
	while(i1 < nRecs && i2 < nRecs) {
		if (KeyCompare((LPCTSTR)p1, (LPCTSTR)p2) <= 0) {
			memcpy(pDest, p1, RECSIZE);
			i1++; p1++; pDest++;
		} else {
			memcpy(pDest, p2, RECSIZE);
			i2++; p2++; pDest++;
		}
	}
	if (i1 >= nRecs)
		memcpy(pDest, p2, RECSIZE * (nRecs - i2));
	else
		memcpy(pDest, p1, RECSIZE * (nRecs - i1));
	memcpy(p1Hold, pDestHold, 2 * nRecs * RECSIZE);
	free(pDestHold);
	return;
}