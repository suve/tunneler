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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *argv0;

static char *conf_path_v1 = NULL;
static char *conf_path_v2 = NULL;

static void Determine_Config_Path_v1(void) {
	char *home;

#ifndef WIN32
	if(conf_path_v1 != NULL) return;

	home = getenv("HOME");
	if(home == NULL) {
		printf("Environment variable HOME not defined!\n");
		printf("Couldn't read configuration file!\n");
		return;
	}

	conf_path_v1 = malloc(strlen(home) + strlen(CONF_FILE_V1) + 2);
	sprintf(conf_path_v1, "%s/%s", home, CONF_FILE_V1);
#else
	char *end;
	char *str;

	if(conf_path_v1 != NULL) return;

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

		conf_path_v1 = malloc(strlen(home) + strlen(CONF_FILE_V1) + 1);
		sprintf(conf_path_v1, "%s\\%s", home, CONF_FILE_V1);
		free(home);
	} else {
		conf_path_v1 = malloc(strlen(CONF_FILE_V1) + 1);
		sprintf(conf_path_v1, "%s", CONF_FILE_V1);
	}
#endif
}

static void Determine_Config_Path_v2(void) {
	char *prefpath;

	if(conf_path_v2 != NULL) return;

	prefpath = SDL_GetPrefPath("", "tunneler");
	if(prefpath == NULL) {
		printf("Failed to determine config path: %s\n", SDL_GetError());
		return; // TODO: Handle this somehow?
	}

	conf_path_v2 = malloc(strlen(prefpath) + strlen(CONF_FILE_V2) + 1);
	sprintf(conf_path_v2, "%s%s", prefpath, CONF_FILE_V2);

	SDL_free(prefpath);
}

/* Write settings to configuration file */
void Write_Config(void) {
	FILE *fp;

	Determine_Config_Path_v2();

	fp = fopen(conf_path_v2, "w");
	if(fp == NULL) {
		printf("Failed to write configuration file at %s: %s\n", conf_path_v2, strerror(errno));
		exit(1);
	}

	printf("Writing %s\n", conf_path_v2);
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

static void Default_Config(void) {
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
}

/* Read contents of configuration file. A new file is created if there
 * is none. */
void Read_Config(void) {
	FILE *fp;
	int k;
	char buf[256];

	Determine_Config_Path_v2();
	fp = fopen(conf_path_v2, "r");
	if(fp == NULL) {
		printf("Failed to read configuration file at %s: %s\n", conf_path_v2, strerror(errno));

		Determine_Config_Path_v1();
		fp = fopen(conf_path_v1, "r");
		if(fp == NULL) {
			printf("Failed to read configuration file at %s: %s\n", conf_path_v1, strerror(errno));

			Default_Config();
			Write_Config();
			return;
		}
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
