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

#include <glib.h>

#include "menu_info.h"
#include "systray.h"
#include "pulseaudio_action.h"

menu_infos_t* menu_infos_create()
{
    menu_infos_t* mis = g_new(menu_infos_t, 1);

    size_t i;
    for(i=0; i<MENU_COUNT; ++i)
        menu_info_init(mis, &mis->menu_info[i], i);

    return mis;
}

void menu_infos_clear(menu_infos_t* mis)
{
    size_t i;
    for(i=0; i<MENU_COUNT; ++i)
    {
        menu_info_t* mi = &mis->menu_info[i];

        GHashTableIter iter;
        gpointer key;
        menu_info_item_t* mii;

        g_hash_table_iter_init(&iter, mi->items);

        while(g_hash_table_iter_next(&iter, &key, (gpointer*)&mii))
        {
            switch(mi->type)
            {
                case MENU_SERVER:
                case MENU_SINK:
                case MENU_SOURCE:
                    systray_remove_radio_item(mi, mii->widget);
                    break;
                case MENU_INPUT:
                case MENU_OUTPUT:
                    systray_remove_menu_item(mi, mii->widget);
                    break;
            }
            g_hash_table_iter_remove(&iter);
        }
        mis->menu_info[i].group = NULL;
    }
}

void menu_infos_destroy(menu_infos_t* mis)
{
    g_free(mis);
}

menu_info_t* menu_info_create(menu_infos_t* mis, menu_type_t type)
{
    menu_info_t* mi = g_new(menu_info_t, 1);
    menu_info_init(mis, mi, type);
    return mi;
}

void menu_info_init(menu_infos_t* mis, menu_info_t* mi, menu_type_t type)
{
    mi->type = type;
    mi->menu = NULL;
    mi->group = NULL;
    mi->items = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, (GDestroyNotify)menu_info_item_destroy);
    mi->menu_infos = mis;
}

void menu_info_item_init(menu_info_item_t* mii)
{
    mii->name = NULL;
    mii->desc = NULL;
    mii->icon = NULL;
    mii->widget = NULL;
    mii->menu_info = NULL;
    mii->submenu = NULL;
}

const char* menu_info_type_name(menu_type_t type)
{
    static const char* MENU_NAME[] = {
        [MENU_SERVER] = "server",
        [MENU_SINK]   = "sink",
        [MENU_SOURCE] = "source",
        [MENU_INPUT]  = "input",
        [MENU_OUTPUT] = "output",
    };

    return MENU_NAME[type];
}

void menu_info_item_add(menu_info_t* mi, uint32_t index, const char* name, const char* desc, char* tooltip, const char* icon)
{
    menu_infos_t* mis = mi->menu_infos;
    menu_info_item_t* item = g_new(menu_info_item_t, 1);
    menu_info_item_init(item);
    item->menu_info = mi;

#ifdef DEBUG
    fprintf(stderr, "[menu_info] adding %s %u %p\n", menu_info_type_name(mi->type), index, item);
#endif

    item->index = index;
    item->name = g_strdup(name);
    item->desc = g_strdup(desc);
    item->icon = g_strdup(icon);

    switch(item->menu_info->type)
    {
        case MENU_INPUT:
            item->submenu = menu_info_create(mis, MENU_SINK);
            item->submenu->parent = item;
            break;
        case MENU_OUTPUT:
            item->submenu = menu_info_create(mis, MENU_SOURCE);
            item->submenu->parent = item;
            break;
        default:
            break;
    }

    switch(mi->type)
    {
        case MENU_SERVER:
            item->widget = systray_add_radio_item(mi, desc, tooltip);
            break;
        case MENU_SINK:
            item->widget = systray_add_radio_item(mi, desc, tooltip);
            systray_add_item_to_all_submenus(item, &mis->menu_info[MENU_INPUT]);
            break;
        case MENU_SOURCE:
            item->widget = systray_add_radio_item(mi, desc, tooltip);
            systray_add_item_to_all_submenus(item, &mis->menu_info[MENU_OUTPUT]);
            break;
        case MENU_INPUT:
            item->widget = systray_menu_add_submenu(mi->menu, item->submenu, desc, tooltip, icon);
            systray_add_all_items_to_submenu(&mis->menu_info[MENU_SINK], item);
            break;
        case MENU_OUTPUT:
            item->widget = systray_menu_add_submenu(mi->menu, item->submenu, desc, tooltip, icon);
            systray_add_all_items_to_submenu(&mis->menu_info[MENU_SOURCE], item);
            break;
    }

    g_signal_connect(item->widget, "button-press-event", G_CALLBACK(menu_info_item_clicked), item);
    g_hash_table_insert(mi->items, GUINT_TO_POINTER(index), item);
}

