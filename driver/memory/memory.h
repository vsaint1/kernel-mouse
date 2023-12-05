#pragma once
#include "../utils/defs.h"
#include "../utils/message.h"

ULONG64 get_kernel_module(const char* module_name);

ULONG64 get_module_imagebase(int pid);

int get_process_id(const char* process_name);

uintptr_t get_module_base(int pid,UNICODE_STRING module_name);
