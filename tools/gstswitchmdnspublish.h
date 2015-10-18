/* gst-switch							    -*- c -*-
 * Copyright (C) 2015 James Henstridge
 *
 * This file is part of gst-switch.
 *
 * gst-switch is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! @file */

#ifndef __GST_SWITCH_MDNS_PUBLISH_H__
#define __GST_SWITCH_MDNS_PUBLISH_H__

#include <glib-object.h>
#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-glib/glib-watch.h>

#define GST_TYPE_SWITCH_MDNS_PUBLISH (gst_switch_mdns_publish_get_type())
#define GST_SWITCH_MDNS_PUBLISH(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), GST_TYPE_SWITCH_MDNS_PUBLISH, GstSwitchMdnsPublish))
#define GST_SWITCH_MDNS_PUBLISH_CLASS(class) (G_TYPE_CHECK_CLASS_CAST ((class), GST_TYPE_SWITCH_MDNS_PUBLISH, GstSwitchMdnsPublishClass))
#define GST_IS_SWITCH_MDNS_PUBLISH(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), GST_TYPE_SWITCH_MDNS_PUBLISH))
#define GST_IS_SWITCH_MDNS_PUBLISH_CLASS(class) (G_TYPE_CHECK_CLASS_TYPE ((class), GST_TYPE_SWITCH_MDNS_PUBLISH))

typedef struct _GstSwitchMdnsPublish GstSwitchMdnsPublish;
typedef struct _GstSwitchMdnsPublishClass GstSwitchMdnsPublishClass;

struct _GstSwitchMdnsPublish
{
  GObject base;

  AvahiGLibPoll *poll;
  AvahiClient *client;
  char *service_name;
  AvahiEntryGroup *group;
};

/**
 *  GstSwitchMdnsPublishClass:
 *  @param base_class the parent class
 */
struct _GstSwitchMdnsPublishClass
{
  GObjectClass base_class;
};

GType gst_switch_mdns_publish_get_type (void);
GstSwitchMdnsPublish *gst_switch_mdns_publish_new ();

#endif //__GST_SWITCH_MDNS_PUBLISH_H__
