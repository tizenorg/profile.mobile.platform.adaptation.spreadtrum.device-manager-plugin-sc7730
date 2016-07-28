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
#include "../shared.h"

#define CONFIG_ROOTPATH         "/sys/class/usb_mode/usb0"
#define CONFIG_LOAD             CONFIG_ROOTPATH"/enable"
#define CONFIG_IDVENDOR         CONFIG_ROOTPATH"/idVendor"
#define CONFIG_IDPRODUCT        CONFIG_ROOTPATH"/idProduct"
#define CONFIG_FUNCTION_FIRST   CONFIG_ROOTPATH"/funcs_fconf"
#define CONFIG_FUNCTION_SECOND  CONFIG_ROOTPATH"/funcs_sconf"
#define CONFIG_CLASS            CONFIG_ROOTPATH"/bDeviceClass"
#define CONFIG_SUBCLASS         CONFIG_ROOTPATH"/bDeviceSubClass"
#define CONFIG_PROTOCOL         CONFIG_ROOTPATH"/bDeviceProtocol"
#define CONFIG_RNDIS_WCEIS      CONFIG_ROOTPATH"/f_rndis/wceis"
#define CONFIG_DIAG_CLIENT      CONFIG_ROOTPATH"/f_diag/clients"
#define CONFIG_RMNET_TRANSPORTS CONFIG_ROOTPATH"/f_rmnet/transports"
#define CONFIG_IPRODUCT         CONFIG_ROOTPATH"/iProduct"

#define DEF_CONFIG_DISABLE  "0"
#define DEF_CONFIG_ENABLE   "1"
#define DEF_CONFIG_IDVENDOR "04e8"
#define DEF_CONFIG_CLASS    "239"
#define DEF_CONFIG_SUBCLASS "2"
#define DEF_CONFIG_PROTOCOL "1"
#define DEF_CONFIG_IPRODUCT "TIZEN"

struct _usb_configs {
	unsigned int gadgets;
	const char *idProduct;
	const char *function;
	const char *rndis;
	const char *diag;
	const char *rmnet;
} usb_configs[] = {
	/* Single configurations */
	{ USB_GADGET_SDB,		"685d", "sdb",		NULL,	NULL,	NULL	},
	{ USB_GADGET_MTP,		"6860", "mtp",		NULL,	NULL,	NULL	},
	{ USB_GADGET_RNDIS,		"6863", "rndis",	"1",	NULL,	NULL	},

	/* Multi configurations */
	{ USB_GADGET_MTP | USB_GADGET_ACM | USB_GADGET_SDB,
		"6860", "mtp,acm,sdb",			NULL,	NULL,	NULL		},
	{ USB_GADGET_MTP | USB_GADGET_ACM | USB_GADGET_SDB | USB_GADGET_DIAG,
		"6860", "mtp,acm,sdb,diag",		NULL,	"diag",	NULL		},
	{ USB_GADGET_RNDIS | USB_GADGET_SDB,
		"6864", "rndis,sdb",			"1",	NULL,	NULL		},
	{ USB_GADGET_RNDIS | USB_GADGET_SDB | USB_GADGET_ACM | USB_GADGET_DIAG,
		"6864", "rndis,sdb,acm,diag",	"1",	"diag",	NULL		},
	{ USB_GADGET_RNDIS | USB_GADGET_DIAG,
		"6864", "rndis,diag",			"1",	"diag",	NULL		},
	{ USB_GADGET_ACM | USB_GADGET_SDB | USB_GADGET_DM,
		"6860", "acm,sdb,dm",			NULL,	NULL,	NULL		},
	{ USB_GADGET_DIAG | USB_GADGET_ACM | USB_GADGET_RMNET,
		"685d", "diag,acm,rmnet",		NULL,	"diag",	"smd,bam"	},
};

