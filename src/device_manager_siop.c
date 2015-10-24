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

#include "devman_define_node_path.h"
#include "device_manager_siop.h"
#include "device_manager_io.h"

#define DEVMGR_LOG
#if defined (DEVMGR_LOG)
#define LOG_TAG "DEVICE_PLUGIN"
#include <dlog/dlog.h>
#define devmgr_log(fmt, args...)	SLOGD(fmt, ##args)
#else
#define devmgr_log(fmt, args...)
#endif

/*===================================================
Redwood SIOP table
Last updated on 2013.5.15
====================================================*/
const device_siop_table siop_table[SIOP_SCENARIO_MAX][SIOP_LEVEL_MAX] =
{
	{/***********  LCD ON *****************************/
		/*  ITEM :   cpu_freq cpu_core  charging  BL lcd_freq */
		/*  UNIT :    MHz		#core		mA		cd		Hz       */
		/* level 0 */{RELEASE,		IGNORE,		IGNORE,		100,		IGNORE},
		/* level 1 */{RELEASE,		IGNORE,		IGNORE,		100,		IGNORE},
		/* level 2 */{RELEASE,		IGNORE,		IGNORE,		69,		IGNORE},
		/* level 3 */{1497600,		IGNORE,		IGNORE,		63,		IGNORE},
		/* level 4 */{1267200,		IGNORE,		IGNORE,		55,		IGNORE},
		/* level 5 */{1036800,		IGNORE,		IGNORE,		55,		IGNORE},
		/* level 6 */{1036800,		IGNORE,		IGNORE,		55,		IGNORE},
		/* level 7 */{1036800,		IGNORE,		IGNORE,		55,		IGNORE},
	},

	{/***********  LCD OFF ****************************/
		/*  ITEM :   cpu_freq cpu_core  charging  BL lcd_freq */
		/*  UNIT :    MHz   		#core		mA		cd		Hz       */
		/* level 0 */{IGNORE,		IGNORE,		IGNORE,		IGNORE,		IGNORE},
		/* level 1 */{IGNORE,		IGNORE,		IGNORE,		IGNORE,		IGNORE},
		/* level 2 */{IGNORE,		IGNORE,		IGNORE,		IGNORE,		IGNORE},
		/* level 3 */{IGNORE,		IGNORE,		IGNORE,		IGNORE,		IGNORE},
		/* level 4 */{IGNORE,		IGNORE,		IGNORE,		IGNORE,		IGNORE},
		/* level 5 */{IGNORE,		IGNORE,		IGNORE,		IGNORE,		IGNORE},
		/* level 6 */{IGNORE,		IGNORE,		IGNORE,		IGNORE,		IGNORE},
		/* level 7 */{IGNORE,		IGNORE,		IGNORE,		IGNORE,		IGNORE},
	},
};


#define MAX_NAME 255
#define BUFF_MAX	255

enum display_type
{
	DISP_MAIN = 0,
	DISP_SUB,
	DISP_MAX
};

struct display_info
{
	enum display_type etype; /* FIXME:!! Main LCD or Sub LCD node */
	char bl_name[MAX_NAME+1]; /* backlight name */
	char lcd_name[MAX_NAME+1]; /* lcd name */
};

extern struct display_info disp_info[DISP_MAX];

int OEM_sys_get_battery_siop_active(int *value)
{
	int ret;
	int siop_activated, siop_level;

	ret = sys_get_int(BATTERY_SIOP_ACTIVATE_PATH, &siop_activated);
	if (ret == -1) {
		return -ENODEV;
	}

	ret = sys_get_int(BATTERY_SIOP_LEVEL_PATH, &siop_level);
	if (ret == -1) {
		return -ENODEV;
	}

	if (siop_activated == 0)
		*value = 0;
	else
		*value = siop_level;

	return ret;

}

int OEM_sys_set_battery_siop_active(int value)
{
	int ret;
	int siop_activated, siop_level;

	siop_level = value;

	if (siop_level > 0)
		siop_activated = 1;
	else
		siop_activated = 0;

	ret = sys_set_int(BATTERY_SIOP_ACTIVATE_PATH, siop_activated);
	if (ret == -1) {
		return -ENODEV;
	}

	ret = sys_set_int(BATTERY_SIOP_LEVEL_PATH, siop_level);
	if (ret == -1) {
		return -ENODEV;
	}

	return ret;
}

