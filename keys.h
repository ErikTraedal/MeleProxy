#ifndef KEYS_H_
#define KEYS_H_

// Keycode maping
struct keymap {
	ushort key;
	ushort new_key;
};

// Keys from 1 to 83 are automatically mapped, so they don't need to be mentioned here but they can be overridden if needed
static const struct keymap keymap_keys[] = {
		{KEY_VOLUMEUP, KEY_VOLUMEUP},
		{KEY_VOLUMEDOWN, KEY_VOLUMEDOWN},
		{BTN_LEFT, KEY_ENTER},
		{BTN_RIGHT, KEY_BACKSPACE},
		{BTN_MIDDLE, KEY_EPG},
		{KEY_LEFT, KEY_LEFT},
		{KEY_RIGHT, KEY_RIGHT},
		{KEY_UP, KEY_UP},
		{KEY_DOWN, KEY_DOWN},
		{KEY_HOME, KEY_HOME},
		{KEY_TV, KEY_TV},
		{KEY_MUTE, KEY_MUTE},
		{KEY_INFO, KEY_INFO},
		{KEY_ZOOMIN, KEY_ZOOMIN},
		{KEY_ZOOMOUT, KEY_ZOOMOUT},
		{KEY_SEARCH, KEY_SEARCH},
		{KEY_SUBTITLE, KEY_SUBTITLE}, // From raw device
		{KEY_AUDIO, KEY_AUDIO}, // From raw device
		{KEY_EJECTCD, KEY_EJECTCD} // From  raw device
};

#endif /* KEYS_H_ */
