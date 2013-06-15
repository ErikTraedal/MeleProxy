#ifndef MELE_H_
#define MELE_H_

#define MAX_STANDARD_KEY_VALUE 83

void signal_handler(int number);
void send_key_from_input_data(int uinput_fd, char *data_buffer, int data_length);
void create_lookup_table(struct key_lookup_table *table);
void show_usage(int argc, char **argv);

#endif /* MELE_H_ */
