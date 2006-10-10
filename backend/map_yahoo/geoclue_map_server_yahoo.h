/* Geomap - A DBus api and wrapper for getting geography pictures
 * Copyright (C) 2006 Garmin
 * 
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2 as published by the Free Software Foundation; 
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#ifndef __GEOCLUE_MAP_SERVER_H__
#define __GEOCLUE_MAP_SERVER_H__

#define DBUS_API_SUBJECT_TO_CHANGE


#include <dbus/dbus-glib.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#define GEOCLUE_MAP_DBUS_SERVICE     "org.foinse_project.geoclue.map.yahoo"
#define GEOCLUE_MAP_DBUS_PATH        "/org/foinse_project/geoclue/map/yahoo"
#define GEOCLUE_MAP_DBUS_INTERFACE   "org.foinse_project.geoclue.map"





G_BEGIN_DECLS

//Let's create a geoclue_map object that has one method of geoclue_map
typedef struct GeoclueMap GeoclueMap;
typedef struct GeoclueMapClass GeoclueMapClass;

GType geoclue_map_get_type (void);
struct GeoclueMap
{
    GObject parent;
   
    char* buffer;
    GArray* OUT_map_buffer;
    gint width;
    gint height;
    
    
    gboolean pending_request;
     
};

struct GeoclueMapClass
{
  GObjectClass parent;
  DBusGConnection *connection;
  

  
          /* Signals */
    void (*get_map_finished) (GeoclueMap*, gint, GArray* , gchar* );
};

#define TYPE_GEOCLUE_MAP              (geoclue_map_get_type ())
#define GEOCLUE_MAP(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), TYPE_GEOCLUE_MAP, GeoclueMap))
#define GEOCLUE_MAP_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_GEOCLUE_MAP, GeoclueMapClass))
#define IS_GEOCLUE_MAP(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), TYPE_GEOCLUE_MAP))
#define IS_GEOCLUE_MAP_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_GEOCLUE_MAP))
#define GEOCLUE_MAP_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_GEOCLUE_MAP, GeoclueMapClass))

gboolean geoclue_map_version (GeoclueMap *obj, gint* OUT_major, gint* OUT_minor, gint* OUT_micro, GError **error);
gboolean geoclue_map_service_provider(GeoclueMap *obj, char** name, GError **error);   
gboolean geoclue_map_max_zoom(GeoclueMap *obj, int* max_zoom, GError **error);
gboolean geoclue_map_min_zoom(GeoclueMap *obj, int* min_zoom, GError **error);
gboolean geoclue_map_max_height(GeoclueMap *obj, int* max_height, GError **error);
gboolean geoclue_map_min_height(GeoclueMap *obj, int* min_height, GError **error);
gboolean geoclue_map_max_width(GeoclueMap *obj, int* max_width, GError **error);
gboolean geoclue_map_min_width(GeoclueMap *obj, int* min_width, GError **error);
gboolean geoclue_map_get_map (GeoclueMap *obj, const gdouble IN_latitude, const gdouble IN_longitude, const gint IN_width, const gint IN_height, const gint IN_zoom, int* return_code, GError **error);
gboolean geoclue_map_latlong_to_offset(GeoclueMap *obj, const gdouble IN_latitude, const gdouble IN_longitude,  const gint IN_zoom, const gdouble IN_center_latitude, const gdouble IN_center_longitude, int* OUT_x_offset, int* OUT_y_offset, GError **error);
gboolean geoclue_map_offset_to_latlong(GeoclueMap *obj, const int IN_x_offset,const int IN_y_offset, const gint IN_zoom, const gdouble IN_center_latitude, const gdouble IN_center_longitude, gdouble* OUT_latitude, gdouble* OUT_longitude,  GError **error );
gboolean geoclue_map_find_zoom_level (GeoclueMap *obj, const gdouble IN_latitude_top_left, const gdouble IN_longitude_top_left, const gdouble IN_latitude_bottom_right, const gdouble IN_longitude_bottom_right, const gint IN_width, const gint IN_height,  gint* OUT_zoom, GError** error);




G_END_DECLS




#endif
