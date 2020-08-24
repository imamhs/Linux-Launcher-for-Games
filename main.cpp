/* main.cpp
 * Copyright (C) 2014-2015 Md. Imam Hossain
 * For conditions of distribution and use, see copyright notice in License.txt
 */

#include "dep.h"

LinuxLauncher linux_launcher;
AppWindow app_window;
IconPage icon_page;
AboutPage about_page;
SetupPage setup_page;
LocationChooserPage location_chooser_page;
ExeChooserPage executable_chooser_page;
GameOptionPage game_option_page;
GameOptionData game_option_data;

int load_resource (void);
void free_resource (void);
void create_icon_page (void);
void create_about_page (void);
void create_setup_page (void);
void create_location_chooser_page (void);
void create_executable_chooser_page(void);
void create_game_option_page (void);

static gboolean icon_press_callback (GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean icon_release_callback (GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean icon_enter_callback (GtkWidget *widget, GdkEvent *event, gpointer data);
static gboolean icon_leave_callback (GtkWidget *widget, GdkEvent *event, gpointer data);
static gboolean icon_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data);
static gboolean icon_page_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data);
static gboolean about_page_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data);
static gboolean setup_page_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data);
static gboolean location_chooser_page_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data);
static gboolean exe_chooser_page_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data);
static gboolean option_page_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data);
static void set_back_press_callback (GtkWidget *widget, gpointer data);
static void set_install_press_callback (GtkWidget *widget, gpointer data);
static void set_browse_press_callback (GtkWidget *widget, gpointer data);
static void option_browse_press_callback (GtkWidget *widget, gpointer data);
static void about_back_press_callback (GtkWidget *widget, gpointer data);
static void option_back_press_callback (GtkWidget *widget, gpointer data);
static void option_default_press_callback (GtkWidget *widget, gpointer data);
static void babout_press_callback (GtkWidget *widget, gpointer data);
static void boption_press_callback (GtkWidget *widget, gpointer data);
static void option_windowed_press_callback (GtkWidget *widget, gpointer window);
static void chooser_select_press_callback (GtkWidget *widget, gpointer data);
static void chooser_cancel_press_callback (GtkWidget *widget, gpointer data);
static void exe_chooser_select_press_callback (GtkWidget *widget, gpointer data);
static void exe_chooser_cancel_press_callback (GtkWidget *widget, gpointer data);
static gboolean window_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data);
static gboolean window_press_callback (GtkWidget *widget, GdkEventButton *event, gpointer data);
static void window_destroy_callback (GtkWidget *widget, gpointer data);
static gboolean window_delete_callback (GtkWidget *widget, GdkEvent *event, gpointer data);

static gboolean tloop (gpointer data);
void install_process_callback (int signal_number);
int run_game (void);
void run_process_callback (int signal_number);
int delete_game_installation (void);
void delete_process_callback (int signal_number);
int check_game_installed (void);
void save_settings (void);
void load_settings (void);

int main (int argc, char *argv[])
{
	int r_load = 0;
	
	if (gtk_init_check (&argc, &argv) == FALSE) {
		fprintf (stderr, "LL: could not initialize GTK!\n");
		return EXIT_FAILURE;
	}
	
	r_load = load_resource ();
	
	if (r_load == 1) {
		fprintf (stderr, "LL: could not load image files!\n");
		return EXIT_FAILURE;
	}
	else if (r_load == 2) {
		fprintf (stderr, "LL: could not shape icons!\n");
		return EXIT_FAILURE;
	}
	
	gtk_rc_parse ("style.rc");
	
	// Define top level application window
	app_window.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	
	gtk_widget_set_events (app_window.window, GDK_BUTTON_PRESS_MASK);
	
	g_signal_connect (G_OBJECT (app_window.window), "delete-event", G_CALLBACK(window_delete_callback), NULL);
	g_signal_connect (G_OBJECT (app_window.window), "destroy", G_CALLBACK(window_destroy_callback), NULL);
	g_signal_connect (G_OBJECT (app_window.window), "button_press_event", G_CALLBACK (window_press_callback), NULL);
	
	memset (linux_launcher.text_buffer, '\0', sizeof linux_launcher.text_buffer);
	sprintf (linux_launcher.text_buffer, "Linux Launcher for %s", linux_launcher.game_title);
	gtk_window_set_title (GTK_WINDOW(app_window.window), linux_launcher.text_buffer);
	gtk_window_set_icon (GTK_WINDOW(app_window.window), create_pixbuf("data/image/iconk.png"));
	gtk_window_set_position (GTK_WINDOW(app_window.window), GTK_WIN_POS_CENTER);
	gtk_widget_set_size_request (app_window.window, app_window.width, app_window.height);
	gtk_container_set_border_width (GTK_CONTAINER(app_window.window), 0);
	gtk_window_set_resizable (GTK_WINDOW(app_window.window), FALSE);
	gtk_window_set_decorated (GTK_WINDOW (app_window.window), FALSE);
	
	app_window.main_page = gtk_event_box_new ();
	gtk_container_add (GTK_CONTAINER(app_window.window), app_window.main_page);
	gtk_widget_show (app_window.main_page);
	
	gtk_widget_show (app_window.window);
	
	create_about_page ();
	
	create_icon_page ();
	
	create_setup_page ();
	
	create_location_chooser_page ();
	
	create_executable_chooser_page ();
	
	create_game_option_page ();
	
	switch (check_game_installed()) {
		case -1:
			fprintf (stderr, "LL: Could not check game installation\n");
			break;
		case 3:
			load_settings ();
			memset (icon_page.text_buffer, 0, sizeof icon_page.text_buffer);
			sprintf (icon_page.text_buffer, "%s is installed on your system.\nPlease, mount %s disk before clicking <b>Play</b>", linux_launcher.game_title, linux_launcher.game_title);
			gtk_label_set_markup (GTK_LABEL(icon_page.status), icon_page.text_buffer);
			gtk_widget_set_sensitive (icon_page.setup_icon, FALSE);
			gtk_widget_show (icon_page.option_button);
			break;
		case 1:
			memset (icon_page.text_buffer, 0, sizeof icon_page.text_buffer);
			sprintf (icon_page.text_buffer, "You have not installed the game on your system yet.\nPlease click <b>Setup</b> to install %s", linux_launcher.game_title);
			gtk_label_set_markup (GTK_LABEL(icon_page.status), icon_page.text_buffer);
			gtk_widget_set_sensitive (icon_page.play_icon, FALSE);
			break;
		case 2:
			memset (icon_page.text_buffer, 0, sizeof icon_page.text_buffer);
			sprintf (icon_page.text_buffer, "You have not installed the game on your system yet.\nPlease click <b>Setup</b> to install %s", linux_launcher.game_title);
			gtk_label_set_markup (GTK_LABEL(icon_page.status), icon_page.text_buffer);
			gtk_widget_set_sensitive (icon_page.play_icon, FALSE);
			break;
		default:
			break;
	}
	
	gtk_main ();
	
	free_resource ();
	
	fprintf (stdout, "LL: Launcher exited normally\n");
	
	return EXIT_SUCCESS;
}

static gboolean window_delete_callback (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	return FALSE;
}

static void window_destroy_callback (GtkWidget *widget, gpointer data)
{
	save_settings ();
	gtk_main_quit ();
}

static gboolean window_press_callback (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if (event->type == GDK_BUTTON_PRESS)
	{
		switch (event->button)
		{
			case 1:
				if (event->y < 10) gtk_window_begin_move_drag (GTK_WINDOW (widget), event->button, event->x_root, event->y_root, event->time);
				break;
			default:
				break;
		}
	}
	return FALSE;
}

static void babout_press_callback (GtkWidget *widget, gpointer data)
{
	g_object_ref (icon_page.window);
	gtk_container_remove (GTK_CONTAINER(app_window.main_page), icon_page.window);
	gtk_container_add (GTK_CONTAINER(app_window.main_page), about_page.window);
	g_object_unref (about_page.window);
}

static void boption_press_callback (GtkWidget *widget, gpointer data)
{
	g_object_ref (icon_page.window);
	gtk_container_remove (GTK_CONTAINER(app_window.main_page), icon_page.window);
	gtk_container_add (GTK_CONTAINER(app_window.main_page), game_option_page.window);
	g_object_unref (game_option_page.window);
}

static void about_back_press_callback (GtkWidget *widget, gpointer data)
{
	icon_page.setup_icon_state = 0;
	icon_page.play_icon_state = 0;
	icon_page.exit_icon_state = 0;
	g_object_ref (about_page.window);
	gtk_container_remove (GTK_CONTAINER(app_window.main_page), about_page.window);
	gtk_container_add (GTK_CONTAINER(app_window.main_page), icon_page.window);
	g_object_unref (icon_page.window);
	gtk_widget_shape_combine_mask (icon_page.setup_icon, icon_page.icon_mask, 0, 0);
	gtk_widget_shape_combine_mask (icon_page.play_icon, icon_page.icon_mask, 0, 0);
	gtk_widget_shape_combine_mask (icon_page.exit_icon, icon_page.icon_mask, 0, 0);
}

static void option_back_press_callback (GtkWidget *widget, gpointer data)
{
	int rval = -1;
	memset (linux_launcher.text_buffer, '\0', sizeof linux_launcher.text_buffer);
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(game_option_page.windowed_check_box))) {
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(game_option_page.grab_mouse_check_box))) sprintf (linux_launcher.text_buffer, "sed -i 's/\"Desktop\"=-/\"Desktop\"=\"Default\"/g' engine/settings.reg ; sed -i 's/\"GrabFullscreen\"=\"N\"/\"GrabFullscreen\"=\"Y\"/g' engine/settings.reg");
		else sprintf (linux_launcher.text_buffer, "sed -i 's/\"Desktop\"=-/\"Desktop\"=\"Default\"/g' engine/settings.reg ; sed -i 's/\"GrabFullscreen\"=\"Y\"/\"GrabFullscreen\"=\"N\"/g' engine/settings.reg");
	}
	else {
		sprintf (linux_launcher.text_buffer, "sed -i 's/\"Desktop\"=\"Default\"/\"Desktop\"=-/g' engine/settings.reg ; sed -i 's/\"GrabFullscreen\"=\"N\"/\"GrabFullscreen\"=\"Y\"/g' engine/settings.reg");
	}
	rval = system (linux_launcher.text_buffer);
	
	if (WEXITSTATUS(rval) != 0) fprintf (stderr, "Could not edit settings.reg file\n");
	icon_page.setup_icon_state = 0;
	icon_page.play_icon_state = 0;
	icon_page.exit_icon_state = 0;
	g_object_ref (game_option_page.window);
	gtk_container_remove (GTK_CONTAINER(app_window.main_page), game_option_page.window);
	gtk_container_add (GTK_CONTAINER(app_window.main_page), icon_page.window);
	g_object_unref (icon_page.window);
	gtk_widget_shape_combine_mask (icon_page.setup_icon, icon_page.icon_mask, 0, 0);
	gtk_widget_shape_combine_mask (icon_page.play_icon, icon_page.icon_mask, 0, 0);
	gtk_widget_shape_combine_mask (icon_page.exit_icon, icon_page.icon_mask, 0, 0);
}

static void option_default_press_callback (GtkWidget *widget, gpointer data)
{
	char *current_dir = g_get_current_dir ();
	memset (linux_launcher.text_buffer, '\0', sizeof linux_launcher.text_buffer);
	sprintf (linux_launcher.text_buffer, "%s/game_folder/%s", current_dir, default_game_executable_macro);
	gtk_entry_set_text (GTK_ENTRY(game_option_page.exe_path_entry), linux_launcher.text_buffer);
	gtk_entry_set_text (GTK_ENTRY(game_option_page.exe_arg_entry), default_game_arguments_macro);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(game_option_page.windowed_check_box), FALSE);
}

