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


#ifndef __DEVMAN_DEFINE_NODE_PATH_H
#define __DEVMAN_DEFINE_NODE_PATH_H

#define BACKLIGHT_PATH "/sys/class/backlight/"
#define BACKLIGHT_MAX_BRIGHTNESS_PATH	BACKLIGHT_PATH"%s/max_brightness"
#define BACKLIGHT_MIN_BRIGHTNESS_PATH	BACKLIGHT_PATH"%s/min_brightness"
#define BACKLIGHT_BRIGHTNESS_PATH 	BACKLIGHT_PATH"%s/brightness"
#define BACKLIGHT_DIMMING_PATH	BACKLIGHT_PATH"%s/dimming"
#define BACKLIGHT_OVERHEATING_PATH  BACKLIGHT_PATH"%s/overheating"
#define MDNIE_BACKLIGHT_BRIGHTNESS_PATH        BACKLIGHT_PATH"mdnie-bl/brightness"
#define MDNIE_BACKLIGHT_OVERHEATING_PATH  BACKLIGHT_PATH"mdnie-bl/overheating"

#define LCD_PATH  "/sys/class/lcd/"
#define LCD_WM_CONTROL_PATH	LCD_PATH"%s/wm_mode"
#define LCD_CABC_CONTROL_PATH	LCD_PATH"%s/cabc"
#define LCD_ACL_CONTROL_PATH	LCD_PATH"%s/device/acl"
#define LCD_ELVSS_CONTROL_PATH	LCD_PATH"%s/device/elvss"
#define LCD_POWER_PATH			LCD_PATH"%s/lcd_power"
#define IMAGE_ENHANCE_PATH			"/sys/class/extension/mdnie/%s"
#define IMAGE_ENHANCE_PATH_INFO			"/sys/class/extension/mdnie"

#define DISPLAY_FRAME_RATE_PATH	"/sys/class/dispfreq/exynos/refresh"

#define UART_PATH			"/sys/devices/virtual/sec/switch/uart_sel"
#define USB_PATH			"/sys/devices/virtual/sec/switch/usb_sel"
#define UART_PATH_TRATS			"/sys/devices/platform/uart-select/path"
#define USB_PATH_TRATS			"/sys/devices/platform/usb-select/path"

#define COVER_STATUS_PATH		"/sys/class/flip/hall_ic/cover_status"
#define HAPTIC_MOTOR_LEVEL_PATH	"/sys/class/haptic/motor/level"
#define HAPTIC_MOTOR_LEVEL_MAX_PATH	"/sys/class/haptic/motor/level_max"
#define HAPTIC_MOTOR_ENABLE_PATH	"/sys/class/haptic/motor/enable"
#define HAPTIC_MOTOR_ONESHOT_PATH	"/sys/class/haptic/motor/oneshot"

#define BATTERY_CAPACITY_PATH		"/sys/class/power_supply/battery/capacity"
#define BATTERY_CAPACITY_RAW_PATH	"/sys/class/power_supply/battery/capacity_raw"
#define BATTERY_CHARGE_FULL_PATH	"/sys/class/power_supply/battery/charge_full"
#define BATTERY_CHARGE_NOW_PATH		"/sys/class/power_supply/battery/charge_now"
#define BATTERY_PRESENT_PATH		"/sys/class/power_supply/battery/present"
#define BATTERY_HEALTH_PATH		"/sys/class/power_supply/battery/health"
#define BATTERY_SIOP_ACTIVATE_PATH	"/sys/class/power_supply/battery/siop_activated"
#define BATTERY_SIOP_LEVEL_PATH	"/sys/class/power_supply/battery/siop_level"
#define BATTERY_SIOP_LEVEL_CURRENT_PATH	"/sys/class/power_supply/battery/siop_level_charge_current"

#define JACK_CHARGER_ONLINE_PATH	"/sys/devices/platform/jack/charger_online"
#define JACK_EARJACK_ONLINE_PATH	"/sys/class/switch/earjack/state"
#define JACK_EARKEY_ONLINE_PATH		"/sys/class/switch/earkey/state"
#define JACK_HDMI_ONLINE_PATH		"/sys/devices/platform/jack/hdmi_online"
#define JACK_USB_ONLINE_PATH		"/sys/devices/virtual/switch/usb_cable/state"
#define JACK_CRADLE_ONLINE_PATH		"/sys/class/switch/cradle/state"
#define JACK_TVOUT_ONLINE_PATH		"/sys/devices/platform/jack/tvout_online"
#define JACK_KEYBOARD_ONLINE_PATH		"/sys/devices/platform/jack/keyboard_online"

