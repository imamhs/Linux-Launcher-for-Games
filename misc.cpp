/* misc.cpp
 * Copyright (C) 2014-2015 Md. Imam Hossain
 * For conditions of distribution and use, see copyright notice in License.txt
 */

#include "dep.h"

GdkPixbuf *create_pixbuf (const char *filename)
{
	GdkPixbuf *pixbuf;
	GError *error = NULL;
	pixbuf = gdk_pixbuf_new_from_file(filename, &error);
	if(!pixbuf) {
		fprintf(stderr, "%s\n", error->message);
		g_error_free(error);
	}
	return pixbuf;
}

// function to convert unix path to WINE dos path
void unix_to_dox_path (char *unix_path, char *dos_path)
{
	char *up = unix_path;
	char *dp = dos_path;
	for (int len = strlen (unix_path) ; len != 0 ; len--) {
		if (*up == '/') {
			*dp = '\\';
			dp++;
			*dp = '\\';
		}
		else *dp = *up;
		up++;
		dp++;
	}
}

// function to discard first '/' occurance from the path
void absolute_to_relative_path (char *abs_path, char *rel_path)
{
	char *ap = abs_path;
	char *rp = rel_path;
	ap++;
	while (*ap != '\0') {
		*rp = *ap;
		ap++;
		rp++;
	}
}