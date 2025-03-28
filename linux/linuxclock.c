#include <gtk/gtk.h>
#include <time.h>

GtkWidget *label;
gboolean title_visible = TRUE;

gboolean update_time(gpointer user_data)
{
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%I:%M:%S %p", timeinfo);

    gtk_label_set_text(GTK_LABEL(label), buffer);
    return TRUE;
}

gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    GtkWindow *window = GTK_WINDOW(widget);

    if (event->keyval == GDK_KEY_f || event->keyval == GDK_KEY_Return)
    {
        gtk_window_set_decorated(window, !title_visible);
        title_visible = !title_visible;
    }
    else if (event->keyval == GDK_KEY_Escape)
    {
        gtk_window_set_decorated(window, TRUE);
        title_visible = TRUE;
    }

    return FALSE;
}

void apply_dark_css(GtkWidget *widget)
{
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "* {"
        "  background-color: black;"
        "  color: white;"
        "  font: bold 48px 'Arial';"
        "}", -1, NULL);

    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_object_unref(provider);
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Digital Clock");
    gtk_window_set_default_size(GTK_WINDOW(window), 480, 120);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 0);

    label = gtk_label_new("");
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);

    gtk_container_add(GTK_CONTAINER(window), label);

    apply_dark_css(window);
    apply_dark_css(label);

    g_timeout_add_seconds(1, update_time, NULL);
    update_time(NULL);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
