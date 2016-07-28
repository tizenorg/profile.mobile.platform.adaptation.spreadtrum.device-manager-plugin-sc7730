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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/limits.h>
#include <dirent.h>

#include <hw/usb_client.h>
#include "usb_config.h"
#include "../shared.h"
#include "../udev.h"

#define CONFIG_ROOTPATH        "/sys/class/usb_mode/usb0"
#define CONFIG_LOAD            CONFIG_ROOTPATH"/enable"
#define CONFIG_IDVENDOR        CONFIG_ROOTPATH"/idVendor"
#define CONFIG_IDPRODUCT       CONFIG_ROOTPATH"/idProduct"
#define CONFIG_FUNCTION_FIRST  CONFIG_ROOTPATH"/funcs_fconf"
#define CONFIG_FUNCTION_SECOND CONFIG_ROOTPATH"/funcs_sconf"
#define CONFIG_CLASS           CONFIG_ROOTPATH"/bDeviceClass"
#define CONFIG_SUBCLASS        CONFIG_ROOTPATH"/bDeviceSubClass"
#define CONFIG_PROTOCOL        CONFIG_ROOTPATH"/bDeviceProtocol"
#define CONFIG_IPRODUCT        CONFIG_ROOTPATH"/iProduct"

#define USB_CONFIGURED_PATH    "/sys//devices/virtual/usb_mode/usb0/uevent"

static struct uevent_data {
	GadgetsEnabled enabled_cb;
	void *data;
} udata = { 0, };

static unsigned int prev_gadgets = 0;

unsigned int get_current_gadgets(void)
{
	int i;
	int ret;
	char buf[32], *tmp;
	size_t len;

	ret = sys_get_str(CONFIG_FUNCTION_FIRST, buf, sizeof(buf));
	if (ret < 0) {
		_E("Failed to get gadgets (%d)", ret);
		return 0;
	}

	tmp = buf;
	while(*tmp != '\0') {
		if (*tmp == '\n' || *tmp == '\r') {
			*tmp = '\0';
			break;
		}
		tmp++;
	}

	len = strlen(buf) + 1;
	for (i = 0 ; i < ARRAY_SIZE(usb_configs) ; i++) {
		if (strncmp(buf, usb_configs[i].function, len))
			continue;
		return usb_configs[i].gadgets;
	}
	_E("Failed to find current gadget");
	return 0;
}

bool gadgets_supported(unsigned int gadgets)
{
	int i;

	if (gadgets == 0)
		return true;

	for (i = 0 ; i < ARRAY_SIZE(usb_configs) ; i++) {
		if (usb_configs[i].gadgets == gadgets)
			return true;
	}

	return false;
}

static void write_to_file(char *path, char *value)
{
	int ret;
	if (!path || !value)
		return;
	ret = sys_set_str(path, value);
	if (ret < 0)
		_E("Failed to write (%s, %s, ret:%d)", path, value, ret);
}

static int update_configuration(unsigned int gadgets)
{
	int i;
	bool found;

	if (prev_gadgets == gadgets)
		return 0;

	found = false;
	for (i = 0 ; i < ARRAY_SIZE(usb_configs) ; i++) {
		if (usb_configs[i].gadgets != gadgets)
			continue;
		found = true;
		break;
	}
	if (!found) {
		_E("Invalid gadgets (%u)", gadgets);
		return -EINVAL;
	}

	write_to_file(CONFIG_LOAD, DEF_CONFIG_DISABLE);
	write_to_file(CONFIG_IDVENDOR, DEF_CONFIG_IDVENDOR);
	write_to_file(CONFIG_IDPRODUCT, (char *)usb_configs[i].idProduct);
	write_to_file(CONFIG_FUNCTION_FIRST, (char *)usb_configs[i].function);
	write_to_file(CONFIG_CLASS, DEF_CONFIG_CLASS);
	write_to_file(CONFIG_SUBCLASS, DEF_CONFIG_SUBCLASS);
	write_to_file(CONFIG_PROTOCOL, DEF_CONFIG_PROTOCOL);
	write_to_file(CONFIG_RNDIS_WCEIS, (char *)usb_configs[i].rndis);
	write_to_file(CONFIG_DIAG_CLIENT, (char *)usb_configs[i].diag);
	write_to_file(CONFIG_RMNET_TRANSPORTS, (char *)usb_configs[i].rmnet);
	write_to_file(CONFIG_IPRODUCT, DEF_CONFIG_IPRODUCT);

	return 0;
}

