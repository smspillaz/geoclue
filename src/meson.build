sources = []

# Use gnome.gbus_codegen once we can generate docs with it:
# https://github.com/mesonbuild/meson/issues/2066
dbus_codegen = find_program('gdbus-codegen')
codegen_cmd = [dbus_codegen,
               '--interface-prefix', 'org.freedesktop.GeoClue2.',
               '--c-namespace', 'GClueDBus',
               '--generate-docbook', '@OUTDIR@/docs',
               '@INPUT@']

# Client interface
cmd = codegen_cmd + ['--generate-c-code', '@OUTDIR@/gclue-client-interface']
sources += [ custom_target('gclue-client-interface',
                                 input: 'org.freedesktop.GeoClue2.Client.xml',
                                 output: [ 'gclue-client-interface.c',
                                           'gclue-client-interface.h' ],
                                 command: cmd) ]

# Location interface
cmd = codegen_cmd + ['--generate-c-code', '@OUTDIR@/gclue-location-interface']
sources += [ custom_target('gclue-location-interface',
                                 input: 'org.freedesktop.GeoClue2.Location.xml',
                                 output: [ 'gclue-location-interface.c',
                                           'gclue-location-interface.h' ],
                                 command: cmd) ]

# Manager interface
cmd = codegen_cmd + ['--generate-c-code', '@OUTDIR@/gclue-manager-interface']
sources += [ custom_target('gclue-manager-interface',
                                 input: 'org.freedesktop.GeoClue2.Manager.xml',
                                 output: [ 'gclue-manager-interface.c',
                                           'gclue-manager-interface.h' ],
                                 command: cmd) ]

# Can't use gnome.gbus_codegen here either because of:
# https://github.com/mesonbuild/meson/issues/2123
cmd = [dbus_codegen,
       '--interface-prefix', 'fi.w1.wpa_supplicant1.',
       '--annotate', 'fi.w1.wpa_supplicant1',
       'org.gtk.GDBus.C.Name', 'WPA_Supplicant',
       '--annotate', 'fi.w1.wpa_supplicant1.Interface',
       'org.gtk.GDBus.C.Name', 'WPA_Interface',
       '--annotate', 'fi.w1.wpa_supplicant1.BSS',
       'org.gtk.GDBus.C.Name', 'WPA_BSS',
       '--annotate', 'fi.w1.wpa_supplicant1.BSS:SSID',
       'org.gtk.GDBus.C.ForceGVariant', 'whatever',
       '--annotate', 'fi.w1.wpa_supplicant1.BSS:BSSID',
       'org.gtk.GDBus.C.ForceGVariant', 'whatever',
       '--annotate', 'fi.w1.wpa_supplicant1.Interface::BSSAdded',
       'org.gtk.GDBus.C.Name', 'BSS_Added',
       '--annotate', 'fi.w1.wpa_supplicant1.Interface::BSSRemoved',
       'org.gtk.GDBus.C.Name', 'BSS_Removed',
       '--generate-c-code', '@OUTDIR@/wpa_supplicant-interface',
       '@INPUT@']

sources += [ custom_target('wpa_supplicant-interface',
                                 input: 'fi.w1.wpa_supplicant1.xml',
                                 output: [ 'wpa_supplicant-interface.c',
                                           'wpa_supplicant-interface.h' ],
                                 command: cmd) ]

# For external interfaces we don't need to generate docs so we can make use of
# gnome.gbus_codegen.
sources += gnome.gdbus_codegen('compass-interface',
                               'net.hadess.SensorProxy.xml',
                               interface_prefix: 'net.hadess.SensorProxy')

sources += gnome.genmarshal('gclue-marshal',
                            prefix: 'gclue_marshal',
                            sources: ['gclue-marshal.list'])

include_dirs = include_directories('..', '../agent', '../public-api')

sources += [ 'gclue-main.c',
             'gclue-3g-tower.h',
             'gclue-client-info.h', 'gclue-client-info.c',
             'gclue-compass.h', 'gclue-compass.c',
             'gclue-config.h', 'gclue-config.c',
             'gclue-error.h', 'gclue-error.c',
             'gclue-location-source.h', 'gclue-location-source.c',
             'gclue-locator.h', 'gclue-locator.c',
             'gclue-service-manager.h', 'gclue-service-manager.c',
             'gclue-service-client.h', 'gclue-service-client.c',
             'gclue-service-location.h', 'gclue-service-location.c',
             'gclue-web-source.c', 'gclue-web-source.h',
             'gclue-wifi.h', 'gclue-wifi.c',
             'gclue-mozilla.h', 'gclue-mozilla.c',
             'gclue-location.h', 'gclue-location.c' ]

if get_option('enable-3g-source') or
   get_option('enable-cdma-source') or
   get_option('enable-modem-gps-source')
    sources += [ 'gclue-modem.c',
				 'gclue-modem.h',
				 'gclue-modem-manager.c',
				 'gclue-modem-manager.h' ]
endif

if get_option('enable-3g-source')
    sources += [ 'gclue-3g.c', 'gclue-3g.h' ]
endif

if get_option('enable-cdma-source')
    sources += [ 'gclue-cdma.c', 'gclue-cdma.h' ]
endif

if get_option('enable-modem-gps-source')
    sources += [ 'gclue-modem-gps.c', 'gclue-modem-gps.h' ]
endif

if get_option('enable-nmea-source')
    sources += [ 'gclue-nmea-source.h', 'gclue-nmea-source.c' ]
endif


executable('geoclue', sources, include_directories: include_dirs)


