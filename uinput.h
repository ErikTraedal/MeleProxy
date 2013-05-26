#ifndef UINPUT_H_
#define UINPUT_H_

int setup_uinput_device();
void send_key_event(int uinput_fd, ushort key);
void send_key_press_event(int uinput_fd, ushort key);
void send_key_release_event(int uinput_fd, ushort key);
int destroy_uinput_device(int uinput_fd);

#endif /* UINPUT_H_ */
