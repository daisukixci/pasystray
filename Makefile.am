#  This file is part of PaSystray
#
#  Copyright (C) 2011-2016  Christoph Gysin
#
#  PaSystray is free software; you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as
#  published by the Free Software Foundation; either version 2.1 of the
#  License, or (at your option) any later version.
#
#  PaSystray is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#  General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with PaSystray; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
#  USA.

EXTRA_DIST = \
    LICENSE \
    README.md \
    bootstrap.sh

iconsdir =  $(datadir)/icons/hicolor/scalable/apps
icons_DATA = data/pasystray.svg
EXTRA_DIST += $(icons_DATA)

pixmapsdir = $(datadir)/pixmaps
pixmaps_DATA = data/pasystray.png
EXTRA_DIST += $(pixmaps_DATA)

desktopdir = $(datadir)/applications
desktop_DATA = data/pasystray.desktop
EXTRA_DIST += $(desktop_DATA)

xdgautostartdir = $(sysconfdir)/xdg/autostart
xdgautostart_DATA = data/pasystray.desktop
EXTRA_DIST += $(xdgautostart_DATA)

man_MANS = man/pasystray.1
EXTRA_DIST += $(man_MANS)

SUBDIRS = src

doc:
	markdown_py README.md > README.html

test:
	@echo "no tests yet"
