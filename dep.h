/* dep.h
 * Copyright (C) 2014-2015 Md. Imam Hossain
 * For conditions of distribution and use, see copyright notice in License.txt
 */

#ifndef DEP_H
#define DEP_H

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <gtk/gtk.h>
#include <cairo.h>

#define game_title_macro "Need for Speed: Porsche Unleased"
#define game_install_executable_macro "setup/Setup.exe"
#define game_executable_macro "/engine/game/drive_c/Program Files/Electronic Arts/Need For Speed - Porsche 2000/Porsche.exe"
#define default_game_executable_macro "Porsche.exe"
#define default_game_arguments_macro ""

using namespace std;

struct LinuxLauncher {
	char game_title[1024];
	char game_install_executable[8192];
	char game_executable[10240];
	char game_executable_directory[10240];
	char game_arguments[8192];
	bool install;
	bool process_active;
	int install_process_no;
	pid_t cpid;
	char text_buffer[1024];
};

struct AppWindow {
	GtkWidget *window;
	GtkWidget *main_page;
	int width;
	int height;
	cairo_surface_t *background;
};

struct IconPage {
	GtkWidget *window;
	GtkWidget *box_fixed;
	GtkWidget *info;
	GtkWidget *status;
	char text_buffer[4096];
	GtkWidget *setup_icon;
	GtkWidget *play_icon;
	GtkWidget *exit_icon;
	GtkWidget *about_button;
	GtkWidget *option_button;
	char setup_icon_state;
	char play_icon_state;
	char exit_icon_state;
	int icon_location_x;
	int icon_location_y;
	cairo_surface_t *icon_shape;
	cairo_surface_t *icon_normal_setup;
	cairo_surface_t *icon_hover_setup;
	cairo_surface_t *icon_pressed_setup;
	cairo_surface_t *icon_normal_play;
	cairo_surface_t *icon_hover_play;
	cairo_surface_t *icon_pressed_play;
	cairo_surface_t *icon_normal_exit;
	cairo_surface_t *icon_hover_exit;
	cairo_surface_t *icon_pressed_exit;
	GdkPixmap *icon_mask;
	cairo_t *cairo_context;
	int icon_width;
	int icon_height;
};

struct AboutPage {
	GtkWidget *window;
	GtkWidget *box_fixed;
	GtkWidget *info;
	GtkWidget *back_button;
	char text_buffer[10240];
};

struct SetupPage {
	GtkWidget *window;
	GtkWidget *box_fixed;
	GtkWidget *box_horizontal;
	GtkWidget *back_box_horizontal;
	GtkWidget *back_box_alignment;
	GtkWidget *status;
	GtkWidget *busy_box;
	GtkWidget *info;
	char text_buffer[4096];
	GtkWidget *location_label;
	GtkWidget *location_entry;
	GtkWidget *location_browse_button;
	GtkWidget *back_button;
	GtkWidget *install_button;
	cairo_surface_t *background;
};

struct LocationChooserPage {
	GtkWidget *window;
	GtkWidget *box_vertical;
	GtkWidget *action_box_horizontal;
	GtkWidget *action_box_alignment;
	GtkWidget *folder_chooser_widget;
	GtkWidget *select_button;
	GtkWidget *cancel_button;
};

struct ExeChooserPage {
	GtkWidget *window;
	GtkWidget *box_vertical;
	GtkWidget *action_box_horizontal;
	GtkWidget *action_box_alignment;
	GtkFileFilter *file_chooser_filter;
	GtkWidget *file_chooser_widget;
	GtkWidget *select_button;
	GtkWidget *cancel_button;
};

struct GameOptionPage {
	GtkWidget *window;
	GtkWidget *box_fixed;
	GtkWidget *exe_path_box_horizontal;
	GtkWidget *exe_arg_box_horizontal;
	GtkWidget *exe_path_label;
	GtkWidget *exe_browse_button;
	GtkWidget *exe_path_entry;
	GtkWidget *exe_arg_label;
	GtkWidget *exe_arg_entry;
	GtkWidget *windowed_check_box;
	GtkWidget *windowed_label;
	GtkWidget *grab_mouse_check_box;
	GtkWidget *grab_mouse_label;
	GtkWidget *back_button;
	GtkWidget *defaults_button;
	char text_buffer[10240];
};

struct GameOptionData {
	int windowed;
	int mouse_lock;
	char game_exe_path[10240];
	char game_exe_arg[1024];
};

typedef struct LinuxLauncher LinuxLauncher;
typedef struct AppWindow AppWindow;
typedef struct IconPage IconPage;
typedef struct AboutPage AboutPage;
typedef struct SetupPage SetupPage;
typedef struct LocationChooserPage LocationChooserPage;
typedef struct ExeChooserPage ExeChooserPage;
typedef struct GameOptionPage GameOptionPage;
typedef struct GameOptionData GameOptionData;

GdkPixbuf *create_pixbuf (const gchar *filename);
void unix_to_dox_path (char *unix_path, char *dos_path);
void absolute_to_relative_path (char *abs_path, char *rel_path);

#endif // DEP_H