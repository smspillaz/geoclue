include $(top_srcdir)/Makefile.decl

CLIENT_INTERFACE_XML = $(top_srcdir)/src/org.freedesktop.GeoClue2.Client.xml
LOCATION_INTERFACE_XML = $(top_srcdir)/src/org.freedesktop.GeoClue2.Location.xml
MANAGER_INTERFACE_XML = $(top_srcdir)/src/org.freedesktop.GeoClue2.Manager.xml

dbus_built_sources = gclue-client.c   \
		     gclue-client.h   \
		     gclue-location.c \
		     gclue-location.h \
		     gclue-manager.c  \
		     gclue-manager.h

gclue-client.c: gclue-client.h
gclue-client.h: Makefile.am $(CLIENT_INTERFACE_XML)
	$(AM_V_GEN)$(GDBUS_CODEGEN) \
		--interface-prefix org.freedesktop.GeoClue2.	\
		--c-namespace GClue				\
		--generate-c-code gclue-client			\
		$(CLIENT_INTERFACE_XML)

gclue-location.c: gclue-location.h
gclue-location.h: Makefile.am $(LOCATION_INTERFACE_XML)
	$(AM_V_GEN)$(GDBUS_CODEGEN) \
		--interface-prefix org.freedesktop.GeoClue2.	\
		--c-namespace GClue				\
		--generate-c-code gclue-location		\
		$(LOCATION_INTERFACE_XML)

gclue-manager.c: gclue-manager.h
gclue-manager.h: Makefile.am $(MANAGER_INTERFACE_XML)
	$(AM_V_GEN)$(GDBUS_CODEGEN) \
		--interface-prefix org.freedesktop.GeoClue2.	\
		--c-namespace GClue				\
		--generate-c-code gclue-manager			\
		$(MANAGER_INTERFACE_XML)

BUILT_SOURCES = 		\
	$(dbus_built_sources)	\
	$(NULL)

lib_LTLIBRARIES = libgeoclue-2.la

geoclue_includedir = $(includedir)/libgeoclue-$(GEOCLUE_API_VERSION)
geoclue_include_HEADERS = geoclue.h          \
			  gclue-enums.h      \
			  gclue-enum-types.h \
			  gclue-client.h     \
			  gclue-location.h   \
			  gclue-manager.h    \
			  $(NULL)

AM_CPPFLAGS = $(LIBGEOCLUE_CFLAGS) 		     	  \
	      $(WARN_CFLAGS)			     	  \
	      -DLOCALEDIR="\"$(datadir)/locale\"" 	  \
	      -DG_LOG_DOMAIN=\""Geoclue"\"	     	  \
	      -DABS_TOP_SRCDIR=\""$(abs_top_srcdir)"\" 	  \
	      -DSYSCONFDIR=\""$(sysconfdir)"\"

libgeoclue_2_la_SOURCES =  \
	gclue-client.c 	   \
	gclue-location.c   \
	gclue-manager.c    \
	gclue-enum-types.c \
	$(NULL)

libgeoclue_2_la_LIBADD =    \
	$(LIBGEOCLUE_LIBS)  \
	$(LIBS)		    \
	$(NULL)

CLEANFILES = $(BUILT_SOURCES) \
	     $(NULL)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgeoclue-$(GEOCLUE_API_VERSION).pc

DISTCLEANFILES = $(pkgconfig_DATA)

-include $(top_srcdir)/git.mk
