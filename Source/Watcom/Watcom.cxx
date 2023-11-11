/*
Copyright (c) 2023 Americus Maximus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Watcom.hxx"

#include <stdio.h>
#include <stdlib.h>

// REFERENCES:
// https://dev.to/wireless90/exploring-the-export-table-windows-pe-internals-4l47
// https://stackoverflow.com/questions/72839654/manually-loading-pe-imports-results-error-when-parsing-pimage-import-descriptor

PIMAGE_NT_HEADERS AcquireHeader(const BYTE* data)
{
    const PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)data;

    if (dos->e_magic != IMAGE_DOS_SIGNATURE) { return NULL; };

    const PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(data + dos->e_lfanew);

    if (nt->Signature != IMAGE_NT_SIGNATURE) { return NULL; };

#if _MSC_VER == 1200
    if (nt->FileHeader.Machine != IMAGE_FILE_MACHINE_I386) { return NULL; }
#else
    if (nt->FileHeader.Machine != IMAGE_FILE_MACHINE_I386 && nt->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64) { return NULL; }
#endif

    return nt;
}

PIMAGE_FILE_HEADER AcquireFileHeader(const PIMAGE_NT_HEADERS header)
{
    return &header->FileHeader;
}

PIMAGE_OPTIONAL_HEADER AcquireOptionalHeader(const PIMAGE_NT_HEADERS header)
{
    return &header->OptionalHeader;
}

PIMAGE_SECTION_HEADER AcquireSectionHeader(const PIMAGE_OPTIONAL_HEADER optional, const PIMAGE_FILE_HEADER file)
{
    return (PIMAGE_SECTION_HEADER)((BYTE*)optional + file->SizeOfOptionalHeader);
}

PIMAGE_DATA_DIRECTORY AcquireExportDataDirectory(const PIMAGE_OPTIONAL_HEADER header)
{
    return &header->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
}

UINT RelativeVirtualAddressToFileOffset(const BYTE* data, UINT rva)
{
    const PIMAGE_NT_HEADERS nt = AcquireHeader(data);

    const PIMAGE_FILE_HEADER file = AcquireFileHeader(nt);
    const PIMAGE_OPTIONAL_HEADER optional = AcquireOptionalHeader(nt);
    const PIMAGE_SECTION_HEADER sections = AcquireSectionHeader(optional, file);

    for (unsigned int x = 0; x < file->NumberOfSections; x++)
    {
        const PIMAGE_SECTION_HEADER section = &sections[x];

        if (rva >= section->VirtualAddress && rva <= section->VirtualAddress + section->Misc.VirtualSize)
        {
            return section->PointerToRawData + rva - section->VirtualAddress;
        }
    }

    return UINT_MAX;
}

PIMAGE_EXPORT_DIRECTORY AcquireExports(const BYTE* data, const PIMAGE_DATA_DIRECTORY dir)
{
    const auto addr = RelativeVirtualAddressToFileOffset(data, dir->VirtualAddress);

    if (addr == UINT_MAX) { return NULL; }

    return (PIMAGE_EXPORT_DIRECTORY)(data  + addr);
}

UINT* AcquireExportNameAddresses(const BYTE* data, const PIMAGE_EXPORT_DIRECTORY dir)
{
    const auto addr = RelativeVirtualAddressToFileOffset(data, dir->AddressOfNames);

    if (addr == UINT_MAX) { return NULL; }

    return (UINT*)(data + addr);
}

char* AcquireExportName(const BYTE* data, const UINT* addresses, const UINT indx)
{
    const auto addr = RelativeVirtualAddressToFileOffset(data, addresses[indx]);

    if (addr == UINT_MAX) { return NULL; }

    return (char*)(data + addr);
}

char* Watcom(char* name)
{
    char* original = name;

    while (*name != NULL)
    {
        if (*name == '#') { *name = '@'; }

        name++;
    }

    return original;
}

int HandleFile(const HANDLE file)
{
    const unsigned int size = GetFileSize(file, NULL);
    if (size == INVALID_FILE_SIZE) { printf("[ERROR] Unable to get size file size."); return FAILURE_READING_FILE; }

    BYTE* mem = (BYTE*)malloc(size);
    if (mem == NULL) { printf("[ERROR] Unable to allocate memory for the file."); return FAILURE_MEMORY_ALLOCATION; }

    DWORD read = 0;
    if (!ReadFile(file, mem, size, &read, NULL)) { free(mem); return FAILURE_READING_FILE; }
    if (size != read) { printf("[ERROR] Unable to read the file."); free(mem); return FAILURE_READING_FILE; }

    const PIMAGE_NT_HEADERS header = AcquireHeader(mem);
    if (header == NULL) { printf("[ERROR] Unable to process file header."); free(mem); return FAILURE_INVALID_HEADER; }

    const PIMAGE_OPTIONAL_HEADER optional = AcquireOptionalHeader(header);
    if (optional == NULL) { printf("[ERROR] Unable to process file header."); free(mem); return FAILURE_INVALID_HEADER; }

    const PIMAGE_DATA_DIRECTORY dir = AcquireExportDataDirectory(optional);
    if (dir == NULL) { printf("[ERROR] Unable to process file header."); free(mem); return FAILURE_INVALID_HEADER; }

    const PIMAGE_EXPORT_DIRECTORY exports = AcquireExports(mem, dir);
    if (exports == NULL) { printf("[ERROR] Unable to process file header."); free(mem); return FAILURE_INVALID_HEADER; }

    const unsigned int* addresses = AcquireExportNameAddresses(mem, exports);
    if (addresses == NULL) { printf("[ERROR] Unable to process file header."); free(mem); return SUCCESS; }

    printf("Export symbols found: %d.\n", exports->NumberOfNames);

    for (unsigned int x = 0; x < exports->NumberOfNames; x++)
    {
        char* value = AcquireExportName(mem, addresses, x);
        printf("[%d] Replacing %s with ", x, value);
        printf("%s\n", Watcom(value));
    }

    printf("Saving the file...\n");

    if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { printf("[ERROR] Unable to save the file."); free(mem); return FAILURE_WRITING_FILE; }

    DWORD written = 0;
    if (!WriteFile(file, mem, size, &written, NULL)) { printf("[ERROR] Unable to save the file."); free(mem); return FAILURE_WRITING_FILE; }
    if (size != written) { printf("[ERROR] Unable to save the file."); free(mem); return FAILURE_WRITING_FILE; }

    free(mem);

    return SUCCESS;
}