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
	printf("Stopping application\n");
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
	struct udev_devices raw_devices;
	struct input_event *event;
	int ret, c, foreground = 0;
	struct key_lookup_table *lookup_table;

	debug("Compiled with debug flag\n");

	// Handle signals
	if (signal(SIGINT, signal_handler) == SIG_ERR )
		printf("Error connecting to SIGINT\n");
	if (signal(SIGTERM, signal_handler) == SIG_ERR )
		printf("Error connecting to SIGTERM\n");

	// Create lookup table
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

	if (get_udev_raw_devices(&raw_devices) < 0) {
		printf("Error setting up udev devices\n");
		exit(EXIT_FAILURE);
	}

	if (open_event_devices(&devices) < 0) {
		printf("Error opening udev event devices\n");
		exit(EXIT_FAILURE);
	}

	if (open_raw_devices(&raw_devices) < 0) {
		printf("Error opening udev raw devices\n");
		exit(EXIT_FAILURE);
	}

	// Parse command line arguments
	while ((c = getopt(argc, argv, "fhd")) != -1) {
		switch (c) {
		case '?':
		case 'h':
			show_usage(argc, argv);
			return 0;
		case 'f':
			foreground = 1;
			break;
		case 'd':
			// I guess I should implement this at some point
			break;
		}
	}

	if (foreground == 0) {
		printf("\nStarting in background\n");
		pid = fork();
	} else {
		printf("\nStarting in foreground\n");
	}

	// Child process
	if (pid == 0 || foreground == 1) {
		event = malloc(sizeof(struct input_event));

		while (running) {
			ret = read_udev_data(&devices, &raw_devices, event);

			if (ret > 0) {
				debug("Got event (%hu:%hu:%hu)\n", event->type, event->code, event->value);
				handle_input_event(uinput_fd, event, lookup_table);
			} else if(ret == SEND_SYN_AFTER_KEY) {
				debug("Got event (%hu:%hu:%hu) and sent syn afterwards\n", event->type, event->code, event->value);
				handle_key_press_event(uinput_fd, event, lookup_table);
			}
		}

		destroy_uinput_device(uinput_fd);

		free(event);
		free(lookup_table);

	}

	return 0;
}

void show_usage(int argc, char **argv) {
	printf("Usage: %s [OPTION]\n", *argv);
	printf("Starts a proxy service for the Mele F10 remote control\n\n");
	printf("Arguments:\n");
	printf("\t-f\tRun in foreground (don't run in background)\n");
	printf("\t-d\tShow debug information\n");
}