static void set_back_press_callback (GtkWidget *widget, gpointer data)
{
	icon_page.setup_icon_state = 0;
	icon_page.play_icon_state = 0;
	icon_page.exit_icon_state = 0;
	g_object_ref (setup_page.window);
	gtk_container_remove (GTK_CONTAINER(app_window.main_page), setup_page.window);
	gtk_container_add (GTK_CONTAINER(app_window.main_page), icon_page.window);
	g_object_unref (icon_page.window);
	gtk_widget_shape_combine_mask (icon_page.setup_icon, icon_page.icon_mask, 0, 0);
	gtk_widget_shape_combine_mask (icon_page.play_icon, icon_page.icon_mask, 0, 0);
	gtk_widget_shape_combine_mask (icon_page.exit_icon, icon_page.icon_mask, 0, 0);
}

static void set_install_press_callback (GtkWidget *widget, gpointer data)
{
	if (strlen(gtk_entry_get_text(GTK_ENTRY(setup_page.location_entry))) != 0) {
		gtk_widget_show (setup_page.busy_box);
		linux_launcher.install_process_no = 0;
		linux_launcher.process_active = false;
		linux_launcher.install = true;
		g_timeout_add (1000, tloop, NULL);
		gtk_widget_set_sensitive (setup_page.install_button, FALSE);
		gtk_widget_set_sensitive (setup_page.back_button, FALSE);
	}
}

static void set_browse_press_callback (GtkWidget *widget, gpointer data)
{
	g_object_ref (setup_page.window);
	gtk_container_remove (GTK_CONTAINER(app_window.main_page), setup_page.window);
	gtk_container_add (GTK_CONTAINER(app_window.main_page), location_chooser_page.window);
	g_object_unref (location_chooser_page.window);
}

static void option_browse_press_callback (GtkWidget *widget, gpointer data)
{
	char *current_dir = g_get_current_dir ();
	GFile *exe_gfile = NULL;
	memset (linux_launcher.text_buffer, '\0', sizeof linux_launcher.text_buffer);
	sprintf (linux_launcher.text_buffer, "%s/game_folder/%s", current_dir, default_game_executable_macro);
	exe_gfile = g_file_new_for_path (linux_launcher.text_buffer);   
	gtk_file_chooser_set_file (GTK_FILE_CHOOSER (executable_chooser_page.file_chooser_widget), exe_gfile, NULL);
	g_object_unref (exe_gfile);
	g_object_ref (game_option_page.window);
	gtk_container_remove (GTK_CONTAINER(app_window.main_page), game_option_page.window);
	gtk_container_add (GTK_CONTAINER(app_window.main_page), executable_chooser_page.window);
	g_object_unref (executable_chooser_page.window);
}

static void chooser_select_press_callback (GtkWidget *widget, gpointer data)
{
	char *folder_uri = gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER(location_chooser_page.folder_chooser_widget));
	if (folder_uri != NULL) {
		gtk_entry_set_text (GTK_ENTRY(setup_page.location_entry), folder_uri);
		g_object_ref (location_chooser_page.window);
		gtk_container_remove (GTK_CONTAINER(app_window.main_page), location_chooser_page.window);
		gtk_container_add (GTK_CONTAINER(app_window.main_page), setup_page.window);
		g_object_unref (setup_page.window);
		g_free (folder_uri);
	}
}

static void chooser_cancel_press_callback (GtkWidget *widget, gpointer data)
{
	g_object_ref (location_chooser_page.window);
	gtk_container_remove (GTK_CONTAINER(app_window.main_page), location_chooser_page.window);
	gtk_container_add (GTK_CONTAINER(app_window.main_page), setup_page.window);
	g_object_unref (setup_page.window);
}

static void exe_chooser_select_press_callback (GtkWidget *widget, gpointer data)
{
	char *file_uri = gtk_file_chooser_get_uri (GTK_FILE_CHOOSER(executable_chooser_page.file_chooser_widget));
	if (file_uri != NULL) {
		GFile *exe_gfile = g_file_new_for_uri (file_uri);
		char *exe_file_path = g_file_get_path (exe_gfile);
		gtk_entry_set_text (GTK_ENTRY(game_option_page.exe_path_entry), exe_file_path);
		g_object_ref (executable_chooser_page.window);
		gtk_container_remove (GTK_CONTAINER(app_window.main_page), executable_chooser_page.window);
		gtk_container_add (GTK_CONTAINER(app_window.main_page), game_option_page.window);
		g_object_unref (game_option_page.window);
		g_free (exe_file_path);
		g_object_unref (exe_gfile);
		g_free (file_uri);
	}
}

static void exe_chooser_cancel_press_callback (GtkWidget *widget, gpointer data)
{
	g_object_ref (executable_chooser_page.window);
	gtk_container_remove (GTK_CONTAINER(app_window.main_page), executable_chooser_page.window);
	gtk_container_add (GTK_CONTAINER(app_window.main_page), game_option_page.window);
	g_object_unref (game_option_page.window);
}

static gboolean icon_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	if (strcmp((gchar *)data, "setup") == 0) {
		if (icon_page.setup_icon_state == 0) {
			cairo_t *cr;
			cr = gdk_cairo_create (widget->window);
			cairo_set_source_surface (cr, icon_page.icon_normal_setup, 0, 0);
			cairo_paint (cr);
			cairo_destroy (cr);
		}
		else if (icon_page.setup_icon_state == 1) {
			cairo_t *cr;
			cr = gdk_cairo_create (widget->window);
			cairo_set_source_surface (cr, icon_page.icon_hover_setup, 0, 0);
			cairo_paint (cr);
			cairo_destroy (cr);
		}
		else if (icon_page.setup_icon_state == 2) {
			cairo_t *cr;
			cr = gdk_cairo_create (widget->window);
			cairo_set_source_surface (cr, icon_page.icon_pressed_setup, 0, 0);
			cairo_paint (cr);
			cairo_destroy (cr);
		}
	}
	if (strcmp((gchar *)data, "play") == 0) {
		if (icon_page.play_icon_state == 0) {
			cairo_t *cr;
			cr = gdk_cairo_create (widget->window);
			cairo_set_source_surface (cr, icon_page.icon_normal_play, 0, 0);
			cairo_paint (cr);
			cairo_destroy (cr);
		}
		else if (icon_page.play_icon_state == 1) {
			cairo_t *cr;
			cr = gdk_cairo_create (widget->window);
			cairo_set_source_surface (cr, icon_page.icon_hover_play, 0, 0);
			cairo_paint (cr);
			cairo_destroy (cr);
		}
		else if (icon_page.play_icon_state == 2) {
			cairo_t *cr;
			cr = gdk_cairo_create (widget->window);
			cairo_set_source_surface (cr, icon_page.icon_pressed_play, 0, 0);
			cairo_paint (cr);
			cairo_destroy (cr);
		}
	}
	if (strcmp((gchar *)data, "exit") == 0) {
		if (icon_page.exit_icon_state == 0) {
			cairo_t *cr;
			cr = gdk_cairo_create (widget->window);
			cairo_set_source_surface (cr, icon_page.icon_normal_exit, 0, 0);
			cairo_paint (cr);
			cairo_destroy (cr);
		}
		else if (icon_page.exit_icon_state == 1) {
			cairo_t *cr;
			cr = gdk_cairo_create (widget->window);
			cairo_set_source_surface (cr, icon_page.icon_hover_exit, 0, 0);
			cairo_paint (cr);
			cairo_destroy (cr);
		}
		else if (icon_page.exit_icon_state == 2) {
			cairo_t *cr;
			cr = gdk_cairo_create (widget->window);
			cairo_set_source_surface (cr, icon_page.icon_pressed_exit, 0, 0);
			cairo_paint (cr);
			cairo_destroy (cr);
		}
	}
	return FALSE;
}

static gboolean icon_press_callback (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if (event->type == GDK_BUTTON_PRESS)
	{
		switch (event->button)
		{
			case 1:
				if (strcmp((gchar *)data, "setup") == 0) {
					icon_page.setup_icon_state = 2;
					gtk_widget_queue_draw (widget);
				}
				else if (strcmp((gchar *)data, "play") == 0) {
					icon_page.play_icon_state = 2;
					gtk_widget_queue_draw (widget);
				}
				else if (strcmp((gchar *)data, "exit") == 0) {
					icon_page.exit_icon_state = 2;
					gtk_widget_queue_draw (widget);
				}
				break;
			default:
				break;
		}
	}
	return FALSE;
}

static gboolean icon_release_callback (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if (event->type == GDK_BUTTON_RELEASE)
	{
		switch (event->button)
		{
			case 1:
				if (strcmp((gchar *)data, "setup") == 0) {
					icon_page.setup_icon_state = 1;
					gtk_widget_queue_draw (widget);
					gtk_widget_set_sensitive (setup_page.install_button, TRUE);
					g_object_ref (icon_page.window);
					gtk_container_remove (GTK_CONTAINER(app_window.main_page), icon_page.window);
					gtk_container_add (GTK_CONTAINER(app_window.main_page), setup_page.window);
					g_object_unref (setup_page.window);
				}
				else if (strcmp((gchar *)data, "play") == 0) {
					icon_page.play_icon_state = 1;
					gtk_widget_queue_draw (widget);
					if (run_game () != 0) {
						memset (linux_launcher.text_buffer, '\0', sizeof linux_launcher.text_buffer);
						sprintf (linux_launcher.text_buffer, "Could not launch %s", linux_launcher.game_title);
						gtk_label_set_text (GTK_LABEL(icon_page.status), linux_launcher.text_buffer);
					}
				}
				else if (strcmp((gchar *)data, "exit") == 0) {
					icon_page.exit_icon_state = 1;
					gtk_widget_queue_draw (widget);
					gtk_widget_destroy (GTK_WIDGET(app_window.window));
				}
				break;
			default:
				break;
		}
	}
	return FALSE;
}

static gboolean icon_enter_callback (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	if (strcmp((gchar *)data, "setup") == 0) {
		icon_page.setup_icon_state = 1;
		gtk_widget_queue_draw (widget);
		memset (linux_launcher.text_buffer, '\0', sizeof linux_launcher.text_buffer);
		sprintf (linux_launcher.text_buffer, "Install %s", linux_launcher.game_title);
		gtk_label_set_text (GTK_LABEL(icon_page.info), linux_launcher.text_buffer);	
	}
	else if (strcmp((gchar *)data, "play") == 0) {
		icon_page.play_icon_state = 1;
		gtk_widget_queue_draw (widget);
		memset (linux_launcher.text_buffer, '\0', sizeof linux_launcher.text_buffer);
		sprintf (linux_launcher.text_buffer, "Run %s", linux_launcher.game_title);
		gtk_label_set_text (GTK_LABEL(icon_page.info), linux_launcher.text_buffer);
	}
	else if (strcmp((gchar *)data, "exit") == 0) {
		icon_page.exit_icon_state = 1;
		gtk_widget_queue_draw (widget);
		memset (linux_launcher.text_buffer, '\0', sizeof linux_launcher.text_buffer);
		sprintf (linux_launcher.text_buffer, "Exit from %s Linux Launcher", linux_launcher.game_title);
		gtk_label_set_text (GTK_LABEL(icon_page.info), linux_launcher.text_buffer);
	}
	return FALSE;
}

