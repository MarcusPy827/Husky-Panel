/*
 * Copyright (C) 2026 MarcusPy827
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "src/info_server/volume_control/volume_control.h"

#include <QMetaObject>
#include <KService>

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"

namespace panel {
namespace backend {

/**
 * @brief The constructor for @c VolumeControl.
 *
 * @details Initializes the PulseAudio threaded main loop and context, sets up
 *          the state callback, and starts the PA thread. Subscribes to all
 *          sink, source, sink-input, source-output, and server events once the
 *          context reaches @c PA_CONTEXT_READY.
 * @param parent (QObject*) The parent QObject.
 * @return void.
 */

VolumeControl::VolumeControl(QObject* parent) : QObject(parent) {
  mainloop_ = pa_threaded_mainloop_new();
  if (!mainloop_) {
    LOG(ERROR) << absl::StrCat("VolumeControl: failed to create PA mainloop.");
    return;
  }

  mainloop_api_ = pa_threaded_mainloop_get_api(mainloop_);

  pa_proplist* proplist = pa_proplist_new();
  pa_proplist_sets(proplist, PA_PROP_APPLICATION_NAME, "HuskyPanel");
  pa_proplist_sets(proplist, PA_PROP_APPLICATION_ID,
                  "io.github.marcuspy827.huskypanel.volume");

  context_ = pa_context_new_with_proplist(mainloop_api_, nullptr, proplist);
  pa_proplist_free(proplist);

  if (!context_) {
    LOG(ERROR) << absl::StrCat("VolumeControl: failed to create PA context.");
    return;
  }

  pa_context_set_state_callback(context_, OnContextState, this);

  pa_threaded_mainloop_start(mainloop_);

  pa_threaded_mainloop_lock(mainloop_);
  if (pa_context_connect(context_, nullptr, PA_CONTEXT_NOFAIL, nullptr) < 0) {
    LOG(ERROR) << absl::StrCat("VolumeControl: pa_context_connect() failed.");
  }
  pa_threaded_mainloop_unlock(mainloop_);
}


/**
 * @brief The destructor for @c VolumeControl.
 *
 * @details Safely disconnects the PulseAudio context, removes callbacks, stops
 *          the PA main loop thread, and frees all PA resources.
 * @return void.
 */

VolumeControl::~VolumeControl() {
  if (!mainloop_) return;

  pa_threaded_mainloop_lock(mainloop_);
  if (context_) {
    pa_context_set_state_callback(context_, nullptr, nullptr);
    pa_context_set_subscribe_callback(context_, nullptr, nullptr);
    pa_context_disconnect(context_);
    pa_context_unref(context_);
    context_ = nullptr;
  }
  pa_threaded_mainloop_unlock(mainloop_);

  pa_threaded_mainloop_stop(mainloop_);
  pa_threaded_mainloop_free(mainloop_);
  mainloop_ = nullptr;
}

/* ---------- Getters ---------- */

/**
 * @brief Returns the cached list of audio sinks (output devices).
 *
 * @return (QVariantList) A list of available audio sinks.
 */

QVariantList VolumeControl::GetSinks() const         { return sinks_; }


/**
 * @brief Returns the cached list of audio sources (input devices).
 *
 * @return (QVariantList) A list of available audio sources.
 */

QVariantList VolumeControl::GetSources() const       { return sources_; }


/**
 * @brief Returns the cached list of active sink inputs (application audio streams).
 *
 * @return (QVariantList) A list of active sink input streams.
 */

QVariantList VolumeControl::GetSinkInputs() const    { return sink_inputs_; }


/**
 * @brief Returns the cached list of active source outputs (application capture streams).
 *
 * @return (QVariantList) A list of active source output streams.
 */

QVariantList VolumeControl::GetSourceOutputs() const { return source_outputs_; }

/* ---------- Helpers ---------- */

/**
 * @brief Converts a raw PulseAudio volume value to a percentage (0–100).
 *
 * @param vol (pa_volume_t) The raw PA volume value.
 * @return (int) The volume expressed as a percentage.
 */

int VolumeControl::VolumeToPercent(pa_volume_t vol) {
  return static_cast<int>(
    (static_cast<uint64_t>(vol) * 100 + PA_VOLUME_NORM / 2) / PA_VOLUME_NORM);
}

