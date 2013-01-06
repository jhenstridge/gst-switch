/* GstSwitch
 * Copyright (C) 2012,2013 Duzy Chan <code@duzy.info>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __GST_SWITCH_UI_H__by_Duzy_Chan__
#define __GST_SWITCH_UI_H__by_Duzy_Chan__ 1
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include "gstworker.h"

#define GST_TYPE_SWITCH_UI (gst_switch_ui_get_type ())
#define GST_SWITCH_UI(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), GST_TYPE_SWITCH_UI, GstSwitchUI))
#define GST_SWITCH_UI_CLASS(class) (G_TYPE_CHECK_CLASS_CAST ((class), GST_TYPE_SWITCH_UI, GstSwitchUIClass))
#define GST_IS_SWITCH_UI(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), GST_TYPE_SWITCH_UI, GstSwitchUI))
#define GST_IS_SWITCH_UI_CLASS(class) (G_TYPE_CHECK_CLASS_TYPE ((class), GST_TYPE_SWITCH_UI, GstSwitchUIClass))

typedef struct _GstSwitchUI GstSwitchUI;
typedef struct _GstSwitchUIClass GstSwitchUIClass;
typedef struct _GstVideoDisp GstVideoDisp;

struct _GstSwitchUI
{
  GObject base;

  GDBusConnection *controller;

  GtkWidget *window;
  GtkWidget *compose_view;
  GtkWidget *preview_box;

  gint compose_port;

  GstVideoDisp *compose_video;
};

struct _GstSwitchUIClass
{
  GObjectClass base_class;

  GHashTable *methods;
};

GType gst_switch_ui_get_type (void);

#endif//__GST_SWITCH_UI_H__by_Duzy_Chan__