static gboolean icon_leave_callback (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	if (strcmp((gchar *)data, "setup") == 0) {
		icon_page.setup_icon_state = 0;
		gtk_widget_queue_draw (widget);
		gtk_label_set_text (GTK_LABEL(icon_page.info), "");
		
	}
	else if (strcmp((gchar *)data, "play") == 0) {
		icon_page.play_icon_state = 0;
		gtk_widget_queue_draw (widget);
		gtk_label_set_text (GTK_LABEL(icon_page.info), "");
	}
	else if (strcmp((gchar *)data, "exit") == 0) {
		icon_page.exit_icon_state = 0;
		gtk_widget_queue_draw (widget);
		gtk_label_set_text (GTK_LABEL(icon_page.info), "");
	}
	return FALSE;
}

static gboolean icon_page_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	cairo_t *cr;
	cr = gdk_cairo_create (widget->window);
	cairo_set_source_surface (cr, app_window.background, 0, 0);
	cairo_paint (cr);
	cairo_destroy (cr);
	
	return FALSE;
}

static gboolean about_page_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	cairo_t *cr;
	cr = gdk_cairo_create (widget->window);
	cairo_set_source_surface (cr, app_window.background, 0, 0);
	cairo_paint (cr);
	cairo_destroy (cr);
	
	return FALSE;
}

static gboolean setup_page_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	cairo_t *cr;
	cr = gdk_cairo_create (widget->window);
	cairo_set_source_surface (cr, setup_page.background, 0, 0);
	cairo_paint (cr);
	cairo_destroy (cr);
	
	return FALSE;
}

static gboolean location_chooser_page_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	cairo_t *cr;
	cr = gdk_cairo_create (widget->window);
	cairo_set_source_surface (cr, app_window.background, 0, 0);
	cairo_paint (cr);
	cairo_destroy (cr);
	
	return FALSE;
}

static gboolean exe_chooser_page_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	cairo_t *cr;
	cr = gdk_cairo_create (widget->window);
	cairo_set_source_surface (cr, app_window.background, 0, 0);
	cairo_paint (cr);
	cairo_destroy (cr);
	
	return FALSE;
}

static gboolean option_page_expose_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	cairo_t *cr;
	cr = gdk_cairo_create (widget->window);
	cairo_set_source_surface (cr, app_window.background, 0, 0);
	cairo_paint (cr);
	cairo_destroy (cr);
	
	return FALSE;
}

static void option_windowed_press_callback (GtkWidget *widget, gpointer window)
{
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(game_option_page.windowed_check_box))) {
		gtk_widget_show (game_option_page.grab_mouse_check_box);
		gtk_widget_show (game_option_page.grab_mouse_label);
	}
	else {
		gtk_widget_hide (game_option_page.grab_mouse_check_box);
		gtk_widget_hide (game_option_page.grab_mouse_label);
	}
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(game_option_page.grab_mouse_check_box), TRUE);
}

int load_resource (void)
{
	char exe_file_dir_rel[10240];
	GFile *exe_gfile = g_file_new_for_path (game_executable_macro);
	GFile *exe_gfile_dir = g_file_get_parent (exe_gfile);
	char *exe_file_dir = g_file_get_path (exe_gfile_dir);
	char *exe_file_name = g_file_get_basename (exe_gfile);
	
	app_window.width = 640;
	app_window.height = 480;
	linux_launcher.install = false;
	linux_launcher.process_active = false;
	linux_launcher.install_process_no = 0;
	icon_page.setup_icon_state = 0;
	icon_page.play_icon_state = 0;
	icon_page.exit_icon_state = 0;
	icon_page.icon_width = 150;
	icon_page.icon_height = 150;
	memset (linux_launcher.game_title, '\0', sizeof linux_launcher.game_title);
	sprintf (linux_launcher.game_title, "%s", game_title_macro);
	memset (linux_launcher.game_install_executable, '\0', sizeof linux_launcher.game_install_executable);
	memset (linux_launcher.text_buffer, '\0', sizeof linux_launcher.text_buffer);
	sprintf (linux_launcher.text_buffer, game_install_executable_macro);
	unix_to_dox_path (linux_launcher.text_buffer, linux_launcher.game_install_executable);
	memset (linux_launcher.game_executable, '\0', sizeof linux_launcher.game_executable);
	sprintf (linux_launcher.game_executable, "%s", exe_file_name);
	memset (exe_file_dir_rel, '\0', sizeof exe_file_dir_rel);
	absolute_to_relative_path (exe_file_dir, exe_file_dir_rel);
	memset (linux_launcher.game_executable_directory, '\0', sizeof linux_launcher.game_executable_directory);
	sprintf (linux_launcher.game_executable_directory, "%s", exe_file_dir_rel);
	memset (linux_launcher.game_arguments, '\0', sizeof linux_launcher.game_arguments);
	sprintf (linux_launcher.game_arguments, "%s", default_game_arguments_macro);
	
	g_free (exe_file_dir);
	g_free (exe_file_name);
	g_object_unref (exe_gfile);
	g_object_unref (exe_gfile_dir);
	
	// load image files for icons and page backgrounds
	icon_page.icon_shape = cairo_image_surface_create_from_png("data/image/iconm.png");
	if (cairo_surface_status(icon_page.icon_shape) != CAIRO_STATUS_SUCCESS) return 1;
	icon_page.icon_normal_setup = cairo_image_surface_create_from_png("data/image/iconn_setup.png");
	if (cairo_surface_status(icon_page.icon_normal_setup) != CAIRO_STATUS_SUCCESS) return 1;
	icon_page.icon_hover_setup = cairo_image_surface_create_from_png("data/image/iconh_setup.png");
	if (cairo_surface_status(icon_page.icon_hover_setup) != CAIRO_STATUS_SUCCESS) return 1;
	icon_page.icon_pressed_setup = cairo_image_surface_create_from_png("data/image/iconp_setup.png");
	if (cairo_surface_status(icon_page.icon_pressed_setup) != CAIRO_STATUS_SUCCESS) return 1;
	icon_page.icon_normal_play = cairo_image_surface_create_from_png("data/image/iconn_play.png");
	if (cairo_surface_status(icon_page.icon_normal_play) != CAIRO_STATUS_SUCCESS) return 1;
	icon_page.icon_hover_play = cairo_image_surface_create_from_png("data/image/iconh_play.png");
	if (cairo_surface_status(icon_page.icon_hover_play) != CAIRO_STATUS_SUCCESS) return 1;
	icon_page.icon_pressed_play = cairo_image_surface_create_from_png("data/image/iconp_play.png");
	if (cairo_surface_status(icon_page.icon_pressed_play) != CAIRO_STATUS_SUCCESS) return 1;
	icon_page.icon_normal_exit = cairo_image_surface_create_from_png("data/image/iconn_exit.png");
	if (cairo_surface_status(icon_page.icon_normal_exit) != CAIRO_STATUS_SUCCESS) return 1;
	icon_page.icon_hover_exit = cairo_image_surface_create_from_png("data/image/iconh_exit.png");
	if (cairo_surface_status(icon_page.icon_hover_exit) != CAIRO_STATUS_SUCCESS) return 1;
	icon_page.icon_pressed_exit = cairo_image_surface_create_from_png("data/image/iconp_exit.png");
	if (cairo_surface_status(icon_page.icon_pressed_exit) != CAIRO_STATUS_SUCCESS) return 1;
	app_window.background = cairo_image_surface_create_from_png("data/image/back.png");
	if (cairo_surface_status(app_window.background) != CAIRO_STATUS_SUCCESS) return 1;
	setup_page.background = cairo_image_surface_create_from_png("data/image/back1.png");
	if (cairo_surface_status(setup_page.background) != CAIRO_STATUS_SUCCESS) return 1;
	
	// prepare mask for the icons
	icon_page.icon_mask = gdk_pixmap_new (NULL, icon_page.icon_width, icon_page.icon_height, 1);
	if (icon_page.icon_mask == NULL) return 2;
	icon_page.cairo_context = gdk_cairo_create (icon_page.icon_mask);
	if (icon_page.cairo_context == NULL) return 2;
	cairo_save (icon_page.cairo_context);
	cairo_rectangle (icon_page.cairo_context, 0, 0, icon_page.icon_width, icon_page.icon_height);
	cairo_set_operator (icon_page.cairo_context, CAIRO_OPERATOR_CLEAR);
	cairo_fill (icon_page.cairo_context);
	cairo_restore (icon_page.cairo_context);
	cairo_set_source_surface(icon_page.cairo_context, icon_page.icon_shape, 0, 0);
	cairo_paint(icon_page.cairo_context);
	cairo_destroy (icon_page.cairo_context);
	
	return 0;
}

// free up the image surfaces
void free_resource (void)
{
	fprintf (stdout, "LL: freeing resources ...\n");
	cairo_surface_destroy (icon_page.icon_shape);
	cairo_surface_destroy (icon_page.icon_normal_setup);
	cairo_surface_destroy (icon_page.icon_hover_setup);
	cairo_surface_destroy (icon_page.icon_pressed_setup);
	cairo_surface_destroy (icon_page.icon_normal_play);
	cairo_surface_destroy (icon_page.icon_hover_play);
	cairo_surface_destroy (icon_page.icon_pressed_play);
	cairo_surface_destroy (icon_page.icon_normal_exit);
	cairo_surface_destroy (icon_page.icon_hover_exit);
	cairo_surface_destroy (icon_page.icon_pressed_exit);
	cairo_surface_destroy (app_window.background);
	cairo_surface_destroy (setup_page.background);
}