/**
 * @brief Converts a percentage (0–100) to a raw PulseAudio volume value.
 *
 * @param percent (int) The volume percentage.
 * @return (pa_volume_t) The equivalent raw PA volume value.
 */

pa_volume_t VolumeControl::PercentToVolume(int percent) {
  return static_cast<pa_volume_t>(
    (static_cast<uint64_t>(percent) * PA_VOLUME_NORM + 50) / 100);
}

/**
 * @brief Removes an entry with the given @p index from a @c QVariantList.
 *
 * @param list (QVariantList&) The list to search and remove from.
 * @param index (uint32_t) The PA device/stream index to remove.
 * @return void.
 */

void VolumeControl::RemoveFromList(QVariantList& list, uint32_t index) {
  for (int i = 0; i < list.size(); ++i) {
    if (list[i].toMap()[QStringLiteral("index")].toUInt() == index) {
      list.removeAt(i);
      return;
    }
  }
}

/* ---------- Setters ---------- */

/**
 * @brief Sets the volume of a PulseAudio sink by index.
 *
 * @details Looks up the channel count from the cached sink list, then issues a
 *          @c pa_context_set_sink_volume_by_index operation on the PA thread.
 * @param index (uint32_t) The sink index.
 * @param volume_percent (int) The desired volume in percent (0–100).
 * @return void.
 */

void VolumeControl::SetSinkVolume(uint32_t index, int volume_percent) {
  if (!context_ || !mainloop_) return;

  uint8_t channels = 2;
  for (const QVariant& v : sinks_) {
    QVariantMap m = v.toMap();
    if (m[QStringLiteral("index")].toUInt() == index) {
      channels = static_cast<uint8_t>(m[QStringLiteral("channels")].toInt());
      break;
    }
  }

  pa_threaded_mainloop_lock(mainloop_);
  pa_cvolume cv;
  pa_cvolume_set(&cv, channels, PercentToVolume(volume_percent));
  pa_operation* o = pa_context_set_sink_volume_by_index(
    context_, index, &cv, nullptr, nullptr);
  if (o) pa_operation_unref(o);
  pa_threaded_mainloop_unlock(mainloop_);
}


/**
 * @brief Sets the volume of a PulseAudio source by index.
 *
 * @details Looks up the channel count from the cached source list, then issues
 *          a @c pa_context_set_source_volume_by_index operation on the PA thread.
 * @param index (uint32_t) The source index.
 * @param volume_percent (int) The desired volume in percent (0–100).
 * @return void.
 */

void VolumeControl::SetSourceVolume(uint32_t index, int volume_percent) {
  if (!context_ || !mainloop_) return;

  uint8_t channels = 2;
  for (const QVariant& v : sources_) {
    QVariantMap m = v.toMap();
    if (m[QStringLiteral("index")].toUInt() == index) {
      channels = static_cast<uint8_t>(m[QStringLiteral("channels")].toInt());
      break;
    }
  }

  pa_threaded_mainloop_lock(mainloop_);
  pa_cvolume cv;
  pa_cvolume_set(&cv, channels, PercentToVolume(volume_percent));
  pa_operation* o = pa_context_set_source_volume_by_index(
    context_, index, &cv, nullptr, nullptr);
  if (o) pa_operation_unref(o);
  pa_threaded_mainloop_unlock(mainloop_);
}


/**
 * @brief Sets the volume of a PulseAudio sink input by index.
 *
 * @details Looks up the channel count from the cached sink-input list, then
 *          issues a @c pa_context_set_sink_input_volume operation on the PA thread.
 * @param index (uint32_t) The sink input index.
 * @param volume_percent (int) The desired volume in percent (0–100).
 * @return void.
 */

