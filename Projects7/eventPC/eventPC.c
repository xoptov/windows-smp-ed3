/* ����� 8. eventPC.c */
/* 
 * ������������ ��� ������ - ������������� � �����������.
 * ������������� ������������ ������ �������� ������ � ������������
 * �������, ��� "����� ���������", ��������������� ����������� � ����������
 * ���������. ����� ����������� ���������� ���������� � ����� �� ������.
 */

#include "EvryThng.h"
#include <time.h>
#define DATA_SIZE 256

typedef struct msg_block_tag {
	volatile DWORD f_ready, f_stop;
	/* ����� ���������� � ����������� ���������. */
	volatile DWORD sequence; /* ���������� ����� ����� ���������. */
	volatile DWORD nCons, nLost;
	time_t timestamp;
	HANDLE mguard; /* �������, ���������� ��������� ����� ���������. */
	HANDLE mready; /* ������� "��������� ������". */
	DWORD checksum; /* ����������� ����� ���������. */
	DWORD data[DATA_SIZE]; /* ���������� ���������. */
} MSG_BLOCK;

/* ... */

DWORD _tmain(DWORD argc, LPTSTR argv[])
{
	DWORD Status, ThId;
	HANDLE produce_h, consume_h;
	/* ���������������� ������� � ������� (������������� ������������)
	   � ����� ���������. */
	mblock.mguard = CreateMutex(NULL, FALSE, NULL);
	mblock.mready = CreateEvent(NULL, FALSE, FALSE, NULL);
	/* ������� ������ ������������� � �����������; ������� �� ����������. */
	/* ... ��� � ��������� 9.1 ... */
	CloseHandle(mblock.mguard);
	CloseHandle(mblock.mready);
	_tprintf(_T("������ ������������� � ����������� ��������� ����������\n"));
	_tprintf(
		_T("����������: %d, ��������: %d, ��������� ������: %d\n"),
		mblock.sequence, mblock.nCons, mblock.nLost
	);
	return 0;
}

DWORD WINAPI produce(void *arg)
/* ����� ������������� -- �������� ����� ��������� ����� ���������
   ��������� �������. */
{
	srand((DWORD)time(NULL)); /* ������� ��������� ����� ��� ����������
							     ��������� �����. */
	while(!mblock.f_stop)
	{
		/* ��������� ��������. */
		Sleep(rand()/10); /* ���������� ������ �������� ����������
						     ���������. */
		/* �������� � ��������� �����. */
		WaitForSingleObject(mblock.mguard, INFINITE);
		__try {
			if (!mblock.f_stop) {
				mblock.f_ready = 0;
				MessageFill(&mblock);
				mblock.f_ready = 1;
				mblock.sequence++;
				SetEvent(mblock.mready); /* ������ "��������� ������". */
			}
		} __finally {
			ReleaseMutex(mblock.mguard);
		}
	}
	return 0;
}

DWORD WINAPI consume(void *arg)
{
	DWORD ShutDown = 0;
	CHAR command, extra;
	/* ������� ��������� ��������� �� ������� ������������. */
	while(!ShutDown) { /* ������������ �����, ���������� ������ �
					      ����������� ����������� �����/������. */
		_tprintf(_T("\n** ������� 'c' ��� �����; 's' ��� ����������� ������: "));
		_tscanf("%c%c", &command, &extra);
		if (command == 's') {
			WaitForSingleObject(mblock.mguard, INFINITE);
			ShutDown = mblock.f_stop = 1;
			ReleaseMutex(mblock.mguard);
		} else if (command == 'c') { /* �������� ����� ����� �����������
									    ���������. */
			WaitForSingleObject(mblock.mready, INFINITE);
			WaitForSingleObject(mblock.mguard, INFINITE);
			__try {
				if (!mblock.f_ready)
					_leave;
				/* ������� ����������� �������, ������������ �� ����������
				   ���������. */
				MessageDisplay(&mblock);
				mblock.nCons++;
				mblock.nLost = mblock.sequence - mblock.nCons;
				mblock.f_ready = 0; /* ����� ������� ��������� �����������. */
			} __finally {
				ReleaseMutex(mblock.mguard);
			}
		} else {
			_tprintf(_T("������������ �������. ��������� �������.\n"));
		}
	}
	return 0;
}