void menu_info_subitem_add(menu_info_t* mi, uint32_t index, const char* name, const char* desc, char* tooltip, const char* icon)
{
    menu_info_item_t* subitem = g_new(menu_info_item_t, 1);
    menu_info_item_init(subitem);
    subitem->index = index;
    subitem->name = g_strdup(name);
    subitem->desc = g_strdup(desc);
    subitem->menu_info = mi;
    subitem->widget = systray_add_radio_item(mi, desc, tooltip);

    g_hash_table_insert(mi->items, GUINT_TO_POINTER(index), subitem);
    g_signal_connect(subitem->widget, "button-press-event", G_CALLBACK(menu_info_subitem_clicked), subitem);
}

menu_info_item_t* menu_info_item_get(menu_info_t* mi, uint32_t index)
{
    return g_hash_table_lookup(mi->items, GUINT_TO_POINTER(index));
}

void menu_info_item_clicked(GtkWidget* item, GdkEvent* event, menu_info_item_t* mii)
{
#ifdef DEBUG
    fprintf(stderr, "clicked %s %s (%s)\n", menu_info_type_name(mii->menu_info->type), mii->desc, mii->name);
#endif

    switch(mii->menu_info->type)
    {
        case MENU_SERVER:
            /* TODO: connect to different server */
            break;
        case MENU_SINK:
            pulseaudio_set_sink(mii);
            break;
        case MENU_SOURCE:
            pulseaudio_set_source(mii);
            break;
        case MENU_INPUT:
        case MENU_OUTPUT:
            break;
    }
}

void menu_info_subitem_clicked(GtkWidget* item, GdkEvent* event, menu_info_item_t* mii)
{
#ifdef DEBUG
    fprintf(stderr, "move %s %s to %s %s\n",
            menu_info_type_name(mii->menu_info->parent->menu_info->type), mii->menu_info->parent->desc,
            menu_info_type_name(mii->menu_info->type), mii->desc);
#endif

    switch(mii->menu_info->type)
    {
        case MENU_SERVER:
            break;
        case MENU_SINK:
            pulseaudio_move_input_to_sink(mii->menu_info->parent, mii);
            break;
        case MENU_SOURCE:
            pulseaudio_move_output_to_source(mii->menu_info->parent, mii);
            break;
        case MENU_INPUT:
        case MENU_OUTPUT:
            break;
    }
}

void menu_info_item_remove(menu_info_t* mi, uint32_t index)
{
    menu_infos_t* mis = mi->menu_infos;
    menu_info_item_t* mii = menu_info_item_get(mi, index);

    if(!mii)
        return;

#ifdef DEBUG
    fprintf(stderr, "[menu_info] removing %s %u\n", menu_info_type_name(mi->type), index);
#endif

    switch(mi->type)
    {
        case MENU_SERVER:
            systray_remove_radio_item(mi, mii->widget);
            break;
        case MENU_SINK:
            systray_remove_item_from_all_submenus(mii, &mis->menu_info[MENU_INPUT]);
            systray_remove_radio_item(mi, mii->widget);
            break;
        case MENU_SOURCE:
            systray_remove_item_from_all_submenus(mii, &mis->menu_info[MENU_OUTPUT]);
            systray_remove_radio_item(mi, mii->widget);
            break;
        case MENU_INPUT:
        case MENU_OUTPUT:
            systray_remove_all_items_from_submenu(mii->submenu);
            systray_remove_menu_item(mi, mii->widget);
            break;
    }

    g_hash_table_remove(mi->items, GUINT_TO_POINTER(index));
}

void menu_info_item_destroy(menu_info_item_t* mii)
{
    g_free(mii->name);
    g_free(mii->icon);
    g_free(mii);
}