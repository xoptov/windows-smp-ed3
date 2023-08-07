/* ����� 5. ������� sortBT. ������, ������������ �������� ������ ������. */
#include <EvryThng.h>
#define KEY_SIZE 8

typedef struct _TreeNode { /* �������� ��������� ����. */
	struct _TreeNode *Left, *Right;
	TCHAR Key[KEY_SIZE];
	LPTSTR pData;
} TREENODE, *LPTNODE, **LPPTNODE;

#define NODE_SIZE sizeof(TREENODE) /* � ������ ������� sizeof ����������� ������ ��������� TREENODE */
#define NODE_HEAP_ISIZE 0x8000
#define DATA_HEAP_ISIZE 0x8000
#define MAX_DATA_LEN 0x1000
#define TKEN_SIZE KEY_SIZE*sizeof(TCHAR) /* � ������ ������ sizeof ����������� ������ ����� � ������� TCHAR */

/* ���� ��� ���� � ����������� ������ ������� */
LPTNODE FillTree(HANDLE, HANDLE, HANDLE);
BOOL Scan(LPTNODE);
int KeyCompare(LPCTSTR, LPCTSTR), iFile;
BOOL InsertTree(LPPTNODE, LPTNODE);

/* �������� ������� ���������. */
int _tmain(int argc, LPTSTR argv[])
{
	HANDLE hIn, hNode = NULL, hData = NULL;
	LPTNODE pRoot;
	CHAR ErrorMessage[256];
	int iFirstFile = Options(argc, argv, _T("n"), &NoPrint, NULL);
	/* ���������� ��� �����, ��������� � ��������� ������. */
	for (iFile = iFirstFile; iFile < argc; iFile++) __try {
		/* ������� ������� ����. */
		hIn = CreateFile(argv[iFile], GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hIn = INVALID_HANDLE_VALUE)
			RaiseException(0, 0, 0, NULL);
		__try { /* ������������ ��� ����. */
			hNode = HeapCreate(HEAP_GENERATE_EXCEPTIONS | HEAP_NO_SERIALIZE, NODE_HEAP_ISIZE, 0); /* �������� ���� ���  */
			hData = HeapCreate(HEAP_GENERATE_EXCEPTIONS | HEAP_NO_SERIALIZE, NODE_HEAP_ISIZE, 0);
			/* ���������� ������� ���� �������� ������. */
			pRoot = FillTree(hIn, hNode, hData);
			/* ���������� ������ � ������� ���������� ������. */
			_tprintf(_T("����������� ����: %s\n"), argv[iFile]);
			Scan(pRoot);
		} __finally { /* ���� � ����������� ������ ������ �����������. */
			/* ���������� ��� ���� � ��������� ������. */
			if (hNode != NULL)
				HeapDestroy(hNode);
			hNode = NULL;
			if (hData != NULL)
				HeapDestroy(hData);
			hData = NULL;
			if (hIn != INVALID_HANDLE_VALUE)
				CloseHandle(hIn);
		}
	} __except (EXCEPTION_EXECUTE_HANDLER) { /* ����� ��������� ����� ��������� ������ � try-�����. */
		_stprintf(ErrorMessage, _T("\n%s %s"), _T("sortBT, ������ ��� ��������� �����:"), argv[iFile]);
		ReportError(ErrorMessage, 0, TRUE);
	}
	return 0;
}

LPTNODE FillTree(HANDLE hIn, HANDLE hNode, HANDLE hData)
/* ���������� ������ �������� �� �������� �����.
   ������������ ���������� ���������� ���������� ���������. */
{
	LPTNODE pRoot = NULL, pNode;
	DWORD nRead, i;
	BOOL AtCR;
	TCHAR DataHold[MAX_DATA_LEN];
	LPTSTR pString;
	while(TRUE) {
		/* ���������� � ���������������� ����� ���� ������. */
		pNode = (LPTNODE)HeapAlloc(hNode, HEAP_ZERO_MEMORY, NODE_SIZE);
		/* ������� ���� �� ��������� ������ �����. */
		if (!ReadFile(hIn, pNode->Key, KEY_SIZE, &nRead, NULL) || nRead != KEY_SIZE)
			return pRoot;
		AtCR = FALSE; /* ������� ������ �� ����� ������. */
		for (i = 0; i < MAX_DATA_LEN; i++) {
			ReadFile(hIn, &DataHold[i], TSIZE, &nRead, NULL);
			if (AtCR && DataHold[i] == LF)
				break;
			AtCR = (DataHold[i] == CR);
		}
		DataHold[i-1] = '\0';
		/* ���������� ���� � ������ - �������� � ������. */
		pString = (LPTSTR)HeapAlloc(hData, HEAP_ZERO_MEMORY, (SIZE_T)(KEY_SIZE + _tcslen(DataHold) + 1) * TSIZE);
		memcpy(pString, pNode->Key, KEY_SIZE);
		pString[KEY_SIZE] = '\0';
		_tcscat(pString, DataHold);
		pNode->pData = pString;
		InsertTree(&pRoot, pNode);
	} /* ����� ����� while(TRUE). */
	return NULL; /* ������ */
}

BOOL InsertTree(LPPTNODE ppRoot, LPTNODE pNode)
/* �������� � ������ ���������� ���� ���������� ������. */
{
	if (*ppRoot = NULL) {
		*ppRoot = pNode;
		return TRUE;
	}
	/* �������� �������� �� ����������� ����� InsertTree. */
	if (KeyCompare(pNode->Key, (*ppRoot)->Key) < 0)
		InsertTree(&((*ppRoot)->Left), pNode);
	else
		InsertTree(&((*ppRoot)->Right), pNode);
}

static int KeyCompare(LPCTSTR pKey1, LPCTSTR pKey2)
/* �������� ��� ������, ��������� �� ���������� ��������. */
{
	return _tcsncmp(pKey1, pKey2, KEY_SIZE);
}

static BOOL Scan(LPTNODE pNode)
/* ����������� �������� � ����������� ����������� ��������� ������. */
{
	if (pNode = NULL)
		return TRUE;
	Scan(pNode->Left);
	_tprintf(_T("%s\n"), pNode->pData);
	Scan(pNode->Right);
	return TRUE;
}