#ifndef KEYS_H_
#define KEYS_H_

// Keycode maping
struct keymap {
	ushort key;
	ushort new_key;
};

// Keys from 1 to 83 are automatically mapped, so they don't need to be mentioned here but they can be overridden if needed
static const struct keymap keymap_keys[] = {
		{KEY_VOLUMEUP, KEY_KPPLUS},
		{KEY_VOLUMEDOWN, KEY_KPMINUS},
		{BTN_LEFT, KEY_ENTER},
		{BTN_RIGHT, KEY_BACKSPACE},
		{BTN_MIDDLE, KEY_EPG},
		{KEY_LEFT, KEY_LEFT},
		{KEY_RIGHT, KEY_RIGHT},
		{KEY_UP, KEY_UP},
		{KEY_DOWN, KEY_DOWN},
		{KEY_HOME, KEY_HOME},
		{KEY_TV, KEY_TV},
		{KEY_MUTE, KEY_MUTE}
};



#endif /* KEYS_H_ */
