#include "injection.h"
const char  szDllFile[] = "C:\\Users\\Alexi\\source\\repos\\manual_mapping\\test.dll";//path to the dll 
const char szProc[] = "Test Console.exe";
int main() {
	PROCESSENTRY32 PE32{ 0 };
	PE32.dwSize = sizeof(PE32);
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		DWORD err = GetLastError();
		printf("CreateToolhelp32Snapshot was not successfull: 0x%X\n");
		system("PAUSE");
		return 0;
	}
	DWORD PID = 0;
	BOOL bRet = Process32First(hSnap, &PE32);
	while (bRet)
	{
		if (!strcmp(szProc,PE32.szExeFile))
		{
			PID = PE32.th32ProcessID;
			break;
		}
		bool bRet = Process32Next(hSnap, &PE32);
	}
	CloseHandle(hSnap);
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (!hProc)
	{
		DWORD err = GetLastError();
		printf("OpenProcess was not successfull: 0x%X\n");
		system("PAUSE");
		return 0;
	}
	if (!ManualMap(hProc,szDllFile))
	{
		CloseHandle(hProc);
		printf("Something went wrong debug for details... \n");
		system("PAUSE");
		return 0;
	}
	CloseHandle(hProc);
	return 0;
}