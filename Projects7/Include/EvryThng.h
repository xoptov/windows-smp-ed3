#include "Exclude.h"
#include "Envirmnt.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <io.h>
#include "Support.h"

#ifdef _MT
#include <process.h>
#endif

#if !defined(_Wp64)
#define DWORD_PTR DWORD
#define LONG_PTR LONG
#define INT_PTR INT
#endif