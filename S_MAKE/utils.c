#include "utils.h"

int ExecuteCommand(char* cmd)
{
	STARTUPINFO				start_info;
	PROCESS_INFORMATION		proc_info;
	DWORD					wait_status, exit_status, proc_flags = 0;
	BOOL					is_created;

	// Get current process info
	GetStartupInfo(&start_info);

	is_created = CreateProcess(
		NULL,			// lpApplicationName
		cmd,			// lpCommandLine
		NULL,			// lpProcessAttributes
		NULL,			// lpThreadAttributes
		NULL,			// bInheritHandles
		proc_flags,		// dwCreationFlags
		NULL,			// lpEnvironment
		NULL,			// lpCurrentDirectory
		&start_info,	// lpStartupInfo
		&proc_info		// lpProcessInformation
		);

	// Failed to create so return the failed flag
	if (!is_created)
		return -1;

	// Infinite timeout currently but we may want to just set that to a large number
	wait_status = WaitForSingleObject(proc_info.hProcess, INFINITE);

	if (wait_status == WAIT_FAILED)
		printf("Failed waiting for cmd to process\n");

	// Get the processes exit code
	GetExitCodeProcess(proc_info.hProcess, &exit_status);

	CloseHandle(proc_info.hProcess);
	CloseHandle(proc_info.hThread);

	return (int)exit_status;

}