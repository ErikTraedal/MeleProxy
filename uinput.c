#include <linux/uinput.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "uinput.h"
#include "debug.h"
#include "keys.h"

// Handles the input events, forwards or drops
void handle_input_event(int uinput_fd, struct input_event *event,
		struct key_lookup_table *table) {
	if (event->type != EV_REL) {
		if (event->type == EV_KEY && *(table->key_table + event->code) != 0) {
			debug("sending event of type %i, with code %i instead of code %i\n", event->type, *(table->key_table + event->code), event->code);

			event->code = *(table->key_table + event->code);
		}

		write(uinput_fd, event, sizeof(struct input_event));
	}
}

// Returns the uinput_fd
int setup_uinput_device(struct key_lookup_table *table) {
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

	// Map all keys in table
	for (i = 1; i <= table->max_value; i++) {
		if (*(table->key_table + i) != 0) {
			ioctl(uinput_fd, UI_SET_KEYBIT, *(table->key_table + i));
		}
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

void handle_key_press_event(int uinput_fd, struct input_event *event, struct key_lookup_table *table) {
	if (*(table->key_table + event->code) != 0) {
		debug("sending event of type %i, with code %i instead of code %i\n", event->type, *(table->key_table + event->code), event->code);

		event->code = *(table->key_table + event->code);
	}

	event->value = 1;
	write(uinput_fd, event, sizeof(struct input_event));
	send_syn(uinput_fd);

	// Key up
	event->value = 0;
	write(uinput_fd, event, sizeof(struct input_event));
	send_syn(uinput_fd);
}

void send_syn(int uinput_fd) {
	struct input_event ev;

	ev.type = EV_SYN;
	ev.code = SYN_REPORT;
	ev.value = 0;
	write(uinput_fd, &ev, sizeof(ev));
}
