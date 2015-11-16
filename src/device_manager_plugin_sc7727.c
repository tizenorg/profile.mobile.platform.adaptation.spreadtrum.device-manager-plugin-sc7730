/*
 * Copyright (c) 2012 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/


#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
/* #include <devman_plugin_intf.h> */
#include <device-oal.h>

#include "devman_define_node_path.h"
#include "device_manager_siop.h"
#include "device_manager_io.h"
#include "device_manager_TRM.h"

#define EXPORT_API  __attribute__((visibility("default")))

#define BUFF_MAX	255
#define MAX_NAME 255
#define MIN(a,b)	((a)<(b) ? (a):(b))
#if 0
#define GENERATE_ACCESSORS_CHAR_RW(_suffix, _item)	\
char *OEM_sys_get_##_suffix()			\
{						\
	return sys_get_str(_item);	\
}						\
						\
int OEM_sys_set_##_suffix(char *str)			\
{						\
	return sys_set_str(_item, value);	\
}

#define GENERATE_ACCESSORS_CHAR_R(_suffix, _item)	\
char *OEM_sys_get_##_suffix()			\
{						\
	return sys_get_str(_item);	\
}

#define GENERATE_ACCESSORS_CHAR_W(_suffix, _item)	\
int OEM_sys_set_##_suffix(char *str)			\
{						\
	return sys_set_str(_item, str);		\
}
#endif

/* TODO: Add APIs has (char *) params */

#define GENERATE_ACCESSORS_INT_RW(_suffix, _item)	\
int OEM_sys_get_##_suffix(int *value)			\
{						\
	return sys_get_int(_item, value);	\
}						\
						\
int OEM_sys_set_##_suffix(int value)	\
{						\
	return sys_set_int(_item, value);	\
}

#define GENERATE_ACCESSORS_INT_R(_suffix, _item)	\
int OEM_sys_get_##_suffix(int *value)			\
{						\
	return sys_get_int(_item, value);	\
}

#define GENERATE_ACCESSORS_INT_W(_suffix, _item)	\
int OEM_sys_set_##_suffix(int value)			\
{						\
	return sys_set_int(_item, value);	\
}

/*
GENERATE_ACCESSORS_INT_R(backlight_max_brightness, BACKLIGHT_MAX_BRIGHTNESS_PATH)
GENERATE_ACCESSORS_INT_RW(backlight_brightness, BACKLIGHT_BRIGHTNESS_PATH)
GENERATE_ACCESSORS_INT_RW(backlight_acl_control, LCD_ACL_CONTROL_PATH)
GENERATE_ACCESSORS_INT_RW(lcd_power, LCD_POWER_PATH)
*/
#define DEVMGR_LOG
#if defined(DEVMGR_LOG)
#define LOG_TAG     "DEVICE_PLUGIN"
#include <dlog/dlog.h>
#define devmgr_log(fmt, args...)	SLOGD(fmt, ##args)
#else
#define devmgr_log(fmt, args...)
#endif

enum display_type
{
	DISP_MAIN = 0,
	DISP_SUB,
	DISP_MAX
};

enum lux_status {
	decrement,
	increment,
};

enum CABC_MODE
{
	CABC_OFF = 0,
	CABC_USER_INTERFACE,
	CABC_STILL_PICTURE,
	CABC_MOVING_IMAGE,
	CABC_MAX,
};

enum {
	TEMP_RANGE_0 = 0,		/* 0 < temperature*/
	TEMP_RANGE_1,		/*-20 < temperature < =0*/
	TEMP_RANGE_2,		/*temperature <= -20*/
};

struct display_info
{
	enum display_type etype; /* FIXME:!! Main LCD or Sub LCD node */
	char bl_name[MAX_NAME+1]; /* backlight name */
	char lcd_name[MAX_NAME+1]; /* lcd name */
};

#define MAX_CANDELA_CRITERION	300
#define PWR_SAVING_CANDELA_CRITERION	20

/* FIXME:!! change to global_ctx */
int lcd_index;
struct display_info disp_info[DISP_MAX];

int current_brightness = -1;
int backlight_max_brightness = 100;
int elvss_temp_stage = TEMP_RANGE_0;
extern int current_level;
extern int current_mode;
extern device_siop_table current_table;

int OEM_sys_get_hardkey_backlight(int *value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	snprintf(path, MAX_NAME, TOUCHKEY_LED_PATH);
	ret = sys_get_int(path, value);
	devmgr_log("path[%s]value[%d]", path, *value);

	return ret;
}

int OEM_sys_set_hardkey_backlight(int value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	snprintf(path, MAX_NAME, TOUCHKEY_LED_PATH);
	ret = sys_set_int(path, value);
	devmgr_log("path[%s]value[%d]", path, value);

	return ret;
}

int OEM_sys_get_hall_status(int *value)
{
	char path[MAX_NAME+1];
	int ret = -1;

	snprintf(path, MAX_NAME, COVER_STATUS_PATH);
	ret = sys_get_int(path, value);

	if (ret != 0)
		devmgr_log("fail to get cover_status\n");
	else
		devmgr_log("path[%s]value[%d]", path, *value);

	return ret;
}

int OEM_sys_get_whitemagic_mode(int index, int *value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	snprintf(path, MAX_NAME, LCD_WM_CONTROL_PATH, disp_info[index].lcd_name);
	ret = sys_get_int(path, value);
	devmgr_log("path[%s]value[%d]", path, *value);

	return ret;
}

int OEM_sys_set_whitemagic_mode(int index, int value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	snprintf(path, MAX_NAME, LCD_WM_CONTROL_PATH, disp_info[index].lcd_name);
	ret = sys_set_int(path, value);

	return ret;
}

int OEM_sys_get_brightness(unsigned int lux)
{
	const unsigned int Nr_Table[] = {
		0, 5, 5, 6, 6, 7, 8, 8, 9, 9,
		10, 11, 12, 13, 13, 14, 15, 16, 18, 19,
		20, 22, 23, 25, 29, 32, 36, 39, 43, 46,
		50, 54, 57, 61, 64, 68, 71, 75, 79, 82,
		86, 89, 93, 96, 100, 119, 138, 157, 176, 195,
		214, 233, 252, 271, 290, 310, 329, 348, 367, 386,
		405, 424, 443, 462, 481, 500, 571, 643, 714, 786,
		857, 929, 1000, 1100, 1200, 1300, 1400, 1500, 1667, 1833,
		2000, 2250, 2500, 2750, 3000, 3333, 3667, 4000, 4083, 4167,
		4250, 4333, 4417, 4500, 4583, 4667, 4750, 4833, 4917, 5000,
	};
	int brightness;

	for (brightness=0; (lux > Nr_Table[brightness]) && (brightness < 99); brightness++);

	return brightness;
}

