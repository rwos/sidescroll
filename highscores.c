#include <stdio.h>
#include "SDL.h"

#include "io.h"
#include "objects.h"
#include "graphics.h"
#include "config.h"

#include "highscores.h"

void show_highscores()
{
	int points, multi, i=0;
	FILE *hs;
	hs = fopen("highscores.txt", "r");
	while ((fscanf(hs, "%d %d", &points, &multi) != -1)) {
		io_printf(1, i++, "Points: %d, Multi: %d", points, multi);
		update_io();
		printf("%d points\n", points);
		printf("%d multi\n", multi);
	}
	sleep(5);
	fclose(hs);
}

void save_highscores(int points, int multi)
{
	FILE *hs;
	hs = fopen("highscores.txt", "a");
	int buffer[] = {points, multi};
//	fwrite(buffer, sizeof(int), sizeof(buffer)/sizeof(int), hs);
	fprintf(hs, "%d %d\n", points, multi);
	fclose(hs);
	show_highscores();
	//exit(0);
}
