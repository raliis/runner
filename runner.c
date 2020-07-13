#include "util.h"

int main(int argc, char **argv)
{
		int mode = select_mode(argv[1]);
		switch (mode)
		{
				case 0:
						if (argv[2] && argv[3] && argv[4] && argv[5])
						{
								printf ("adding new(id: 56): %.2f(km), %s, %d, %d(kcal)\n", atof(argv[2]), argv[3], atoi(argv[4]), atoi(argv[5]));
						}
						else
						{
								printf ("Missing some arguments, recheck the command\n");
						}
						break;
				case 1:
						if (argv[2])
						{
								printf ("Removing run with id %d\n", atoi(argv[2]));
						}
						else
						{
								printf ("Missing id, cant remove run\n");
						}
						break;
				default:
						printf ("usage: runner add [dist(float)] [duration(str)], [heart rate(int)], [cals(int)]\n");
						break;
		}
		
		return 0;
}