void create_icon_page (void)
{
	icon_page.setup_icon = gtk_event_box_new ();
	g_signal_connect (G_OBJECT(icon_page.setup_icon), "expose_event", G_CALLBACK(icon_expose_callback), (gpointer) "setup");
	g_signal_connect (G_OBJECT(icon_page.setup_icon), "button_press_event", G_CALLBACK(icon_press_callback), (gpointer) "setup");
	g_signal_connect (G_OBJECT(icon_page.setup_icon), "button_release_event", G_CALLBACK(icon_release_callback), (gpointer) "setup");
	g_signal_connect (G_OBJECT(icon_page.setup_icon), "enter_notify_event", G_CALLBACK(icon_enter_callback), (gpointer) "setup");
	g_signal_connect (G_OBJECT(icon_page.setup_icon), "leave_notify_event", G_CALLBACK(icon_leave_callback), (gpointer) "setup");
	gtk_widget_set_app_paintable (icon_page.setup_icon, TRUE);
	gtk_widget_set_size_request (icon_page.setup_icon, icon_page.icon_width, icon_page.icon_height);
	gtk_widget_shape_combine_mask (icon_page.setup_icon, icon_page.icon_mask, 0, 0);
	
	icon_page.icon_location_x = 450;
	icon_page.icon_location_y = 10;
	
	icon_page.box_fixed = gtk_fixed_new ();
	gtk_fixed_put (GTK_FIXED(icon_page.box_fixed), icon_page.setup_icon, icon_page.icon_location_x, icon_page.icon_location_y);
	gtk_widget_show (icon_page.setup_icon);
	
	icon_page.play_icon = gtk_event_box_new ();
	g_signal_connect (G_OBJECT(icon_page.play_icon), "expose_event", G_CALLBACK(icon_expose_callback), (gpointer) "play");
	g_signal_connect (G_OBJECT(icon_page.play_icon), "button_press_event", G_CALLBACK(icon_press_callback), (gpointer) "play");
	g_signal_connect (G_OBJECT(icon_page.play_icon), "button_release_event", G_CALLBACK(icon_release_callback), (gpointer) "play");
	g_signal_connect (G_OBJECT(icon_page.play_icon), "enter_notify_event", G_CALLBACK(icon_enter_callback), (gpointer) "play");
	g_signal_connect (G_OBJECT(icon_page.play_icon), "leave_notify_event", G_CALLBACK(icon_leave_callback), (gpointer) "play");
	gtk_widget_set_app_paintable (icon_page.play_icon, TRUE);
	gtk_widget_set_size_request (icon_page.play_icon, icon_page.icon_width, icon_page.icon_height);
	gtk_widget_shape_combine_mask (icon_page.play_icon, icon_page.icon_mask, 0, 0);
	
	icon_page.icon_location_x = 450;
	icon_page.icon_location_y = 165;
	
	gtk_fixed_put (GTK_FIXED(icon_page.box_fixed), icon_page.play_icon, icon_page.icon_location_x, icon_page.icon_location_y);
	gtk_widget_show (icon_page.play_icon);
	
	icon_page.exit_icon = gtk_event_box_new ();
	g_signal_connect (G_OBJECT(icon_page.exit_icon), "expose_event", G_CALLBACK(icon_expose_callback), (gpointer) "exit");
	g_signal_connect (G_OBJECT(icon_page.exit_icon), "button_press_event", G_CALLBACK(icon_press_callback), (gpointer) "exit");
	g_signal_connect (G_OBJECT(icon_page.exit_icon), "button_release_event", G_CALLBACK (icon_release_callback), (gpointer) "exit");
	g_signal_connect (G_OBJECT(icon_page.exit_icon), "enter_notify_event", G_CALLBACK(icon_enter_callback), (gpointer) "exit");
	g_signal_connect (G_OBJECT(icon_page.exit_icon), "leave_notify_event", G_CALLBACK(icon_leave_callback), (gpointer) "exit");
	gtk_widget_set_app_paintable (icon_page.exit_icon, TRUE);
	gtk_widget_set_size_request (icon_page.exit_icon, icon_page.icon_width, icon_page.icon_height);
	gtk_widget_shape_combine_mask (icon_page.exit_icon, icon_page.icon_mask, 0, 0);
	
	icon_page.icon_location_x = 450;
	icon_page.icon_location_y = 320;
	
	gtk_fixed_put (GTK_FIXED(icon_page.box_fixed), icon_page.exit_icon, icon_page.icon_location_x, icon_page.icon_location_y);
	gtk_widget_show (icon_page.exit_icon);
	
	icon_page.about_button = gtk_button_new_with_label (" About ");
	g_signal_connect (G_OBJECT(icon_page.about_button), "clicked", G_CALLBACK (babout_press_callback), NULL);
	gtk_widget_set_size_request (icon_page.about_button, -1, 25);
	gtk_widget_set_name (icon_page.about_button, "Icon_About");
	
	icon_page.icon_location_x = 40;
	icon_page.icon_location_y = 400;
	
	gtk_fixed_put (GTK_FIXED(icon_page.box_fixed), icon_page.about_button, icon_page.icon_location_x, icon_page.icon_location_y);
	gtk_widget_show (icon_page.about_button);
	
	icon_page.option_button = gtk_button_new_with_label (" Game options ");
	g_signal_connect (G_OBJECT(icon_page.option_button), "clicked", G_CALLBACK (boption_press_callback), NULL);
	gtk_widget_set_name (icon_page.option_button, "Icon_Option");
	
	icon_page.icon_location_x = 100;
	icon_page.icon_location_y = 400;
	
	gtk_fixed_put (GTK_FIXED(icon_page.box_fixed), icon_page.option_button, icon_page.icon_location_x, icon_page.icon_location_y);
	
	icon_page.info = gtk_label_new (NULL);
	gtk_widget_set_name (icon_page.info, "Icon_Info");
	
	icon_page.icon_location_x = 30;
	icon_page.icon_location_y = 100;
	
	gtk_fixed_put (GTK_FIXED(icon_page.box_fixed), icon_page.info, icon_page.icon_location_x, icon_page.icon_location_y);
	gtk_widget_show (icon_page.info);
	
	icon_page.status = gtk_label_new (NULL);
	gtk_widget_set_name (icon_page.status, "Icon_Status");
	
	icon_page.icon_location_x = 30;
	icon_page.icon_location_y = 140;
	
	gtk_fixed_put (GTK_FIXED(icon_page.box_fixed), icon_page.status, icon_page.icon_location_x, icon_page.icon_location_y);
	gtk_widget_show (icon_page.status);
	
	icon_page.window = gtk_event_box_new ();
	g_signal_connect (G_OBJECT(icon_page.window), "expose_event", G_CALLBACK(icon_page_expose_callback), NULL);
	gtk_widget_set_app_paintable (icon_page.window, TRUE);
	gtk_container_add (GTK_CONTAINER(icon_page.window), icon_page.box_fixed);
	gtk_widget_show (icon_page.box_fixed);
	
	gtk_container_add (GTK_CONTAINER(app_window.main_page), icon_page.window);
	gtk_widget_show (icon_page.window);
}

void create_about_page (void)
{
	about_page.info = gtk_label_new (NULL);
	gtk_widget_set_name (about_page.info, "About_Info");
	memset (about_page.text_buffer, 0, sizeof about_page.text_buffer);
	sprintf (about_page.text_buffer, "<b>%s Linux Launcher</b>\nSetup and Play %s from game installation disk\n\n(C) 2014 Md. Imam Hossain\nemamhd@gmail.com\n\n\n\n%s Linux Launcher uses software from the following projects:\n\n<b>1.</b> WINE    <a href=\"https://www.winehq.org/\">www.winehq.org</a>\n<b>2.</b> GTK+    <a href=\"http://www.gtk.org/\">www.gtk.org</a>", linux_launcher.game_title, linux_launcher.game_title, linux_launcher.game_title);
	gtk_label_set_markup (GTK_LABEL(about_page.info), about_page.text_buffer);
	
	about_page.box_fixed = gtk_fixed_new ();
	
	gtk_fixed_put (GTK_FIXED(about_page.box_fixed), about_page.info, 20, 40);
	gtk_widget_show (about_page.info);
	
	about_page.back_button = gtk_button_new_with_label (" << Back ");
	g_signal_connect (G_OBJECT(about_page.back_button), "clicked", G_CALLBACK (about_back_press_callback), NULL);
	gtk_widget_set_size_request (about_page.back_button, -1, 25);
	gtk_widget_set_name (about_page.back_button, "About_Back");
	
	gtk_fixed_put (GTK_FIXED(about_page.box_fixed), about_page.back_button, 20, 400);
	gtk_widget_show (about_page.back_button);
	
	about_page.window = gtk_event_box_new ();
	g_signal_connect (G_OBJECT(about_page.window), "expose_event", G_CALLBACK(about_page_expose_callback), NULL);
	gtk_widget_set_app_paintable (about_page.window, TRUE);
	gtk_container_add (GTK_CONTAINER(about_page.window), about_page.box_fixed);
	gtk_widget_show (about_page.box_fixed);
	gtk_widget_show (about_page.window);
	g_object_ref_sink (about_page.window);
}

void create_setup_page (void)
{
	setup_page.info = gtk_label_new (NULL);
	gtk_widget_set_name (setup_page.info, "Setup_Info");
	memset (setup_page.text_buffer, 0, sizeof setup_page.text_buffer);
	sprintf (setup_page.text_buffer, "<b>Please, follow these steps to install %s on your system</b>\n\n<b>1.</b> Mount %s disk on your System\n<b>2.</b> Under Disk location click Browse and enter the %s mounted folder\nand click Select\n<b>3.</b> Click Start Installation\n<b>4.</b> In %s Setup, please do not change default Destination Directory\n<b>5.</b> Finish %s setup", linux_launcher.game_title, linux_launcher.game_title, linux_launcher.game_title, linux_launcher.game_title, linux_launcher.game_title);
	gtk_label_set_markup (GTK_LABEL(setup_page.info), setup_page.text_buffer);
	
	setup_page.box_fixed = gtk_fixed_new ();
	
	gtk_fixed_put (GTK_FIXED(setup_page.box_fixed), setup_page.info, 20, 40);
	gtk_widget_show (setup_page.info);
	
	setup_page.status = gtk_label_new (NULL);
	gtk_widget_set_name (setup_page.status, "Setup_Status");
	
	gtk_fixed_put (GTK_FIXED(setup_page.box_fixed), setup_page.status, 220, 200);
	gtk_widget_show (setup_page.status);
	
	setup_page.busy_box = gtk_image_new_from_file ("data/image/busy.gif");
	
	gtk_fixed_put (GTK_FIXED(setup_page.box_fixed), setup_page.busy_box, 220, 240);
	
	setup_page.location_entry = gtk_entry_new ();
	gtk_entry_set_has_frame (GTK_ENTRY(setup_page.location_entry), FALSE);
	setup_page.location_browse_button = gtk_button_new_with_label (" Browse ");
	g_signal_connect (G_OBJECT(setup_page.location_browse_button), "clicked", G_CALLBACK (set_browse_press_callback), NULL);
	gtk_widget_set_size_request (setup_page.location_browse_button, -1, 25);
	gtk_widget_set_name (setup_page.location_browse_button, "Setup_Browse");
	
	setup_page.location_label = gtk_label_new ("Disk Location:");
	gtk_widget_set_name (setup_page.location_label, "Setup_Label");
	
	setup_page.box_horizontal = gtk_hbox_new (FALSE, 5);
	gtk_widget_set_size_request (setup_page.box_horizontal, 600, -1);
	gtk_box_pack_start (GTK_BOX(setup_page.box_horizontal), setup_page.location_label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(setup_page.box_horizontal), setup_page.location_entry, TRUE, TRUE, 0);
	gtk_box_pack_end (GTK_BOX(setup_page.box_horizontal), setup_page.location_browse_button, FALSE, FALSE, 0);
	gtk_widget_show (setup_page.location_label);
	gtk_widget_show (setup_page.location_entry);
	gtk_widget_show (setup_page.location_browse_button);
	
	gtk_fixed_put (GTK_FIXED(setup_page.box_fixed), setup_page.box_horizontal, 20, 300);
	gtk_widget_show (setup_page.box_horizontal);
	
	setup_page.install_button = gtk_button_new_with_label (" Start Installation >> ");
	g_signal_connect (G_OBJECT(setup_page.install_button), "clicked", G_CALLBACK (set_install_press_callback), NULL);
	gtk_widget_set_size_request (setup_page.install_button, -1, 25);
	gtk_widget_set_name (setup_page.install_button, "Setup_Install");
	
	setup_page.back_button = gtk_button_new_with_label (" << Back ");
	g_signal_connect (G_OBJECT(setup_page.back_button), "clicked", G_CALLBACK (set_back_press_callback), NULL);
	gtk_widget_set_size_request (setup_page.back_button, -1, 25);
	gtk_widget_set_name (setup_page.back_button, "Setup_Back");
	
	setup_page.back_box_horizontal = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX(setup_page.back_box_horizontal), setup_page.install_button, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(setup_page.back_box_horizontal), setup_page.back_button, FALSE, FALSE, 0);
	gtk_widget_show (setup_page.back_button);
	gtk_widget_show (setup_page.install_button);
	
	setup_page.back_box_alignment = gtk_alignment_new (0, 0, 0, 0);
	gtk_container_add (GTK_CONTAINER(setup_page.back_box_alignment), setup_page.back_box_horizontal);
	gtk_widget_show (setup_page.back_box_horizontal);
	
	gtk_fixed_put (GTK_FIXED(setup_page.box_fixed), setup_page.back_box_alignment, 20, 400);
	gtk_widget_show (setup_page.back_box_alignment);
	
	setup_page.window = gtk_event_box_new ();
	g_signal_connect (G_OBJECT(setup_page.window), "expose_event", G_CALLBACK(setup_page_expose_callback), NULL);
	gtk_widget_set_app_paintable (setup_page.window, TRUE);
	gtk_container_add (GTK_CONTAINER(setup_page.window), setup_page.box_fixed);
	gtk_widget_show (setup_page.box_fixed);
	gtk_widget_show (setup_page.window);
	g_object_ref_sink (setup_page.window);
}

