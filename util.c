#include "util.h"

int select_mode(char* mode)
{
		if (!(strcmp (mode, "add") && strcmp (mode, "a")))
		{
				return 0;
		}
		else if (!(strcmp (mode, "remove") && strcmp (mode, "rm") && strcmp (mode, "r")))
		{
				return 1;
		}
		else if (!(strcmp (mode, "edit") && strcmp (mode, "e")))
		{
				return 2;
		}
		else
		{
				return 3;
		}
}

void print_count(int count)
{
		printf ("count is: %d\n", count);
}