int OEM_sys_get_backlight_brightness_by_lux(unsigned int lux, int *value)
{
	const unsigned int Max_Table[] = {
		15, 15, 15, 15, 16, 17, 18, 20, 21, 23,
		26, 28, 31, 33, 35, 38, 40, 44, 48, 51,
		55, 60, 65, 70, 81, 92, 103, 114, 125, 136,
		147, 158, 169, 180, 190, 201, 212, 223, 234, 245,
		256, 267, 278, 289, 300, 357, 414, 471, 529, 586,
		643, 700, 757, 814, 871, 929, 986, 1043, 1100, 1157,
		1214, 1271, 1329, 1386, 1443, 1500, 1678, 1855, 2033, 2211,
		2389, 2566, 2744, 2977, 3209, 3442, 3674, 3907, 4274, 4642,
		5009, 5215, 5422, 5628, 5834, 6057, 6279, 6502, 6585, 6669,
		6752, 6835, 6919, 7002, 7085, 7169, 7252, 7335, 7419, 7502,
	};
	const unsigned int Min_Table[] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 2, 2, 3,
		3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		8, 9, 9, 10, 10, 12, 14, 16, 17, 19,
		21, 23, 25, 27, 29, 30, 32, 34, 36, 38,
		40, 42, 43, 45, 47, 49, 60, 70, 81, 91,
		102, 112, 123, 140, 157, 174, 191, 208, 240, 271,
		303, 356, 409, 461, 514, 592, 670, 748, 769, 790,
		811, 832, 853, 874, 895, 916, 937, 958, 979, 1000,
	};
	static int brightness = -1;

	if (brightness == -1) {
		brightness = OEM_sys_get_brightness(lux);
		devmgr_log("lux: %d, brightness: %d.\n", lux, brightness+1);
	} else
		if ((lux > Max_Table[brightness]) || (lux < Min_Table[brightness])) {
			brightness = OEM_sys_get_brightness(lux);
			devmgr_log("lux: %d, brightness: %d.\n", lux, brightness+1);
		}

	*value = brightness+1;

	return 0;
}

static int OEM_sys_display_info(struct display_info *disp_info)
{
	struct dirent *dent;
	DIR *dirp;
	int i, index;
	const char * bl_path = BACKLIGHT_PATH;
	const char * lcd_path = LCD_PATH;

	/* Backlight */
	index = 0;
	dirp = opendir(bl_path);
	if (dirp) {
		while(dent = readdir(dirp)) {
			if (index >= DISP_MAX) {
				devmgr_log("supports %d display node", DISP_MAX);
				break;
			}

			if (!strcmp(".", dent->d_name) || !strcmp("..", dent->d_name))
				continue;
			else {
				strcpy(disp_info[index].bl_name, dent->d_name);
				index++;
			}
		}
		closedir(dirp);
	}

	/*for (i = 0; i < index; i++)*/
		/*devmgr_log("bl_name[%s]", disp_info[i].bl_name);*/

	/* LCD */
	index = 0;
	dirp = opendir(lcd_path);
	if (dirp) {
		while(dent = readdir(dirp)) {
			if (index >= DISP_MAX) {
				devmgr_log("supports %d display node", DISP_MAX);
				break;
			}

			if (!strcmp(".", dent->d_name) || !strcmp("..", dent->d_name))
				continue;
			else {
				strcpy(disp_info[index].lcd_name, dent->d_name);
				index++;
			}
		}
		closedir(dirp);
	}

	/*for (i = 0; i < index; i++)*/
		/*devmgr_log("lcd_name[%s]", disp_info[i].lcd_name);*/

	lcd_index = index;

	return 0;
}

int OEM_sys_get_display_count(int *value)
{
	int ret = -1;

	/* TODO: We should implement to find out current number of display */
	*value = lcd_index;
	ret = 0;
	/* ********************* */

	/*devmgr_log("value[%d]", *value);*/

	return ret;
}

int OEM_sys_get_backlight_max_brightness(int index, int *value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	snprintf(path, MAX_NAME, BACKLIGHT_MAX_BRIGHTNESS_PATH, disp_info[index].bl_name);
	ret = sys_get_int(path, value);
	devmgr_log("path[%s]value[%d]", path, *value);

	return ret;
}

int OEM_sys_set_backlight_max_brightness(int index, int value)
{
	int ret = 0;
	int new_val = 0;

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return -EINVAL;
	}

	if (value < 0)
		return -EINVAL;

	backlight_max_brightness = value;

	devmgr_log("value[%d]", backlight_max_brightness);

	if (current_brightness == -1)
		return 0;

	new_val = MIN(backlight_max_brightness, current_brightness);

	ret = set_backlight_brightness(index, new_val);
	if (ret)
		devmgr_log("Can't set backlight brightness (%d)", new_val);

	return ret;
}

int OEM_sys_get_backlight_min_brightness(int index, int *value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	snprintf(path, MAX_NAME, BACKLIGHT_MIN_BRIGHTNESS_PATH, disp_info[index].bl_name);
	ret = sys_get_int(path, value);
	devmgr_log("path[%s]value[%d]", path, *value);

	return ret;
}


int OEM_sys_get_backlight_brightness(int index, int *value, int power_saving)
{
	int ret = -1;
	char path[MAX_NAME+1];
	int max_brightness;
	int pwr_saving_offset;

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	snprintf(path, MAX_NAME, MDNIE_BACKLIGHT_BRIGHTNESS_PATH);
	if(!sys_check_node((char *)path)) {
		ret = sys_get_int(path, value);
	} else {
		snprintf(path, MAX_NAME, BACKLIGHT_BRIGHTNESS_PATH, disp_info[index].bl_name);
		ret = sys_get_int(path, value);
	}

	/*devmgr_log("path[%s]value[%d]power_saving[%d]", path, *value, power_saving);*/

	if (power_saving){
		snprintf(path, MAX_NAME, BACKLIGHT_MAX_BRIGHTNESS_PATH, disp_info[index].bl_name);
		ret = sys_get_int(path, &max_brightness);
		if (ret)
		{
			devmgr_log("Can't read max_brightness node[%s]", path);
			return ret;
		}
		pwr_saving_offset = (PWR_SAVING_CANDELA_CRITERION * max_brightness / MAX_CANDELA_CRITERION) + 0.5;

		if (*value > max_brightness - pwr_saving_offset)
			*value = max_brightness;
		else
			*value = *value + pwr_saving_offset;

		devmgr_log("power_saving result[%d]", *value);
	}

	return ret;
}

int OEM_sys_set_backlight_dimming(int index, int value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	devmgr_log("index is %d, value is %d",  index, value);
	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	snprintf(path, MAX_NAME, BACKLIGHT_DIMMING_PATH, disp_info[index].bl_name);
	devmgr_log("path[%s]value[%d]", path, value);
	ret = sys_set_int(path, value);

	return ret;
}

int set_backlight_brightness(int index, int value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	snprintf(path, MAX_NAME, MDNIE_BACKLIGHT_BRIGHTNESS_PATH);
	if(!sys_check_node((char *)path)) {
		ret = sys_set_int(path, value);
	} else {
		snprintf(path, MAX_NAME, BACKLIGHT_BRIGHTNESS_PATH, disp_info[index].bl_name);
		ret = sys_set_int(path, value);
	}

	return ret;
}

int OEM_sys_set_backlight_brightness(int index, int value, int power_saving)
{
	int ret = -1;
	char path[MAX_NAME+1];
	int max_brightness;
	int pwr_saving_offset;

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	devmgr_log("path[%s]value[%d]power_saving[%d]", path, value, power_saving);

	if (power_saving)
		devmgr_log("Doesn't support power saving for brightness control.\n");

	current_brightness = value;

	if (value > backlight_max_brightness)
		value = backlight_max_brightness;

	ret = set_backlight_brightness(index, value);
	if (ret)
	{
		devmgr_log("Can't set backlight brightness");
		return ret;
	}

	return ret;
}

