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


#ifndef	__DEVMAN_SIOP_H__
#define	__DEVMAN_SIOP_H__

enum {
	SIOP_LEVEL_0,
	SIOP_LEVEL_1,
	SIOP_LEVEL_2,
	SIOP_LEVEL_3,
	SIOP_LEVEL_4,
	SIOP_LEVEL_5,
	SIOP_LEVEL_6,
	SIOP_LEVEL_7,
	SIOP_LEVEL_MAX,
};

enum {
	LCD_ON,
	LCD_OFF,
	SIOP_SCENARIO_MAX,
};

enum {
	CPU_DUAL = 2,
	CPU_QUAD = 4,
};

#define RELEASE -1
#define IGNORE -2

typedef struct {
	int cpu_freq;
	int cpu_max_core;
	int battery_charing;
	int backlight;
	int lcd_freq;
} device_siop_table;

int OEM_sys_get_battery_siop_active(int *value);
int OEM_sys_set_battery_siop_active(int value);
int OEM_sys_get_backlight_overheating_control(int index, int *value);
int OEM_sys_set_backlight_overheating_control(int index, int value);
int OEM_sys_set_siop_control(int level, int mode);

#endif	/* __DEVMAN_PLUGIN_INTF_H__ */
