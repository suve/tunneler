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

struct KeyMapping {
	unsigned int old; // SDL1 key
	SDL_KeyCode new; // SDL2 key
};
#define KEYMAP(o, n) \
	(struct KeyMapping) { \
		.old = (o), .new = (n) \
	}

static const struct KeyMapping KEY_MAPPING[] = {
	KEYMAP(8, SDLK_BACKSPACE),
	KEYMAP(9, SDLK_TAB),
	KEYMAP(12, SDLK_CLEAR),
	KEYMAP(13, SDLK_RETURN),
	KEYMAP(19, SDLK_PAUSE),
	KEYMAP(27, SDLK_ESCAPE),
	KEYMAP(32, SDLK_SPACE),
	KEYMAP(33, SDLK_EXCLAIM),
	KEYMAP(34, SDLK_QUOTEDBL),
	KEYMAP(35, SDLK_HASH),
	KEYMAP(36, SDLK_DOLLAR),
	KEYMAP(38, SDLK_AMPERSAND),
	KEYMAP(39, SDLK_QUOTE),
	KEYMAP(40, SDLK_LEFTPAREN),
	KEYMAP(41, SDLK_RIGHTPAREN),
	KEYMAP(42, SDLK_ASTERISK),
	KEYMAP(43, SDLK_PLUS),
	KEYMAP(44, SDLK_COMMA),
	KEYMAP(45, SDLK_MINUS),
	KEYMAP(46, SDLK_PERIOD),
	KEYMAP(47, SDLK_SLASH),
	KEYMAP(48, SDLK_0),
	KEYMAP(49, SDLK_1),
	KEYMAP(50, SDLK_2),
	KEYMAP(51, SDLK_3),
	KEYMAP(52, SDLK_4),
	KEYMAP(53, SDLK_5),
	KEYMAP(54, SDLK_6),
	KEYMAP(55, SDLK_7),
	KEYMAP(56, SDLK_8),
	KEYMAP(57, SDLK_9),
	KEYMAP(58, SDLK_COLON),
	KEYMAP(59, SDLK_SEMICOLON),
	KEYMAP(60, SDLK_LESS),
	KEYMAP(61, SDLK_EQUALS),
	KEYMAP(62, SDLK_GREATER),
	KEYMAP(63, SDLK_QUESTION),
	KEYMAP(64, SDLK_AT),
	KEYMAP(91, SDLK_LEFTBRACKET),
	KEYMAP(92, SDLK_BACKSLASH),
	KEYMAP(93, SDLK_RIGHTBRACKET),
	KEYMAP(94, SDLK_CARET),
	KEYMAP(95, SDLK_UNDERSCORE),
	KEYMAP(96, SDLK_BACKQUOTE),
	KEYMAP(97, SDLK_a),
	KEYMAP(98, SDLK_b),
	KEYMAP(99, SDLK_c),
	KEYMAP(100, SDLK_d),
	KEYMAP(101, SDLK_e),
	KEYMAP(102, SDLK_f),
	KEYMAP(103, SDLK_g),
	KEYMAP(104, SDLK_h),
	KEYMAP(105, SDLK_i),
	KEYMAP(106, SDLK_j),
	KEYMAP(107, SDLK_k),
	KEYMAP(108, SDLK_l),
	KEYMAP(109, SDLK_m),
	KEYMAP(110, SDLK_n),
	KEYMAP(111, SDLK_o),
	KEYMAP(112, SDLK_p),
	KEYMAP(113, SDLK_q),
	KEYMAP(114, SDLK_r),
	KEYMAP(115, SDLK_s),
	KEYMAP(116, SDLK_t),
	KEYMAP(117, SDLK_u),
	KEYMAP(118, SDLK_v),
	KEYMAP(119, SDLK_w),
	KEYMAP(120, SDLK_x),
	KEYMAP(121, SDLK_y),
	KEYMAP(122, SDLK_z),
	KEYMAP(127, SDLK_DELETE),
	KEYMAP(256, SDLK_KP_0),
	KEYMAP(257, SDLK_KP_1),
	KEYMAP(258, SDLK_KP_2),
	KEYMAP(259, SDLK_KP_3),
	KEYMAP(260, SDLK_KP_4),
	KEYMAP(261, SDLK_KP_5),
	KEYMAP(262, SDLK_KP_6),
	KEYMAP(263, SDLK_KP_7),
	KEYMAP(264, SDLK_KP_8),
	KEYMAP(265, SDLK_KP_9),
	KEYMAP(266, SDLK_KP_PERIOD),
	KEYMAP(267, SDLK_KP_DIVIDE),
	KEYMAP(268, SDLK_KP_MULTIPLY),
	KEYMAP(269, SDLK_KP_MINUS),
	KEYMAP(270, SDLK_KP_PLUS),
	KEYMAP(271, SDLK_KP_ENTER),
	KEYMAP(272, SDLK_KP_EQUALS),
	KEYMAP(273, SDLK_UP),
	KEYMAP(274, SDLK_DOWN),
	KEYMAP(275, SDLK_RIGHT),
	KEYMAP(276, SDLK_LEFT),
	KEYMAP(277, SDLK_INSERT),
	KEYMAP(278, SDLK_HOME),
	KEYMAP(279, SDLK_END),
	KEYMAP(280, SDLK_PAGEUP),
	KEYMAP(281, SDLK_PAGEDOWN),
	KEYMAP(282, SDLK_F1),
	KEYMAP(283, SDLK_F2),
	KEYMAP(284, SDLK_F3),
	KEYMAP(285, SDLK_F4),
	KEYMAP(286, SDLK_F5),
	KEYMAP(287, SDLK_F6),
	KEYMAP(288, SDLK_F7),
	KEYMAP(289, SDLK_F8),
	KEYMAP(290, SDLK_F9),
	KEYMAP(291, SDLK_F10),
	KEYMAP(292, SDLK_F11),
	KEYMAP(293, SDLK_F12),
	KEYMAP(294, SDLK_F13),
	KEYMAP(295, SDLK_F14),
	KEYMAP(296, SDLK_F15),
	KEYMAP(300, SDLK_NUMLOCKCLEAR),
	KEYMAP(301, SDLK_CAPSLOCK),
	KEYMAP(302, SDLK_SCROLLLOCK),
	KEYMAP(303, SDLK_RSHIFT),
	KEYMAP(304, SDLK_LSHIFT),
	KEYMAP(305, SDLK_RCTRL),
	KEYMAP(306, SDLK_LCTRL),
	KEYMAP(307, SDLK_RALT),
	KEYMAP(308, SDLK_LALT),
	KEYMAP(309, SDLK_RGUI),
	KEYMAP(310, SDLK_LGUI),
	KEYMAP(311, SDLK_LGUI), // Left "Windows" key
	KEYMAP(312, SDLK_RGUI), // Right "Windows" key
	KEYMAP(313, SDLK_MODE), // "Alt Gr" key
	KEYMAP(314, SDLK_APPLICATION), // Multi-key compose key
	KEYMAP(315, SDLK_HELP),
	KEYMAP(316, SDLK_PRINTSCREEN),
	KEYMAP(317, SDLK_SYSREQ),
	KEYMAP(318, SDLK_PAUSE),
	KEYMAP(319, SDLK_MENU),
	KEYMAP(320, SDLK_POWER), // Power Macintosh power key
	KEYMAP(322, SDLK_UNDO), // Atari keyboard has Undo
};

