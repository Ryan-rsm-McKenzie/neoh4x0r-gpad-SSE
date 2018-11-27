#include "IATUtils.h"


void* GetIATAddrEX(void* module, const char* searchDllName, const char* searchImportName)
{
	UInt8*						base = (UInt8*)module;
	IMAGE_DOS_HEADER*			dosHeader = (IMAGE_DOS_HEADER*)base;
	IMAGE_NT_HEADERS*			ntHeader = (IMAGE_NT_HEADERS*)(base + dosHeader->e_lfanew);
	IMAGE_IMPORT_DESCRIPTOR*	importTable = (IMAGE_IMPORT_DESCRIPTOR*)(base + ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	for (; importTable->Characteristics; ++importTable) {
		const char* dllName = (const char*)(base + importTable->Name);

		if (!_stricmp(dllName, searchDllName)) {
			// found the dll
			IMAGE_THUNK_DATA*	thunkData = (IMAGE_THUNK_DATA*)(base + importTable->OriginalFirstThunk);
			uintptr_t*			iat = (uintptr_t*)(base + importTable->FirstThunk);

			for (; thunkData->u1.Ordinal; ++thunkData, ++iat) {
				if (!IMAGE_SNAP_BY_ORDINAL(thunkData->u1.Ordinal)) {
					IMAGE_IMPORT_BY_NAME* importInfo = (IMAGE_IMPORT_BY_NAME*)(base + thunkData->u1.AddressOfData);

					if (!_stricmp((char*)importInfo->Name, searchImportName)) {
						// found the import
						return iat;
					}

				} else {
					HMODULE					lib = GetModuleHandle(searchDllName);
					IMAGE_NT_HEADERS*		ntHeader = (IMAGE_NT_HEADERS*)((BYTE*)lib + ((IMAGE_DOS_HEADER*)lib)->e_lfanew);
					IMAGE_EXPORT_DIRECTORY*	exportTable = (IMAGE_EXPORT_DIRECTORY*)((BYTE*)lib + ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
					PVOID					names = (BYTE *)lib + exportTable->AddressOfNames;
					short*					ordinals = (short*)(exportTable->AddressOfNameOrdinals + (BYTE*)lib);

					for (int i = 0; i < exportTable->NumberOfNames; i++) {
						char* export_name = (char*)(BYTE*)lib + ((DWORD*)names)[i];
						short export_ordinal = ordinals[i] + 1;
						short lookup_ordinal = (short)thunkData->u1.Ordinal;

						if (!_stricmp(export_name, searchImportName) && (export_ordinal == lookup_ordinal)) {
							// found the export
							return iat;
						}
					}
				}
			}
			return NULL;
		}
	}
	return NULL;
}