void VolumeControl::SetSinkInputVolume(uint32_t index, int volume_percent) {
  if (!context_ || !mainloop_) return;

  uint8_t channels = 2;
  for (const QVariant& v : sink_inputs_) {
    QVariantMap m = v.toMap();
    if (m[QStringLiteral("index")].toUInt() == index) {
      channels = static_cast<uint8_t>(m[QStringLiteral("channels")].toInt());
      break;
    }
  }

  pa_threaded_mainloop_lock(mainloop_);
  pa_cvolume cv;
  pa_cvolume_set(&cv, channels, PercentToVolume(volume_percent));
  pa_operation* o = pa_context_set_sink_input_volume(
    context_, index, &cv, nullptr, nullptr);
  if (o) pa_operation_unref(o);
  pa_threaded_mainloop_unlock(mainloop_);
}


/**
 * @brief Sets the volume of a PulseAudio source output by index.
 *
 * @details Looks up the channel count from the cached source-output list, then
 *          issues a @c pa_context_set_source_output_volume operation on the PA thread.
 * @param index (uint32_t) The source output index.
 * @param volume_percent (int) The desired volume in percent (0–100).
 * @return void.
 */

void VolumeControl::SetSourceOutputVolume(uint32_t index, int volume_percent) {
  if (!context_ || !mainloop_) return;

  uint8_t channels = 2;
  for (const QVariant& v : source_outputs_) {
    QVariantMap m = v.toMap();
    if (m[QStringLiteral("index")].toUInt() == index) {
      channels = static_cast<uint8_t>(m[QStringLiteral("channels")].toInt());
      break;
    }
  }

  pa_threaded_mainloop_lock(mainloop_);
  pa_cvolume cv;
  pa_cvolume_set(&cv, channels, PercentToVolume(volume_percent));
  pa_operation* o = pa_context_set_source_output_volume(
    context_, index, &cv, nullptr, nullptr);
  if (o) pa_operation_unref(o);
  pa_threaded_mainloop_unlock(mainloop_);
}


/**
 * @brief Sets the mute state of a PulseAudio sink by index.
 *
 * @param index (uint32_t) The sink index.
 * @param muted (bool) @c true to mute, @c false to unmute.
 * @return void.
 */

void VolumeControl::SetSinkMute(uint32_t index, bool muted) {
  if (!context_ || !mainloop_) return;
  pa_threaded_mainloop_lock(mainloop_);
  pa_operation* o = pa_context_set_sink_mute_by_index(
    context_, index, muted ? 1 : 0, nullptr, nullptr);
  if (o) pa_operation_unref(o);
  pa_threaded_mainloop_unlock(mainloop_);
}


/**
 * @brief Sets the mute state of a PulseAudio source by index.
 *
 * @param index (uint32_t) The source index.
 * @param muted (bool) @c true to mute, @c false to unmute.
 * @return void.
 */

void VolumeControl::SetSourceMute(uint32_t index, bool muted) {
  if (!context_ || !mainloop_) return;
  pa_threaded_mainloop_lock(mainloop_);
  pa_operation* o = pa_context_set_source_mute_by_index(
    context_, index, muted ? 1 : 0, nullptr, nullptr);
  if (o) pa_operation_unref(o);
  pa_threaded_mainloop_unlock(mainloop_);
}


/**
 * @brief Sets the mute state of a PulseAudio sink input by index.
 *
 * @param index (uint32_t) The sink input index.
 * @param muted (bool) @c true to mute, @c false to unmute.
 * @return void.
 */

void VolumeControl::SetSinkInputMute(uint32_t index, bool muted) {
  if (!context_ || !mainloop_) return;
  pa_threaded_mainloop_lock(mainloop_);
  pa_operation* o = pa_context_set_sink_input_mute(
    context_, index, muted ? 1 : 0, nullptr, nullptr);
  if (o) pa_operation_unref(o);
  pa_threaded_mainloop_unlock(mainloop_);
}


/**
 * @brief Sets the mute state of a PulseAudio source output by index.
 *
 * @param index (uint32_t) The source output index.
 * @param muted (bool) @c true to mute, @c false to unmute.
 * @return void.
 */

void VolumeControl::SetSourceOutputMute(uint32_t index, bool muted) {
  if (!context_ || !mainloop_) return;
  pa_threaded_mainloop_lock(mainloop_);
  pa_operation* o = pa_context_set_source_output_mute(
    context_, index, muted ? 1 : 0, nullptr, nullptr);
  if (o) pa_operation_unref(o);
  pa_threaded_mainloop_unlock(mainloop_);
}


