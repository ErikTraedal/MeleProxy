MeleProxy
=========

Proxy service for the Mele F10 remote control.

I bought a Mele F10 remote control, and were amazed by the design and how well the remote worked.
But in reality I only wanted it as a RF remote for XBMC, and the air mouse functionality is of no use for me.
By all means, I think its a very nice and precise air mouse, but I don't need it myself.

Instead of writing a driver for it, I opted for just writing an userspace application that is operating on hidraw and the event devices.
It attaches to the hidraw and event devices of the usb dongle and creates a virtual uinput device that is used for sending the events.

The key mappings can be changed in keys.h, by default most of them are mapped up so the XBMC setup is fast and painless.
There is a small hack on subtitle, audio and eject key, so there is no key repeat on them, but I guess that should be ok.

Dependencies:
libudev	- Install with your favorite packagemanager (e.g. apt-get install libudev-dev)


Compiling:
There is a very simple make file, so if you have installed an build environment its very simple, just type "make".
The executable is named "mele"


Installation:
It can be installed with make install.
To autostart, just put it in rc.local (or the one fitting for your distro).