void create_location_chooser_page (void)
{
	location_chooser_page.select_button = gtk_button_new_with_label (" Select ");
	g_signal_connect (G_OBJECT(location_chooser_page.select_button), "clicked", G_CALLBACK (chooser_select_press_callback), NULL);
	gtk_widget_set_size_request (location_chooser_page.select_button, -1, 25);
	gtk_widget_set_name (location_chooser_page.select_button, "Location_Select");
	location_chooser_page.cancel_button = gtk_button_new_with_label (" Cancel ");
	g_signal_connect (G_OBJECT(location_chooser_page.cancel_button), "clicked", G_CALLBACK (chooser_cancel_press_callback), NULL);
	gtk_widget_set_size_request (location_chooser_page.cancel_button, -1, 25);
	gtk_widget_set_name (location_chooser_page.cancel_button, "Location_Cancel");
	
	location_chooser_page.action_box_horizontal = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX(location_chooser_page.action_box_horizontal), location_chooser_page.select_button, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(location_chooser_page.action_box_horizontal), location_chooser_page.cancel_button, FALSE, FALSE, 0);
	gtk_widget_show (location_chooser_page.select_button);
	gtk_widget_show (location_chooser_page.cancel_button);
	
	location_chooser_page.action_box_alignment = gtk_alignment_new (1, 0, 0, 0);
	gtk_container_add (GTK_CONTAINER(location_chooser_page.action_box_alignment), location_chooser_page.action_box_horizontal);
	gtk_widget_show (location_chooser_page.action_box_horizontal);
	
	location_chooser_page.folder_chooser_widget = gtk_file_chooser_widget_new (GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
	gtk_file_chooser_set_local_only (GTK_FILE_CHOOSER(location_chooser_page.folder_chooser_widget), true);
	
	gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER(location_chooser_page.folder_chooser_widget), false);
	
	location_chooser_page.box_vertical = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX(location_chooser_page.box_vertical), location_chooser_page.folder_chooser_widget, TRUE, TRUE, 0);
	gtk_box_pack_end (GTK_BOX(location_chooser_page.box_vertical), location_chooser_page.action_box_alignment, FALSE, FALSE, 0);
	gtk_widget_show (location_chooser_page.folder_chooser_widget);
	gtk_widget_show (location_chooser_page.action_box_alignment);
	
	location_chooser_page.window = gtk_event_box_new ();
	g_signal_connect (G_OBJECT(location_chooser_page.window), "expose_event", G_CALLBACK(location_chooser_page_expose_callback), NULL);
	gtk_widget_set_app_paintable (location_chooser_page.window, TRUE);
	gtk_container_add (GTK_CONTAINER(location_chooser_page.window), location_chooser_page.box_vertical);
	gtk_widget_show (location_chooser_page.box_vertical);
	gtk_widget_show (location_chooser_page.window);
	g_object_ref_sink (location_chooser_page.window);
}

void create_executable_chooser_page (void)
{
	executable_chooser_page.select_button = gtk_button_new_with_label (" Select ");
	g_signal_connect (G_OBJECT(executable_chooser_page.select_button), "clicked", G_CALLBACK (exe_chooser_select_press_callback), NULL);
	gtk_widget_set_size_request (executable_chooser_page.select_button, -1, 25);
	gtk_widget_set_name (executable_chooser_page.select_button, "EXE_Select");
	executable_chooser_page.cancel_button = gtk_button_new_with_label (" Cancel ");
	g_signal_connect (G_OBJECT(executable_chooser_page.cancel_button), "clicked", G_CALLBACK (exe_chooser_cancel_press_callback), NULL);
	gtk_widget_set_size_request (executable_chooser_page.cancel_button, -1, 25);
	gtk_widget_set_name (executable_chooser_page.cancel_button, "EXE_Cancel");
	
	executable_chooser_page.action_box_horizontal = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX(executable_chooser_page.action_box_horizontal), executable_chooser_page.select_button, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(executable_chooser_page.action_box_horizontal), executable_chooser_page.cancel_button, FALSE, FALSE, 0);
	gtk_widget_show (executable_chooser_page.select_button);
	gtk_widget_show (executable_chooser_page.cancel_button);
	
	executable_chooser_page.action_box_alignment = gtk_alignment_new (1, 0, 0, 0);
	gtk_container_add (GTK_CONTAINER(executable_chooser_page.action_box_alignment), executable_chooser_page.action_box_horizontal);
	gtk_widget_show (executable_chooser_page.action_box_horizontal);
	
	executable_chooser_page.file_chooser_filter = gtk_file_filter_new ();
	gtk_file_filter_add_pattern (executable_chooser_page.file_chooser_filter, "*.exe");
	gtk_file_filter_add_pattern (executable_chooser_page.file_chooser_filter, "*.EXE");
	
	executable_chooser_page.file_chooser_widget = gtk_file_chooser_widget_new (GTK_FILE_CHOOSER_ACTION_OPEN);
	gtk_file_chooser_set_local_only (GTK_FILE_CHOOSER(executable_chooser_page.file_chooser_widget), true);
	gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER(executable_chooser_page.file_chooser_widget), false);
	gtk_file_chooser_set_filter (GTK_FILE_CHOOSER(executable_chooser_page.file_chooser_widget), GTK_FILE_FILTER(executable_chooser_page.file_chooser_filter));
	
	executable_chooser_page.box_vertical = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX(executable_chooser_page.box_vertical), executable_chooser_page.file_chooser_widget, TRUE, TRUE, 0);
	gtk_box_pack_end (GTK_BOX(executable_chooser_page.box_vertical), executable_chooser_page.action_box_alignment, FALSE, FALSE, 0);
	gtk_widget_show (executable_chooser_page.file_chooser_widget);
	gtk_widget_show (executable_chooser_page.action_box_alignment);
	
	executable_chooser_page.window = gtk_event_box_new ();
	g_signal_connect (G_OBJECT(executable_chooser_page.window), "expose_event", G_CALLBACK(exe_chooser_page_expose_callback), NULL);
	gtk_widget_set_app_paintable (executable_chooser_page.window, TRUE);
	gtk_container_add (GTK_CONTAINER(executable_chooser_page.window), executable_chooser_page.box_vertical);
	gtk_widget_show (executable_chooser_page.box_vertical);
	gtk_widget_show (executable_chooser_page.window);
	g_object_ref_sink (executable_chooser_page.window);
}

void create_game_option_page (void)
{
	game_option_page.exe_path_label = gtk_label_new ("Game executable:");
	gtk_widget_set_name (game_option_page.exe_path_label, "Option_Path");
	game_option_page.exe_path_entry = gtk_entry_new ();
	gtk_entry_set_has_frame (GTK_ENTRY(game_option_page.exe_path_entry), FALSE);
	game_option_page.exe_browse_button = gtk_button_new_with_label (" Browse ");
	g_signal_connect (G_OBJECT(game_option_page.exe_browse_button), "clicked", G_CALLBACK (option_browse_press_callback), NULL);
	gtk_widget_set_size_request (game_option_page.exe_browse_button, -1, 25);
	gtk_widget_set_name (game_option_page.exe_browse_button, "Option_Browse");
	
	game_option_page.exe_path_box_horizontal = gtk_hbox_new (FALSE, 5);
	gtk_widget_set_size_request (game_option_page.exe_path_box_horizontal, 600, -1);
	gtk_box_pack_start (GTK_BOX(game_option_page.exe_path_box_horizontal), game_option_page.exe_path_label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(game_option_page.exe_path_box_horizontal), game_option_page.exe_path_entry, TRUE, TRUE, 0);
	gtk_box_pack_end (GTK_BOX(game_option_page.exe_path_box_horizontal), game_option_page.exe_browse_button, FALSE, FALSE, 0);
	gtk_widget_show (game_option_page.exe_path_label);
	gtk_widget_show (game_option_page.exe_path_entry);
	gtk_widget_show (game_option_page.exe_browse_button);
	
	game_option_page.box_fixed = gtk_fixed_new ();
	
	gtk_fixed_put (GTK_FIXED(game_option_page.box_fixed), game_option_page.exe_path_box_horizontal, 20, 40);
	gtk_widget_show (game_option_page.exe_path_box_horizontal);
	
	game_option_page.exe_arg_label = gtk_label_new ("Executable arguments:");
	gtk_widget_set_name (game_option_page.exe_arg_label, "Option_Arg");
	game_option_page.exe_arg_entry = gtk_entry_new ();
	gtk_entry_set_has_frame (GTK_ENTRY(game_option_page.exe_arg_entry), FALSE);
	
	game_option_page.exe_arg_box_horizontal = gtk_hbox_new (FALSE, 5);
	gtk_widget_set_size_request (game_option_page.exe_arg_box_horizontal, 530, -1);
	gtk_box_pack_start (GTK_BOX(game_option_page.exe_arg_box_horizontal), game_option_page.exe_arg_label, FALSE, FALSE, 0);
	gtk_box_pack_end (GTK_BOX(game_option_page.exe_arg_box_horizontal), game_option_page.exe_arg_entry, TRUE, TRUE, 0);
	gtk_widget_show (game_option_page.exe_arg_label);
	gtk_widget_show (game_option_page.exe_arg_entry);
	
	gtk_fixed_put (GTK_FIXED(game_option_page.box_fixed), game_option_page.exe_arg_box_horizontal, 20, 80);
	gtk_widget_show (game_option_page.exe_arg_box_horizontal);
	
	game_option_page.windowed_check_box = gtk_check_button_new ();
	g_signal_connect(game_option_page.windowed_check_box, "clicked", G_CALLBACK(option_windowed_press_callback), NULL);
	game_option_page.windowed_label = gtk_label_new ("Run in the Windowed Mode");
	gtk_widget_set_name (game_option_page.windowed_label, "Option_Windowed");
	
	gtk_fixed_put (GTK_FIXED(game_option_page.box_fixed), game_option_page.windowed_check_box, 20, 120);
	gtk_fixed_put (GTK_FIXED(game_option_page.box_fixed), game_option_page.windowed_label, 50, 124);
	gtk_widget_show (game_option_page.windowed_check_box);
	gtk_widget_show (game_option_page.windowed_label);
	
	game_option_page.grab_mouse_check_box = gtk_check_button_new ();
	game_option_page.grab_mouse_label = gtk_label_new ("Grab Mouse Pointer");
	gtk_widget_set_name (game_option_page.grab_mouse_label, "Option_Mouse");
	
	gtk_fixed_put (GTK_FIXED(game_option_page.box_fixed), game_option_page.grab_mouse_check_box, 20, 140);
	gtk_fixed_put (GTK_FIXED(game_option_page.box_fixed), game_option_page.grab_mouse_label, 50, 144);
	
	game_option_page.back_button = gtk_button_new_with_label (" << Back ");
	g_signal_connect (G_OBJECT(game_option_page.back_button), "clicked", G_CALLBACK (option_back_press_callback), NULL);
	gtk_widget_set_size_request (game_option_page.back_button, -1, 25);
	gtk_widget_set_name (game_option_page.back_button, "Option_Back");
	
	gtk_fixed_put (GTK_FIXED(game_option_page.box_fixed), game_option_page.back_button, 20, 400);
	gtk_widget_show (game_option_page.back_button);
	
	game_option_page.defaults_button = gtk_button_new_with_label (" Defaults ");
	g_signal_connect (G_OBJECT(game_option_page.defaults_button), "clicked", G_CALLBACK (option_default_press_callback), NULL);
	gtk_widget_set_size_request (game_option_page.defaults_button, -1, 25);
	gtk_widget_set_name (game_option_page.defaults_button, "Option_Defaults");
	
	gtk_fixed_put (GTK_FIXED(game_option_page.box_fixed), game_option_page.defaults_button, 100, 400);
	gtk_widget_show (game_option_page.defaults_button);
	
	game_option_page.window = gtk_event_box_new ();
	g_signal_connect (G_OBJECT(game_option_page.window), "expose_event", G_CALLBACK(option_page_expose_callback), NULL);
	gtk_widget_set_app_paintable (game_option_page.window, TRUE);
	gtk_container_add (GTK_CONTAINER(game_option_page.window), game_option_page.box_fixed);
	gtk_widget_show (game_option_page.box_fixed);
	gtk_widget_show (game_option_page.window);
	g_object_ref_sink (game_option_page.window);
}