/**
 * @brief Sets the default PulseAudio sink (output device) by name.
 *
 * @param name (const QString&) The PulseAudio sink name to set as default.
 * @return void.
 */

void VolumeControl::SetDefaultSink(const QString& name) {
  if (!context_ || !mainloop_) return;
  QByteArray bytes = name.toUtf8();
  pa_threaded_mainloop_lock(mainloop_);
  pa_operation* o = pa_context_set_default_sink(
    context_, bytes.constData(), nullptr, nullptr);
  if (o) pa_operation_unref(o);
  pa_threaded_mainloop_unlock(mainloop_);
}


/**
 * @brief Sets the default PulseAudio source (input device) by name.
 *
 * @param name (const QString&) The PulseAudio source name to set as default.
 * @return void.
 */

void VolumeControl::SetDefaultSource(const QString& name) {
  if (!context_ || !mainloop_) return;
  QByteArray bytes = name.toUtf8();
  pa_threaded_mainloop_lock(mainloop_);
  pa_operation* o = pa_context_set_default_source(
    context_, bytes.constData(), nullptr, nullptr);
  if (o) pa_operation_unref(o);
  pa_threaded_mainloop_unlock(mainloop_);
}

/* ---------- PA Callbacks ---------- */

/**
 * @brief PulseAudio context state callback.
 *
 * @details Called by the PA thread when the context state changes. On
 *          @c PA_CONTEXT_READY, subscribes to PA events and fetches the initial
 *          device/stream lists. On @c PA_CONTEXT_FAILED, clears cached data
 *          and emits the corresponding changed signals.
 * @param c (pa_context*) The PulseAudio context.
 * @param userdata (void*) Pointer to the owning @c VolumeControl instance.
 * @return void.
 */

void VolumeControl::OnContextState(pa_context* c, void* userdata) {
  auto* self = static_cast<VolumeControl*>(userdata);

  switch (pa_context_get_state(c)) {
    case PA_CONTEXT_READY: {
      LOG(INFO) << absl::StrCat("VolumeControl: PulseAudio context ready.");

      // Clear stale data from a potential reconnect.
      QMetaObject::invokeMethod(self, [self]() {
        self->sinks_.clear();
        self->sources_.clear();
        self->sink_inputs_.clear();
        self->source_outputs_.clear();
      }, Qt::QueuedConnection);

      pa_context_set_subscribe_callback(c, OnSubscribe, self);
      pa_operation* o = pa_context_subscribe(c,
        static_cast<pa_subscription_mask_t>(
          PA_SUBSCRIPTION_MASK_SINK |
          PA_SUBSCRIPTION_MASK_SOURCE |
          PA_SUBSCRIPTION_MASK_SINK_INPUT |
          PA_SUBSCRIPTION_MASK_SOURCE_OUTPUT |
          PA_SUBSCRIPTION_MASK_SERVER),
        nullptr, nullptr);
      if (o) pa_operation_unref(o);

      o = pa_context_get_server_info(c, OnServerInfo, self);
      if (o) pa_operation_unref(o);

      o = pa_context_get_sink_info_list(c, OnSinkInfo, self);
      if (o) pa_operation_unref(o);

      o = pa_context_get_source_info_list(c, OnSourceInfo, self);
      if (o) pa_operation_unref(o);

      o = pa_context_get_sink_input_info_list(c, OnSinkInputInfo, self);
      if (o) pa_operation_unref(o);

      o = pa_context_get_source_output_info_list(c, OnSourceOutputInfo, self);
      if (o) pa_operation_unref(o);
      break;
    }

    case PA_CONTEXT_FAILED:
      LOG(WARNING) << absl::StrCat(
        "VolumeControl: PulseAudio context failed, will retry.");
      QMetaObject::invokeMethod(self, [self]() {
        self->sinks_.clear();
        self->sources_.clear();
        self->sink_inputs_.clear();
        self->source_outputs_.clear();
        emit self->SinksChanged();
        emit self->SourcesChanged();
        emit self->SinkInputsChanged();
        emit self->SourceOutputsChanged();
      }, Qt::QueuedConnection);
      break;

    default:
      break;
  }
}


