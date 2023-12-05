#pragma once
#include "../utils/defs.h"

NTSTATUS read_virtual_memory(int pid, PVOID source_addr, PVOID target_addr, SIZE_T size);

NTSTATUS write_virtual_memory(int pid, PVOID source_addr, PVOID target_addr, SIZE_T size);

NTSTATUS write_safe_memory(int pid, PVOID source_addr, PVOID target_addr, SIZE_T size);