// timer to create, prepare WINE prefix and complete game installation
static gboolean tloop (gpointer data)
{
	if (linux_launcher.install_process_no == 0 && linux_launcher.process_active == false) {	// install_process_no: 0 for creating WINE prefix
		char comm[8192];
		struct sigaction sa;
		
		memset (comm, 0, sizeof comm);
		memset (&sa, 0, sizeof sa);
		linux_launcher.cpid = -1;
		
		sprintf (comm, "cd engine ; export WINEVERPATH=\"`pwd`\" ; export PATH=\"$WINEVERPATH/bin:$PATH\" ; export WINESERVER=\"$WINEVERPATH/bin/wineserver\" ;  export WINELOADER=\"$WINEVERPATH/bin/wine\" ; export WINEDLLPATH=\"$WINEVERPATH/lib/wine/fakedlls\" ; export LD_LIBRARY_PATH=\"$WINEVERPATH/lib:$LD_LIBRARY_PATH\" ; export XDG_DATA_DIRS=\"$WINEVERPATH/share:$XDG_DATA_DIRS\" ; export WINEPREFIX=\"`pwd`/game\" ; export WINEARCH=\"win32\" ; wineboot");
		
		linux_launcher.cpid = fork ();
		
		if (linux_launcher.cpid == -1) {
			linux_launcher.install = false;
			return false;
		}
		else if (linux_launcher.cpid == 0) {
			int rval = system (comm);
			_exit (WEXITSTATUS(rval));
		}
		else {
			gtk_label_set_text (GTK_LABEL(setup_page.status), "Creating WINE Prefix");
			linux_launcher.process_active = true;
			sa.sa_handler = &install_process_callback;
			sigaction (SIGCHLD, &sa, NULL);
		}
	}
	else if (linux_launcher.install_process_no == 1 && linux_launcher.process_active == false) {	// install_process_no: 1 for changing WINE documents folder location to game_save folder
		char comm[8192];
		struct sigaction sa;
		
		memset (comm, 0, sizeof comm);
		memset (&sa, 0, sizeof sa);
		linux_launcher.cpid = -1;
		
		sprintf (comm, "mkdir game_save ; export INDV=\"`pwd`\" ; cd \"engine/game/drive_c/users/`whoami`/\" ; rm \"My Documents\" ; ln -s \"$INDV/game_save\" \"My Documents\"");
		
		linux_launcher.cpid = fork ();
		
		if (linux_launcher.cpid == -1) {
			linux_launcher.install = false;
			return false;
			
		}
		else if (linux_launcher.cpid == 0) {
			int rval = system (comm);
			_exit (WEXITSTATUS(rval));
		}
		else {
			gtk_label_set_text (GTK_LABEL(setup_page.status), "Configuring WINE documents");
			linux_launcher.process_active = true;
			sa.sa_handler = &install_process_callback;
			sigaction (SIGCHLD, &sa, NULL);
		}
	}
	else if (linux_launcher.install_process_no == 2 && linux_launcher.process_active == false) {	// install_process_no: 2 for creating WINE device m: linking to game disk location
		char comm[8192];
		char cd_location[8192];
		struct sigaction sa;
		
		memset (comm, 0, sizeof comm);
		memset (cd_location, 0, sizeof cd_location);
		memset (&sa, 0, sizeof sa);
		linux_launcher.cpid = -1;
		
		sprintf (cd_location, "%s", gtk_entry_get_text(GTK_ENTRY(setup_page.location_entry)));
		sprintf (comm, "cd \"engine/game/dosdevices/\" ; ln -s \"%s\" \"m:\"", cd_location);
		
		linux_launcher.cpid = fork ();
		
		if (linux_launcher.cpid == -1) {
			linux_launcher.install = false;
			return false;
		}
		else if (linux_launcher.cpid == 0) {
			int rval = system (comm);
			_exit (WEXITSTATUS(rval));
		}
		else {
			gtk_label_set_text (GTK_LABEL(setup_page.status), "Configuring WINE CD-ROM");
			linux_launcher.process_active = true;
			sa.sa_handler = &install_process_callback;
			sigaction (SIGCHLD, &sa, NULL);
		}
	}
	else if (linux_launcher.install_process_no == 3 && linux_launcher.process_active == false) {	// install_process_no: 3 for changing WINE settings for game installation
		char comm[8192];
		struct sigaction sa;
		
		memset (comm, 0, sizeof comm);
		memset (&sa, 0, sizeof sa);
		linux_launcher.cpid = -1;
		
		sprintf (comm, "cd engine ; export WINEVERPATH=\"`pwd`\" ; export PATH=\"$WINEVERPATH/bin:$PATH\" ; export WINESERVER=\"$WINEVERPATH/bin/wineserver\" ;  export WINELOADER=\"$WINEVERPATH/bin/wine\" ; export WINEDLLPATH=\"$WINEVERPATH/lib/wine/fakedlls\" ; export LD_LIBRARY_PATH=\"$WINEVERPATH/lib:$LD_LIBRARY_PATH\" ; export XDG_DATA_DIRS=\"$WINEVERPATH/share:$XDG_DATA_DIRS\" ; export WINEPREFIX=\"`pwd`/game\" ; export WINEARCH=\"win32\" ; regedit install.reg");
		
		linux_launcher.cpid = fork ();
		
		if (linux_launcher.cpid == -1) {
			linux_launcher.install = false;
			return false;
		}
		else if (linux_launcher.cpid == 0) {
			int rval = system (comm);
			_exit (WEXITSTATUS(rval));
		}
		else {
			gtk_label_set_text (GTK_LABEL(setup_page.status), "Changing WINE settings");
			linux_launcher.process_active = true;
			sa.sa_handler = &install_process_callback;
			sigaction (SIGCHLD, &sa, NULL);
		}
	}
	else if (linux_launcher.install_process_no == 4 && linux_launcher.process_active == false) {	// install_process_no: 4 for starting game installation from game disk
		char comm[8192];
		struct sigaction sa;
		
		memset (comm, 0, sizeof comm);
		memset (&sa, 0, sizeof sa);
		linux_launcher.cpid = -1;
		
		sprintf (comm, "cd engine ; export WINEVERPATH=\"`pwd`\" ; export PATH=\"$WINEVERPATH/bin:$PATH\" ; export WINESERVER=\"$WINEVERPATH/bin/wineserver\" ;  export WINELOADER=\"$WINEVERPATH/bin/wine\" ; export WINEDLLPATH=\"$WINEVERPATH/lib/wine/fakedlls\" ; export LD_LIBRARY_PATH=\"$WINEVERPATH/lib:$LD_LIBRARY_PATH\" ; export XDG_DATA_DIRS=\"$WINEVERPATH/share:$XDG_DATA_DIRS\" ; export WINEPREFIX=\"`pwd`/game\" ; export WINEARCH=\"win32\" ; wine m:\\\\%s ; while : ; do pgrep wineserver >/dev/null 2>&1 ; if [ \"$?\" -ne 1 ] ; then sleep 1 ; else break ; fi ; done", linux_launcher.game_install_executable);
		
		linux_launcher.cpid = fork ();
		
		if (linux_launcher.cpid == -1) {
			linux_launcher.install = false;
			return false;
		}
		else if (linux_launcher.cpid == 0) {
			int rval = system (comm);
			_exit (WEXITSTATUS(rval));
		}
		else {
			gtk_label_set_text (GTK_LABEL(setup_page.status), "Running installation");
			linux_launcher.process_active = true;
			sa.sa_handler = &install_process_callback;
			sigaction (SIGCHLD, &sa, NULL);
		}
	}
	else if (linux_launcher.install_process_no == 5 && linux_launcher.process_active == false) {	// install_process_no: 5 for changing WINE settings for game play
		char comm[8192];
		struct sigaction sa;
		
		memset (comm, 0, sizeof comm);
		memset (&sa, 0, sizeof sa);
		linux_launcher.cpid = -1;
		
		sprintf (comm, "cd engine ; export WINEVERPATH=\"`pwd`\" ; export PATH=\"$WINEVERPATH/bin:$PATH\" ; export WINESERVER=\"$WINEVERPATH/bin/wineserver\" ;  export WINELOADER=\"$WINEVERPATH/bin/wine\" ; export WINEDLLPATH=\"$WINEVERPATH/lib/wine/fakedlls\" ; export LD_LIBRARY_PATH=\"$WINEVERPATH/lib:$LD_LIBRARY_PATH\" ; export XDG_DATA_DIRS=\"$WINEVERPATH/share:$XDG_DATA_DIRS\" ; export WINEPREFIX=\"`pwd`/game\" ; export WINEARCH=\"win32\" ; regedit game.reg");
		
		linux_launcher.cpid = fork ();
		
		if (linux_launcher.cpid == -1) {
			linux_launcher.install = false;
			return false;
			
		}
		else if (linux_launcher.cpid == 0) {
			int rval = system (comm);
			_exit (WEXITSTATUS(rval));
		}
		else {
			gtk_label_set_text (GTK_LABEL(setup_page.status), "Applying post installation WINE settings");
			linux_launcher.process_active = true;
			sa.sa_handler = &install_process_callback;
			sigaction (SIGCHLD, &sa, NULL);
		}
	}
	else if (linux_launcher.install_process_no == 6 && linux_launcher.process_active == false) {	// install_process_no: 6 for creating a link to game folder
		char comm[8192];
		struct sigaction sa;
		
		memset (comm, 0, sizeof comm);
		memset (&sa, 0, sizeof sa);
		linux_launcher.cpid = -1;
		
		sprintf (comm, "ln -s \"%s\" game_folder", linux_launcher.game_executable_directory);
		
		linux_launcher.cpid = fork ();
		
		if (linux_launcher.cpid == -1) {
			linux_launcher.install = false;
			return false;
		}
		else if (linux_launcher.cpid == 0) {
			int rval = system (comm);
			_exit (WEXITSTATUS(rval));
		}
		else {
			gtk_label_set_text (GTK_LABEL(setup_page.status), "Linking game folder");
			linux_launcher.process_active = true;
			sa.sa_handler = &install_process_callback;
			sigaction (SIGCHLD, &sa, NULL);
		}
	}
	else if (linux_launcher.install_process_no == 7 && linux_launcher.process_active == false) {	// install_process_no: 7 for copying pe.exe to game folder
		char comm[8192];
		struct sigaction sa;
		
		memset (comm, 0, sizeof comm);
		memset (&sa, 0, sizeof sa);
		linux_launcher.cpid = -1;
		
		sprintf (comm, "cd engine ; export WINEVERPATH=\"`pwd`\" ; export PATH=\"$WINEVERPATH/bin:$PATH\" ; export WINESERVER=\"$WINEVERPATH/bin/wineserver\" ;  export WINELOADER=\"$WINEVERPATH/bin/wine\" ; export WINEDLLPATH=\"$WINEVERPATH/lib/wine/fakedlls\" ; export LD_LIBRARY_PATH=\"$WINEVERPATH/lib:$LD_LIBRARY_PATH\" ; export XDG_DATA_DIRS=\"$WINEVERPATH/share:$XDG_DATA_DIRS\" ; export WINEPREFIX=\"`pwd`/game\" ; export WINEARCH=\"win32\" ; wine SETUP.EXE");
		
		linux_launcher.cpid = fork ();
		
		if (linux_launcher.cpid == -1) {
			linux_launcher.install = false;
			return false;
		}
		else if (linux_launcher.cpid == 0) {
			int rval = system (comm);
			_exit (WEXITSTATUS(rval));
		}
		else {
			gtk_label_set_text (GTK_LABEL(setup_page.status), "Running game patch");
			linux_launcher.process_active = true;
			sa.sa_handler = &install_process_callback;
			sigaction (SIGCHLD, &sa, NULL);
		}
	}
	else if (linux_launcher.install_process_no == 8 && linux_launcher.process_active == false) {	// install_process_no: 8 for fixing game executable with pe.exe
		char comm[8192];
		struct sigaction sa;
		
		memset (comm, 0, sizeof comm);
		memset (&sa, 0, sizeof sa);
		linux_launcher.cpid = -1;
		
		sprintf (comm, "rm \"%s/gimme.dll\"", linux_launcher.game_executable_directory);
        
		linux_launcher.cpid = fork ();
		
		if (linux_launcher.cpid == -1) {
			linux_launcher.install = false;
			return false;
		}
		else if (linux_launcher.cpid == 0) {
			int rval = system (comm);
			_exit (WEXITSTATUS(rval));
		}
		else {
			gtk_label_set_text (GTK_LABEL(setup_page.status), "Removing buggy library");
			linux_launcher.process_active = true;
			sa.sa_handler = &install_process_callback;
			sigaction (SIGCHLD, &sa, NULL);
		}
	}
	return linux_launcher.install;
}