/**
 * @brief PulseAudio subscription event callback.
 *
 * @details Called by the PA thread for each subscribed event. Dispatches to
 *          the appropriate info callback for add/change events, or removes the
 *          entry from the cached list for remove events.
 * @param c (pa_context*) The PulseAudio context.
 * @param t (pa_subscription_event_type_t) The event type and facility mask.
 * @param index (uint32_t) The device/stream index that changed.
 * @param userdata (void*) Pointer to the owning @c VolumeControl instance.
 * @return void.
 */

void VolumeControl::OnSubscribe(pa_context* c, pa_subscription_event_type_t t,
                                uint32_t index, void* userdata) {
  auto* self = static_cast<VolumeControl*>(userdata);

  const uint32_t facility = t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK;
  const uint32_t type     = t & PA_SUBSCRIPTION_EVENT_TYPE_MASK;

  if (type == PA_SUBSCRIPTION_EVENT_REMOVE) {
    switch (facility) {
      case PA_SUBSCRIPTION_EVENT_SINK:
        QMetaObject::invokeMethod(self, [self, index]() {
          self->RemoveFromList(self->sinks_, index);
          emit self->SinksChanged();
        }, Qt::QueuedConnection);
        break;
      case PA_SUBSCRIPTION_EVENT_SOURCE:
        QMetaObject::invokeMethod(self, [self, index]() {
          self->RemoveFromList(self->sources_, index);
          emit self->SourcesChanged();
        }, Qt::QueuedConnection);
        break;
      case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
        QMetaObject::invokeMethod(self, [self, index]() {
          self->RemoveFromList(self->sink_inputs_, index);
          emit self->SinkInputsChanged();
        }, Qt::QueuedConnection);
        break;
      case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
        QMetaObject::invokeMethod(self, [self, index]() {
          self->RemoveFromList(self->source_outputs_, index);
          emit self->SourceOutputsChanged();
        }, Qt::QueuedConnection);
        break;
    }
    return;
  }

  pa_operation* o = nullptr;
  switch (facility) {
    case PA_SUBSCRIPTION_EVENT_SINK:
      o = pa_context_get_sink_info_by_index(c, index, OnSinkInfo, self);
      break;
    case PA_SUBSCRIPTION_EVENT_SOURCE:
      o = pa_context_get_source_info_by_index(c, index, OnSourceInfo, self);
      break;
    case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
      o = pa_context_get_sink_input_info(c, index, OnSinkInputInfo, self);
      break;
    case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
      o = pa_context_get_source_output_info(c, index, OnSourceOutputInfo, self);
      break;
    case PA_SUBSCRIPTION_EVENT_SERVER:
      o = pa_context_get_server_info(c, OnServerInfo, self);
      break;
  }
  if (o) pa_operation_unref(o);
}


/**
 * @brief PulseAudio sink info callback.
 *
 * @details Called by the PA thread for each sink returned by a sink-info query.
 *          Marshals the data to the Qt main thread and upserts the entry in
 *          @c sinks_. Emits @c SinksChanged() on @p eol.
 * @param c (pa_context*) The PulseAudio context (unused).
 * @param i (const pa_sink_info*) Sink information, or @c nullptr on error.
 * @param eol (int) Non-zero when the list is exhausted or an error occurred.
 * @param userdata (void*) Pointer to the owning @c VolumeControl instance.
 * @return void.
 */

void VolumeControl::OnSinkInfo(pa_context*, const pa_sink_info* i, int eol,
                               void* userdata) {
  auto* self = static_cast<VolumeControl*>(userdata);
  if (eol < 0) return;
  if (eol > 0) {
    QMetaObject::invokeMethod(self, [self]() {
      emit self->SinksChanged();
    }, Qt::QueuedConnection);
    return;
  }

  const uint32_t index    = i->index;
  const QString  name     = QString::fromUtf8(i->name);
  const QString  desc     = i->description
                            ? QString::fromUtf8(i->description) : name;
  const int      vol      = VolumeToPercent(pa_cvolume_avg(&i->volume));
  const bool     muted    = (i->mute != 0);
  const int      channels = static_cast<int>(i->volume.channels);

  QMetaObject::invokeMethod(self,
      [self, index, name, desc, vol, muted, channels]() {
    QVariantMap entry;
    entry[QStringLiteral("index")]      = index;
    entry[QStringLiteral("name")]       = name;
    entry[QStringLiteral("description")] = desc;
    entry[QStringLiteral("volume")]     = vol;
    entry[QStringLiteral("muted")]      = muted;
    entry[QStringLiteral("channels")]   = channels;
    entry[QStringLiteral("is_default")] =
      (name.toUtf8() == self->default_sink_name_);

    for (int j = 0; j < self->sinks_.size(); ++j) {
      if (self->sinks_[j].toMap()[QStringLiteral("index")].toUInt() == index) {
        self->sinks_[j] = entry;
        return;
      }
    }
    self->sinks_.append(entry);
  }, Qt::QueuedConnection);
}


