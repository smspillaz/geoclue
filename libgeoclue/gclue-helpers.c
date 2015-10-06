/* vim: set et ts=8 sw=8: */
/*
 * Geoclue convenience library.
 *
 * Copyright (C) 2015 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Author: Zeeshan Ali (Khattak) <zeeshanak@gnome.org>
 */

#include "gclue-helpers.h"

#define BUS_NAME "org.freedesktop.GeoClue2"
#define MANAGER_PATH "/org/freedesktop/GeoClue2/Manager"

/**
 * SECTION: gclue-helpers
 * @title: Geoclue convenience API
 * @short_description: Geoclue convenience API
 *
 * TODO
 */

typedef struct {
        char              *desktop_id;
        GClueAccuracyLevel accuracy_level;

        gulong notify_id;
} ClientCreateData;

static ClientCreateData *
client_create_data_new (const char        *desktop_id,
                        GClueAccuracyLevel accuracy_level)
{
        ClientCreateData *data = g_slice_new0 (ClientCreateData);

        data->desktop_id = g_strdup (desktop_id);
        data->accuracy_level = accuracy_level;

        return data;
}

static void
client_create_data_free (ClientCreateData *data)
{
        g_free (data->desktop_id);
        g_slice_free (ClientCreateData, data);
}

static void
on_client_proxy_ready (GObject      *source_object,
                       GAsyncResult *res,
                       gpointer      user_data)
{
        GTask *task = G_TASK (user_data);
        ClientCreateData *data;
        GClueClient *client;
        GError *error = NULL;

        client = gclue_client_proxy_new_for_bus_finish (res, &error);
        if (error != NULL) {
                g_task_return_error (task, error);
                g_object_unref (task);

                return;
        }

        data = g_task_get_task_data (task);
        gclue_client_set_desktop_id (client, data->desktop_id);
        gclue_client_set_requested_accuracy_level (client, data->accuracy_level);

        g_task_return_pointer (task, client, g_object_unref);
        g_object_unref (task);
}

static void
on_get_client_ready (GObject      *source_object,
                     GAsyncResult *res,
                     gpointer      user_data)
{
        GTask *task = G_TASK (user_data);
        GClueManager *manager = GCLUE_MANAGER (source_object);
        char *client_path;
        GError *error = NULL;

        if (!gclue_manager_call_get_client_finish (manager,
                                                   &client_path,
                                                   res,
                                                   &error)) {
                g_task_return_error (task, error);
                g_object_unref (task);
                g_object_unref (manager);

                return;
        }

        gclue_client_proxy_new_for_bus (G_BUS_TYPE_SYSTEM,
                                        G_DBUS_PROXY_FLAGS_NONE,
                                        BUS_NAME,
                                        client_path,
                                        g_task_get_cancellable (task),
                                        on_client_proxy_ready,
                                        task);
        g_free (client_path);
        g_object_unref (manager);
}

static void
on_manager_proxy_ready (GObject      *source_object,
                        GAsyncResult *res,
                        gpointer      user_data)
{
        GTask *task = G_TASK (user_data);
        GClueManager *manager;
        GError *error = NULL;

        manager = gclue_manager_proxy_new_finish (res, &error);
        if (error != NULL) {
                g_task_return_error (task, error);
                g_object_unref (task);

                return;
        }

        gclue_manager_call_get_client (manager,
                                       g_task_get_cancellable (task),
                                       on_get_client_ready,
                                       task);
}

/**
 * gclue_client_proxy_create:
 * @desktop_id: The desktop file id (the basename of the desktop file).
 * @accuracy_level: The requested accuracy level as #GClueAccuracyLevel.
 * @cancellable: (allow-none): A #GCancellable or %NULL.
 * @callback: A #GAsyncReadyCallback to call when the results are ready.
 * @user_data: User data to pass to @callback.
 *
 * A utility function to create a #GClueClientProxy without having to deal with
 * a #GClueManager.
 *
 * See #gclue_client_proxy_create_sync() for the synchronous, blocking version
 * of this function.
 */
void
gclue_client_proxy_create (const char         *desktop_id,
                           GClueAccuracyLevel  accuracy_level,
                           GCancellable       *cancellable,
                           GAsyncReadyCallback callback,
                           gpointer            user_data)
{
        GTask *task;
        ClientCreateData *data;

        task = g_task_new (NULL, cancellable, callback, user_data);

        data = client_create_data_new (desktop_id, accuracy_level);
        g_task_set_task_data (task,
                              data,
                              (GDestroyNotify) client_create_data_free);

        gclue_manager_proxy_new_for_bus (G_BUS_TYPE_SYSTEM,
                                         G_DBUS_PROXY_FLAGS_NONE,
                                         BUS_NAME,
                                         MANAGER_PATH,
                                         cancellable,
                                         on_manager_proxy_ready,
                                         task);
}

