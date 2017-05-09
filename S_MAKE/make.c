#include "make.h"

int make(char* makefile)
{
	FILE* file = fopen(makefile, "rb");
	if (file == NULL)
	{
		printf("Failed to find file: %s\n", makefile);
		return -1;
	}

	HANDLE curr_target_h;
	HANDLE curr_dep_h;
	FILETIME* curr_target_ft;
	FILETIME* curr_dep_ft;

	char current_char;
	BOOL in_comment = FALSE;
	BOOL newline = TRUE;
	BOOL rebuild = FALSE;
	BOOL parse_deps = FALSE;
	BOOL parse_cmds = FALSE;

	char current_dep[MAX_PATH];
	char current_target[MAX_PATH];

	// 8192 is the max commandline length for WinXP which I think is good enough
	char cmd[MAX_CMDLEN];
	

	// Parse lines
	while ((current_char = fgetc(file)) != EOF)
	{
		if (rebuild)
		{
			strcat_s(cmd, MAX_CMDLEN, current_char);
			// TODO: If we are rebuilding, jump to next line which MUST not be whitespace and then 
			// copy the commands in, strip whitespace from front, and put into util(ExecuteCommand)
			// If that fails then break and return that exit code
			// Check at the top if we have an empty line, if we do then exit and reset everything
			// for next target/dep check
		}


		// Empty line or finished processing current line
		if (current_char == '\n') 
		{
			newline = TRUE;

			// Finished parsing the deps
			if (parse_deps)
			{
				parse_deps = FALSE;
				parse_cmds = TRUE;
				strcpy_s(current_target, MAX_PATH, "");
				strcpy_s(current_dep, MAX_PATH, "");
			}

			continue;
		}

		// Do we need to handle this?
		if (current_char == '\r')
		{
			continue;
		}

		if (newline)
		{
			// Check if this line is a comment
			if (current_char == '#')
			{
				newline = FALSE;
				in_comment = TRUE;

				continue;
			}

			if (current_char == ':')
			{
				printf("Syntax error! No target, found ':'\n");
				return -1;
			}
		}

		// If we are in a comment, check to see if we can exit
		if (in_comment)
		{
			if (current_char == '\n')
			{
				in_comment = FALSE;
				newline = TRUE;
			}
			continue;
		}

		// We won't ever get to this point unless we are parsing 
		// the target : dependencies line or a command

		// Get our target
		if (current_char != ' ' && current_char != ':' && !parse_deps)
		{
			strcat_s(current_target, MAX_PATH, current_char);
			continue;
		}

		// Start our deps processing and reset char count
		if (current_char == ':')
		{
			// Might need to add null(\0) to end of current_target
			curr_target_h = CreateFile(
				current_target,			// lpFileName
				GENERIC_READ,			// dwDesiredAccess
				0,						// dwShareMode
				NULL,					// lpSecurityAttributes
				OPEN_EXISTING,			// dwCreationDisposition
				NULL,					// dwFlagsAndAttributes
				NULL					// hTemplateFile
				);

			// If this succeeds then the target does not exist and we want to build
			if (curr_target_h == INVALID_HANDLE_VALUE)
			{
				rebuild = TRUE;
				continue;
			}
		
			// If we cant get the filetime just rebuild
			if (!GetFileTime(curr_target_h, NULL, curr_target_ft, NULL))
			{
				rebuild = TRUE;
				continue;
			}
			
			parse_deps = TRUE;
			continue;
		}

		// At this point we must be past the ':' and will be taking in commands
		if (parse_deps)
		{
			// Either new dep or just whitespace which we don't care about
			if (current_char == ' ')
			{
				// Switch to a new dep but first make sure this one is good
				if (strcmp(current_dep, "") != 0)
				{

					// Might need to add null(\0) to end of current_dep
					curr_dep_h = CreateFile(
						current_dep,			// lpFileName
						GENERIC_READ,			// dwDesiredAccess
						0,						// dwShareMode
						NULL,					// lpSecurityAttributes
						OPEN_EXISTING,			// dwCreationDisposition
						NULL,					// dwFlagsAndAttributes
						NULL					// hTemplateFile
						);

					// If a dependency doesn't exist that is a fatal error so we exit
					if (curr_dep_h == INVALID_HANDLE_VALUE)
					{
						printf("Dependency %s doesn't exist!\n", current_dep);
						return -1;
					}

					// If we cant get the filetime then something funky is going on and we'll error out
					if (!GetFileTime(curr_dep_h, NULL, curr_dep_ft, NULL))
					{
						printf("Couldn't get depdency %s filetime!\n", current_dep);
						return -1;
					}

					// Check if we need to rebuild
					if (CompareFileTime(curr_target_ft, curr_dep_ft) != 0)
					{
						rebuild = TRUE;
						continue;
					}
					strcpy_s(current_dep, MAX_PATH, ""); // reset
				}

				// Continue until we get to the end of this line or until we find another dep
				// TODO: Note that if we finish getting a dependency and it is the last in the list
				// The current newline check may skip it....so we might need to still make sure we process
				// that before checking for rebuilds or continuing on 
				continue;
			}

			// Append the next char of the name
			strcat_s(current_dep, MAX_PATH, current_char);
			
		}

		
	}

	return 0;
}