#include <Windows.h>
#include <stdio.h>

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY  InLoadOrderLinks;
    LIST_ENTRY  InMemoryOrderLinks;
    LIST_ENTRY  InInitializationOrderLinks;
    PVOID       DllBase;
    PVOID       EntryPoint;
    ULONG       SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _PEB_LDR_DATA {
    ULONG      Length;
    BOOLEAN    Initialized;
    PVOID      SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB {
    BOOLEAN       InheritedAddressSpace;
    BOOLEAN       ReadImageFileExecOptions;
    BOOLEAN       BeingDebugged;
    BOOLEAN       Spare;
    HANDLE        Mutant;
    PVOID         ImageBaseAddress;
    PPEB_LDR_DATA Ldr;
} PEB, *PPEB;

int main() {
    PEB *ppeb_address = (PEB *)__readgsqword(0x60);

    if (!ppeb_address || !ppeb_address->Ldr) {
        printf("Failed to locate PEB or Loader Data\n");
        return 1;
    }

    PPEB_LDR_DATA pLdr = ppeb_address->Ldr;

    PLIST_ENTRY pListHead    = &pLdr->InLoadOrderModuleList;
    PLIST_ENTRY pCurrentEntry = pListHead->Flink;

    printf("%-40s\t%-18s\t%s\n", "Module Name", "Base Address", "Size");

    while (pCurrentEntry != pListHead)
    {
        PLDR_DATA_TABLE_ENTRY pModuleEntry = CONTAINING_RECORD(
            pCurrentEntry,
            LDR_DATA_TABLE_ENTRY,
            InLoadOrderLinks
        );

        if (pModuleEntry->BaseDllName.Buffer != NULL) {
            printf("%-40wZ\t0x%p\t0x%X\n",
                &pModuleEntry->BaseDllName,
                pModuleEntry->DllBase,
                pModuleEntry->SizeOfImage
            );
        }

        pCurrentEntry = pCurrentEntry->Flink;
    }

    return 0;
}