/*
 * udev.h
 *
 *  Created on: 19 May 2013
 *      Author: santa
 */

#ifndef UDEV_H_
#define UDEV_H_

// Some wild guesses here
#define MAX_NUMBER_OF_DEVICES 5
#define MAX_DEVICE_NAME_LENGTH 100

#define MAX_EVENT_DATA_LENGTH 100

struct udev_devices {
	int number_of_devices;
	char devices[MAX_NUMBER_OF_DEVICES][MAX_DEVICE_NAME_LENGTH];
	int fd[MAX_NUMBER_OF_DEVICES];
	int max_fd; // For select
};

int setup_udev_device(struct udev_devices *devices);
int open_devices(struct udev_devices *devices);
void destroy_devices(struct udev_devices *devices);
int read_data(struct udev_devices *devices, char *data, int data_length);

#endif /* UDEV_H_ */
