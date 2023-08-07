/* Глава 8. eventPC.c */
/* 
 * Поддерживает два потока - производителя и потребителя.
 * Производитель переодически создаёт буферные данные с контрольными
 * суммами, или "блоки сообщений", сигнализирующие потребителю о готовности
 * сообщения. Поток потребителя отображает информацию в ответ на запрос.
 */

#include "EvryThng.h"
#include <time.h>
#define DATA_SIZE 256

typedef struct msg_block_tag {
	volatile DWORD f_ready, f_stop;
	/* Флаги готовности и прекращения сообщений. */
	volatile DWORD sequence; /* Порядковый номер блока сообщения. */
	volatile DWORD nCons, nLost;
	time_t timestamp;
	HANDLE mguard; /* Мьютекс, защищающий структуру блока сообщения. */
	HANDLE mready; /* Событие "Сообщение готово". */
	DWORD checksum; /* Контрольная сумма сообщения. */
	DWORD data[DATA_SIZE]; /* Содержимое сообщения. */
} MSG_BLOCK;

/* ... */

DWORD _tmain(DWORD argc, LPTSTR argv[])
{
	DWORD Status, ThId;
	HANDLE produce_h, consume_h;
	/* Инициализировать мьютекс и событие (автоматически сбрасываемое)
	   в блоке сообщения. */
	mblock.mguard = CreateMutex(NULL, FALSE, NULL);
	mblock.mready = CreateEvent(NULL, FALSE, FALSE, NULL);
	/* Создать потоки производителя и потребителя; ожидать их завершения. */
	/* ... Как в программе 9.1 ... */
	CloseHandle(mblock.mguard);
	CloseHandle(mblock.mready);
	_tprintf(_T("Потоки производителя и потребителя завершили выполнение\n"));
	_tprintf(
		_T("Отправлено: %d, Получено: %d, Известные потери: %d\n"),
		mblock.sequence, mblock.nCons, mblock.nLost
	);
	return 0;
}

DWORD WINAPI produce(void *arg)
/* Поток производителя -- создание новых сообщений через случайные
   интервалы времени. */
{
	srand((DWORD)time(NULL)); /* Создать начальное число для генератора
							     случайных чисел. */
	while(!mblock.f_stop)
	{
		/* Случайная задержка. */
		Sleep(rand()/10); /* Длительный период ожидания следующего
						     сообщения. */
		/* Получить и заполнить буфер. */
		WaitForSingleObject(mblock.mguard, INFINITE);
		__try {
			if (!mblock.f_stop) {
				mblock.f_ready = 0;
				MessageFill(&mblock);
				mblock.f_ready = 1;
				mblock.sequence++;
				SetEvent(mblock.mready); /* Сигнал "Сообщение готово". */
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
	/* Принять ОЧЕРЕДНОЕ сообщение по запросу пользователя. */
	while(!ShutDown) { /* Единственный поток, получающий доступ к
					      стандартным устройствам ввода/вывода. */
		_tprintf(_T("\n** Введите 'c' для приёма; 's' для прекращения работы: "));
		_tscanf("%c%c", &command, &extra);
		if (command == 's') {
			WaitForSingleObject(mblock.mguard, INFINITE);
			ShutDown = mblock.f_stop = 1;
			ReleaseMutex(mblock.mguard);
		} else if (command == 'c') { /* получить новый буфер принимаемых
									    сообщений. */
			WaitForSingleObject(mblock.mready, INFINITE);
			WaitForSingleObject(mblock.mguard, INFINITE);
			__try {
				if (!mblock.f_ready)
					_leave;
				/* Ожидать наступление собятия, указывающего на готовность
				   сообщения. */
				MessageDisplay(&mblock);
				mblock.nCons++;
				mblock.nLost = mblock.sequence - mblock.nCons;
				mblock.f_ready = 0; /* Новые готовые сообщения отсутствуют. */
			} __finally {
				ReleaseMutex(mblock.mguard);
			}
		} else {
			_tprintf(_T("Недопустимая команда. Повторите попытку.\n"));
		}
	}
	return 0;
}