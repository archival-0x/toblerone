#include "toblerone.h"

/* fun xlib stuff for later use */
Display *dpy;
Screen *scrn;
Window root;

/* holds the program_name globally, such that fprintf() can display */
char * program_name = NULL;

/* note that screen_number differs from the scrn object */
int screen_number;

/* define a file wrapper type */
typedef struct {
	const char * filename;
	FILE *stream;
} filestream_t ;


static void
die(int err, char * msg)
{
    fprintf(stderr, "%s: %s\n", program_name, msg);
    exit(err);
}


static void
usage(char * error)
{
	/* if an error message is supplied, print it */
	if (strcmp(error, ""))
		fprintf(stdout, "%s: %s\n", program_name, error);

	/* print the help menu */
	fprintf(stderr, "usage: \n\t%s -hbr [FILENAME]\n\n"
		"-h / --help	-- print help menu\n"
		"-b / --background [FILENAME] -- set background as image\n"
		"-r / --random	-- set random background image (ignored if -b is called)\n",
		program_name);

	exit(1);
}


static size_t
write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	/* create another pointer */
	filestream_t *out = (filestream_t *) userp;

	/* create a filestream to specified location */
	if (out && !out->stream) {
		out->stream = fopen(out->filename, "wb");

		if (!out->stream)
	        die(-1, "failed to create filestream type");
    }
	return fwrite(buffer, size, nmemb, out->stream);
}


int
get_screen_size(unsigned int *w, unsigned int *h)
{
	/* attempt to open display, call error if unable to */
	dpy = XOpenDisplay(NULL);
	if (!dpy)
	    die(-1, "failed to open default display");


	/* get default screen of display, if not call error */
	scrn = DefaultScreenOfDisplay( dpy );
	if (!scrn)
		die(-1, "failed to obtain the default screen of given display");


	/* set the width and height variables as properties of result scrn struct */
	*w = scrn->width;
	*h = scrn->height;


	/* clean up, close display */
	XCloseDisplay(dpy);
	return 0;
}


void
set_wallpaper(Imlib_Image image, int imgHeight, int imgWidth)
{
	Pixmap pix;

	/* open the display */
	dpy = XOpenDisplay(NULL);

	/* determine the root window */
	screen_number = DefaultScreen(dpy);
	root = RootWindow(dpy, screen_number);

	/* create pixmap with canvas properties */
	pix = XCreatePixmap(dpy, root,
			            imgHeight, imgWidth,
			            DefaultDepth(dpy, screen_number));

	/* set the display, visual, colorable, and drawable */
	imlib_context_set_display(dpy);
	imlib_context_set_visual(DefaultVisual(dpy, screen_number));
	imlib_context_set_colormap(DefaultColormap(dpy, screen_number));
	imlib_context_set_drawable(pix);
	imlib_render_image_on_drawable(0, 0);

	/* set the background as the pixmap */
	XSetWindowBackgroundPixmap(dpy, root, pix);

	fprintf(stdout, "%s: Wallpaper set!\n", program_name);

	/* clear the window, and free the pixmap */
	XClearWindow(dpy, root);

	while (XPending(dpy)) {
		XEvent ev;
		XNextEvent(dpy, &ev);
	}

	/* free the pixmap */
	XFreePixmap(dpy, pix);

	/* free the image and clear cache */
	imlib_free_image_and_decache();

	/* clean up, close display */
	XCloseDisplay(dpy);
}


void
set_background(char * filename, unsigned int width, unsigned int height)
{
	Imlib_Image image;
	int imgWidth, imgHeight;

	/* load the actual image filepath */
	image = imlib_load_image(filename);
	if (!image)
		die(-1, "cannot load image file");

	/* set the loaded image as the current working context */
	imlib_context_set_image(image);

	/* get image width and height properties */
	imgWidth = imlib_image_get_width();
	imgHeight = imlib_image_get_height();

    /* set the wallpaper */
	set_wallpaper(image, imgWidth, imgHeight);
}



int
get_random_image_url(char * url)
{
	CURL *curl;
	CURLcode res;
	char * location;

	filestream_t fp = {
		LOCAL_IMG_FILE,
		NULL
	};

	/* initialize curl */
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if (curl) {

		/* set the url to send the curl request, and send it */
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &fp);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

		res = curl_easy_perform(curl);

		/* error-check the curl request */
		if (res != CURLE_OK)
			fprintf(stderr, "%s: curl_easy_perform() failed: %s", program_name, curl_easy_strerror(res));
		else if (res == CURLE_OK)
			res = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &location);

	} else
		fprintf(stderr, "%s: could not initalize libcurl", program_name);

	/* close pointer to local file */
	if (fp.stream)
		fclose(fp.stream);

	/* call cleanup function */
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	/* return the location of the redirected url */
	if (location)
		return 0;

	return -1;
}


int
main(int argc, char * argv[])
{
	char * pixfile = NULL;
	register int i;

	program_name = argv[0];

	unsigned int scrnWidth;
	unsigned int scrnHeight;

	/* get screen width and height */
	get_screen_size(&scrnWidth, &scrnHeight);

	/* parse through each arg iteratively */
	for (i = 0; i < argc; i++) {

		/* progname NO_ARGS */
		if (argc < 2)
			usage("no arguments supplied");

		/* progname -h / --help */
		if (!strcmp("-h", argv[i]) || !strcmp("--help", argv[i]))
			usage(NULL);

		/* progname -b / --background */
		if (!strcmp("-b", argv[i]) || !strcmp("--background", argv[i])) {

			/* if an additional argument is not supplied, exit with usage() */
			if (++i >= argc)
				usage("you must supply a pixmap file");

			/* since we incremented again, set current arg as the pixmap image */
			pixfile = argv[i];

			/* set pixfile as background and exit */
			set_background(pixfile, scrnWidth, scrnHeight);
			break;
		}

		/* progname -r / --random */
		if (!strcmp("-r", argv[i]) || !strcmp("--random", argv[i])) {

			int randFd;

			/* construct a new url */
			char buffer[BUFFER_SIZE];
			sprintf(buffer, "%s%dx%d", RAND_IMG_URL, scrnWidth, scrnHeight);

			randFd = get_random_image_url(buffer);

			/* error-handling */
			if (randFd < 0)
				die(-1, "unable to download random image");

			/* set background as downloaded image with absolute path */
			set_background(LOCAL_IMG_FILE, scrnWidth, scrnHeight);
			break;
		}
	}
	exit(0);
}
