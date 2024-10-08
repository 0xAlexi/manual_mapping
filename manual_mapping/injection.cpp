#include "injection.h"
using namespace std;

void _stdcall Shellcode(MANUAL_MAPPING_DATA* pData);
bool ManualMap(HANDLE hProc, const char* szDllFile) { //Checking the file legit as well 
	BYTE* pSrcData = nullptr;
	IMAGE_NT_HEADERS* pOldNtHeader = nullptr;
	IMAGE_OPTIONAL_HEADER* pOldOptHeader = nullptr;
	IMAGE_FILE_HEADER* pOldFileHeader = nullptr;
	BYTE* pTargetBase = nullptr;

	DWORD dwCheck = 0;
	if (!GetFileAttributesA(szDllFile))
	{
		printf("File doesn't exist.\n");
		return false;

	}
	ifstream File(szDllFile, std::ios::binary || std::ios::ate);
	if (File.fail()) // if the opening of the fail due to something.
	{
		printf("Couldn't open the file: %X\n", (DWORD)File.rdstate()); //returns the state
		return false;
	}
	//check if the passed file is valid or not:
	auto FileSize = File.tellg();
	if (FileSize < 0x1000)
	{
		printf("Invalid file size. \b");
		File.close();

		return false;
	}
	pSrcData = new BYTE[static_cast <UINT_PTR>(FileSize)]; // Read into memory
	if (!pSrcData)
	{
		printf("Memory allocation failed.\n");
		File.close();
		return false;
	}
	File.seekg(0, std::ios::beg); //file pointer is set back to beggining
	File.read(reinterpret_cast<char*>(pSrcData), FileSize);
	File.close();
	if (reinterpret_cast<IMAGE_DOS_HEADER*>(pSrcData)->e_magic != 0x5A4D)//magic number used by all executables? look into wikipedia DOS MZ executable
	{
		printf("Invalid file.\n");
		delete[] pSrcData;
		return false;
	}
	pOldNtHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(pSrcData + reinterpret_cast<IMAGE_DOS_HEADER*>(pSrcData)->e_lfanew);
	pOldOptHeader = &pOldNtHeader->OptionalHeader;
	pOldFileHeader = &pOldNtHeader->FileHeader;

#ifdef _WIN64
	if (pOldFileHeader->Machine != IMAGE_FILE_MACHINE_AMD64) {
		printf("Invalid x64 platform.\n");
		delete[]pSrcData;
		return false;
	}
#else
	if (pOldFileHeader->Machine != IMAGE_FILE_MACHINE_I386) {
		printf("Invalid x64 platform.\n");
		delete[]pSrcData;
		return false;
	}  
#endif
	pTargetBase = reinterpret_cast<BYTE*>(VirtualAllocEx(hProc, reinterpret_cast<void*>(pOldOptHeader->ImageBase), pOldOptHeader->SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
	if (pTargetBase)
	{
		pTargetBase = reinterpret_cast<BYTE*>(VirtualAllocEx(hProc, nullptr, pOldOptHeader->SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
		if (!pTargetBase)
		{
			printf("Memmory allocation failed,(ex): 0x%X\n",GetLastError());
			delete[]	pSrcData;
			return false;
		}

	}
	MANUAL_MAPPING_DATA data{ 0 };
	data.pLoadLibrary = LoadLibraryA;
	data.pGetProcAddress = reinterpret_cast<f_GetProcAddress>( GetProcAddress);
	auto* pSectionHeader = IMAGE_FIRST_SECTION(pOldNtHeader);
	for (UINT i = 0; i != pOldFileHeader -> NumberOfSections; ++i,++pSectionHeader ) //Number of sections to copy
	{
		if (pSectionHeader->SizeOfRawData)
		{
			if (!WriteProcessMemory(hProc,pTargetBase + pSectionHeader-> VirtualAddress,pSrcData + pSectionHeader->PointerToRawData,pSectionHeader->SizeOfRawData,nullptr))
			{
				printf("Could not map the sections: 0x%x\n", GetLastError());
				delete[]	pSrcData;
				VirtualFreeEx(hProc, pTargetBase, MEM_RELEASE);
				return false;
			}
		}
	}
	delete[] pSrcData;

}

void _stdcall Shellcode(MANUAL_MAPPING_DATA* pData)
{
	if (!pData)
	{
		return;
	}
	BYTE* pBase = reinterpret_cast<BYTE*>(pData);
	auto* pOpt = &reinterpret_cast<IMAGE_NT_HEADERS*>(pBase + reinterpret_cast<IMAGE_DOS_HEADER*>(pData)->e_lfanew)->OptionalHeader;
	auto _LoadLibaryA = pData->pLoadLibrary;
	auto _GetProcAddress = pData->pGetProcAddress;
	auto _DllMain = reinterpret_cast<f_DLL_ENTRY_POINT>(pBase + pOpt->AddressOfEntryPoint);
	BYTE* LocationData = pBase - pOpt->ImageBase;
	if (!pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size)
		return;
	auto*
	
}