#define LEDS_TORCH_MAX_BRIGHTNESS_PATH	"/sys/class/leds/torch-sec1/max_brightness"
#define LEDS_TORCH_BRIGHTNESS_PATH	"/sys/class/leds/torch-sec1/brightness"

#define MUIC_USBID_PATH   "/sys/devices/virtual/sec/switch/adc"

#define POWER_STATE_PATH		"/sys/power/state"
#define POWER_AUTOSLEEP_PATH		"/sys/power/autosleep"
#define POWER_UNLOCK_PATH		"/sys/power/wake_unlock"
#define POWER_LOCK_PATH		"/sys/power/wake_lock"
#define POWER_WAKEUP_COUNT_PATH		"/sys/power/wakeup_count"

#define MEMNOTIFY_NODE			"/dev/memnotify"
#define MEMNOTIFY_VICTIM_TASK_PATH	"/sys/class/memnotify/victim_task"
#define MEMNOTIFY_THRESHOLD_LV1_PATH	"/sys/class/memnotify/threshold_lv1"
#define MEMNOTIFY_THRESHOLD_LV2_PATH	"/sys/class/memnotify/threshold_lv2"

#define TOUCH_EVENT_NODE		"/dev/event2"

#define PROCESS_MONITOR_NODE		"/dev/pmon"
#define PROCESS_MONITOR_MP_PNP_PATH	"/sys/class/pmon/mp_pnp"
#define PROCESS_MONITOR_MP_VIP_PATH	"/sys/class/pmon/mp_vip"

#define CPU_ENABLE_MAX_NUMBER_PATH		"/sys/devices/system/cpu/cpufreq/slp/max_cpu_lock"
#define CPUFREQ_CPUINFO_MAX_FREQ_PATH	"/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq"
#define CPUFREQ_CPUINFO_MIN_FREQ_PATH	"/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_min_freq"
#define CPUFREQ_SCALING_MAX_FREQ_PATH	"/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq"
#define CPUFREQ_SCALING_MIN_FREQ_PATH	"/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq"
#define CPUFREQ_POWER_MAX_FREQ_PATH	"/sys/devices/system/cpu/cpufreq/pmqos/cpufreq_max"
#define CPUFREQ_POWER_MIN_FREQ_PATH	"/sys/devices/system/cpu/cpufreq/pmqos/cpufreq_min"
/* For Non-Interactive settings */
#define CPUFREQ_GOVERNOR_PATH	"/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
#define CPUFREQ_ONDEMAND_SAMPLING_RATE	"/sys/devices/system/cpu/cpufreq/sprdemand/sampling_rate"
#define DMCFREQ_ONDEMAND_POLLING_INTERVAL	"/sys/devices/platform/scxx30-dmcfreq.0/devfreq/scxx30-dmcfreq.0/polling_interval"


#define TEMPERATURE_ADC_PATH	"/sys/devices/platform/sec-thermistor/temp-adc"
#define TEMPERATURE_VALUE_PATH 	"/sys/devices/platform/sec-thermistor/temperature"

#define IRLED_CONTROL_PATH 	"/sys/class/sec/sec_ir/ir_send"

#define SERVICE_LED_R_PATH "/sys/class/sec/led/led_r"
#define SERVICE_LED_G_PATH "/sys/class/sec/led/led_g"
#define SERVICE_LED_B_PATH "/sys/class/sec/led/led_b"

#define SERVICE_LED_BLINK_PATH "/sys/class/sec/led/led_blink"
#define SERVICE_LED_PATTERN_PATH "/sys/class/sec/led/led_pattern"

#define KEY_MANUAL_RESET_PMIC_PATH "/sys/bus/platform/drivers/max77686-pmic/max77686-pmic/mrstb"
#define KEY_MANUAL_RESET_SAFEOUT_PATH "/sys/bus/platform/drivers/max77693-safeout/max77693-safeout/mrstb"

#define TOUCHKEY_LED_PATH "/sys/devices/virtual/sec/sec_touchkey/brightness"

#define BATTERY_TECHNOLOGY_PATH		"/sys/class/power_supply/battery/technology"
#define BATTERY_TEMPERATURE_PATH		"/sys/class/power_supply/battery/temp"
#define BATTERY_VOLTAGE_PATH			"/sys/class/power_supply/battery/voltage_now"


#endif /* __DEVMAN_DEFINE_NODE_PATH_H */