int init_gadgets(unsigned int gadgets)
{
	return update_configuration(gadgets);
}

int deinit_gadgets(void)
{
	return 0;
}

int enable_gadgets(unsigned int gadgets)
{
	int ret;

	ret = update_configuration(gadgets);
	if (ret < 0) {
		_E("Failed to update doncifguration (%u, ret:%d)", gadgets, ret);
		return ret;
	}

	return sys_set_str(CONFIG_LOAD, DEF_CONFIG_ENABLE);
}

int disable_gadgets(void)
{
	return sys_set_str(CONFIG_LOAD, DEF_CONFIG_DISABLE);
}

static void uevent_delivered(struct udev_device *dev)
{
	char *state;

	_I("usb_mode uevent is delivered");

	if (!udata.enabled_cb)
		return;

	state = (char *)udev_device_get_property_value(dev, "USB_STATE");
	if (!state)
		return;

	if (strncmp(state, "CONFIGURED", strlen(state) + 1))
		return;

	udata.enabled_cb(get_current_gadgets(), udata.data);
}

static struct uevent_handler uh = {
	.subsystem = "usb_mode",
	.uevent_func = uevent_delivered,
};

int register_gadgets_enabled_event(GadgetsEnabled enabled_cb, void *data)
{
	int ret;

	ret = uevent_control_kernel_start();
	if (ret == 0) {
		ret = register_kernel_event_control(&uh);
		if (ret < 0)
			_E("Failed to register kernel event control (%d)", ret);
	}

	if (udata.enabled_cb == NULL) {
		udata.enabled_cb = enabled_cb;
		udata.data = data;
	} else
		_E("enabled callback is already registered");

	return 0;
}

void unregister_gadgets_enabled_event(GadgetsEnabled enabled_cb)
{
	unregister_kernel_event_control(&uh);
	uevent_control_kernel_stop();
	udata.enabled_cb = NULL;
	udata.data = NULL;
}

static int usb_client_open(struct hw_info *info,
		const char *id, struct hw_common **common)
{
	struct usb_client_device *usb_client_dev;

	if (!info || !common)
		return -EINVAL;

	usb_client_dev = calloc(1, sizeof(struct usb_client_device));
	if (!usb_client_dev)
		return -ENOMEM;

	usb_client_dev->common.info = info;
	usb_client_dev->get_current_gadgets	= get_current_gadgets;
	usb_client_dev->gadgets_supported	= gadgets_supported;
	usb_client_dev->init_gadgets		= init_gadgets;
	usb_client_dev->deinit_gadgets		= deinit_gadgets;
	usb_client_dev->enable_gadgets		= enable_gadgets;
	usb_client_dev->disable_gadgets		= disable_gadgets;
	usb_client_dev->register_gadgets_enabled_event
		= register_gadgets_enabled_event;
	usb_client_dev->unregister_gadgets_enabled_event
		= unregister_gadgets_enabled_event;

	*common = (struct hw_common *)usb_client_dev;
	return 0;
}

static int usb_client_close(struct hw_common *common)
{
	if (!common)
		return -EINVAL;

	free(common);
	return 0;
}

HARDWARE_MODULE_STRUCTURE = {
	.magic = HARDWARE_INFO_TAG,
	.hal_version = HARDWARE_INFO_VERSION,
	.device_version = USB_CLIENT_HARDWARE_DEVICE_VERSION,
	.id = USB_CLIENT_HARDWARE_DEVICE_ID,
	.name = "usb_client",
	.open = usb_client_open,
	.close = usb_client_close,
};