static int sys_set_battery_siop_control(int level, int charge_current)
{
	int ret;
	int siop_activated=0;
	char buf[BUFF_MAX];

	if (charge_current>=0)
		siop_activated = 1;

	ret = sys_set_int(BATTERY_SIOP_ACTIVATE_PATH, siop_activated);
	if (ret == -1) {
		return -ENODEV;
	}

	if(siop_activated == 1) {
		ret = sys_set_int(BATTERY_SIOP_LEVEL_PATH, level);
		if (ret == -1) {
			return -ENODEV;
		}

		snprintf(buf, sizeof(buf), "%d %d", level, charge_current);

		ret = sys_set_str(BATTERY_SIOP_LEVEL_CURRENT_PATH, buf);
		if (ret == -1) {
			return -ENODEV;
		}
	}

	return ret;
}

int OEM_sys_get_backlight_overheating_control(int index, int *value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	snprintf(path, MAX_NAME, MDNIE_BACKLIGHT_OVERHEATING_PATH);
	if(!sys_check_node((char *)path)) {
		ret = sys_get_int(path, value);
	} else {
		snprintf(path, MAX_NAME, BACKLIGHT_OVERHEATING_PATH, disp_info[index].bl_name);
		ret = sys_get_int(path, value);
	}

	return ret;
}

extern int current_brightness;
extern int set_backlight_brightness(int index, int value);

int OEM_sys_set_backlight_overheating_control(int index, int value)
{
	int ret = -1;
	char path[MAX_NAME+1];

	if (index >= DISP_MAX) {
		devmgr_log("supports %d display node", DISP_MAX);
		return ret;
	}

	if ((current_brightness != -1) && (value > current_brightness)) {
		value = current_brightness;
	}

	ret = set_backlight_brightness(index, value);
	if (ret)
	{
		devmgr_log("Can't set backlight brightness");
		return ret;
	}

	return ret;
}

int current_level=-1, current_mode=-1;
device_siop_table current_table = {RELEASE, RELEASE, RELEASE, 100, 60};

int OEM_sys_set_siop_control(int level, int mode)
{
	device_siop_table new_table;

	memcpy(&new_table, &siop_table[mode][level], sizeof(device_siop_table));

	devmgr_log("level %d, mode %d",level, mode);
	devmgr_log("current_siop {%d, %d, %d, %d, %d}",current_table.cpu_freq, current_table.cpu_max_core,\
		current_table.battery_charing, current_table.backlight, current_table.lcd_freq);
	devmgr_log("new_siop {%d, %d, %d, %d, %d}",new_table.cpu_freq, new_table.cpu_max_core,\
		new_table.battery_charing, new_table.backlight, new_table.lcd_freq);

	if((current_level==level) && (current_mode==mode))
		return 0;

	/* CPU */
	if((new_table.cpu_freq != IGNORE) && (new_table.cpu_freq != current_table.cpu_freq)) {
		devmgr_log("CPU max clock %d",new_table.cpu_freq);
		sys_set_int(CPUFREQ_POWER_MAX_FREQ_PATH, new_table.cpu_freq);
		current_table.cpu_freq = new_table.cpu_freq;
	}
	if((new_table.cpu_max_core != IGNORE) && (new_table.cpu_max_core != current_table.cpu_max_core)) {
		devmgr_log("CPU max core num %d",new_table.cpu_max_core);
		sys_set_int(CPU_ENABLE_MAX_NUMBER_PATH, new_table.cpu_max_core);
		current_table.cpu_max_core = new_table.cpu_max_core;
	}

	/* Battery charging */
	//OEM_sys_set_battery_siop_active(siop_table[level].battery_charing);
	if((new_table.battery_charing != IGNORE) && (new_table.battery_charing != current_table.battery_charing)) {
		devmgr_log("battery_charing current %d",new_table.battery_charing);
		sys_set_battery_siop_control(level, new_table.battery_charing);
		current_table.battery_charing = new_table.battery_charing;
	}

	/* LCD */
	if((new_table.backlight != IGNORE) && (new_table.backlight != current_table.backlight)) {
		devmgr_log("backlight %d",new_table.backlight);
		OEM_sys_set_backlight_overheating_control(DISP_MAIN, new_table.backlight);
		current_table.backlight = new_table.backlight;
	}

	if((new_table.lcd_freq != IGNORE) && (new_table.lcd_freq != current_table.lcd_freq)) {
		devmgr_log("lcd_freq %d",new_table.lcd_freq);
		sys_set_int(DISPLAY_FRAME_RATE_PATH, new_table.lcd_freq);
		current_table.lcd_freq = new_table.lcd_freq;
	}

	current_level=level;
	current_mode=mode;

	return 0;
}

