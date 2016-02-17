/*
 * device-node
 *
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/limits.h>
#include <dirent.h>

#include <hw/ir.h>
#include "../shared.h"

#define IR_PATH		"/tmp/ir_test"

bool ir_is_available()
{
	return true;
}

int ir_transmit(int *frequency_pattern, int size)
{
	int i, ret;

	if (!frequency_pattern)
		return -EINVAL;

	for (i = 0; i < size; i++) {
		_D("%d", frequency_pattern[i]);
		ret = sys_set_int(IR_PATH, frequency_pattern[i]);
		if (ret < 0) {
			_E("Failed to transmit ir pattern");
			return ret;
		}
	}
	return 0;
}

static int ir_open(struct hw_info *info,
		const char *id, struct hw_common **common)
{
	struct ir_device *ir_dev;

	if (!info || !common)
		return -EINVAL;

	ir_dev = calloc(1, sizeof(struct ir_device));
	if (!ir_dev)
		return -ENOMEM;

	ir_dev->common.info = info;
	ir_dev->is_available = ir_is_available;
	ir_dev->transmit = ir_transmit;

	*common = (struct hw_common *)ir_dev;
	return 0;
}

static int ir_close(struct hw_common *common)
{
	if (!common)
		return -EINVAL;

	free(common);
	return 0;
}

HARDWARE_MODULE_STRUCTURE = {
	.magic = HARDWARE_INFO_TAG,
	.hal_version = HARDWARE_INFO_VERSION,
	.device_version = IR_HARDWARE_DEVICE_VERSION,
	.id = IR_HARDWARE_DEVICE_ID,
	.name = "ir",
	.open = ir_open,
	.close = ir_close,
};
