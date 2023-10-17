#include "player.h"

#include "stdio.h"

static void start_feed(GstElement *source, guint size, player_t *data) {
  //   if (data->sourceid == 0) {
  //     g_print ("Start feeding\n");
  //     data->sourceid = g_idle_add ((GSourceFunc) push_data, data);
  //   }
}

static void stop_feed(GstElement *source, player_t *data) {
  //   if (data->sourceid != 0) {
  //     g_print ("Stop feeding\n");
  //     g_source_remove (data->sourceid);
  //     data->sourceid = 0;
  //   }
}

static void pad_added_handler(GstElement *src, GstPad *new_pad,
                              player_t *data) {
  printf("Pad added!\n");
  GstCaps *new_pad_caps = NULL;
  new_pad_caps = gst_pad_get_current_caps(new_pad);
}

static GstBusSyncReply bus_sync_handler(GstBus *bus, GstMessage *message,
                                        player_t *data) {
  if (GST_MESSAGE_SRC(message) == GST_OBJECT(data->pipeline)) {
    if (GST_MESSAGE_TYPE(message) == GST_MESSAGE_STATE_CHANGED) {
      printf("state changed!\n");
      GstState old_state, new_state;
      gst_message_parse_state_changed(message, &old_state, &new_state, NULL);
    }
  }
  return GST_BUS_PASS;
}

int PlayerInit(player_t *player) {
  gst_init(NULL, NULL);

//   player->appsrc = gst_element_factory_make("appsrc", "app_source");
//   GstElement *tsdemux = gst_element_factory_make("tsdemux", "demux");
//   GstElement *parse = gst_element_factory_make("h264parse", "parse");
//   GstElement *dec = gst_element_factory_make("decodebin", "dec");
//   GstElement *vconv = gst_element_factory_make("videoconvert", "vconv");
//   GstElement *vsink = gst_element_factory_make("autovideosink", "vsink");

//   player->pipeline = gst_pipeline_new("player-pipeline");
//   printf("- -A- -\n");

//   if (!player->appsrc || !tsdemux || !parse || !dec || !vconv || !vsink || !player->pipeline) {
//     g_printerr("Not all elements could be created.\n");
//     return -1;
//   }

//   g_signal_connect(player->appsrc, "need-data", G_CALLBACK(start_feed), player);
//   g_signal_connect(player->appsrc, "enough-data", G_CALLBACK(stop_feed), player);

// //   g_object_set (player->dec, "message-forward", TRUE, NULL);
// //   g_signal_connect(player->dec, "pad-added", G_CALLBACK(pad_added_handler), player);

//   printf("- -A- -\n");
//   gst_bin_add_many(GST_BIN(player->pipeline), player->appsrc, tsdemux, parse, dec, vconv, vsink, NULL);

//   if (gst_element_link_many(player->appsrc, tsdemux, parse, dec, vconv, vsink, NULL) != TRUE) {
//     g_printerr("Elements could not be linked.\n");
//     gst_object_unref(player->pipeline);
//     return -1;
//   }

  GError* g_error = NULL;
  player->pipeline = gst_parse_launch("appsrc name=data_app_src max-bytes=0 ! queue ! tsdemux name=demux demux. ! h264parse ! queue ! decodebin ! videoconvert ! autovideosink  demux. ! queue ! decodebin ! audioconvert ! autoaudiosink", &g_error);
  if (g_error != NULL) {
    g_printerr("Pipeline ERROR.\n");    
  }

  player->appsrc =
      gst_bin_get_by_name(GST_BIN(player->pipeline), "data_app_src");  

  GstBus *bus = gst_element_get_bus(player->pipeline);
  gst_bus_set_sync_handler(bus, (GstBusSyncHandler)(bus_sync_handler), player,
                           NULL);
  gst_object_unref(bus);
  
  return 0;
}

int PlayerStart(player_t *player) {
  if (player->pipeline) {
    gst_element_set_state(player->pipeline, GST_STATE_PLAYING);
  }
}

int InjectData(player_t *player, const unsigned char *data, unsigned int size) {
  if (!player->pipeline) {
    printf("Error Inject\n");
    return -1;
  }
//   GstElement *appsrc =
//       gst_bin_get_by_name(GST_BIN(player->pipeline), "app_source");
  if (!player->appsrc) {
    printf("AppSrc not found\n");
    return -1;
  }

  GstBuffer* buffer = gst_buffer_new_allocate(NULL, size, NULL);
  gst_buffer_fill(buffer, 0, data, size);
  GstFlowReturn ret;
  g_signal_emit_by_name(player->appsrc, "push-buffer", buffer, &ret);
  gst_buffer_unref(buffer);

  if (ret != GST_FLOW_OK) {
    printf("Unable to InjectData, failed to push buffer to queue\n");
    return 0;
  }

  return size;
}
