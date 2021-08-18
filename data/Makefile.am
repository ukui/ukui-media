NULL =

SUBDIRS =	\
	icons	\
	$(NULL)

@INTLTOOL_DESKTOP_RULE@
autostartdir = $(sysconfdir)/xdg/autostart
autostart_in_files = ukui-volume-control-applet.desktop.in
autostart_DATA = $(autostart_in_files:.desktop.in=.desktop)

appsdir = $(datadir)/applications
apps_in_files = ukui-volume-control.desktop.in
apps_DATA = $(apps_in_files:.desktop.in=.desktop)

systemddir = /lib/systemd/system
systemd_DATA = ukui-media-control-mute-led.service

gsettingsschema_in_files = org.ukui.media.gschema.xml.in
gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)
@GSETTINGS_RULES@
@INTLTOOL_XML_RULE@

EXTRA_DIST =					\
	$(autostart_in_files)			\
	ukui-volume-control.desktop.in.in	\
	$(gsettingsschema_in_files)		\
	$(NULL)

CLEANFILES =					\
	ukui-volume-control.desktop		\
	$(gsettings_SCHEMAS)			\
	$(NULL)

DISTCLEANFILES =				\
	ukui-volume-control-applet.desktop	\
	$(NULL)

MAINTAINERCLEANFILES =			\
	Makefile.in			\
	$(gsettings_SCHEMAS:.xml=.valid)

-include $(top_srcdir)/git.mk