/**
 * @brief PulseAudio source info callback.
 *
 * @details Called by the PA thread for each source. Monitor sources (virtual
 *          copies of sinks) are skipped. Marshals the data to the Qt main
 *          thread and upserts the entry in @c sources_. Emits
 *          @c SourcesChanged() on @p eol.
 * @param c (pa_context*) The PulseAudio context (unused).
 * @param i (const pa_source_info*) Source information.
 * @param eol (int) Non-zero when the list is exhausted or an error occurred.
 * @param userdata (void*) Pointer to the owning @c VolumeControl instance.
 * @return void.
 */

void VolumeControl::OnSourceInfo(pa_context*, const pa_source_info* i, int eol,
                                 void* userdata) {
  auto* self = static_cast<VolumeControl*>(userdata);
  if (eol < 0) return;
  if (eol > 0) {
    QMetaObject::invokeMethod(self, [self]() {
      emit self->SourcesChanged();
    }, Qt::QueuedConnection);
    return;
  }

  // Skip monitor sources (virtual copies of sinks).
  if (i->monitor_of_sink != PA_INVALID_INDEX) return;

  const uint32_t index    = i->index;
  const QString  name     = QString::fromUtf8(i->name);
  const QString  desc     = i->description
                            ? QString::fromUtf8(i->description) : name;
  const int      vol      = VolumeToPercent(pa_cvolume_avg(&i->volume));
  const bool     muted    = (i->mute != 0);
  const int      channels = static_cast<int>(i->volume.channels);

  QMetaObject::invokeMethod(self,
      [self, index, name, desc, vol, muted, channels]() {
    QVariantMap entry;
    entry[QStringLiteral("index")]      = index;
    entry[QStringLiteral("name")]       = name;
    entry[QStringLiteral("description")] = desc;
    entry[QStringLiteral("volume")]     = vol;
    entry[QStringLiteral("muted")]      = muted;
    entry[QStringLiteral("channels")]   = channels;
    entry[QStringLiteral("is_default")] =
      (name.toUtf8() == self->default_source_name_);

    for (int j = 0; j < self->sources_.size(); ++j) {
      if (self->sources_[j].toMap()[QStringLiteral("index")]
          .toUInt() == index) {
        self->sources_[j] = entry;
        return;
      }
    }
    self->sources_.append(entry);
  }, Qt::QueuedConnection);
}


/**
 * @brief PulseAudio sink input info callback.
 *
 * @details Called by the PA thread for each sink input (application audio
 *          stream). Resolves the application icon via
 *          @c PA_PROP_APPLICATION_ICON_NAME and @c KService lookup. Upserts
 *          the entry in @c sink_inputs_. Emits @c SinkInputsChanged() on @p eol.
 * @param c (pa_context*) The PulseAudio context (unused).
 * @param i (const pa_sink_input_info*) Sink input information.
 * @param eol (int) Non-zero when the list is exhausted or an error occurred.
 * @param userdata (void*) Pointer to the owning @c VolumeControl instance.
 * @return void.
 */

