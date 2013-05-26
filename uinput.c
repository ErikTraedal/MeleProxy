#include <linux/uinput.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "uinput.h"
#include "keys.h"

// Returns the uinput_fd
int setup_uinput_device() {
	int uinput_fd = -1;
	struct uinput_user_dev uidev;
	int i;

	// Open uinput device
	uinput_fd = open("/dev/uinput", O_WRONLY | O_NDELAY);

	if (uinput_fd < 0) {
		printf("Unable to open uinput device\n");
		return -1;
	}

	// Create device
	memset(&uidev, 0, sizeof(uidev));
	strncpy(uidev.name, "Mele F10 proxy", UINPUT_MAX_NAME_SIZE);
	uidev.id.bustype = BUS_USB;
	uidev.id.version = 1;

	if (ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY) < 0)
		return -1;

	// Device 0
	for(i = 0;i < NUMBER_OF_DEVICE_0_KEYS;i++) {
		if (ioctl(uinput_fd, UI_SET_KEYBIT, device_0_key_table[i].key) < 0) return -1;
	}

	// Device 1
	for(i = 0;i < NUMBER_OF_DEVICE_1_KEYS;i++) {
		if (ioctl(uinput_fd, UI_SET_KEYBIT, device_1_key_table[i].key) < 0) return -1;
	}

	// Device 2
	for(i = 0;i < NUMBER_OF_DEVICE_2_KEYS;i++) {
		if (ioctl(uinput_fd, UI_SET_KEYBIT, device_2_key_table[i].key) < 0) return -1;
	}

	if (write(uinput_fd, &uidev, sizeof(uidev)) < 0)
		return -1;
	if (ioctl(uinput_fd, UI_DEV_CREATE) < 0)
		return -1;

	return uinput_fd;
}

int destroy_uinput_device(int uinput_fd) {
	// What about the uinput_user_dev structure, check if we need to free it
	if (ioctl(uinput_fd, UI_DEV_DESTROY) < 0)
		return -1;

	return close(uinput_fd);
}

void send_key_event(int uinput_fd, ushort key) {
	send_key_press_event(uinput_fd, key);
	send_key_release_event(uinput_fd, key);
}

void send_key_press_event(int uinput_fd, ushort key) {
	struct input_event ev;
	memset(&ev, 0, sizeof(ev));

	ev.type = EV_KEY;
	ev.code = key;
	ev.value = 1;
	write(uinput_fd, &ev, sizeof(ev));

	ev.type = EV_SYN;
	ev.code = SYN_REPORT;
	ev.value = 0;
	write(uinput_fd, &ev, sizeof(ev));
}

void send_key_release_event(int uinput_fd, ushort key) {
	struct input_event ev;
	memset(&ev, 0, sizeof(ev));

	ev.type = EV_KEY;
	ev.code = key;
	ev.value = 0;
	write(uinput_fd, &ev, sizeof(ev));

	ev.type = EV_SYN;
	ev.code = SYN_REPORT;
	ev.value = 0;
	write(uinput_fd, &ev, sizeof(ev));
}

