/* ����� 8. simplePC.c */
/* ������������ ��� ������ - ������������� � �����������. */
/* ������������� ����������� ������ �������� ������ � ������������
   �������, ��� "����� ���������", ������������ ������������ �� �������
   ������������. */
#include "EvryThng.h"
#include <time.h>
#define DATA_SIZE 256

typedef struct msg_block_tag { /* ���� ���������. */
	volatile DWORD f_ready, f_stop; /* ����� ���������� � ����������� ���������. */
	volatile DWORD sequence; /* ���������� ����� ����� ���������. */
	volatile DWORD nCons, nLost;
	time_t timestamp;
	CRITICAL_SECTION mguard; /* ��������� ������ ����� ���������. */
	DWORD checksum; /* ����������� ����� ����������� ���������. */
	DWORD data[DATA_SIZE]; /* ���������� ���������. */
} MSG_BLOCK;

/* ��������� ����, �������������� � ���������� ������ �����������. */
MSG_BLOCK mblock = {0, 0, 0, 0, 0};

DWORD WINAPI produce(void *);
DWORD WINAPI consume(void *);
void MessageFill(MSG_BLOCK *);
void MessageDisplay(MSG_BLOCK *);

DWORD _tmain(DWORD argc, LPTSTR argv[])
{
	DWORD ThId;
	HANDLE produce_h, consume_h;

	/* ���������������� ����������� ������� ����� ���������. */
	InitializeCriticalSection(&mblock.mguard);

	/* ������� ��� ������. */
	produce_h = (HANDLE)_beginthreadex(NULL, 0, produce, NULL, 0, &ThId);
	consume_h = (HANDLE)_beginthreadex(NULL, 0, consume, NULL, 0, &ThId);

	/* ������� ���������� ������� ������������� � �����������. */
	WaitForSingleObject(consume_h, INFINITE);
	WaitForSingleObject(produce_h, INFINITE);
	DeleteCriticalSection(&mblock.mguard);
	
	_tprintf(_T("������ ������������� � ����������� ��������� ����������\n"));
	_tprintf(
		_T("����������L %d, ��������: %d, ���������� ������L %d\n"),
		mblock.sequence, mblock.nCons, mblock.nLost
	);
	return 0;
}

DWORD WINAPI produce(void *arg)
/* ����� ������������� - �������� ����� ��������� ����� ���������
   ��������� ��������. */
{
	srand((DWORD)time(NULL)); /* ������� ��������� ����� ��� ����������
							     ��������� �����. */
	while (!mblock.f_stop) {
		/* ��������� ��������. */
		Sleep(rand()/100);
		/* �������� � ��������� �����. */
		EnterCriticalSection(&mblock.mguard);
		__try {
			if (!mblock.f_stop) {
				mblock.f_ready = 0;
				MessageFill(&mblock);
				mblock.f_ready = 1;
				mblock.sequence++;
			}
		} __finally {
			LeaveCriticalSection(&mblock.mguard);
		}
	}
	return 0;
}

DWORD WINAPI consume(void *arg)
{
	DWORD ShutDown = 0;
	CHAR command, extra;
	/* ������� ��������� ��������� �� ������� ������������. */
	while (!ShutDown) { /* ������������ �����, ���������� �����
						   � ����������� ����������� �����/������. */
		_tprintf(_T("\n**������� 'c' ��� �����; 's' ��� ����������� ������: "));
		_tscanf("%c%c", &command, &extra);
		if (command == 's') {
			EnterCriticalSection(&mblock.mguard);
			ShutDown = mblock.f_stop = 1;
			LeaveCriticalSection(&mblock.mguard);
		} else if (command == 'c') { /* �������� ����� ������ ��� �����������
									    ���������. */
			EnterCriticalSection(&mblock.mguard);
			__try {
				if (mblock.f_ready == 0)
					_tprintf(_T("����� ��������� �����������. ��������� �������.\n"));
				else {
					MessageDisplay(&mblock);
					mblock.nCons++;
					mblock.nLost = mblock.sequence - mblock.nCons;
					mblock.f_ready = 0; /* ����� ��������� �����������. */
				}
			} __finally {
				LeaveCriticalSection(&mblock.mguard);
			}
		} else
			_tprintf(_T("����� ������� �����������. ��������� �������.\n"));
	}
	return 0;
}

void MessageFill(MSG_BLOCK *mblock)
{
	/* ��������� ����� ��������� ����������, ������� ����������� �����
	   � ������� �������. */
	DWORD i;
	mblock->checksum = 0;
	for (i = 0; i < DATA_SIZE; i++) {
		mblock->data[i] = rand();
		mblock->checksum ^= mblock->data[i];
	}
	mblock->timestamp = time(NULL);
}

void MessageDisplay(MSG_BLOCK *mblock)
{
	/* ���������� ����� ���������, ������� ������� � ����������� �����. */
	DWORD i, tcheck = 0;
	for (i = 0; i < DATA_SIZE; i++)
		tcheck ^= mblock->data[i];
	_tprintf(
		_T("\n����� ��������� ��������� � %d: %s"),
		mblock->sequence, _tctime(&(mblock->timestamp))
	);
	_tprintf(
		_T("������ � ��������� ������: %x %x\n"),
		mblock->data[0], mblock->data[DATA_SIZE-1]
	);
	if (tcheck == mblock->checksum)
		_tprintf(_T("�������� ��������� ->����������� ����� ���������.\n"));
	else {
		_tprintf(_T("���� -> ������������ ����������� �����. ��������� ���������.\n"));
	}
}