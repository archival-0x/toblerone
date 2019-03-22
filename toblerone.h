/*
 * toblerone.h
 *
 *	Defines toblerone functions for interacting with
 *	background and wallpaper manipulation through Xlib
 */

#ifndef _TOBLERONE_H_
#define _TOBLERONE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <Imlib2.h>
#include <curl/curl.h>

#define RAND_IMG_URL "https://source.unsplash.com/random/"
#define BUFFER_SIZE 4068
#define LOCAL_IMG_FILE "image.jpg"

struct FileStream;

int
get_screen_size(unsigned int *w, unsigned int *h);

void
set_wallpaper(Imlib_Image image, int imgHeight, int imgWidth);

void
set_background(char * filename, unsigned int width, unsigned int height);

int
get_random_image_url(char * url);

#endif