void install_process_callback (int signal_number)
{
	siginfo_t pstat;
	if (waitid (P_PID, linux_launcher.cpid, &pstat, WEXITED) != 0) return;
	
	if (pstat.si_code == CLD_EXITED) {
		if (linux_launcher.install_process_no == 0) {
			if (pstat.si_status == 0) {
				linux_launcher.install_process_no = 1;
				linux_launcher.process_active = false;
				gtk_label_set_text (GTK_LABEL(setup_page.status), "WINE Prefix created");
			}
			else {
				gtk_widget_hide (setup_page.busy_box);
				linux_launcher.install_process_no = -1;
				linux_launcher.install = false;
				linux_launcher.process_active = false;
				gtk_label_set_text (GTK_LABEL(setup_page.status), "Failed to create WINE Prefix");
				gtk_widget_set_sensitive (setup_page.back_button, TRUE);
				delete_game_installation ();
			}
		}
		else if (linux_launcher.install_process_no == 1) {
			if (pstat.si_status == 0) {
				linux_launcher.install_process_no = 2;
				linux_launcher.process_active = false;
				gtk_label_set_text (GTK_LABEL(setup_page.status), "Configured WINE documents");
			}
			else {
				gtk_widget_hide (setup_page.busy_box);
				linux_launcher.install_process_no = -1;
				linux_launcher.install = false;
				linux_launcher.process_active = false;
				gtk_label_set_text (GTK_LABEL(setup_page.status), "Failed to configure WINE documents");
				gtk_widget_set_sensitive (setup_page.back_button, TRUE);
				delete_game_installation ();
			}
		}
		else if (linux_launcher.install_process_no == 2) {
			if (pstat.si_status == 0) {
				linux_launcher.install_process_no = 3;
				linux_launcher.process_active = false;
				gtk_label_set_text (GTK_LABEL(setup_page.status), "WINE CD-ROM Configured");
			}
			else {
				gtk_widget_hide (setup_page.busy_box);
				linux_launcher.install_process_no = -1;
				linux_launcher.install = false;
				linux_launcher.process_active = false;
				gtk_label_set_text (GTK_LABEL(setup_page.status), "Failed to configure WINE CD-ROM");
				gtk_widget_set_sensitive (setup_page.back_button, TRUE);
				delete_game_installation ();
			}
		}
		else if (linux_launcher.install_process_no == 3) {
			if (pstat.si_status == 0) {
				linux_launcher.install_process_no = 4;
				linux_launcher.process_active = false;
				gtk_label_set_text (GTK_LABEL(setup_page.status), "Successfully changed WINE settings");
			}
			else {
				gtk_widget_hide (setup_page.busy_box);
				linux_launcher.install_process_no = -1;
				linux_launcher.install = false;
				linux_launcher.process_active = false;
				gtk_label_set_text (GTK_LABEL(setup_page.status), "Unable to change WINE settings");
				gtk_widget_set_sensitive (setup_page.back_button, TRUE);
				delete_game_installation ();
			}
		}
		else if (linux_launcher.install_process_no == 4) {
			if (pstat.si_status == 0) {
				linux_launcher.install_process_no = 5;
				linux_launcher.process_active = false;
				gtk_label_set_text (GTK_LABEL(setup_page.status), "Installation finished");
			}
			else {
				gtk_widget_hide (setup_page.busy_box);
				linux_launcher.install_process_no = -1;
				linux_launcher.install = false;
				linux_launcher.process_active = false;
				gtk_label_set_text (GTK_LABEL(setup_page.status), "Unable to finish installation");
				gtk_widget_set_sensitive (setup_page.back_button, TRUE);
				delete_game_installation ();
			}
		}
		else if (linux_launcher.install_process_no == 5) {
			if (pstat.si_status == 0) {
				linux_launcher.install_process_no = 6;
				linux_launcher.process_active = false;
				gtk_label_set_text (GTK_LABEL(setup_page.status), "Post installation WINE settings applied");
			}
			else {
				gtk_widget_hide (setup_page.busy_box);
				linux_launcher.install_process_no = -1;
				linux_launcher.install = false;
				linux_launcher.process_active = false;
				gtk_label_set_text (GTK_LABEL(setup_page.status), "Unable to apply post installation WINE settings");
				gtk_widget_set_sensitive (setup_page.back_button, TRUE);
				delete_game_installation ();
			}
		}
		else if (linux_launcher.install_process_no == 6) {
			if (pstat.si_status == 0) {
				linux_launcher.install_process_no = 7;
				linux_launcher.process_active = false;
				gtk_label_set_text (GTK_LABEL(setup_page.status), "Linked game folder");
			}
			else {
				gtk_widget_hide (setup_page.busy_box);
				linux_launcher.install_process_no = -1;
				linux_launcher.install = false;
				linux_launcher.process_active = false;
				gtk_label_set_text (GTK_LABEL(setup_page.status), "Unable to link game folder");
				gtk_widget_set_sensitive (setup_page.back_button, TRUE);
				delete_game_installation ();
			}
		}
		else if (linux_launcher.install_process_no == 7) {
			if (pstat.si_status == 0) {
				linux_launcher.install_process_no = 8;
				linux_launcher.process_active = false;
				gtk_label_set_text (GTK_LABEL(setup_page.status), "Game patched properly");
			}
			else {
				gtk_widget_hide (setup_page.busy_box);
				linux_launcher.install_process_no = -1;
				linux_launcher.install = false;
				linux_launcher.process_active = false;
				gtk_label_set_text (GTK_LABEL(setup_page.status), "Unable to patch game properly");
				gtk_widget_set_sensitive (setup_page.back_button, TRUE);
				delete_game_installation ();
			}
		}
		else if (linux_launcher.install_process_no == 8) {
			if (pstat.si_status == 0) {
				char *current_dir = g_get_current_dir ();
				linux_launcher.install_process_no = -1;
				linux_launcher.install = false;
				linux_launcher.process_active = false;
				switch (check_game_installed()) {
					case -1:
						fprintf (stderr, "LL: Could not check game installation\n");
						break;
					case 3:
						memset (linux_launcher.text_buffer, '\0', sizeof linux_launcher.text_buffer);
						sprintf (linux_launcher.text_buffer, "%s/game_folder/%s", current_dir, default_game_executable_macro);
						gtk_entry_set_text (GTK_ENTRY(game_option_page.exe_path_entry), linux_launcher.text_buffer);
						gtk_entry_set_text (GTK_ENTRY(game_option_page.exe_arg_entry), default_game_arguments_macro);
						memset (linux_launcher.text_buffer, '\0', sizeof linux_launcher.text_buffer);
						sprintf (linux_launcher.text_buffer, "%s is installed successfully", linux_launcher.game_title);
						gtk_label_set_text (GTK_LABEL(setup_page.status), linux_launcher.text_buffer);
						memset (icon_page.text_buffer, 0, sizeof icon_page.text_buffer);
						sprintf (icon_page.text_buffer, "%s is installed on your system.\nNow, you can start the game by clicking <b>Play</b>", linux_launcher.game_title);
						gtk_label_set_markup (GTK_LABEL(icon_page.status), icon_page.text_buffer);
						gtk_widget_set_sensitive (icon_page.setup_icon, FALSE);
						gtk_widget_set_sensitive (icon_page.play_icon, TRUE);
						gtk_widget_show (icon_page.option_button);
						break;
					case 1:
						delete_game_installation ();
						memset (linux_launcher.text_buffer, '\0', sizeof linux_launcher.text_buffer);
						sprintf (linux_launcher.text_buffer, "%s installation was unsuccessful", linux_launcher.game_title);
						gtk_label_set_text (GTK_LABEL(setup_page.status), linux_launcher.text_buffer);
						memset (icon_page.text_buffer, 0, sizeof icon_page.text_buffer);
						sprintf (icon_page.text_buffer, "You have not installed the game on your system yet.\nPlease click <b>Setup</b> to install %s", linux_launcher.game_title);
						gtk_label_set_markup (GTK_LABEL(icon_page.status), icon_page.text_buffer);
						gtk_widget_set_sensitive (icon_page.play_icon, FALSE);
						break;
					case 2:
						delete_game_installation ();
						memset (linux_launcher.text_buffer, '\0', sizeof linux_launcher.text_buffer);
						sprintf (linux_launcher.text_buffer, "%s installation was unsuccessful", linux_launcher.game_title);
						gtk_label_set_text (GTK_LABEL(setup_page.status), linux_launcher.text_buffer);
						memset (icon_page.text_buffer, 0, sizeof icon_page.text_buffer);
						sprintf (icon_page.text_buffer, "You have not installed the game on your system yet.\nPlease click <b>Setup</b> to install %s", linux_launcher.game_title);
						gtk_label_set_markup (GTK_LABEL(icon_page.status), icon_page.text_buffer);
						gtk_widget_set_sensitive (icon_page.play_icon, FALSE);
						break;
					default:
						break;
				}
				gtk_widget_hide (setup_page.busy_box);
				gtk_widget_set_sensitive (setup_page.back_button, TRUE);
			}
			else {
				linux_launcher.install_process_no = -1;
				linux_launcher.install = false;
				linux_launcher.process_active = false;
				delete_game_installation ();
				memset (linux_launcher.text_buffer, '\0', sizeof linux_launcher.text_buffer);
				sprintf (linux_launcher.text_buffer, "%s installation was unsuccessful", linux_launcher.game_title);
				gtk_label_set_text (GTK_LABEL(setup_page.status), linux_launcher.text_buffer);
				gtk_widget_hide (setup_page.busy_box);
				gtk_widget_set_sensitive (setup_page.back_button, TRUE);
			}
		}
	}
	else fprintf (stderr, "LL: Installation process is interrupted\n");
}

