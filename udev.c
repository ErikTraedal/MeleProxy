#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libudev.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <sys/select.h>
#include "udev.h"

int get_udev_raw_devices(struct udev_devices *devices) {
	struct udev *udev;
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices_entry, *dev_list_entry;
	struct udev_device *dev, *rawdev;

	devices->number_of_devices = 0;

	udev = udev_new();
	if (!udev) {
		printf("Can't create udev\n");
		exit(1);
	}

	enumerate = udev_enumerate_new(udev);
	udev_enumerate_add_match_subsystem(enumerate, "hidraw");
	udev_enumerate_scan_devices(enumerate);

	devices_entry = udev_enumerate_get_list_entry(enumerate);
	udev_list_entry_foreach(dev_list_entry, devices_entry)
	{
		const char *path;
		const char *temp_string;

		path = udev_list_entry_get_name(dev_list_entry);
		rawdev = udev_device_new_from_syspath(udev, path);

		dev = udev_device_get_parent_with_subsystem_devtype(rawdev, "usb",
				"usb_device");
		if (!dev) {
			printf("Unable to find parent usb device.");
			exit(1);
		}

		temp_string = udev_device_get_sysattr_value(dev, "idVendor");

		if (strcmp(temp_string, "1915") == 0) {
			temp_string = udev_device_get_sysattr_value(dev, "idProduct");

			if (strcmp(temp_string, "af11") == 0) {
				// We have the right device, add to list
				strncpy(devices->devices[devices->number_of_devices++],
						udev_device_get_devnode(rawdev),
						MAX_DEVICE_NAME_LENGTH);

				// Two calls is not good, but then again we only do this six times
				printf("Adding device with path = %s\n",
						udev_device_get_devnode(rawdev));
			}
		}

		udev_device_unref(dev);
	}

	udev_enumerate_unref(enumerate);
	udev_unref(udev);

	return 0;
}

int get_udev_input_devices(struct udev_devices *devices) {
	struct udev *udev;
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices_entry, *dev_list_entry;
	struct udev_device *input_dev;

	devices->number_of_devices = 0;

	udev = udev_new();
	if (!udev) {
		printf("Can't create udev\n");
		exit(1);
	}

	enumerate = udev_enumerate_new(udev);
	udev_enumerate_add_match_subsystem(enumerate, "input");
	udev_enumerate_add_match_property(enumerate, "ID_VENDOR_ID", "1915");
	udev_enumerate_add_match_property(enumerate, "ID_PRODUCT_ID", "1915");
	udev_enumerate_scan_devices(enumerate);

	devices_entry = udev_enumerate_get_list_entry(enumerate);
	udev_list_entry_foreach(dev_list_entry, devices_entry)
	{
		const char *path;
		const char *device_node;

		path = udev_list_entry_get_name(dev_list_entry);
		printf("Found device with path %s\n", path);


		input_dev = udev_device_new_from_syspath(udev, path);

		device_node = udev_device_get_devnode(input_dev);
		if(!device_node) continue;

		printf("Got device type %s\n", device_node);

		strncpy(devices->devices[devices->number_of_devices++],
				device_node,
				MAX_DEVICE_NAME_LENGTH);

		// Two calls is not good, but then again we only do this six times
		printf("Adding device with path = %s\n", device_node);

		udev_device_unref(input_dev);
	}

	udev_enumerate_unref(enumerate);
	udev_unref(udev);

	return 0;
}

int open_event_devices(struct udev_devices *devices) {
	int i;

	for (i = 0; i < devices->number_of_devices; i++) {
		printf("Trying to open device: %s\n", devices->devices[i]);

		// Try to open the device with exclusive rights
		devices->fd[i] = open(devices->devices[i], O_RDONLY);

		// Try to get exclusive access (we know we won't be able to grab the mouse device, but thats ok)
		if(ioctl(devices->fd[i], EVIOCGRAB, 1) == 0) {
			printf("Got exclusive access\n");
		}
		else {
			printf("Failed to get exclusive access\n");
		}

		if (devices->fd[i] < 0) {
			printf("Error opening device %s, bailing out.",
					devices->devices[i]);
			return -1;
		}

		if (devices->fd[i] > devices->max_fd)
			devices->max_fd = devices->fd[i];
	}

	return 0;
}

int open_raw_devices(struct udev_devices *devices) {
	int i;

	for (i = 0; i < devices->number_of_devices; i++) {
		printf("Trying to open device: %s\n", devices->devices[i]);

		// Try to open the device with exclusive rights
		devices->fd[i] = open(devices->devices[i], O_RDONLY);

		if (devices->fd[i] < 0) {
			printf("Error opening device %s, bailing out.",
					devices->devices[i]);
			return -1;
		}

		if (devices->fd[i] > devices->max_fd)
			devices->max_fd = devices->fd[i];
	}

	return 0;
}

void destroy_devices(struct udev_devices *devices) {
	for (; devices->number_of_devices > 0; devices->number_of_devices--) {
		// Close the device (should we do an ioctl call here to remove the exclusive access?)
		close(devices->fd[devices->number_of_devices]);
	}
}

int read_raw_data(struct udev_devices *devices, char *data, int data_length) {
	// If we don't have any data, there is nothing for us to do
	// To simplify things a bit we only read from one device at a time
	int i, ret;
	fd_set rfds;

	FD_ZERO(&rfds);

	for (i = 0; i < devices->number_of_devices; i++) {
		FD_SET(devices->fd[i], &rfds);
	}

	ret = select(devices->max_fd + 1, &rfds, NULL, NULL, NULL );

	if (ret < 0)
		return -1;

	for (i = 0; i < devices->number_of_devices; i++) {
		if (FD_ISSET(devices->fd[i], &rfds)) {
			// Read data
			data_length = read(devices->fd[i], data, MAX_EVENT_DATA_LENGTH);

			return data_length;
		}
	}

	return -1;
}

int read_event_data(struct udev_devices *devices, struct input_event *event) {
	// If we don't have any data, there is nothing for us to do
	// To simplify things a bit we only read from one device at a time
	int i, ret, size;
	fd_set rfds;

	FD_ZERO(&rfds);

	for (i = 0; i < devices->number_of_devices; i++) {
		FD_SET(devices->fd[i], &rfds);
	}

	ret = select(devices->max_fd + 1, &rfds, NULL, NULL, NULL);

	if (ret == -1) {
		perror("select()");
		return -1;
	}
	if(ret) {
		for (i = 0; i < devices->number_of_devices; i++) {
			if (FD_ISSET(devices->fd[i], &rfds)) {
				// Read data
				size = read(devices->fd[i], event, sizeof(struct input_event));
				if(size == -1) perror("read()");
				return size;
			}
		}
	}
	else {
		printf("No data\n");
		return -1;
	}

	return -1;
}
