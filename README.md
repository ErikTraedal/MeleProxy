MeleProxy
=========

Proxy service for the Mele F10 remote control

Userspace application for handling communication with the Mele F10 air mouse.
It uses raw access to the usb receiver for the commands, and resends them using uinput.
Due to some issues with XBMC (SDL) we send - and + instead of vol up and down events.