void VolumeControl::OnSinkInputInfo(pa_context*, const pa_sink_input_info* i,
                                    int eol, void* userdata) {
  auto* self = static_cast<VolumeControl*>(userdata);
  if (eol < 0) return;
  if (eol > 0) {
    QMetaObject::invokeMethod(self, [self]() {
      emit self->SinkInputsChanged();
    }, Qt::QueuedConnection);
    return;
  }

  const uint32_t index = i->index;
  const char* app_raw = pa_proplist_gets(i->proplist, PA_PROP_APPLICATION_NAME);
  const QString name = app_raw ? QString::fromUtf8(app_raw)
    : QStringLiteral("Unknown");
  const int      vol      = VolumeToPercent(pa_cvolume_avg(&i->volume));
  const bool     muted    = (i->mute != 0);
  const int      channels = static_cast<int>(i->volume.channels);

  // Capture proplist fields for icon lookup on the Qt main thread.
  const char* icon_raw = pa_proplist_gets(i->proplist,
    PA_PROP_APPLICATION_ICON_NAME);
  const char* id_raw = pa_proplist_gets(i->proplist, PA_PROP_APPLICATION_ID);
  const char* bin_raw = pa_proplist_gets(i->proplist,
    PA_PROP_APPLICATION_PROCESS_BINARY);
  const QString pa_icon = icon_raw ? QString::fromUtf8(icon_raw) : QString();
  const QString app_id  = id_raw   ? QString::fromUtf8(id_raw) : QString();
  const QString binary  = bin_raw  ? QString::fromUtf8(bin_raw) : QString();

  QMetaObject::invokeMethod(self,
      [self, index, name, pa_icon, app_id, binary, vol, muted, channels]() {
    // 1. Honor PA_PROP_APPLICATION_ICON_NAME if set (Chrome, Firefox, VLC…).
    // 2. Fall back to KService lookup via application.id then process.binary.
    QString icon_name = QStringLiteral("audio-card");
    if (!pa_icon.isEmpty()) {
      icon_name = pa_icon;
    } else {
      auto try_lookup = [&icon_name](const QString& key) {
        if (key.isEmpty()) return;
        KService::Ptr svc = KService::serviceByDesktopName(key);
        if (!svc) svc = KService::serviceByDesktopName(key.toLower());
        if (svc && !svc->icon().isEmpty()) icon_name = svc->icon();
      };
      try_lookup(app_id);
      if (icon_name == QLatin1String("audio-card")) try_lookup(binary);
    }

    QVariantMap entry;
    entry[QStringLiteral("index")] = index;
    entry[QStringLiteral("name")] = name;
    entry[QStringLiteral("icon_name")] = icon_name;
    entry[QStringLiteral("volume")] = vol;
    entry[QStringLiteral("muted")] = muted;
    entry[QStringLiteral("channels")] = channels;

    for (int j = 0; j < self->sink_inputs_.size(); ++j) {
      if (self->sink_inputs_[j].toMap()[QStringLiteral("index")]
          .toUInt() == index) {
        self->sink_inputs_[j] = entry;
        return;
      }
    }
    self->sink_inputs_.append(entry);
  }, Qt::QueuedConnection);
}


/**
 * @brief PulseAudio source output info callback.
 *
 * @details Called by the PA thread for each source output (application capture
 *          stream). Resolves the application icon via
 *          @c PA_PROP_APPLICATION_ICON_NAME and @c KService lookup. Upserts
 *          the entry in @c source_outputs_. Emits @c SourceOutputsChanged()
 *          on @p eol.
 * @param c (pa_context*) The PulseAudio context (unused).
 * @param i (const pa_source_output_info*) Source output information.
 * @param eol (int) Non-zero when the list is exhausted or an error occurred.
 * @param userdata (void*) Pointer to the owning @c VolumeControl instance.
 * @return void.
 */

