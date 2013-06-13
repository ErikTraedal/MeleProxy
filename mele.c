#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <linux/input.h>
#include "keys.h"
#include "uinput.h"
#include "udev.h"
#include "mele.h"
#include "debug.h"

int running = 1;
ushort current_key_press_device_0;
ushort current_key_press_device_1;
ushort current_key_press_device_2;
char ignore_next_up_event_device_1 = 0;

void signal_handler(int number) {
	running = 0;

	// Close all handles
}

void create_lookup_table(struct key_lookup_table *table) {
	int i, number_of_items;

	printf("Creating lookup table\n");
	number_of_items = sizeof(keymap_keys) / sizeof(keymap_keys[0]);

	// Find the max value
	for (i = 0; i < number_of_items; i++) {
		if (keymap_keys[i].key > table->max_value)
			table->max_value = keymap_keys[i].key;
	}

	// Allocate memory for lookup table
	table->key_table = malloc(table->max_value * sizeof(ushort));

	printf("Filling lookup table with data (number of items = %i\n",
			table->max_value);
	// Fill table, first priority is keymap, if value is not found and value is less than 83 map directly. Else ignore
	for (i = 0; i <= table->max_value; i++) {
		if (i > 0 && i <= MAX_STANDARD_KEY_VALUE) {
			// Map one to one
			*(table->key_table + i) = i;
		} else {
			*(table->key_table + i) = 0;
		}
	}

	// Overwrite with keys from lookup table
	for (i = 0; i < number_of_items; i++) {
		printf("Replacing key %i with key %i\n", keymap_keys[i].key,
				keymap_keys[i].new_key);
		*(table->key_table + keymap_keys[i].key) = keymap_keys[i].new_key;
	}

	printf("Finished creating lookup table\n");
}

int main(int argc, char **argv) {
	pid_t pid;
	int uinput_fd = -1;
	struct udev_devices devices;
//	char *data_buffer;
	struct input_event *event;
	int ret, c, foreground;
#ifdef DEBUG
	int i;
	printf("Compiled with debug flag\n");
#endif

	// Handle signals
	if (signal(SIGINT, signal_handler) == SIG_ERR )
		printf("Error connecting to SIGINT\n");
	if (signal(SIGTERM, signal_handler) == SIG_ERR )
		printf("Error connecting to SIGTERM\n");

	// Create lookup table
	struct key_lookup_table *lookup_table;
	lookup_table = malloc(sizeof(struct key_lookup_table));
	create_lookup_table(lookup_table);

	uinput_fd = setup_uinput_device(lookup_table);
	if (uinput_fd < 0) {
		printf("Error initializing the uinput device\n");
		exit(EXIT_FAILURE);
	}

	if (get_udev_input_devices(&devices) < 0) {
		printf("Error setting up udev devices\n");
		exit(EXIT_FAILURE);
	}

	if (open_event_devices(&devices) < 0) {
		printf("Error opening udev devices\n");
		exit(EXIT_FAILURE);
	}

	// Parse command line arguments
	while ((c = getopt(argc, argv, "f")) != -1) {
		switch (c) {
		case 'f':
			foreground = 1;
			break;
		}
	}

	if (foreground != 1) {
		printf("\nStarting in background\n");
		pid = fork();
	} else {
		printf("\nStarting in foreground\n");
	}

	// Child process
	if (pid == 0 || foreground) {

//		// Allocate memory for data buffer
//		data_buffer = malloc(30);

		event = malloc(sizeof(struct input_event));

		while (running) {
			ret = read_event_data(&devices, event);

			if (ret > 0) {
//				send_key_from_input_data(uinput_fd, data_buffer, ret);
				handle_input_event(uinput_fd, event, lookup_table);

#ifdef DEBUG
				debug("Got event (%hu:%hu:%hu)\n", event->type, event->code, event->value);

//				// Do something with the data
//				for (i = 0; i < ret; i++) {
//					debug("\\x%.2x", data_buffer[i]);
//				}

//				debug("\n");
#endif
			}
		}

		destroy_uinput_device(uinput_fd);

		free(event);
		free(lookup_table);

	} else {
		// Parent process
	}

	return 0;
}
