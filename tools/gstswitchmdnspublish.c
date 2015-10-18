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


#include "gstswitchmdnspublish.h"
#include "gstswitchserver.h"

#include <avahi-common/alternative.h>
#include <avahi-common/error.h>
#include <avahi-glib/glib-malloc.h>

G_DEFINE_TYPE (GstSwitchMdnsPublish, gst_switch_mdns_publish, G_TYPE_OBJECT);

static void client_callback (AvahiClient *client, AvahiClientState state, void *user_data);
static void group_callback (AvahiEntryGroup *group, AvahiEntryGroupState state, void *user_data);
static void create_services (GstSwitchMdnsPublish *self);

static void
gst_switch_mdns_publish_finalize (GObject *object)
{
  GstSwitchMdnsPublish *self = GST_SWITCH_MDNS_PUBLISH (object);

  if (self->server) {
    g_object_unref (self->server);
  }

  if (self->group) {
    avahi_entry_group_free (self->group);
  }

  g_free(self->service_name);

  if (self->client) {
    avahi_client_free (self->client);
  }

  if (self->poll) {
    avahi_glib_poll_free (self->poll);
  }

  if (G_OBJECT_CLASS (gst_switch_mdns_publish_parent_class)->finalize)
    G_OBJECT_CLASS (gst_switch_mdns_publish_parent_class)->finalize (object);
}

static void
gst_switch_mdns_publish_class_init (GstSwitchMdnsPublishClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  object_class->finalize = gst_switch_mdns_publish_finalize;
}

static void
gst_switch_mdns_publish_init (GstSwitchMdnsPublish *self)
{
  avahi_set_allocator (avahi_glib_allocator ());

  self->poll = avahi_glib_poll_new (NULL, G_PRIORITY_DEFAULT);
  if (!self->poll) {
    g_warning ("Could not create AvahiGLibPoll");
    return;
  }

  int error = 0;
  self->client = avahi_client_new (avahi_glib_poll_get (self->poll),
                                   0,
                                   client_callback,
                                   self,
                                   &error);
  if (!self->client) {
    g_warning ("Could not create Avahi client: %s", avahi_strerror(error));
  }
}

GstSwitchMdnsPublish *
gst_switch_mdns_publish_new (GstSwitchServer *server, const char *service_name)
{
  g_return_if_fail(server != NULL);
  g_return_if_fail(service_name != NULL);

  GstSwitchMdnsPublish *self = g_object_new (GST_TYPE_SWITCH_MDNS_PUBLISH, NULL);
  if (!self) {
    return NULL;
  }

  self->service_name = g_strdup(service_name);
  self->server = g_object_ref (server);
  if (self->client && avahi_client_get_state (self->client) == AVAHI_CLIENT_S_RUNNING) {
    create_services (self);
  }
  return self;
}

static void
create_services (GstSwitchMdnsPublish *self)
{
  char *new_name = NULL;

  if (!self->group) {
    self->group = avahi_entry_group_new (self->client, group_callback, self);
    if (!self->group) {
      g_warning ("avahi_entry_group_new() failed: %s", avahi_strerror(avahi_client_errno(self->client)));
      goto fail;
    }
  }

  /* The group will be empty if it was just created, or if we reset it
   * due to a server name collision.
   *
   * Either way, add our entries */
  if (avahi_entry_group_is_empty (self->group)) {
    int error;
    const char version_record[] = "txtvers=1";
    char *caps_record;

    /* Publish the video service */
    caps_record = g_strconcat(
      "caps=", gst_switch_server_get_video_caps_str(), NULL);
    error = avahi_entry_group_add_service (
      self->group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, 0,
      self->service_name, "_gstswitch-video._tcp", NULL, NULL, self->server->video_acceptor_port,
      version_record, caps_record, NULL);
    g_free(caps_record);
    if (error == AVAHI_ERR_COLLISION) {
      goto collision;
    } else if (error != 0) {
      g_warning ("Failed to add _gstswitch-video._tcp service: %s", avahi_strerror (error));
      goto fail;
    }

    /* Publish the video service */
    caps_record = g_strconcat(
      "caps=", gst_switch_server_get_audio_caps_str(), NULL);
    error = avahi_entry_group_add_service (
      self->group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, 0,
      self->service_name, "_gstswitch-audio._tcp", NULL, NULL, self->server->audio_acceptor_port,
      version_record, caps_record, NULL);
    g_free(caps_record);
    if (error == AVAHI_ERR_COLLISION) {
      goto collision;
    } else if (error != 0) {
      g_warning ("Failed to add _gstswitch-audio._tcp service: %s", avahi_strerror (error));
      goto fail;
    }

    error = avahi_entry_group_commit (self->group);
    if (error != 0) {
      g_warning ("Failed to commit entry group: %s", avahi_strerror (error));
      goto fail;
    }
  }
  return;

collision:
  new_name = avahi_alternative_service_name (self->service_name);
  g_free(self->service_name);
  self->service_name = new_name;

  g_info ("Service name collision.  Renaming to %s", self->service_name);
  avahi_entry_group_reset (self->group);
  create_services (self);
  return;

fail:
  if (self->group) {
    avahi_entry_group_reset (self->group);
  }
}

static void
group_callback (AvahiEntryGroup *group, AvahiEntryGroupState state, void *user_data)
{
  GstSwitchMdnsPublish *self = user_data;

  switch (state) {
  case AVAHI_ENTRY_GROUP_ESTABLISHED:
    break;

  case AVAHI_ENTRY_GROUP_COLLISION: {
    char *new_name = avahi_alternative_service_name (self->service_name);
    g_free(self->service_name);
    self->service_name = new_name;

    g_info ("Service name collision.  Renaming to %s", self->service_name);
    avahi_entry_group_reset (group);
    create_services (self);
    break;
  }

  case AVAHI_ENTRY_GROUP_FAILURE:
    g_warning ("Entry group failure: %s", avahi_strerror (avahi_client_errno (self->client)));
    break;

  case AVAHI_ENTRY_GROUP_UNCOMMITED:
  case AVAHI_ENTRY_GROUP_REGISTERING:
    break;
  }
}

static void
client_callback (AvahiClient *client, AvahiClientState state, void *user_data)
{
  GstSwitchMdnsPublish *self = user_data;

  switch (state) {
  case AVAHI_CLIENT_S_RUNNING:
    if (self->server) {
      create_services(self);
    }
    break;

  case AVAHI_CLIENT_FAILURE:
    g_warning("Avahi client failed: %s", avahi_strerror(avahi_client_errno(client)));
    break;

  case AVAHI_CLIENT_S_COLLISION:
  case AVAHI_CLIENT_S_REGISTERING:
    if (self->group) {
      avahi_entry_group_reset (self->group);
    }
    break;

  case AVAHI_CLIENT_CONNECTING:
    break;
  }
}
