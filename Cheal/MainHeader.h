#pragma once
#include <wchar.h>
#include <Windows.h>
#include <process.h>
#include <tlhelp32.h>
#include <ShlObj.h>
#include <MemoryFunctions.h>

#pragma comment(linker, "/MERGE:.data=.text")
#pragma comment(linker, "/MERGE:.rdata=.text")
#pragma comment(linker, "/SECTION:.text,EWR")