/**
 * gclue_client_proxy_create_finish:
 * @result: The #GAsyncResult obtained from the #GAsyncReadyCallback passed to
 *          gclue_client_proxy_create().
 * @error: Return location for error or %NULL.
 *
 * Finishes an operation started with gclue_client_proxy_create().
 *
 * Returns: (transfer full) (type GClueClientProxy): The constructed proxy
 * object or %NULL if @error is set.
 */
GClueClient *
gclue_client_proxy_create_finish (GAsyncResult *result,
                                  GError      **error)
{
        g_return_val_if_fail (g_task_is_valid (result, NULL), NULL);

        return g_task_propagate_pointer (G_TASK (result), error);
}

typedef struct {
        GClueClient *client;
        GError     **error;

        GMainLoop *main_loop;
} ClientCreateSyncData;

static void
on_client_proxy_created (GObject      *source_object,
                         GAsyncResult *res,
                         gpointer      user_data)
{
        ClientCreateSyncData *data = (ClientCreateSyncData *) user_data;

        data->client = gclue_client_proxy_create_finish (res, data->error);

        g_main_loop_quit (data->main_loop);
}

/**
 * gclue_client_proxy_create_sync:
 * @desktop_id: The desktop file id (the basename of the desktop file).
 * @accuracy_level: The requested accuracy level as #GClueAccuracyLevel.
 * @cancellable: (allow-none): A #GCancellable or %NULL.
 * @error: Return location for error or %NULL.
 *
 * The synchronous and blocking version of #gclue_client_proxy_create().
 *
 * Returns: (transfer full) (type GClueClientProxy): The constructed proxy
 * object or %NULL if @error is set.
 */
GClueClient *
gclue_client_proxy_create_sync (const char        *desktop_id,
                                GClueAccuracyLevel accuracy_level,
                                GCancellable      *cancellable,
                                GError           **error)
{
        GClueClient *client;
        ClientCreateSyncData *data = g_slice_new0 (ClientCreateSyncData);

        data->error = error;
        data->main_loop = g_main_loop_new (NULL, FALSE);
        gclue_client_proxy_create (desktop_id,
                                   accuracy_level,
                                   cancellable,
                                   on_client_proxy_created,
                                   data);

        g_main_loop_run (data->main_loop);
        g_main_loop_unref (data->main_loop);

        client = data->client;
        g_slice_free (ClientCreateSyncData, data);

        return client;
}

typedef struct {
        GClueClient *client;

        gulong notify_id;
} FetchLocationData;

static void
fetch_location_data_free (FetchLocationData *data)
{
        g_clear_object (&data->client);
        g_slice_free (FetchLocationData, data);
}

static void
on_location_proxy_ready (GObject      *source_object,
                         GAsyncResult *res,
                         gpointer      user_data)
{
        GClueLocation *location;
        GTask *task = G_TASK (user_data);
        GError *error = NULL;

        location = gclue_location_proxy_new_for_bus_finish (res, &error);
        if (error != NULL) {
                g_task_return_error (task, error);
                g_object_unref (task);

                return;
        }

        g_task_return_pointer (task, location, g_object_unref);
        g_object_unref (task);
}

static void
on_location_updated (GClueClient *client,
                     const char  *old_location,
                     const char  *new_location,
                     gpointer     user_data)
{
        GTask *task = G_TASK (user_data);
        FetchLocationData *data;

        if (new_location == NULL || g_strcmp0 (new_location, "/") == 0)
                return;

        data = g_task_get_task_data (task);
        g_signal_handler_disconnect (client, data->notify_id);

        gclue_location_proxy_new_for_bus (G_BUS_TYPE_SYSTEM,
                                          G_DBUS_PROXY_FLAGS_NONE,
                                          BUS_NAME,
                                          new_location,
                                          g_task_get_cancellable (task),
                                          on_location_proxy_ready,
                                          task);
}

static void
on_client_started (GObject      *source_object,
                   GAsyncResult *res,
                   gpointer      user_data)
{
        GTask *task = G_TASK (user_data);
        GClueClient *client = GCLUE_CLIENT (source_object);
        GError *error = NULL;

        gclue_client_call_start_finish (client, res, &error);
        if (error != NULL) {
                g_task_return_error (task, error);
                g_object_unref (task);
        }
}

static void
on_client_created (GObject      *source_object,
                   GAsyncResult *res,
                   gpointer      user_data)
{
        GTask *task = G_TASK (user_data);
        FetchLocationData *data;
        GClueClient *client;
        GError *error = NULL;

        client = gclue_client_proxy_create_finish (res, &error);
        if (error != NULL) {
                g_task_return_error (task, error);
                g_object_unref (task);

                return;
        }

        data = g_task_get_task_data (task);

        data->client = client; /* Take ref */
        data->notify_id =
                g_signal_connect (client,
                                  "location-updated",
                                  G_CALLBACK (on_location_updated),
                                  task);

        gclue_client_call_start (client,
                                 g_task_get_cancellable (task),
                                 on_client_started,
                                 task);
}