int OEM_sys_get_backlight_acl_control(int index, int *value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	snprintf(path, MAX_NAME, LCD_ACL_CONTROL_PATH, disp_info[index].lcd_name);
	ret = sys_get_int(path, value);
	devmgr_log("path[%s]value[%d]", path, *value);

	return ret;
}

int OEM_sys_set_backlight_acl_control(int index, int value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	snprintf(path, MAX_NAME, LCD_ACL_CONTROL_PATH, disp_info[index].lcd_name);
	devmgr_log("path[%s]value[%d]", path, value);
	ret = sys_set_int(path, value);

	return ret;
}

int OEM_sys_get_backlight_elvss_control(int index, int *value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	snprintf(path, MAX_NAME, LCD_ELVSS_CONTROL_PATH, disp_info[index].lcd_name);
	ret = sys_get_int(path, value);
	devmgr_log("path[%s]value[%d]", path, *value);

	return ret;
}

int OEM_sys_set_backlight_elvss_control(int index, int value)
{
	int ret = -1;
	int temp;
	char path[MAX_NAME+1];

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	if (value > 0)
		temp = TEMP_RANGE_0;
	else if (value > -200)
		temp = TEMP_RANGE_1;
	else
		temp = TEMP_RANGE_2;

	if (elvss_temp_stage == temp)
		ret = 0;
	else {
		elvss_temp_stage = temp;
		snprintf(path, MAX_NAME, LCD_ELVSS_CONTROL_PATH, disp_info[index].lcd_name);
		SLOGI("path[%s]value[%d]", path, elvss_temp_stage);
		ret = sys_set_int(path, elvss_temp_stage);
	}

	return ret;
}

int OEM_sys_get_lcd_power(int index, int *value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	snprintf(path, MAX_NAME, LCD_POWER_PATH, disp_info[index].lcd_name);
	ret = sys_get_int(path, value);
	/*devmgr_log("path[%s]value[%d]", path, *value);*/

	return ret;
}

int OEM_sys_set_lcd_power(int index, int value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	snprintf(path, MAX_NAME, LCD_POWER_PATH, disp_info[index].lcd_name);
	devmgr_log("path[%s]value[%d]", path, value);
	ret = sys_set_int(path, value);

	return ret;
}

/* image_enhance */
/* mode - dynamic, standard, natural, movie */
enum image_enhance_mode {
	MODE_DYNAMIC = 0,
	MODE_STANDARD,
	MODE_NATURAL,
	MODE_MOVIE,
	MODE_AUTO,
	MODE_MAX,
};

/* scenario - ui, gallery, video, vtcall, camera, browser, negative, bypass */
enum image_enhance_scenario {
	SCENARIO_UI = 0,
	SCENARIO_GALLERY,
	SCENARIO_VIDEO,
	SCENARIO_VTCALL,
	SCENARIO_CAMERA,
	SCENARIO_BROWSER,
	SCENARIO_NEGATIVE,
	SCENARIO_EMAIL,
	SCENARIO_EBOOK,
	SCEANRIO_GRAY,
	SCENARIO_MAX,
};

/* tone - normal, warm, cold */
enum image_enhance_tone {
	TONE_NORMAL = 0,
	TONE_WARM,
	TONE_COLD,
};

/* tone browser - tone1, tone2, tone3 */
enum image_enhance_tone_br {
	TONE_1 = 0,
	TONE_2,
	TONE_3,
};

/* outdoor - off, on */
enum image_enhance_outdoor {
	OUTDOOR_OFF = 0,
	OUTDOOR_ON,
};

/* index - mode, scenario, tone, outdoor, tune */
enum image_enhance_index {
	INDEX_MODE,
	INDEX_SCENARIO,
	INDEX_TONE,
	INDEX_OUTDOOR,
	INDEX_TUNE,
	INDEX_COLOR_BLIND,
	INDEX_CABC,
	INDEX_MAX,
};

const char *image_enhance_str[INDEX_MAX] = {
	"mode",
	"scenario",
	"tone",
	"outdoor",
	"tune",
	"accessibility",
	"cabc",
};

struct image_enhance_info {
	enum image_enhance_mode mode;
	enum image_enhance_scenario scenario;
	enum image_enhance_tone tone;
	enum image_enhance_outdoor outdoor;
};

int OEM_sys_get_lcd_cabc(int index, int *value)
{
	char lcd_cabc_path[MAX_NAME+1];
	char mdnie_cabc_path[MAX_NAME+1];
	int ret = -1;

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	snprintf(lcd_cabc_path, MAX_NAME, LCD_CABC_CONTROL_PATH, disp_info[index].lcd_name);
	snprintf(mdnie_cabc_path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_CABC]);

	if(!sys_check_node((char *)mdnie_cabc_path)) {
		ret = sys_get_int((char *)mdnie_cabc_path, value);
		devmgr_log("path[%s]value[%d]", mdnie_cabc_path, *value);
	} else if (!sys_check_node((char *)lcd_cabc_path)) {
		ret = sys_get_int((char *)lcd_cabc_path, value);
		devmgr_log("path[%s]value[%d]", lcd_cabc_path, *value);
	} else {
		devmgr_log("fail to get cabc mode.");
		ret = -1;
	}

	return ret;
}

int OEM_sys_set_lcd_cabc(int index, int value)
{
	char lcd_cabc_path[MAX_NAME+1];
	char mdnie_cabc_path[MAX_NAME+1];
	int ret = -1;

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	snprintf(lcd_cabc_path, MAX_NAME, LCD_CABC_CONTROL_PATH, disp_info[index].lcd_name);
	snprintf(mdnie_cabc_path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_CABC]);

	if(!sys_check_node((char *)mdnie_cabc_path)) {
		ret = sys_set_int((char *)mdnie_cabc_path, value);
		devmgr_log("path[%s]value[%d]", mdnie_cabc_path, value);
	} else if (!sys_check_node((char *)lcd_cabc_path)) {
		ret = sys_set_int((char *)lcd_cabc_path, value);
		devmgr_log("path[%s]value[%d]", lcd_cabc_path, value);
	} else {
		devmgr_log("fail to set cabc mode.");
		ret = -1;
	}

	return ret;
}

int OEM_sys_get_auto_screen_tone(int index, int *value)
{
	char acl_path[MAX_NAME+1];
	char lcd_cabc_path[MAX_NAME+1];
	char mdnie_cabc_path[MAX_NAME+1];
	int ret = -1;

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	snprintf(acl_path, MAX_NAME, LCD_ACL_CONTROL_PATH, disp_info[index].lcd_name);
	snprintf(lcd_cabc_path, MAX_NAME, LCD_CABC_CONTROL_PATH, disp_info[index].lcd_name);
	snprintf(mdnie_cabc_path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_CABC]);

	if(!sys_check_node((char *)acl_path)) {
			ret = sys_get_int((char *)acl_path, value);
			devmgr_log("path[%s]value[%d]", acl_path, *value);
	} else {
		if(!sys_check_node((char *)mdnie_cabc_path)) {
			ret = sys_get_int((char *)mdnie_cabc_path, value);
			devmgr_log("path[%s]value[%d]", mdnie_cabc_path, *value);
		} else if (!sys_check_node((char *)lcd_cabc_path)) {
			ret = sys_get_int((char *)lcd_cabc_path, value);
			devmgr_log("path[%s]value[%d]", lcd_cabc_path, *value);
		} else {
			devmgr_log("fail to get auto screen tone.");
			ret = -1;
		}
	}
	return ret;
}

