/* main.c
 * Tunneler main and menus
 * Last modified 30 May 2007
 *
 * Copyright (c) 2004,2007 Taneli Kalvas
 *
 * This file is part of SDL Tunneler
 *
 * SDL Tunneler is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * SDL Tunneler is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SDL Tunneler ; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 * Questions, comments and bug reports should be sent to the author
 * directly via email at tvkalvas@cc.jyu.fi
 */

#include "ai.h"
#include "config.h"
#include "game.h"
#include "graphics.h"
#include "keys.h"
#include "terrain.h"
#include "tunneler.h"
#include "types.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *argv0;
char *conffile;

static void Determine_Config_Path(void) {
	char *home;

#ifndef WIN32
	if(conffile != NULL) return;

	home = getenv("HOME");
	if(home == NULL) {
		printf("Environment variable HOME not defined!\n");
		printf("Couldn't read configuration file!\n");
		return;
	}

	conffile = malloc(strlen(home) + strlen(CONF_FILE) + 2);
	sprintf(conffile, "%s/%s", home, CONF_FILE);
#else
	char *end;
	char *str;

	if(conffile != NULL) return;

	if(*argv0 == '\"') argv0++;

	end = argv0;
	while(*end != '\0') end++;

	while(*end != '\\' && end != argv0) end--;

	if(*end == '\\') {
		end++;

		home = malloc(end - argv0 + 1);
		str = home;
		while(argv0 != end) {
			*str = *argv0;
			argv0++;
			str++;
		}
		*str = '\0';

		conffile = malloc(strlen(home) + strlen(CONF_FILE) + 1);
		sprintf(conffile, "%s\\%s", home, CONF_FILE);
		free(home);
	} else {
		conffile = malloc(strlen(CONF_FILE) + 1);
		sprintf(conffile, "%s", CONF_FILE);
	}
#endif
}

/* Write settings to configuration file */
void Write_Config(void) {
	FILE *fp;

	Determine_Config_Path();

	fp = fopen(conffile, "w");
	if(fp == NULL) {
		printf("Couldn't write configuration file %s!\n", conffile);
		exit(1);
	}

	printf("Writing %s\n", conffile);
	fprintf(fp, "# Tunneler configuration file.\n");
	fprintf(fp, "# Do not edit.\n");
	fprintf(fp, "fullscreen = %d\n", !!Video_fullscreen);
	fprintf(fp, "width = %d\n", Video_X);
	fprintf(fp, "height = %d\n", Video_Y);

	fprintf(fp, "pl0_up = %d\n", sym_pl[0].up);
	fprintf(fp, "pl0_down = %d\n", sym_pl[0].down);
	fprintf(fp, "pl0_left = %d\n", sym_pl[0].left);
	fprintf(fp, "pl0_right = %d\n", sym_pl[0].right);
	fprintf(fp, "pl0_fire = %d\n", sym_pl[0].fire);

	fprintf(fp, "pl1_up = %d\n", sym_pl[1].up);
	fprintf(fp, "pl1_down = %d\n", sym_pl[1].down);
	fprintf(fp, "pl1_left = %d\n", sym_pl[1].left);
	fprintf(fp, "pl1_right = %d\n", sym_pl[1].right);
	fprintf(fp, "pl1_fire = %d\n", sym_pl[1].fire);

	fclose(fp);
}

/* Read contents of configuration file. A new file is created if there
 * is none. */
void Read_Config(void) {
	FILE *fp;
	int k;
	char buf[256];

	Determine_Config_Path();

	fp = fopen(conffile, "r");
	if(fp == NULL) {
		printf("Couldn't read configuration file!\n");

		sym_pl[0].up = SDLK_UP;
		sym_pl[0].down = SDLK_DOWN;
		sym_pl[0].left = SDLK_LEFT;
		sym_pl[0].right = SDLK_RIGHT;
		sym_pl[0].fire = SDLK_RSHIFT;

		sym_pl[1].up = SDLK_w;
		sym_pl[1].down = SDLK_s;
		sym_pl[1].left = SDLK_a;
		sym_pl[1].right = SDLK_d;
		sym_pl[1].fire = SDLK_LCTRL;

		Write_Config();
		return;
	}

	/* Read configuration file */
	k = 0;
	while(1) {
		k++;
		if(fgets(buf, 256, fp) == NULL) break;

		/* Skip commented and empty lines */
		if(buf[0] == '#' || buf[0] == '\0' || buf[0] == '\n') continue;

		if(!strncmp(buf, "fullscreen =", 12)) {
			if(atoi(&buf[12]) == 1) Video_fullscreen = 1;
		} else if(!strncmp(buf, "width =", 7))
			Video_X = atoi(&buf[7]);
		else if(!strncmp(buf, "height =", 8))
			Video_Y = atoi(&buf[8]);
		else if(!strncmp(buf, "pl0_up =", 8))
			sym_pl[0].up = atoi(&buf[8]);
		else if(!strncmp(buf, "pl0_down =", 10))
			sym_pl[0].down = atoi(&buf[10]);
		else if(!strncmp(buf, "pl0_left =", 10))
			sym_pl[0].left = atoi(&buf[10]);
		else if(!strncmp(buf, "pl0_right =", 11))
			sym_pl[0].right = atoi(&buf[11]);
		else if(!strncmp(buf, "pl0_fire =", 10))
			sym_pl[0].fire = atoi(&buf[10]);
		else if(!strncmp(buf, "pl1_up =", 8))
			sym_pl[1].up = atoi(&buf[8]);
		else if(!strncmp(buf, "pl1_down =", 10))
			sym_pl[1].down = atoi(&buf[10]);
		else if(!strncmp(buf, "pl1_left =", 10))
			sym_pl[1].left = atoi(&buf[10]);
		else if(!strncmp(buf, "pl1_right =", 11))
			sym_pl[1].right = atoi(&buf[11]);
		else if(!strncmp(buf, "pl1_fire =", 10))
			sym_pl[1].fire = atoi(&buf[10]);
		else {
			printf("Unknown field in configuration file on line %d\n", k);
			exit(1);
		}
	}

	fclose(fp);
}
