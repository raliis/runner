#include "util.h"

int main(int argc, char **argv)
{
		int mode = select_mode(argv[1]);
		switch (mode)
		{
				case 0:
						if (argv[2] && argv[3] && argv[4] && argv[5])
						{
								printf ("Adding new(id: 56): %.2f(km), %s, %d, %d(kcal)\n", atof(argv[2]), argv[3], atoi(argv[4]), atoi(argv[5]));
						}
						else
						{
								printf ("Missing some arguments, cant add entry\n");
						}
						break;
				case 1:
						if (argv[2])
						{
								printf ("Removing entry with id %d\n", atoi(argv[2]));
						}
						else
						{
								printf ("Missing id, cant remove entry\n");
						}
						break;
				case 2:
						if (argv[2])
						{
								printf ("Launching editor to change entry\n");
						}
						else
						{
								printf ("Missing id, cant edit entry\n");
						}
						break;
				default:
						printf ("Usage: runner add [dist(float)] [duration(str)], [heart rate(int)], [cals(int)]\n");
						break;
		}
		
		return 0;
}

