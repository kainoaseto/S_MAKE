#ifndef __UTILS_H__
#define __UTILS_H__

#include <Windows.h>
#include <WinBase.h>

// Executes command in a seperate process and returns the exit code
// char* cmd argument must have no leading whitespace and contains the command to be executed
// the exit status is returned as an int
int ExecuteCommand(char* cmd);

#endif // __UTILS_H__