int OEM_sys_set_auto_screen_tone(int index, int value)
{
	char acl_path[MAX_NAME+1];
	char lcd_cabc_path[MAX_NAME+1];
	char mdnie_cabc_path[MAX_NAME+1];
	int ret = -1;

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	snprintf(acl_path, MAX_NAME, LCD_ACL_CONTROL_PATH, disp_info[index].lcd_name);
	snprintf(lcd_cabc_path, MAX_NAME, LCD_CABC_CONTROL_PATH, disp_info[index].lcd_name);
	snprintf(mdnie_cabc_path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_CABC]);

	if(!sys_check_node((char *)acl_path)) {
			ret = sys_set_int((char *)acl_path, value);
			devmgr_log("path[%s]value[%d]", acl_path, value);
	} else {
		if(!sys_check_node((char *)mdnie_cabc_path)) {
			if (value > CABC_OFF)
				value = CABC_USER_INTERFACE;
			ret = sys_set_int((char *)mdnie_cabc_path, value);
			devmgr_log("path[%s]value[%d]", mdnie_cabc_path, value);
		} else if (!sys_check_node((char *)lcd_cabc_path)) {
			if (value > CABC_OFF)
				value = CABC_OFF;
			ret = sys_set_int((char *)lcd_cabc_path, value);
			devmgr_log("path[%s]value[%d]", lcd_cabc_path, value);
		} else {
			devmgr_log("fail to set auto screen tone.");
			ret = -1;
		}
	}
	return ret;
}

int OEM_sys_get_image_enhance_color_blind(int *value)
{
	char path[MAX_NAME+1];
	int ret = -1;

	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_COLOR_BLIND]);
	ret = sys_get_int(path, value);
	devmgr_log("path[%s] value[%d]", path, *value);

	return ret;
}

int OEM_sys_set_image_enhance_color_blind(void *value)
{
	struct color_blind_info *color_blind_value = (struct color_blind_info *)value;
	char value_string[MAX_NAME+1];
	char path[MAX_NAME+1];
	int ret = -1;

	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_COLOR_BLIND]);
	sprintf(value_string, "%d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",\
		color_blind_value->mode,\
		color_blind_value->RrCr, color_blind_value->RgCg, color_blind_value->RbCb,\
		color_blind_value->GrMr, color_blind_value->GgMg, color_blind_value->GbMb,\
		color_blind_value->BrYr, color_blind_value->BgYg, color_blind_value->BbYb);

	ret = sys_set_str(path, value_string);

	devmgr_log("path[%s] value[%s]", path, value_string);

	return ret;
}

int OEM_sys_get_image_enhance_save(void *image_enhance)
{
	int ret = -1;
	char path[MAX_NAME+1];
	struct image_enhance_info *image_enhance_save = (struct image_enhance_info *)image_enhance;

	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_MODE]);
	ret = sys_get_int(path, &image_enhance_save->mode);
	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_SCENARIO]);
	ret = sys_get_int(path, &image_enhance_save->scenario);
	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_TONE]);
	ret = sys_get_int(path, &image_enhance_save->tone);
	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_OUTDOOR]);
	ret = sys_get_int(path, &image_enhance_save->outdoor);
	devmgr_log("path[%s]mode[%d]scenario[%d]tone[%d]outdoor[%d]", path, image_enhance_save->mode,
		image_enhance_save->scenario, image_enhance_save->tone, image_enhance_save->outdoor);

	return ret;
}

int OEM_sys_set_image_enhance_restore(void *image_enhance)
{
	int ret = -1;
	char path[MAX_NAME+1];
	struct image_enhance_info *image_enhance_restore = (struct image_enhance_info *)image_enhance;

	devmgr_log("path[%s]mode[%d]scenario[%d]tone[%d]outdoor[%d]", path, image_enhance_restore->mode,
		image_enhance_restore->scenario, image_enhance_restore->tone, image_enhance_restore->outdoor);
	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_MODE]);
	ret = sys_set_int(path, image_enhance_restore->mode);
	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_SCENARIO]);
	ret = sys_set_int(path, image_enhance_restore->scenario);
	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_TONE]);
	ret = sys_set_int(path, image_enhance_restore->tone);
	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_OUTDOOR]);
	ret = sys_set_int(path, image_enhance_restore->outdoor);

	return ret;
}

int OEM_sys_get_image_enhance_mode(int *value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_MODE]);
	ret = sys_get_int(path, value);
	devmgr_log("path[%s]value[%d]", path, *value);

	return ret;
}

int OEM_sys_set_image_enhance_mode(int value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_MODE]);
	devmgr_log("path[%s]value[%d]", path, value);
	ret = sys_set_int(path, value);

	return ret;
}

int OEM_sys_get_image_enhance_scenario(int *value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_SCENARIO]);
	ret = sys_get_int(path, value);
	devmgr_log("path[%s]value[%d]", path, *value);

	return ret;
}

int OEM_sys_set_image_enhance_scenario(int value)
{
	int ret = -1;
	int screen_tone = 0;
	char path[MAX_NAME+1];

	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_SCENARIO]);
	devmgr_log("path[%s]value[%d]", path, value);
	ret = sys_set_int(path, value);

	return ret;
}

int OEM_sys_get_image_enhance_tone(int *value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_TONE]);
	ret = sys_get_int(path, value);
	devmgr_log("path[%s]value[%d]", path, *value);

	return ret;
}

int OEM_sys_set_image_enhance_tone(int value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_TONE]);
	devmgr_log("path[%s]value[%d]", path, value);
	ret = sys_set_int(path, value);

	return ret;
}

int OEM_sys_get_image_enhance_outdoor(int *value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_OUTDOOR]);
	ret = sys_get_int(path, value);
	devmgr_log("path[%s]value[%d]", path, *value);

	return ret;
}

int OEM_sys_set_image_enhance_outdoor(int value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_OUTDOOR]);
	devmgr_log("path[%s]value[%d]", path, value);
	ret = sys_set_int(path, value);

	return ret;
}

int OEM_sys_get_image_enhance_tune(int *value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_TUNE]);
	ret = sys_get_int(path, value);
	devmgr_log("path[%s]value[%d]", path, *value);

	return ret;
}

int OEM_sys_set_image_enhance_tune(int value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	snprintf(path, MAX_NAME, IMAGE_ENHANCE_PATH, image_enhance_str[INDEX_TUNE]);
	devmgr_log("path[%s]value[%d]", path, value);
	ret = sys_set_int(path, value);

	return ret;
}

int OEM_sys_image_enhance_info(int *value)
{
	DIR *dir_info;
	struct dirent *dir_entry;
	int ret = -1;
	const char * image_enhance_path_info = IMAGE_ENHANCE_PATH_INFO;

	dir_info = opendir(image_enhance_path_info);

	if (NULL != dir_info) {
		*value = 1;
		ret = 0;
	} else {
		*value = 0;
		ret = -ENOENT;
	}

	if (NULL != dir_info)
		closedir(dir_info);

	return ret;
}

int OEM_sys_set_display_frame_rate(int value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	snprintf(path, MAX_NAME, DISPLAY_FRAME_RATE_PATH);
	devmgr_log("path[%s]value[%d]", path, value);
	ret = sys_set_int(path, value);

	return ret;
}

