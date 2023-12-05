#pragma once


#ifdef _DEBUG
#define message(...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[KM] - " __VA_ARGS__)
#else
#define message(...)
#endif)