void VolumeControl::OnSourceOutputInfo(pa_context*,
                                       const pa_source_output_info* i,
                                       int eol, void* userdata) {
  auto* self = static_cast<VolumeControl*>(userdata);
  if (eol < 0) return;
  if (eol > 0) {
    QMetaObject::invokeMethod(self, [self]() {
      emit self->SourceOutputsChanged();
    }, Qt::QueuedConnection);
    return;
  }

  const uint32_t index = i->index;
  const char* app_raw = pa_proplist_gets(i->proplist, PA_PROP_APPLICATION_NAME);
  const QString name = app_raw ? QString::fromUtf8(app_raw)
    : QStringLiteral("Unknown");
  const int vol = VolumeToPercent(pa_cvolume_avg(&i->volume));
  const bool muted = (i->mute != 0);
  const int channels = static_cast<int>(i->volume.channels);

  const char* icon_raw2 = pa_proplist_gets(i->proplist,
    PA_PROP_APPLICATION_ICON_NAME);
  const char* id_raw2 = pa_proplist_gets(i->proplist, PA_PROP_APPLICATION_ID);
  const char* bin_raw2 = pa_proplist_gets(i->proplist,
    PA_PROP_APPLICATION_PROCESS_BINARY);
  const QString pa_icon2 = icon_raw2 ? QString::fromUtf8(icon_raw2) : QString();
  const QString app_id2  = id_raw2 ? QString::fromUtf8(id_raw2) : QString();
  const QString binary2  = bin_raw2 ? QString::fromUtf8(bin_raw2) : QString();

  QMetaObject::invokeMethod(self,
      [self, index, name, pa_icon2, app_id2, binary2, vol, muted, channels]() {
    QString icon_name = QStringLiteral("audio-card");
    if (!pa_icon2.isEmpty()) {
      icon_name = pa_icon2;
    } else {
      auto try_lookup = [&icon_name](const QString& key) {
        if (key.isEmpty()) return;
        KService::Ptr svc = KService::serviceByDesktopName(key);
        if (!svc) svc = KService::serviceByDesktopName(key.toLower());
        if (svc && !svc->icon().isEmpty()) icon_name = svc->icon();
      };
      try_lookup(app_id2);
      if (icon_name == QLatin1String("audio-card")) try_lookup(binary2);
    }

    QVariantMap entry;
    entry[QStringLiteral("index")] = index;
    entry[QStringLiteral("name")] = name;
    entry[QStringLiteral("icon_name")] = icon_name;
    entry[QStringLiteral("volume")] = vol;
    entry[QStringLiteral("muted")] = muted;
    entry[QStringLiteral("channels")] = channels;

    for (int j = 0; j < self->source_outputs_.size(); ++j) {
      if (self->source_outputs_[j].toMap()[QStringLiteral("index")]
          .toUInt() == index) {
        self->source_outputs_[j] = entry;
        return;
      }
    }
    self->source_outputs_.append(entry);
  }, Qt::QueuedConnection);
}


/**
 * @brief PulseAudio server info callback.
 *
 * @details Called by the PA thread when server info is available. Updates
 *          @c default_sink_name_ and @c default_source_name_ on the Qt main
 *          thread, refreshes the @c is_default field of all cached entries,
 *          and emits the corresponding changed signals.
 * @param c (pa_context*) The PulseAudio context (unused).
 * @param i (const pa_server_info*) Server information.
 * @param userdata (void*) Pointer to the owning @c VolumeControl instance.
 * @return void.
 */

void VolumeControl::OnServerInfo(pa_context*, const pa_server_info* i,
    void* userdata) {
  auto* self = static_cast<VolumeControl*>(userdata);
  if (!i) return;

  const QByteArray default_sink =
    QByteArray(i->default_sink_name ? i->default_sink_name : "");
  const QByteArray default_source =
    QByteArray(i->default_source_name ? i->default_source_name : "");

  QMetaObject::invokeMethod(self, [self, default_sink, default_source]() {
    const bool sink_changed = (self->default_sink_name_ != default_sink);
    const bool source_changed = (self->default_source_name_ != default_source);

    self->default_sink_name_ = default_sink;
    self->default_source_name_ = default_source;

    if (sink_changed) {
      for (int j = 0; j < self->sinks_.size(); ++j) {
        QVariantMap m = self->sinks_[j].toMap();
        m[QStringLiteral("is_default")] =
          (m[QStringLiteral("name")].toByteArray() == default_sink);
        self->sinks_[j] = m;
      }
      emit self->SinksChanged();
    }

    if (source_changed) {
      for (int j = 0; j < self->sources_.size(); ++j) {
        QVariantMap m = self->sources_[j].toMap();
        m[QStringLiteral("is_default")] =
          (m[QStringLiteral("name")].toByteArray() == default_source);
        self->sources_[j] = m;
      }
      emit self->SourcesChanged();
    }
  }, Qt::QueuedConnection);
}

}  // namespace backend
}  // namespace panel