/**
 * gclue_fetch_location:
 * @desktop_id: The desktop file id (the basename of the desktop file).
 * @accuracy_level: The requested accuracy level as #GClueAccuracyLevel.
 * @cancellable: (allow-none): A #GCancellable or %NULL.
 * @callback: A #GAsyncReadyCallback to call when the results are ready.
 * @user_data: User data to pass to @callback.
 *
 * A utility function that takes care of the typical steps of creating a
 * #GClueClientProxy instance, starting it, waiting till we have a location fix
 * and then creating a #GClueLocation instance for it. Use
 * #gclue_fetch_location_finish() to get the #GClueLocation and (optionally) the
 * #GClueClient instances created.
 *
 * While most applications will find this function very useful, it is  most
 * useful for applications that simply want to get the current location as
 * quickly as possible and do not care about accuracy (much).
 *
 * See #gclue_fetch_location_sync() for the synchronous, blocking version
 * of this function.
 */
void
gclue_fetch_location (const char         *desktop_id,
                      GClueAccuracyLevel  accuracy_level,
                      GCancellable       *cancellable,
                      GAsyncReadyCallback callback,
                      gpointer            user_data)
{
        GTask *task;
        FetchLocationData *data;

        task = g_task_new (NULL,
                           cancellable,
                           callback,
                           user_data);
        data = g_slice_new0 (FetchLocationData);
        g_task_set_task_data (task,
                              data,
                              (GDestroyNotify) fetch_location_data_free);

        gclue_client_proxy_create (desktop_id,
                                   accuracy_level,
                                   cancellable,
                                   on_client_created,
                                   task);
}

/**
 * gclue_fetch_location_finish:
 * @client: (transfer full) (allow-none): Return location for #GClueClient or
 *          %NULL.
 * @result: The #GAsyncResult obtained from the #GAsyncReadyCallback passed to
 *          gclue_client_proxy_create().
 * @error: Return location for error or %NULL.
 *
 * Finishes an operation started with gclue_fetch_location(). Use @client
 * parameter to get created and started client. You can then connect to either
 * #GClueClient::location-updated signal or notify signal for
 * #GClueClient:location property on the provided client instance to be notified
 * of location updates.
 *
 * Returns: (transfer full) (type GClueLocation): The fetched location object
 * or %NULL if @error is set.
 */
GClueLocation *
gclue_fetch_location_finish (GClueClient  **client,
                             GAsyncResult  *result,
                             GError       **error)
{
        GTask *task;
        FetchLocationData *data;

        g_return_val_if_fail (g_task_is_valid (result, NULL), NULL);

        task = G_TASK (result);
        data = g_task_get_task_data (task);

        if (client != NULL)
                *client = g_object_ref (data->client);

        return g_task_propagate_pointer (task, error);
}

typedef struct {
        GClueLocation *location;
        GClueClient  **client;
        GError       **error;

        GMainLoop *main_loop;
} FetchLocationSyncData;

static void
on_fetch_location (GObject      *source_object,
                   GAsyncResult *res,
                   gpointer      user_data)
{
        FetchLocationSyncData *data = (FetchLocationSyncData *) user_data;

        data->location = gclue_fetch_location_finish (data->client,
                                                      res,
                                                      data->error);

        g_main_loop_quit (data->main_loop);
}

/**
 * gclue_fetch_location_sync:
 * @desktop_id: The desktop file id (the basename of the desktop file).
 * @accuracy_level: The requested accuracy level as #GClueAccuracyLevel.
 * @client: (transfer full) (allow-none): Return location for #GClueClient or
 *          %NULL.
 * @cancellable: (allow-none): A #GCancellable or %NULL.
 * @error: Return location for error or %NULL.
 *
 * The synchronous and blocking version of #gclue_fetch_location().
 *
 * Returns: (transfer full) (type GClueLocation): The fetched location object
 * or %NULL if @error is set.
 */
GClueLocation *
gclue_fetch_location_sync (const char        *desktop_id,
                           GClueAccuracyLevel accuracy_level,
                           GClueClient      **client,
                           GCancellable      *cancellable,
                           GError           **error)
{
        GClueLocation *location;
        FetchLocationSyncData *data = g_slice_new0 (FetchLocationSyncData);

        data->client = client;
        data->error = error;
        data->main_loop = g_main_loop_new (NULL, FALSE);
        gclue_fetch_location (desktop_id,
                              accuracy_level,
                              cancellable,
                              on_fetch_location,
                              data);

        g_main_loop_run (data->main_loop);
        g_main_loop_unref (data->main_loop);

        location = data->location;
        g_slice_free (FetchLocationSyncData, data);

        return location;
}