static int km_cmp(const void *a, const void *b) {
	const struct KeyMapping *item = a;
	const unsigned int *search = b;

	if(item->old < *search) return -1;
	if(item->old > *search) return +1;
	return 0;
}

static SDL_KeyCode Translate_Keycode(unsigned int old) {
	void *found =
		bsearch(&old, KEY_MAPPING, sizeof(KEY_MAPPING) / sizeof(struct KeyMapping), sizeof(struct KeyMapping), &km_cmp);

	if(found != NULL) {
		struct KeyMapping *km = found;
		return km->new;
	} else {
		return SDLK_UNKNOWN;
	}
}

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
	int ver;
	char buf[256];

	Determine_Config_Path_v2();
	fp = fopen(conf_path_v2, "r");
	if(fp != NULL) {
		ver = 2;
	} else {
		printf("Failed to read configuration file at %s: %s\n", conf_path_v2, strerror(errno));

		Determine_Config_Path_v1();
		fp = fopen(conf_path_v1, "r");
		if(fp != NULL) {
			ver = 1;
		} else {
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
		} else if(!strncmp(buf, "width =", 7)) {
			Video_X = atoi(&buf[7]);
		} else if(!strncmp(buf, "height =", 8)) {
			Video_Y = atoi(&buf[8]);
		} else if(!strncmp(buf, "pl0_up =", 8)) {
			sym_pl[0].up = atoi(&buf[8]);
			if(ver == 1) sym_pl[0].up = Translate_Keycode(sym_pl[0].up);
		} else if(!strncmp(buf, "pl0_down =", 10)) {
			sym_pl[0].down = atoi(&buf[10]);
			if(ver == 1) sym_pl[0].down = Translate_Keycode(sym_pl[0].down);
		} else if(!strncmp(buf, "pl0_left =", 10)) {
			sym_pl[0].left = atoi(&buf[10]);
			if(ver == 1) sym_pl[0].left = Translate_Keycode(sym_pl[0].left);
		} else if(!strncmp(buf, "pl0_right =", 11)) {
			sym_pl[0].right = atoi(&buf[11]);
			if(ver == 1) sym_pl[0].right = Translate_Keycode(sym_pl[0].right);
		} else if(!strncmp(buf, "pl0_fire =", 10)) {
			sym_pl[0].fire = atoi(&buf[10]);
			if(ver == 1) sym_pl[0].fire = Translate_Keycode(sym_pl[0].fire);
		} else if(!strncmp(buf, "pl1_up =", 8)) {
			sym_pl[1].up = atoi(&buf[8]);
			if(ver == 1) sym_pl[1].up = Translate_Keycode(sym_pl[1].up);
		} else if(!strncmp(buf, "pl1_down =", 10)) {
			sym_pl[1].down = atoi(&buf[10]);
			if(ver == 1) sym_pl[1].down = Translate_Keycode(sym_pl[1].down);
		} else if(!strncmp(buf, "pl1_left =", 10)) {
			sym_pl[1].left = atoi(&buf[10]);
			if(ver == 1) sym_pl[1].left = Translate_Keycode(sym_pl[1].left);
		} else if(!strncmp(buf, "pl1_right =", 11)) {
			sym_pl[1].right = atoi(&buf[11]);
			if(ver == 1) sym_pl[1].right = Translate_Keycode(sym_pl[1].right);
		} else if(!strncmp(buf, "pl1_fire =", 10)) {
			sym_pl[1].fire = atoi(&buf[10]);
			if(ver == 1) sym_pl[1].fire = Translate_Keycode(sym_pl[1].fire);
		} else {
			printf("Unknown field in configuration file on line %d\n", k);
			exit(1);
		}
	}

	fclose(fp);
}
