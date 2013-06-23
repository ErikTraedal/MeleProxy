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
		{BTN_LEFT, KEY_ENTER}, // Center OK button
		{BTN_RIGHT, KEY_BACKSPACE}, // Back button
		{BTN_MIDDLE, KEY_I}, // Square with arrow pointing out
		{KEY_ENTER, KEY_C}, // For some reason KEY_MENU doesn't work out of the box
		{KEY_LEFT, KEY_LEFT},
		{KEY_RIGHT, KEY_RIGHT},
		{KEY_UP, KEY_UP},
		{KEY_DOWN, KEY_DOWN},
		{KEY_HOME, KEY_HOMEPAGE},
		{KEY_TV, KEY_TAB},
		{KEY_MUTE, KEY_MUTE},
		{KEY_F1, KEY_STOPCD},
		{KEY_F2, KEY_PREVIOUSSONG},
		{KEY_F3, KEY_NEXTSONG},
		{KEY_F4, KEY_PLAYPAUSE},
		{KEY_INFO, KEY_INFO},
		{KEY_ZOOMIN, KEY_ZOOMIN},
		{KEY_ZOOMOUT, KEY_ZOOMOUT},
		{KEY_SEARCH, KEY_SEARCH},
		{KEY_SUBTITLE, KEY_T}, // From raw device
		{KEY_AUDIO, KEY_SOUND}, // From raw device
		{KEY_EJECTCD, KEY_EJECTCD} // From  raw device
};

#endif /* KEYS_H_ */