// checks wheather NFS II is already installed using this application
// return 1 if game directory does not exist
// return 2 if game executable does not exist
// return -1 if checking fails
// return 3 if both game directory and game executable exist
int check_game_installed (void)
{
	int prv = -1;
	FILE *fpipe;
	
	char comm[1024];
	char buffer[8];
	
	memset (comm, 0, sizeof comm);
	memset (buffer, 0, sizeof buffer);
	
	sprintf (comm, "if [ -d \"%s\" ] ; then echo \"1\" ; else echo \"0\" ; fi", linux_launcher.game_executable_directory);
	
	if (!(fpipe = (FILE*)popen (comm, "r"))) return -1;
	
	fgets (buffer, sizeof buffer, fpipe);
	
	prv = pclose (fpipe);
	
	if (WEXITSTATUS(prv) == 0) if (atoi (buffer) == 0) return 1;
	else if (WEXITSTATUS(prv) == -1) return -1;
	
	memset (comm, 0, sizeof comm);
	memset (buffer, 0, sizeof buffer);
	prv = -1;
	
	sprintf (comm, "if [ -f \"%s/%s\" ] ; then echo \"1\" ; else exit \"0\" ; fi", linux_launcher.game_executable_directory, linux_launcher.game_executable);
	
	if (!(fpipe = (FILE*)popen (comm, "r"))) return -1;
	
	fgets (buffer, sizeof buffer, fpipe);
	
	prv = pclose (fpipe);
	
	if (WEXITSTATUS(prv) == 0) if (atoi (buffer) == 0) return 2;
	else if (WEXITSTATUS(prv) == -1) return -1;
	
	// return 3 if game dir and game file both exist
	return 3;
}

int run_game (void)
{
	char comm[8192];
	struct sigaction sa;
	int srval = -1;
	
	if (strlen(gtk_entry_get_text(GTK_ENTRY(game_option_page.exe_path_entry))) != 0) {
		GFile *exe_gfile = g_file_new_for_path (gtk_entry_get_text(GTK_ENTRY(game_option_page.exe_path_entry)));
		GFile *exe_gfile_dir = g_file_get_parent (exe_gfile);
		char *exe_file_dir = g_file_get_path (exe_gfile_dir);
		char *exe_file_name = g_file_get_basename (exe_gfile);
		strcpy (linux_launcher.game_executable_directory, exe_file_dir);
		strcpy (linux_launcher.game_executable, exe_file_name);
		g_free (exe_file_dir);
		g_free (exe_file_name);
		g_object_unref (exe_gfile);
		g_object_unref (exe_gfile_dir);
	}
	if (strlen(gtk_entry_get_text(GTK_ENTRY(game_option_page.exe_arg_entry))) != 0) strcpy (linux_launcher.game_arguments, gtk_entry_get_text(GTK_ENTRY(game_option_page.exe_arg_entry)));
	
	memset (comm, 0, sizeof comm);
	
	sprintf (comm, "cd engine ; export WINEVERPATH=\"`pwd`\" ; export PATH=\"$WINEVERPATH/bin:$PATH\" ; export WINESERVER=\"$WINEVERPATH/bin/wineserver\" ;  export WINELOADER=\"$WINEVERPATH/bin/wine\" ; export WINEDLLPATH=\"$WINEVERPATH/lib/wine/fakedlls\" ; export LD_LIBRARY_PATH=\"$WINEVERPATH/lib:$LD_LIBRARY_PATH\" ; export XDG_DATA_DIRS=\"$WINEVERPATH/share:$XDG_DATA_DIRS\" ; export WINEPREFIX=\"`pwd`/game\" ; export WINEARCH=\"win32\" ; regedit settings.reg");
	
	srval = system (comm);
	if (WEXITSTATUS(srval) != 0) fprintf (stderr, "LL: Could not apply game options");
	
	memset (comm, 0, sizeof comm);
	memset (&sa, 0, sizeof sa);
	linux_launcher.cpid = -1;
	
	sprintf (comm, "cd engine ; export WINEVERPATH=\"`pwd`\" ; export PATH=\"$WINEVERPATH/bin:$PATH\" ; export WINESERVER=\"$WINEVERPATH/bin/wineserver\" ;  export WINELOADER=\"$WINEVERPATH/bin/wine\" ; export WINEDLLPATH=\"$WINEVERPATH/lib/wine/fakedlls\" ; export LD_LIBRARY_PATH=\"$WINEVERPATH/lib:$LD_LIBRARY_PATH\" ; export XDG_DATA_DIRS=\"$WINEVERPATH/share:$XDG_DATA_DIRS\" ; export WINEPREFIX=\"`pwd`/game\" ; export WINEARCH=\"win32\" ; cd .. ; cd \"%s\" ; schedtool -a 0x1 -e wine \"%s\" \"%s\"", linux_launcher.game_executable_directory, linux_launcher.game_executable, linux_launcher.game_arguments);
	
	linux_launcher.cpid = fork ();
	
	if (linux_launcher.cpid == -1) return -1;
	else if (linux_launcher.cpid == 0) {
		int rval = system (comm);
		_exit (WEXITSTATUS(rval));	
	}
	else {
		memset (linux_launcher.text_buffer, '\0', sizeof linux_launcher.text_buffer);
		sprintf (linux_launcher.text_buffer, "Trying to launch %s", linux_launcher.game_title);
		gtk_label_set_text (GTK_LABEL(icon_page.status), linux_launcher.text_buffer);
		gtk_widget_set_sensitive ((icon_page.play_icon), FALSE);
		sa.sa_handler = &run_process_callback;
		sigaction (SIGCHLD, &sa, NULL);
	}
	return 0;
}

void run_process_callback (int signal_number)
{
	siginfo_t pstat;
	if (waitid (P_PID, linux_launcher.cpid, &pstat, WEXITED) != 0) return;
	
	if (pstat.si_code == CLD_EXITED) {
		if (pstat.si_status == 0) {
			int srval = -1;
			char comm[8192];
			memset (comm, 0, sizeof comm);
			
			sprintf (comm, "cd engine ; export WINEVERPATH=\"`pwd`\" ; export PATH=\"$WINEVERPATH/bin:$PATH\" ; export WINESERVER=\"$WINEVERPATH/bin/wineserver\" ;  export WINELOADER=\"$WINEVERPATH/bin/wine\" ; export WINEDLLPATH=\"$WINEVERPATH/lib/wine/fakedlls\" ; export LD_LIBRARY_PATH=\"$WINEVERPATH/lib:$LD_LIBRARY_PATH\" ; export XDG_DATA_DIRS=\"$WINEVERPATH/share:$XDG_DATA_DIRS\" ; export WINEPREFIX=\"`pwd`/game\" ; export WINEARCH=\"win32\" ; wineserver -k");
			
			srval = system (comm);
			if (WEXITSTATUS(srval) != 0) fprintf (stderr, "LL: Could not shutdown WINE server");
			gtk_label_set_text (GTK_LABEL(icon_page.status), "Game closed without problems :)");
			gtk_widget_set_sensitive ((icon_page.play_icon), TRUE);
		}
		else {
			gtk_label_set_text (GTK_LABEL(icon_page.status), "Game crashed!");
			gtk_widget_set_sensitive ((icon_page.play_icon), TRUE);
		}
	}
	else fprintf (stderr, "LL: Running process is interrupted\n");
}

int delete_game_installation (void)
{
	char comm[8192];
	struct sigaction sa;
	
	memset (comm, 0, sizeof comm);
	memset (&sa, 0, sizeof sa);
	linux_launcher.cpid = -1;
	
	sprintf (comm, "rm -Rf game_save ; rm game_folder ; cd engine ; rm -Rf game");
	
	linux_launcher.cpid = fork ();
	
	if (linux_launcher.cpid == -1) return -1;
	else if (linux_launcher.cpid == 0) {
		int rval = system (comm);
		_exit (WEXITSTATUS(rval));
	}
	else {
		sa.sa_handler = &delete_process_callback;
		sigaction (SIGCHLD, &sa, NULL);
	}
	return 0;
}

void delete_process_callback (int signal_number)
{
	siginfo_t pstat;
	if (waitid (P_PID, linux_launcher.cpid, &pstat, WEXITED) != 0) return;
	
	if (pstat.si_code == CLD_EXITED) {
		if (pstat.si_status == 0) {
			fprintf (stderr, "LL: game folder cleaned\n");
			
		}
		else {
			fprintf (stderr, "LL: Unable to clean game folder\n");
		}
	}
	else fprintf (stderr, "LL: Deletion process is interrupted\n");
}

void save_settings (void)
{
	FILE *filep;
	
	filep = fopen ("settings.bin","wb");
	
	if (filep != NULL)
	{
		memset (&game_option_data, '\0', sizeof game_option_data);
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(game_option_page.windowed_check_box))) {
			game_option_data.windowed = 1;
			if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(game_option_page.grab_mouse_check_box))) game_option_data.mouse_lock = 1;
			else game_option_data.mouse_lock = 0;
		}
		else {
			game_option_data.windowed = 0;
			game_option_data.mouse_lock = 0;
		}
		if (strlen(gtk_entry_get_text(GTK_ENTRY(game_option_page.exe_path_entry))) != 0) strcpy (game_option_data.game_exe_path, gtk_entry_get_text(GTK_ENTRY(game_option_page.exe_path_entry)));
		else strcpy (game_option_data.game_exe_path, "");
		if (strlen(gtk_entry_get_text(GTK_ENTRY(game_option_page.exe_arg_entry))) != 0) strcpy (game_option_data.game_exe_arg, gtk_entry_get_text(GTK_ENTRY(game_option_page.exe_arg_entry)));
		else strcpy (game_option_data.game_exe_arg, "");
		if (fwrite (&game_option_data, sizeof game_option_data, 1, filep) != 1) fprintf (stderr, "LL: Could not write to settings.bin file\n");
		fclose (filep);
	}
	else fprintf (stderr, "LL: Could not write settings.bin file\n");
}

void load_settings (void)
{
	FILE *filep;
	
	filep = fopen ("settings.bin","rb");
	
	memset (&game_option_data, '\0', sizeof game_option_data);
	
	if (filep != NULL)
	{
		if (fread (&game_option_data, sizeof game_option_data, 1, filep) != 1) fprintf (stderr, "LL: Could not read from settings.bin file\n");
		else {
			if (game_option_data.windowed == 1) {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(game_option_page.windowed_check_box), TRUE);
				if (game_option_data.mouse_lock == 1) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(game_option_page.grab_mouse_check_box), TRUE);
				else gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(game_option_page.grab_mouse_check_box), FALSE);
			}
			else {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(game_option_page.windowed_check_box), FALSE);
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(game_option_page.grab_mouse_check_box), TRUE);
			}
			if (strlen (game_option_data.game_exe_path) != 0) gtk_entry_set_text (GTK_ENTRY(game_option_page.exe_path_entry), game_option_data.game_exe_path);
			if (strlen (game_option_data.game_exe_arg) != 0) gtk_entry_set_text (GTK_ENTRY(game_option_page.exe_arg_entry), game_option_data.game_exe_arg);
		}
		fclose (filep);
	}
	else fprintf (stderr, "LL: Could not read settings.bin file\n");
}