GENERATE_ACCESSORS_INT_RW(haptic_motor_level, HAPTIC_MOTOR_LEVEL_PATH)
GENERATE_ACCESSORS_INT_R(haptic_motor_level_max, HAPTIC_MOTOR_LEVEL_MAX_PATH)
GENERATE_ACCESSORS_INT_W(haptic_motor_enable, HAPTIC_MOTOR_ENABLE_PATH)
GENERATE_ACCESSORS_INT_W(haptic_motor_oneshot, HAPTIC_MOTOR_ONESHOT_PATH)

GENERATE_ACCESSORS_INT_R(battery_capacity, BATTERY_CAPACITY_PATH)
GENERATE_ACCESSORS_INT_R(battery_charge_full, BATTERY_CHARGE_FULL_PATH)
GENERATE_ACCESSORS_INT_R(battery_charge_now, BATTERY_CHARGE_NOW_PATH)
GENERATE_ACCESSORS_INT_R(battery_present, BATTERY_PRESENT_PATH)

int OEM_sys_get_battery_capacity_raw(int *value)
{
	int ret;

	ret = sys_get_int(BATTERY_CAPACITY_RAW_PATH, value);
	if (ret == -1) {
		return -ENODEV;
	}

	return ret;
}

static char *health_text[] = {
	"Unknown", "Good", "Overheat", "Dead", "Over voltage",
	"Unspecified failure", "Cold",
};

int OEM_sys_get_battery_health(int *value)
{
	char buf[BUFF_MAX] = {0};
	int ret = 0;
	int i = 0;

	ret = sys_get_str(BATTERY_HEALTH_PATH, buf);
	if (ret == -1)
		return -1;

	for (i = 0; i < BATTERY_HEALTH_MAX; i++) {
		if (strncmp(buf, health_text[i], strlen(health_text[i])) == 0) {
			*value = i;
			return 0;
		}
	}

	return -1;
}

int OEM_sys_get_battery_polling_required(int *value)
{
	*value = 0;

	return 0;
}

int OEM_sys_get_battery_support_insuspend_charging(int *value)
{
	*value = 1;

	return 0;
}

static char uart_node_path[MAX_NAME];
static char usb_node_path[MAX_NAME];

/* find uart/usb node path */
static int OEM_sys_muic_node_path_info()
{
	int err = -1;

	err = sys_check_node(UART_PATH);
	if (!err)
		sys_get_node(UART_PATH, uart_node_path);
	else {
		err = sys_check_node(UART_PATH_TRATS);
		if (err) {
			devmgr_log("uart path node not found");
			return -1;
		}
		sys_get_node(UART_PATH_TRATS, uart_node_path);
	}

	err = sys_check_node(USB_PATH);
	if (!err)
		sys_get_node(USB_PATH, usb_node_path);
	else {
		err = sys_check_node(USB_PATH_TRATS);
		if (err) {
			devmgr_log("usb path node not found");
			return -1;
		}
		sys_get_node(USB_PATH_TRATS, usb_node_path);
	}
	return 0;
}

int OEM_sys_get_uart_path(int *value)
{
	char buf[BUFF_MAX] = {0};
	int ret = 0;

	ret = sys_get_str(uart_node_path, buf);
	if (ret == -1)
		return -1;

	if (strncmp(buf, "CP", 2) == 0) {
		*value = PATH_CP;
		return 0;
	} else if (strncmp(buf, "AP", 2) == 0) {
		*value = PATH_AP;
		return 0;
	}

	return -1;
}

int OEM_sys_set_uart_path(int value)
{
	switch (value) {
	case PATH_CP:
		return sys_set_str(uart_node_path, "CP");
	case PATH_AP:
		return sys_set_str(uart_node_path, "AP");
	}

	return -1;
}


int OEM_sys_get_usb_path(int *value)
{
	char buf[BUFF_MAX] = {0};
	int ret = 0;

	ret = sys_get_str(usb_node_path, buf);
	if (ret == -1)
		return -1;

	if (strncmp(buf, "PDA", 3) == 0) {
		*value = PATH_AP;
		return 0;
	} else if (strncmp(buf, "MODEM", 5) == 0) {
		*value = PATH_CP;
		return 0;
	}

	return -1;
}

int OEM_sys_set_usb_path(int value)
{
	switch (value) {
	case PATH_CP:
		return sys_set_str(usb_node_path, "MODEM");
	case PATH_AP:
		return sys_set_str(usb_node_path, "PDA");
	}

	return -1;
}

GENERATE_ACCESSORS_INT_R(jack_charger_online, JACK_CHARGER_ONLINE_PATH)
GENERATE_ACCESSORS_INT_R(jack_earjack_online, JACK_EARJACK_ONLINE_PATH)
GENERATE_ACCESSORS_INT_R(jack_earkey_online, JACK_EARKEY_ONLINE_PATH)
GENERATE_ACCESSORS_INT_R(jack_hdmi_online, JACK_HDMI_ONLINE_PATH)
GENERATE_ACCESSORS_INT_R(jack_usb_online, JACK_USB_ONLINE_PATH)
GENERATE_ACCESSORS_INT_R(jack_cradle_online, JACK_CRADLE_ONLINE_PATH)
GENERATE_ACCESSORS_INT_R(jack_tvout_online, JACK_TVOUT_ONLINE_PATH)

int OEM_sys_get_jack_keyboard_online(int *value)
{
	/* Currently, We don't provide SLP Based platform with keyboard I/F */
	int ret = -1;
	/*return sys_get_int(JACK_KEYBOARD_ONLINE_PATH, value);*/
	return ret;
}

int OEM_sys_get_hdmi_support(int *value)
{
	//TODO: remove for SC7727
	*value = 1;

	return 0;
}

int OEM_sys_set_irled_control(char *value)
{
	sys_set_str(IRLED_CONTROL_PATH, value);

	return 0;
}

 

static int OEM_sys_get_extcon(int type, int *value)
{
	int ret = -1;

	switch (type) {
		case USB_ID:
		{
			char buf[BUFF_MAX] = {0};
			char *stop;

			ret = sys_get_str(MUIC_USBID_PATH, buf);
			*value = (int)strtol(buf, &stop, 16);
			devmgr_log("USB ID:[0x%x]\n", buf);
		break;
		}
		default:
		break;
	}

	return ret;
}


GENERATE_ACCESSORS_INT_R(leds_torch_max_brightness, LEDS_TORCH_MAX_BRIGHTNESS_PATH)
GENERATE_ACCESSORS_INT_RW(leds_torch_brightness, LEDS_TORCH_BRIGHTNESS_PATH)

