#include "memory.h"

ULONG64 get_kernel_module(const char* module_name) {
	ULONG64 module_base = 0;
	ULONG module_size = 0;
	PRTL_PROCESS_MODULES modules = NULL;
	NTSTATUS status = ZwQuerySystemInformation(0x0B, 0, 0, &module_size);

	if (status != STATUS_INFO_LENGTH_MISMATCH)
		return 0;


	modules = (PRTL_PROCESS_MODULES)ExAllocatePool2(POOL_FLAG_NON_PAGED, module_size, KM_POOL_TAG);
	if (!modules)
		return 0;

	status = ZwQuerySystemInformation(0x0B, modules, module_size, &module_size);
	if (!NT_SUCCESS(status)) {
		ExFreePoolWithTag(modules, KM_POOL_TAG);
		return 0;
	}

	PRTL_PROCESS_MODULE_INFORMATION module = modules->Modules;
	for (ULONG i = 0; i < modules->NumberOfModules; i++) {
		message("Module Name: %s\n", module[i].FullPathName + module[i].OffsetToFileName);
		message("Module Base: %p\n", module[i].ImageBase);
		message("Module Size: %d\n", module[i].ImageSize);

		if (strcmp((char*)module[i].FullPathName + module[i].OffsetToFileName, module_name) == 0) {
			module_base = (ULONG64)module[i].ImageBase;
			break;
		}
	}

	ExFreePoolWithTag(modules, KM_POOL_TAG);
	return module_base;
}

ULONG64 get_module_imagebase(int pid) {
	PEPROCESS proc;
	if (PsLookupProcessByProcessId((HANDLE)pid, &proc) != STATUS_SUCCESS)
		return 0;

	return (ULONG64)PsGetProcessSectionBaseAddress(proc);

}

// driver built for windows 10 22H2, change the offsets if you're using a different version
//https://www.vergiliusproject.com/kernels/x64/Windows%2010%20%7C%202016

int get_process_id(const char* process_name) {
	PEPROCESS proc;
	PEPROCESS sysproc = PsInitialSystemProcess;
	PLIST_ENTRY list = (PLIST_ENTRY)((char*)sysproc + ActiveProcessLinks); // _EPROCESS.ActiveProcessLinks
	PLIST_ENTRY head = list;
	do {
		proc = (PEPROCESS)((char*)list - ActiveProcessLinks); // _EPROCESS.ActiveProcessLinks
		if (strstr((char*)proc + ImageFileName, process_name)) { // _EPROCESS.ImageFileName
			return (int)PsGetProcessId(proc);
		}
		list = list->Flink;
	} while (list != head);

	return 0;
}

uintptr_t get_module_base(int pid, UNICODE_STRING module_name) {

	PEPROCESS proc;
	if (PsLookupProcessByProcessId((HANDLE)pid, &proc) != STATUS_SUCCESS)
		return 0;

	PPEB p_peb = (PPEB)PsGetProcessPeb(proc); 

	if (!p_peb)
		return 0; 

	KAPC_STATE state;

	KeStackAttachProcess(proc, &state);

	PPEB_LDR_DATA pLdr = (PPEB_LDR_DATA)p_peb->Ldr;

	if (!pLdr) {
		KeUnstackDetachProcess(&state);
		return 0; 
	}

	
	for (PLIST_ENTRY list = (PLIST_ENTRY)pLdr->InLoadOrderModuleList.Flink;
		list != &pLdr->InLoadOrderModuleList; list = (PLIST_ENTRY)list->Flink)
	{
		PLDR_DATA_TABLE_ENTRY pEntry =
			CONTAINING_RECORD(list, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);


		if (RtlCompareUnicodeString(&pEntry->BaseDllName, &module_name, TRUE) == 0) {
			message("Module Name: %wZ\n", pEntry->BaseDllName);
			message("Module Base: %p\n", pEntry->DllBase);
			message("Module Size: %d\n", pEntry->SizeOfImage);
			uintptr_t module_base = (uintptr_t)pEntry->DllBase;
			KeUnstackDetachProcess(&state);
		
			return module_base;
		}


	}

	KeUnstackDetachProcess(&state);
	message("Failed to find module\n");
	return 0; 
}
