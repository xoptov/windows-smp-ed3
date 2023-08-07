/* Глава 5. Команда sortBT. Версия, использующая бинарное дерево поиска. */
#include <EvryThng.h>
#define KEY_SIZE 8

typedef struct _TreeNode { /* Описание структуры узла. */
	struct _TreeNode *Left, *Right;
	TCHAR Key[KEY_SIZE];
	LPTSTR pData;
} TREENODE, *LPTNODE, **LPPTNODE;

#define NODE_SIZE sizeof(TREENODE) /* В данном случает sizeof расчитывает размер структуры TREENODE */
#define NODE_HEAP_ISIZE 0x8000
#define DATA_HEAP_ISIZE 0x8000
#define MAX_DATA_LEN 0x1000
#define TKEN_SIZE KEY_SIZE*sizeof(TCHAR) /* В данном случае sizeof расчитывает размер ключа в формате TCHAR */

/* Ниже идёт блок с прототипами других функций */
LPTNODE FillTree(HANDLE, HANDLE, HANDLE);
BOOL Scan(LPTNODE);
int KeyCompare(LPCTSTR, LPCTSTR), iFile;
BOOL InsertTree(LPPTNODE, LPTNODE);

/* Основная функция программы. */
int _tmain(int argc, LPTSTR argv[])
{
	HANDLE hIn, hNode = NULL, hData = NULL;
	LPTNODE pRoot;
	CHAR ErrorMessage[256];
	int iFirstFile = Options(argc, argv, _T("n"), &NoPrint, NULL);
	/* Обработать все файлы, указанные в командной строке. */
	for (iFile = iFirstFile; iFile < argc; iFile++) __try {
		/* Открыть входной файл. */
		hIn = CreateFile(argv[iFile], GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hIn = INVALID_HANDLE_VALUE)
			RaiseException(0, 0, 0, NULL);
		__try { /* Распределить две кучи. */
			hNode = HeapCreate(HEAP_GENERATE_EXCEPTIONS | HEAP_NO_SERIALIZE, NODE_HEAP_ISIZE, 0); /* Создание кучи для  */
			hData = HeapCreate(HEAP_GENERATE_EXCEPTIONS | HEAP_NO_SERIALIZE, NODE_HEAP_ISIZE, 0);
			/* Обработать входной файл создавая дерево. */
			pRoot = FillTree(hIn, hNode, hData);
			/* Отобразить дерево в порядке следования ключей. */
			_tprintf(_T("Сортируемый файл: %s\n"), argv[iFile]);
			Scan(pRoot);
		} __finally { /* Кучи и дескрипторы файлов всегда закрываются. */
			/* Уничтожить обе кучи и структуры данных. */
			if (hNode != NULL)
				HeapDestroy(hNode);
			hNode = NULL;
			if (hData != NULL)
				HeapDestroy(hData);
			hData = NULL;
			if (hIn != INVALID_HANDLE_VALUE)
				CloseHandle(hIn);
		}
	} __except (EXCEPTION_EXECUTE_HANDLER) { /* Конец основного цикла обработки файлов и try-блока. */
		_stprintf(ErrorMessage, _T("\n%s %s"), _T("sortBT, ошибка при обработке файла:"), argv[iFile]);
		ReportError(ErrorMessage, 0, TRUE);
	}
	return 0;
}

LPTNODE FillTree(HANDLE hIn, HANDLE hNode, HANDLE hData)
/* Заполнение дерева записями из входного файла.
   Используется обработчик исключений вызывающей программы. */
{
	LPTNODE pRoot = NULL, pNode;
	DWORD nRead, i;
	BOOL AtCR;
	TCHAR DataHold[MAX_DATA_LEN];
	LPTSTR pString;
	while(TRUE) {
		/* Разместить и инициализировать новый узел дерева. */
		pNode = (LPTNODE)HeapAlloc(hNode, HEAP_ZERO_MEMORY, NODE_SIZE);
		/* Считать ключ из следующей записи файла. */
		if (!ReadFile(hIn, pNode->Key, KEY_SIZE, &nRead, NULL) || nRead != KEY_SIZE)
			return pRoot;
		AtCR = FALSE; /* Считать данные до конца строки. */
		for (i = 0; i < MAX_DATA_LEN; i++) {
			ReadFile(hIn, &DataHold[i], TSIZE, &nRead, NULL);
			if (AtCR && DataHold[i] == LF)
				break;
			AtCR = (DataHold[i] == CR);
		}
		DataHold[i-1] = '\0';
		/* Объеденить ключ и данные - вставить в дерево. */
		pString = (LPTSTR)HeapAlloc(hData, HEAP_ZERO_MEMORY, (SIZE_T)(KEY_SIZE + _tcslen(DataHold) + 1) * TSIZE);
		memcpy(pString, pNode->Key, KEY_SIZE);
		pString[KEY_SIZE] = '\0';
		_tcscat(pString, DataHold);
		pNode->pData = pString;
		InsertTree(&pRoot, pNode);
	} /* Конец цикла while(TRUE). */
	return NULL; /* Ошибка */
}

BOOL InsertTree(LPPTNODE ppRoot, LPTNODE pNode)
/* Добавить в дерево одиночныйы узел содержащий данные. */
{
	if (*ppRoot = NULL) {
		*ppRoot = pNode;
		return TRUE;
	}
	/* Обратите внимание на рекурсивный вызов InsertTree. */
	if (KeyCompare(pNode->Key, (*ppRoot)->Key) < 0)
		InsertTree(&((*ppRoot)->Left), pNode);
	else
		InsertTree(&((*ppRoot)->Right), pNode);
}

static int KeyCompare(LPCTSTR pKey1, LPCTSTR pKey2)
/* Сравнить две записи, состоящие из обобщённых символов. */
{
	return _tcsncmp(pKey1, pKey2, KEY_SIZE);
}

static BOOL Scan(LPTNODE pNode)
/* Рекурсивный просмотр и отображение содержимого бинарного дерева. */
{
	if (pNode = NULL)
		return TRUE;
	Scan(pNode->Left);
	_tprintf(_T("%s\n"), pNode->pData);
	Scan(pNode->Right);
	return TRUE;
}