#include "process.h"

NTSTATUS read_virtual_memory(int pid, PVOID source_addr, PVOID target_addr, SIZE_T size) {
	SIZE_T bytes;
	NTSTATUS status = STATUS_SUCCESS;
	PEPROCESS process;

	if(!NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)pid,&process)))
		return STATUS_INVALID_PARAMETER;

	status = MmCopyVirtualMemory(process, source_addr, process, target_addr, size, KernelMode, &bytes);
	if (!NT_SUCCESS(status))
		return status;

	return status;

}

NTSTATUS write_virtual_memory(int pid, PVOID source_addr, PVOID target_addr, SIZE_T size) {
	SIZE_T bytes;
	NTSTATUS status = STATUS_SUCCESS;
	PEPROCESS process;

	if (!NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)pid, &process)))
		return STATUS_INVALID_PARAMETER;

	status = MmCopyVirtualMemory(process, source_addr, process, target_addr, size, KernelMode, &bytes);
	if (!NT_SUCCESS(status))
		return status;

	return status;
}

NTSTATUS write_safe_memory(int pid, PVOID source_addr, PVOID target_addr, SIZE_T size) {
	NTSTATUS status = STATUS_SUCCESS;
	PEPROCESS process;
	PMDL mdl = NULL;
	PVOID mapped_buffer = NULL;
	SIZE_T bytes;

	if (!NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)pid, &process)))
		return STATUS_INVALID_PARAMETER;

	mdl = IoAllocateMdl(target_addr, size, FALSE, FALSE, NULL);
	if (!mdl)
		return STATUS_INSUFFICIENT_RESOURCES;

	__try {
		MmProbeAndLockPages(mdl, KernelMode, IoReadAccess);
		mapped_buffer = MmMapLockedPagesSpecifyCache(mdl, KernelMode, MmNonCached, NULL, FALSE, NormalPagePriority);
		if (!mapped_buffer) {
			status = STATUS_INSUFFICIENT_RESOURCES;
			__leave;
		}

		RtlCopyMemory(mapped_buffer, source_addr, size);
		MmUnmapLockedPages(mapped_buffer, mdl);
		MmUnlockPages(mdl);
	}
	__finally {
		if (mdl)
			IoFreeMdl(mdl);
	}

	return status;
}