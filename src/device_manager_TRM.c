/*
	Tizen Resource Manager
	yu.baek@samsung.com

*/
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "device_manager_TRM.h"

#define DEVMGR_LOG
#if defined(DEVMGR_LOG)
#define LOG_TAG "DEVICE_PLUGIN"
#include <dlog/dlog.h>
#define devmgr_log(fmt, args...)	SLOGI(fmt, ##args)
#else
#define devmgr_log(fmt, args...)
#endif


#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/un.h>

static struct sockaddr_un addr;

void TRM_send_socket(char *soket_path, char *write_buf)
{
	int socket_fd = 0;
	int ret = 0, running_step;
	struct sockaddr_un addr;
	static int socket_exist = 0;

	if (access(soket_path, F_OK) == 0) {
		socket_exist = 1;
	} else {
		devmgr_log("%s do not exist\n", soket_path);
	}

	if (socket_exist == 0)
		goto fail;

	socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		running_step = 1;
		goto fail;
	}

	memset(&addr, 0, sizeof(addr));
	snprintf(addr.sun_path, UNIX_PATH_MAX, "%s", soket_path);
	addr.sun_family = AF_LOCAL;

	ret = connect(socket_fd, (struct sockaddr *) &addr, sizeof(sa_family_t) + strlen(soket_path));
	if (ret != 0) {
		running_step = 2;
		close(socket_fd);
		goto fail;
	}

	send(socket_fd, write_buf, strlen(write_buf), MSG_NOSIGNAL);

	close(socket_fd);

	return;

fail:
	return;

}



#define TRM_SOCKET_FOR_SCENARIO_INFO       "/dev/socket/scenario_info"

int Tizen_Resource_Manager(char *event_lock)
{
	TRM_send_socket(TRM_SOCKET_FOR_SCENARIO_INFO, event_lock);

	return 0;
}