int OEM_sys_set_power_state(int value)
{
	char buf[BUFF_MAX] = {0};
	int ret = 0;
	switch (value) {
	case POWER_STATE_SUSPEND:
		return sys_set_str(POWER_STATE_PATH, "mem");
#if 0
	case POWER_STATE_PRE_SUSPEND:
		return sys_set_str(POWER_STATE_PATH, "pre_suspend");
#else
	case POWER_STATE_PRE_SUSPEND:
		{
			ret = sys_get_str(CPUFREQ_GOVERNOR_PATH, buf);

			if ((ret != -1) && (strncmp(buf, "sprdemand", 9) == 0)) {
				devmgr_log("---[LCD OFF] Non-Interactive mode setings for cpufreq parameters---\n");
				sys_set_int(CPUFREQ_ONDEMAND_SAMPLING_RATE, 500000);
				sys_set_int(DMCFREQ_ONDEMAND_POLLING_INTERVAL, 500000);
				}

			return sys_set_str(POWER_AUTOSLEEP_PATH, "mem");
		}
#endif
	case POWER_STATE_POST_RESUME:
		{
			ret = sys_get_str(CPUFREQ_GOVERNOR_PATH, buf);

			if ((ret != -1) && (strncmp(buf, "sprdemand", 9) == 0)) {
				devmgr_log("---[LCD ON] Interactive mode setings for cpufreq parameters---\n");
				sys_set_int(CPUFREQ_ONDEMAND_SAMPLING_RATE, 50000);
				sys_set_int(DMCFREQ_ONDEMAND_POLLING_INTERVAL, 50000);
				}

		return sys_set_str(POWER_STATE_PATH, "post_resume");
		}
	}

	return -1;
}

int OEM_sys_set_power_lock(int value)
{
	static int power_lock_state=-1;

	if(power_lock_state == value)
		return -1;
	else
		power_lock_state = value;

	switch (value) {
	case POWER_UNLOCK:
		return sys_set_str(POWER_UNLOCK_PATH, "mainlock");
	case POWER_LOCK:
		return sys_set_str(POWER_LOCK_PATH, "mainlock");
	}

	return 0;
}

int OEM_sys_get_power_lock_support(int *value)
{
	int err = -1;

	err = sys_check_node(POWER_LOCK_PATH);
	if (err == -1) {
		devmgr_log("power lock node not found");
		*value = 0;
	}
	else
		*value = 1;

	return 0;
}

int OEM_sys_set_resetkey_disable (int value)
{
	int ret = -1;

	if (value == 0)
		value = 1;
	else
		value = 0;

	ret= sys_set_int(KEY_MANUAL_RESET_PMIC_PATH, value);
	ret= sys_set_int(KEY_MANUAL_RESET_SAFEOUT_PATH, value);

	return ret;
}

GENERATE_ACCESSORS_INT_RW(power_wakeup_count, POWER_WAKEUP_COUNT_PATH)

GENERATE_ACCESSORS_INT_W(memnotify_threshold_lv1, MEMNOTIFY_THRESHOLD_LV1_PATH)
GENERATE_ACCESSORS_INT_W(memnotify_threshold_lv2, MEMNOTIFY_THRESHOLD_LV2_PATH)

GENERATE_ACCESSORS_INT_R(cpufreq_cpuinfo_max_freq, CPUFREQ_CPUINFO_MAX_FREQ_PATH)
GENERATE_ACCESSORS_INT_R(cpufreq_cpuinfo_min_freq, CPUFREQ_CPUINFO_MIN_FREQ_PATH)
GENERATE_ACCESSORS_INT_RW(cpufreq_scaling_max_freq, CPUFREQ_SCALING_MAX_FREQ_PATH)
GENERATE_ACCESSORS_INT_RW(cpufreq_scaling_min_freq, CPUFREQ_SCALING_MIN_FREQ_PATH)
GENERATE_ACCESSORS_INT_RW(cpufreq_power_max_freq, CPUFREQ_POWER_MAX_FREQ_PATH)
GENERATE_ACCESSORS_INT_RW(cpufreq_power_min_freq, CPUFREQ_POWER_MIN_FREQ_PATH)
GENERATE_ACCESSORS_INT_R(cpu_enable_max_number, CPU_ENABLE_MAX_NUMBER_PATH)
GENERATE_ACCESSORS_INT_W(cpu_enable_max_number, CPU_ENABLE_MAX_NUMBER_PATH)

#define GENERATE_ACCESSORS_INT_R_NO_CONVERT(_suffix, _item)	\
int OEM_sys_get_##_suffix(int *value)			\
{						\
	return sys_get_int_wo_convert(_item, value);	\
}

#define GENERATE_ACCESSORS_INT_W_NO_CONVERT(_suffix, _item)	\
int OEM_sys_set_##_suffix(int value)			\
{						\
	return sys_set_int_wo_convert(_item, value);	\
}

GENERATE_ACCESSORS_INT_R_NO_CONVERT(memnotify_victim_task, MEMNOTIFY_VICTIM_TASK_PATH)
GENERATE_ACCESSORS_INT_W_NO_CONVERT(process_monitor_mp_pnp, PROCESS_MONITOR_MP_PNP_PATH)
GENERATE_ACCESSORS_INT_W_NO_CONVERT(process_monitor_mp_vip, PROCESS_MONITOR_MP_VIP_PATH)

#define GENERATE_ACCESSORS_GET_NODE_PATH(_suffix, _item)	\
int OEM_sys_get_##_suffix(char *node)			\
{						\
	return sys_get_node(_item, node);	\
}

GENERATE_ACCESSORS_GET_NODE_PATH(touch_event, TOUCH_EVENT_NODE)
GENERATE_ACCESSORS_GET_NODE_PATH(memnotify_node, MEMNOTIFY_NODE)
GENERATE_ACCESSORS_GET_NODE_PATH(process_monitor_node, PROCESS_MONITOR_NODE)

GENERATE_ACCESSORS_INT_R(temperature_adc, TEMPERATURE_ADC_PATH)
GENERATE_ACCESSORS_INT_R(temperature_value, TEMPERATURE_VALUE_PATH)

#define SVCLED_PATTERN_MASK	0xFF000000
#define SVCLED_PATTERN_SHIFT		24
#define SVCLED_RGB_MASK		0x00FFFFFF

int OEM_sys_set_svcled_color(int rgb, int led_on_ms, int led_off_ms)
{
	int ret = -1;
	unsigned int svcled_pattern, svcled_rgb;
	char buf[BUFF_MAX];

	devmgr_log("RGB: [%d], LED_On_ms: [%d], LED_Off_ms: [%d] ", rgb, led_on_ms, led_off_ms);

	svcled_pattern = (rgb & SVCLED_PATTERN_MASK) >> SVCLED_PATTERN_SHIFT;

	if (svcled_pattern)
		ret = sys_set_int(SERVICE_LED_PATTERN_PATH, (int)svcled_pattern);
	else {
		svcled_rgb = rgb & SVCLED_RGB_MASK;
		snprintf(buf, sizeof(buf), "0x%x %d %d", svcled_rgb, led_on_ms, led_off_ms);

		ret = sys_set_str(SERVICE_LED_BLINK_PATH, buf);
		if (ret != 0)
			return ret;
	}

	return ret;
}

int OEM_sys_get_battery_technology(char *value)
{
	int err = -1;

	err = sys_get_str(BATTERY_TECHNOLOGY_PATH, value);
	if (err == 0)
		return 0;
	else
		return -1;
}

int OEM_sys_get_battery_temperature(int *value)
{
	int err = -1;

	err = sys_get_int(BATTERY_TEMPERATURE_PATH, value);

	if (err == 0)
		return 0;
	else
		return -1;
}

int OEM_sys_get_battery_voltage(int *value)
{
	int err = -1;

	err = sys_get_int(BATTERY_VOLTAGE_PATH, value);

	if (err == 0)
		return 0;
	else
		return -1;
}


static OEM_sys_devman_plugin_interface devman_plugin_interface_sc7727;

