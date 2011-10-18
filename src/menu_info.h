/***
  This file is part of PaSystray

  Copyright 2011 Christoph Gysin

  PaSystray is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) any later version.

  PaSystray is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with PaSystray; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA.
***/

#ifndef PASYSTRAY_MENU_INFO_H
#define PASYSTRAY_MENU_INFO_H

#include <gtk/gtk.h>
#include <stdint.h>

typedef enum {
    MENU_SERVER = 0,
    MENU_SINK = 1,
    MENU_SOURCE = 2,
    MENU_INPUT = 3,
    MENU_OUTPUT = 4,
} menu_type_t;
#define MENU_COUNT 5

typedef struct menu_infos_t_ menu_infos_t;
typedef struct menu_info_t_ menu_info_t;
typedef struct menu_info_item_t_ menu_info_item_t;

struct menu_info_t_ {
    menu_type_t type;
    GtkMenuShell* menu;
    GSList* group;
    GHashTable* items;
    menu_infos_t* menu_infos;
    menu_info_item_t* parent;
};

struct menu_infos_t_ {
    GtkStatusIcon* icon;
    GtkMenuShell* menu;
    menu_info_t menu_info[MENU_COUNT];
};

struct menu_info_item_t_ {
    int index;
    char* name;
    char* desc;
    char* icon;
    GtkWidget* widget;
    menu_info_t* menu_info;
    menu_info_t* submenu;
};

menu_infos_t* menu_infos_create();
void menu_infos_clear(menu_infos_t* mi);
void menu_infos_destroy(menu_infos_t* mi);

menu_info_t* menu_info_create(menu_infos_t* mis, menu_type_t type);
void menu_info_init(menu_infos_t* mis, menu_info_t* mi, menu_type_t type);

void menu_info_item_init(menu_info_item_t* mii);

const char* menu_info_type_name(menu_type_t type);
void menu_info_item_add(menu_info_t* mi, uint32_t index, const char* name, const char* desc, char* tooltip, const char* icon);
void menu_info_subitem_add(menu_info_t* mi, uint32_t index, const char* name, const char* desc, char* tooltip, const char* icon);
menu_info_item_t* menu_info_item_get(menu_info_t* mi, uint32_t index);
void menu_info_item_clicked(GtkWidget* item, GdkEvent* event, menu_info_item_t* mii);
void menu_info_subitem_clicked(GtkWidget* item, GdkEvent* event, menu_info_item_t* mii);
void menu_info_item_remove(menu_info_t* mi, uint32_t index);
void menu_info_item_destroy(menu_info_item_t* mii);

#endif /* PASYSTRAY_MENU_INFO_H */