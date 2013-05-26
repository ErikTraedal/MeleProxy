MeleProxy
=========

Proxy service for the Mele F10 remote control.

I bought a Mele F10 remote control, and were amazed by the design and how well the remote worked.
But in reality I only wanted it as a RF remote for XBMC, and the air mouse functionality is of no use for me.
By all means, I think its a very nice and precise air mouse, but I don't need it myself.

Instead of writing a driver for it, I opted for just writing an userspace application that is operating on hidraw.
It attaches to the hidraw devices of the usb dongle and creates a virtual uinput device that is used for sending the events.

The key mappings can be changed in keys.h, by default most of them are mapped up so the XBMC setup is fast and painless.
Due to some issues with XBMC (SDL) we send - and + instead of vol up and down events.


Dependencies:
libudev	- Install with your favorite packagemanager (e.g. apt-get install libudev-dev)


Compiling:
There is a very simple make file, so if you have installed an build environment its very simple, just type "make".
The executable is named "mele"


Installation:
Since the software is far from finished, I don't think anyone want to install it yet.
But you can run it from command line, and it will start to run in the background.
For now there are no command line arguments, but debug messages (a lot of them) can be enabled by compiling with the DEBUG flag set.

To make things work you have to make sure the driver is ignored by X.
An example configuration for this can be found in the "configurations" folder.
To use, just copy the file to your X.org configuration file folder (or add it to the xorg.conf).
In Ubuntu you can just copy the file "60-mele.conf" to "/usr/share/X11/xorg.conf.d" (I guess there should be some user folder for this, but couldn't find any in Ubuntu)
After restarting X, the remote won't register with X anymore.