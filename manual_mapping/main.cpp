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
		printf("CreateToolhelp32Snapshot was not successfull: 0x%Xn");
		system("PAUSE");
		return 0;
	}

}