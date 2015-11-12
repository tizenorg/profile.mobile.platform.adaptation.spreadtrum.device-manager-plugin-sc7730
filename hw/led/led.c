/*
 * device-node
 *
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/limits.h>

#include <hw/led.h>
#include "../shared.h"

static int led_open(struct hw_info *info,
		const char *id, struct hw_common **common)
{
	return 0;
}

static int led_close(struct hw_common *common)
{
	return 0;
}

HARDWARE_MODULE_STRUCTURE = {
	.magic = HARDWARE_INFO_TAG,
	.hal_version = HARDWARE_INFO_VERSION,
	.device_version = LED_HARDWARE_DEVICE_VERSION,
	.id = LED_HARDWARE_DEVICE_ID,
	.name = "Default LED",
	.author = "Jiyoung Yun <jy910.yun@samsung.com>",
	.open = led_open,
	.close = led_close,
};
