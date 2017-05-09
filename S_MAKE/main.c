#include "make.h"

int main(int argc, char* argv[])
{
	if (argc > 2)
	{
		printf("Usage: s_make [makefile](optional)\n");
		return 0;
	}

	// Might need to append \0 to the end of this...
	char makefile[MAX_PATH] = "PROJECT";

	if (argc == 2)
		strcpy_s(makefile, MAX_PATH, argv[1]);

	// Make that shit
	return make(makefile);
}