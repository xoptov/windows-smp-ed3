/* Глава 8. simplePC.c */
/* Поддерживает два потока - производителя и потребителя. */
/* Производитель преодически создаёт буферные данные с контрольными
   суммами, или "блоки сообщений", отображаемые потребителем по запросу
   пользователя. */
#include "EvryThng.h"
#include <time.h>
#define DATA_SIZE 256

typedef struct msg_block_tag { /* Блок сообщения. */
	volatile DWORD f_ready, f_stop; /* Флаги готовности и прекращения сообщений. */
	volatile DWORD sequence; /* Порядковый номер блока сообщения. */
	volatile DWORD nCons, nLost;
	time_t timestamp;
	CRITICAL_SECTION mguard; /* Структура защиты блока сообщения. */
	DWORD checksum; /* Контрольная сумма содержимого сообщения. */
	DWORD data[DATA_SIZE]; /* Содержимое сообщения. */
} MSG_BLOCK;

/* Одиночный блок, подготовленный к заполнению новыми сообщениями. */
MSG_BLOCK mblock = {0, 0, 0, 0, 0};

DWORD WINAPI produce(void *);
DWORD WINAPI consume(void *);
void MessageFill(MSG_BLOCK *);
void MessageDisplay(MSG_BLOCK *);

DWORD _tmain(DWORD argc, LPTSTR argv[])
{
	DWORD ThId;
	HANDLE produce_h, consume_h;

	/* Инициализировать критический участок блока сообщения. */
	InitializeCriticalSection(&mblock.mguard);

	/* Создать два потока. */
	produce_h = (HANDLE)_beginthreadex(NULL, 0, produce, NULL, 0, &ThId);
	consume_h = (HANDLE)_beginthreadex(NULL, 0, consume, NULL, 0, &ThId);

	/* Ожидать завершения потоков производителя и потребителя. */
	WaitForSingleObject(consume_h, INFINITE);
	WaitForSingleObject(produce_h, INFINITE);
	DeleteCriticalSection(&mblock.mguard);
	
	_tprintf(_T("Потоки производителя и потребителя завершили выполнение\n"));
	_tprintf(
		_T("ОтправленоL %d, Получено: %d, Иизвестные потериL %d\n"),
		mblock.sequence, mblock.nCons, mblock.nLost
	);
	return 0;
}

DWORD WINAPI produce(void *arg)
/* Поток производителя - создание новых сообщений через случайные
   интервалы веремени. */
{
	srand((DWORD)time(NULL)); /* Создать начальное число для генератора
							     случайных чисел. */
	while (!mblock.f_stop) {
		/* Случайная задержка. */
		Sleep(rand()/100);
		/* Получить и заполнить буфер. */
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
	/* Принять ОЧЕРЕДНОЕ сообщение по запросу пользователя. */
	while (!ShutDown) { /* Единственный поток, получающий досуп
						   к стандартным устройствам ввода/вывода. */
		_tprintf(_T("\n**Введите 'c' для приёма; 's' для прикращения работы: "));
		_tscanf("%c%c", &command, &extra);
		if (command == 's') {
			EnterCriticalSection(&mblock.mguard);
			ShutDown = mblock.f_stop = 1;
			LeaveCriticalSection(&mblock.mguard);
		} else if (command == 'c') { /* Получить новый буффер для принимаемых
									    сообщений. */
			EnterCriticalSection(&mblock.mguard);
			__try {
				if (mblock.f_ready == 0)
					_tprintf(_T("Новые сообщения отсутствуют. Повторите попытку.\n"));
				else {
					MessageDisplay(&mblock);
					mblock.nCons++;
					mblock.nLost = mblock.sequence - mblock.nCons;
					mblock.f_ready = 0; /* Новые сообщения отсутствуют. */
				}
			} __finally {
				LeaveCriticalSection(&mblock.mguard);
			}
		} else
			_tprintf(_T("Такая команда отсутствует. Повторите попытку.\n"));
	}
	return 0;
}

void MessageFill(MSG_BLOCK *mblock)
{
	/* Заполнить буфер сообщения содержимым, включая контрольную сумму
	   и отметку времени. */
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
	/* Отобразить буфер сообщения, отметку времени и контрольную сумму. */
	DWORD i, tcheck = 0;
	for (i = 0; i < DATA_SIZE; i++)
		tcheck ^= mblock->data[i];
	_tprintf(
		_T("\nВремя генерации сообщения № %d: %s"),
		mblock->sequence, _tctime(&(mblock->timestamp))
	);
	_tprintf(
		_T("Первая и последняя записи: %x %x\n"),
		mblock->data[0], mblock->data[DATA_SIZE-1]
	);
	if (tcheck == mblock->checksum)
		_tprintf(_T("УСПЕШНАЯ ОБРАБОТКА ->Контрольная сумма совпадает.\n"));
	else {
		_tprintf(_T("СБОЙ -> Несовпадение контрольной суммы. Сообщение запорчено.\n"));
	}
}