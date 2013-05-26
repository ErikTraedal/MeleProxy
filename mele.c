#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include "uinput.h"
#include "udev.h"
#include "keys.h"
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

void send_key_from_input_data(int uinput_fd, char *data_buffer, int data_length) {
	int i;

	// Device 0 data
	if (data_buffer[0] == 0x00) {

		// Key up event
		if (memcmp(data_buffer, MELE_DEVICE_0_KEY_UP, data_length) == 0) {
			if (current_key_press_device_0 != 0) {
				debug("Sending release for key %i\n", current_key_press_device_0);
				send_key_release_event(uinput_fd, current_key_press_device_0);
				current_key_press_device_0 = 0;
				return;
			}
		}

		for (i = 0; i < NUMBER_OF_DEVICE_0_KEYS; i++) {
			if (memcmp(data_buffer, device_0_key_table[i].data, data_length)
					== 0) {
				if (current_key_press_device_0 != device_0_key_table[i].key) {
					debug("Got match. Sending key %x\n",
							device_0_key_table[i].key);
					send_key_press_event(uinput_fd, device_0_key_table[i].key);
					current_key_press_device_0 = device_0_key_table[i].key;
					return;
				}
			}
		}
	}

	// Device 1 data, we ignore movement data for now
	if (data_buffer[0] == 0x01) {

		// We have a movement or key end event
		if (memcmp(data_buffer, MELE_DEVICE_1_KEY_UP, data_length) == 0) {
			if (ignore_next_up_event_device_1 > 0) {
				ignore_next_up_event_device_1 = 0;
				return;
			} else {
				if (current_key_press_device_1 != 0) {
					debug("Sending release for key %x\n",
							current_key_press_device_1);
					send_key_release_event(uinput_fd,
							current_key_press_device_1);
					current_key_press_device_1 = 0;
					return;
				}
			}
		}

		if (data_buffer[1] == 0x00) {
			// We have an movement event 0x01 0x00
//			debug("Got movement event, ignoring next up event\n");
			ignore_next_up_event_device_1 = 1;
			return;
		}

		for (i = 0; i < NUMBER_OF_DEVICE_1_KEYS; i++) {
			if (memcmp(data_buffer, device_1_key_table[i].data, data_length)
					== 0) {
				if (current_key_press_device_1 != device_1_key_table[i].key) {
					debug("Got match. Sending key %x\n",
							device_1_key_table[i].key);
					send_key_press_event(uinput_fd, device_1_key_table[i].key);
					current_key_press_device_1 = device_1_key_table[i].key;
					ignore_next_up_event_device_1 = 0;
					return;
				}
			}
		}
	}

	// Device 2 data
	if (data_buffer[0] == 0x02) {

		// Key up event
		if (memcmp(data_buffer, MELE_DEVICE_2_KEY_UP, data_length) == 0) {
			if (current_key_press_device_2 != 0) {
				debug("Sending release for key %x\n",
						current_key_press_device_2);
				send_key_release_event(uinput_fd, current_key_press_device_2);
				current_key_press_device_2 = 0;
				return;
			}
		}

		for (i = 0; i < NUMBER_OF_DEVICE_2_KEYS; i++) {
			if (memcmp(data_buffer, device_2_key_table[i].data, data_length)
					== 0) {
				if (current_key_press_device_2 != device_2_key_table[i].key) {
					debug("Got match. Sending key %x\n",
							device_2_key_table[i].key);
					send_key_press_event(uinput_fd, device_2_key_table[i].key);
					current_key_press_device_2 = device_2_key_table[i].key;
					return;
				}
			}
		}
	}
}

int main(int argc, char **argv) {
	pid_t pid;
	int uinput_fd = -1;
	struct udev_devices devices;
	char *data_buffer;
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

	uinput_fd = setup_uinput_device();
	if (uinput_fd < 0) {
		printf("Error initializing the uinput device\n");
		exit(EXIT_FAILURE);
	}

	if (setup_udev_device(&devices) < 0) {
		printf("Error setting up udev devices\n");
		exit(EXIT_FAILURE);
	}

	if (open_devices(&devices) < 0) {
		printf("Error opening udev devices\n");
		exit(EXIT_FAILURE);
	}

	// Parse command line arguments
    while ((c = getopt(argc, argv, "f")) != -1) {
        switch(c) {
        case 'f':
            foreground = 1;
            break;
        }
    }

    if(foreground != 1) {
		printf("\nStarting in background\n");
    	pid = fork();
    }
    else {
    	printf("\nStarting in foreground\n");
    }

	// Child process
	if (pid == 0 || foreground) {

		// Allocate memory for data buffer
		data_buffer = malloc(30);

		while (running) {
			ret = read_data(&devices, data_buffer, sizeof(data_buffer));

			if (ret > 0) {
				send_key_from_input_data(uinput_fd, data_buffer, ret);

#ifdef DEBUG
				debug("Got data (%i): ", ret);

				// Do something with the data
				for (i = 0; i < ret; i++) {
					debug("\\x%.2x", data_buffer[i]);
				}

				debug("\n");
#endif
			}
		}

		destroy_uinput_device(uinput_fd);

		free(data_buffer);

	} else {
		// Parent process
	}

	return 0;
}
