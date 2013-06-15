#ifndef UINPUT_H_
#define UINPUT_H_

struct key_lookup_table {
	ushort max_value;
	ushort *key_table;
};

int setup_uinput_device(struct key_lookup_table *table);
int destroy_uinput_device(int uinput_fd);
void handle_input_event(int uinput_fd, struct input_event *event, struct key_lookup_table *table);
void handle_key_press_event(int uinput_fd, struct input_event *event, struct key_lookup_table *table);
void send_syn(int uinput_fd);

#endif /* UINPUT_H_ */