EXPORT_API const OEM_sys_devman_plugin_interface *OEM_sys_get_devman_plugin_interface()
{
	/* Light interfaces */
	devman_plugin_interface_sc7727.OEM_sys_get_display_count = &OEM_sys_get_display_count;
	devman_plugin_interface_sc7727.OEM_sys_get_backlight_min_brightness = &OEM_sys_get_backlight_min_brightness;
	devman_plugin_interface_sc7727.OEM_sys_get_backlight_max_brightness = &OEM_sys_get_backlight_max_brightness;
	devman_plugin_interface_sc7727.OEM_sys_set_backlight_max_brightness = &OEM_sys_set_backlight_max_brightness;
	devman_plugin_interface_sc7727.OEM_sys_get_backlight_brightness = &OEM_sys_get_backlight_brightness;
	devman_plugin_interface_sc7727.OEM_sys_set_backlight_brightness = &OEM_sys_set_backlight_brightness;
	devman_plugin_interface_sc7727.OEM_sys_set_backlight_dimming = &OEM_sys_set_backlight_dimming;
	devman_plugin_interface_sc7727.OEM_sys_get_backlight_acl_control = &OEM_sys_get_backlight_acl_control;
	devman_plugin_interface_sc7727.OEM_sys_set_backlight_acl_control = &OEM_sys_set_backlight_acl_control;

	devman_plugin_interface_sc7727.OEM_sys_get_lcd_power = &OEM_sys_get_lcd_power;
	devman_plugin_interface_sc7727.OEM_sys_set_lcd_power = &OEM_sys_set_lcd_power;

	/* Image Ehnhace interfaces */
	devman_plugin_interface_sc7727.OEM_sys_get_image_enhance_mode = &OEM_sys_get_image_enhance_mode;
	devman_plugin_interface_sc7727.OEM_sys_set_image_enhance_mode = &OEM_sys_set_image_enhance_mode;
	devman_plugin_interface_sc7727.OEM_sys_get_image_enhance_scenario = &OEM_sys_get_image_enhance_scenario;
	devman_plugin_interface_sc7727.OEM_sys_set_image_enhance_scenario = &OEM_sys_set_image_enhance_scenario;
	devman_plugin_interface_sc7727.OEM_sys_get_image_enhance_tone = &OEM_sys_get_image_enhance_tone;
	devman_plugin_interface_sc7727.OEM_sys_set_image_enhance_tone = &OEM_sys_set_image_enhance_tone;
	devman_plugin_interface_sc7727.OEM_sys_get_image_enhance_outdoor = &OEM_sys_get_image_enhance_outdoor;
	devman_plugin_interface_sc7727.OEM_sys_set_image_enhance_outdoor = &OEM_sys_set_image_enhance_outdoor;

	devman_plugin_interface_sc7727.OEM_sys_get_image_enhance_tune = &OEM_sys_get_image_enhance_tune;
	devman_plugin_interface_sc7727.OEM_sys_set_image_enhance_tune = &OEM_sys_set_image_enhance_tune;

	devman_plugin_interface_sc7727.OEM_sys_image_enhance_info = &OEM_sys_image_enhance_info;

	devman_plugin_interface_sc7727.OEM_sys_set_display_frame_rate = &OEM_sys_set_display_frame_rate;

	devman_plugin_interface_sc7727.OEM_sys_set_auto_screen_tone = &OEM_sys_set_auto_screen_tone;
	devman_plugin_interface_sc7727.OEM_sys_get_auto_screen_tone = &OEM_sys_get_auto_screen_tone;

	devman_plugin_interface_sc7727.OEM_sys_get_image_enhance_color_blind = &OEM_sys_get_image_enhance_color_blind;
	devman_plugin_interface_sc7727.OEM_sys_set_image_enhance_color_blind = &OEM_sys_set_image_enhance_color_blind;

	/* UART path interfaces */
	devman_plugin_interface_sc7727.OEM_sys_get_uart_path = &OEM_sys_get_uart_path;
	devman_plugin_interface_sc7727.OEM_sys_set_uart_path = &OEM_sys_set_uart_path;

	/* USB path interfaces */
	devman_plugin_interface_sc7727.OEM_sys_get_usb_path = &OEM_sys_get_usb_path;
	devman_plugin_interface_sc7727.OEM_sys_set_usb_path = &OEM_sys_set_usb_path;

	/* Vibrator interfaces */
	devman_plugin_interface_sc7727.OEM_sys_get_haptic_vibetones_level_max = &OEM_sys_get_haptic_motor_level_max;
	devman_plugin_interface_sc7727.OEM_sys_get_haptic_vibetones_level = &OEM_sys_get_haptic_motor_level;
	devman_plugin_interface_sc7727.OEM_sys_set_haptic_vibetones_level = &OEM_sys_set_haptic_motor_level;
	devman_plugin_interface_sc7727.OEM_sys_set_haptic_vibetones_enable = &OEM_sys_set_haptic_motor_enable;
	devman_plugin_interface_sc7727.OEM_sys_set_haptic_vibetones_oneshot = &OEM_sys_set_haptic_motor_oneshot;

	/* Battery interfaces */
	devman_plugin_interface_sc7727.OEM_sys_get_battery_capacity = &OEM_sys_get_battery_capacity;
	devman_plugin_interface_sc7727.OEM_sys_get_battery_capacity_raw = &OEM_sys_get_battery_capacity_raw;
	devman_plugin_interface_sc7727.OEM_sys_get_battery_charge_full = &OEM_sys_get_battery_charge_full;
	devman_plugin_interface_sc7727.OEM_sys_get_battery_charge_now = &OEM_sys_get_battery_charge_now;
	devman_plugin_interface_sc7727.OEM_sys_get_battery_present = &OEM_sys_get_battery_present;
	devman_plugin_interface_sc7727.OEM_sys_get_battery_health = &OEM_sys_get_battery_health;
	devman_plugin_interface_sc7727.OEM_sys_get_battery_polling_required= &OEM_sys_get_battery_polling_required;
	devman_plugin_interface_sc7727.OEM_sys_get_battery_support_insuspend_charging = &OEM_sys_get_battery_support_insuspend_charging;

	/* Connection interfaces  */
	devman_plugin_interface_sc7727.OEM_sys_get_jack_charger_online = &OEM_sys_get_jack_charger_online;
	devman_plugin_interface_sc7727.OEM_sys_get_jack_earjack_online = &OEM_sys_get_jack_earjack_online;
	devman_plugin_interface_sc7727.OEM_sys_get_jack_earkey_online = &OEM_sys_get_jack_earkey_online;
	devman_plugin_interface_sc7727.OEM_sys_get_jack_hdmi_online = &OEM_sys_get_jack_hdmi_online;
	devman_plugin_interface_sc7727.OEM_sys_get_jack_usb_online = &OEM_sys_get_jack_usb_online;
	devman_plugin_interface_sc7727.OEM_sys_get_jack_cradle_online = &OEM_sys_get_jack_cradle_online;
	devman_plugin_interface_sc7727.OEM_sys_get_jack_tvout_online = &OEM_sys_get_jack_tvout_online;
	devman_plugin_interface_sc7727.OEM_sys_get_jack_keyboard_online = &OEM_sys_get_jack_keyboard_online;

	devman_plugin_interface_sc7727.OEM_sys_get_hdmi_support = &OEM_sys_get_hdmi_support;
	
	devman_plugin_interface_sc7727.OEM_sys_get_extcon = &OEM_sys_get_extcon;

	/* Torch interfaces */
	devman_plugin_interface_sc7727.OEM_sys_get_leds_torch_max_brightness = &OEM_sys_get_leds_torch_max_brightness;
	devman_plugin_interface_sc7727.OEM_sys_get_leds_torch_brightness = &OEM_sys_get_leds_torch_brightness;
	devman_plugin_interface_sc7727.OEM_sys_set_leds_torch_brightness = &OEM_sys_set_leds_torch_brightness;

	/* Power management interfaces */
	devman_plugin_interface_sc7727.OEM_sys_set_power_state = &OEM_sys_set_power_state;
	devman_plugin_interface_sc7727.OEM_sys_set_power_lock = &OEM_sys_set_power_lock;
	devman_plugin_interface_sc7727.OEM_sys_get_power_lock_support = &OEM_sys_get_power_lock_support;

	/* TODO: Should determine enum values of wakeup_count nodes */
	devman_plugin_interface_sc7727.OEM_sys_get_power_wakeup_count = &OEM_sys_get_power_wakeup_count;
	devman_plugin_interface_sc7727.OEM_sys_set_power_wakeup_count = &OEM_sys_set_power_wakeup_count;

	/* OOM interfaces */
	devman_plugin_interface_sc7727.OEM_sys_get_memnotify_node = &OEM_sys_get_memnotify_node;
	devman_plugin_interface_sc7727.OEM_sys_get_memnotify_victim_task = &OEM_sys_get_memnotify_victim_task;
	devman_plugin_interface_sc7727.OEM_sys_set_memnotify_threshold_lv1 = &OEM_sys_set_memnotify_threshold_lv1;
	devman_plugin_interface_sc7727.OEM_sys_set_memnotify_threshold_lv2 = &OEM_sys_set_memnotify_threshold_lv2;

	/* Process monitor interfaces */
	devman_plugin_interface_sc7727.OEM_sys_get_process_monitor_node = &OEM_sys_get_process_monitor_node;
	devman_plugin_interface_sc7727.OEM_sys_set_process_monitor_mp_pnp = &OEM_sys_set_process_monitor_mp_pnp;
	devman_plugin_interface_sc7727.OEM_sys_set_process_monitor_mp_vip = &OEM_sys_set_process_monitor_mp_vip;

	/* UART path interfaces */
	devman_plugin_interface_sc7727.OEM_sys_get_cpufreq_cpuinfo_max_freq = &OEM_sys_get_cpufreq_cpuinfo_max_freq;
	devman_plugin_interface_sc7727.OEM_sys_get_cpufreq_cpuinfo_min_freq = &OEM_sys_get_cpufreq_cpuinfo_min_freq;
	devman_plugin_interface_sc7727.OEM_sys_get_cpufreq_scaling_max_freq = &OEM_sys_get_cpufreq_scaling_max_freq;
	devman_plugin_interface_sc7727.OEM_sys_set_cpufreq_scaling_max_freq = &OEM_sys_set_cpufreq_scaling_max_freq;
	devman_plugin_interface_sc7727.OEM_sys_get_cpufreq_scaling_min_freq = &OEM_sys_get_cpufreq_scaling_min_freq;
	devman_plugin_interface_sc7727.OEM_sys_set_cpufreq_scaling_min_freq = &OEM_sys_set_cpufreq_scaling_min_freq;
	devman_plugin_interface_sc7727.OEM_sys_get_cpufreq_power_max_freq = &OEM_sys_get_cpufreq_power_max_freq;
	devman_plugin_interface_sc7727.OEM_sys_set_cpufreq_power_max_freq = &OEM_sys_set_cpufreq_power_max_freq;
	devman_plugin_interface_sc7727.OEM_sys_get_cpufreq_power_min_freq = &OEM_sys_get_cpufreq_power_min_freq;
	devman_plugin_interface_sc7727.OEM_sys_set_cpufreq_power_min_freq = &OEM_sys_set_cpufreq_power_min_freq;

	devman_plugin_interface_sc7727.OEM_sys_get_battery_siop_active = &OEM_sys_get_battery_siop_active;
	devman_plugin_interface_sc7727.OEM_sys_set_battery_siop_active = &OEM_sys_set_battery_siop_active;

	devman_plugin_interface_sc7727.OEM_sys_get_cpu_enable_max_number = &OEM_sys_get_cpu_enable_max_number;
	devman_plugin_interface_sc7727.OEM_sys_set_cpu_enable_max_number= &OEM_sys_set_cpu_enable_max_number;

	devman_plugin_interface_sc7727.OEM_sys_set_pm_scenario = &Tizen_Resource_Manager;

	devman_plugin_interface_sc7727.OEM_sys_get_temperature_adc = &OEM_sys_get_temperature_adc;
	devman_plugin_interface_sc7727.OEM_sys_get_temperature_value = &OEM_sys_get_temperature_value;

	devman_plugin_interface_sc7727.OEM_sys_get_backlight_overheating_control = &OEM_sys_get_backlight_overheating_control;
	devman_plugin_interface_sc7727.OEM_sys_set_backlight_overheating_control = &OEM_sys_set_backlight_overheating_control;

	devman_plugin_interface_sc7727.OEM_sys_get_backlight_brightness_by_lux = &OEM_sys_get_backlight_brightness_by_lux;

	devman_plugin_interface_sc7727.OEM_sys_get_whitemagic_mode = &OEM_sys_get_whitemagic_mode;
	devman_plugin_interface_sc7727.OEM_sys_set_whitemagic_mode = &OEM_sys_set_whitemagic_mode;

	devman_plugin_interface_sc7727.OEM_sys_get_lcd_cabc = &OEM_sys_get_lcd_cabc;
	devman_plugin_interface_sc7727.OEM_sys_set_lcd_cabc = &OEM_sys_set_lcd_cabc;

	devman_plugin_interface_sc7727.OEM_sys_set_irled_control = &OEM_sys_set_irled_control;
	devman_plugin_interface_sc7727.OEM_sys_set_svcled_color = &OEM_sys_set_svcled_color;

	devman_plugin_interface_sc7727.OEM_sys_set_siop_control = &OEM_sys_set_siop_control;

	devman_plugin_interface_sc7727.OEM_sys_get_hall_status = &OEM_sys_get_hall_status;

	devman_plugin_interface_sc7727.OEM_sys_set_resetkey_disable = &OEM_sys_set_resetkey_disable;

	devman_plugin_interface_sc7727.OEM_sys_get_hardkey_backlight = &OEM_sys_get_hardkey_backlight;
	devman_plugin_interface_sc7727.OEM_sys_set_hardkey_backlight = &OEM_sys_set_hardkey_backlight;

	devman_plugin_interface_sc7727.OEM_sys_get_battery_technology = &OEM_sys_get_battery_technology;
	devman_plugin_interface_sc7727.OEM_sys_get_battery_temperature = &OEM_sys_get_battery_temperature;
	devman_plugin_interface_sc7727.OEM_sys_get_battery_voltage = &OEM_sys_get_battery_voltage;

	devman_plugin_interface_sc7727.OEM_sys_get_backlight_elvss_control = &OEM_sys_get_backlight_elvss_control;
	devman_plugin_interface_sc7727.OEM_sys_set_backlight_elvss_control = &OEM_sys_set_backlight_elvss_control;

	OEM_sys_display_info(disp_info);
	OEM_sys_muic_node_path_info();

	return &devman_plugin_interface_sc7727;
}
