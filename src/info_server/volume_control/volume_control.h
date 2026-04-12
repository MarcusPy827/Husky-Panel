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

#ifndef SRC_INFO_SERVER_VOLUME_CONTROL_VOLUME_CONTROL_H_
#define SRC_INFO_SERVER_VOLUME_CONTROL_VOLUME_CONTROL_H_

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QVariantList>
#include <cstdint>

#include <pulse/pulseaudio.h>
#include <pulse/thread-mainloop.h>

namespace panel {
namespace backend {

// Manages a PulseAudio connection and exposes sink/source/stream data via
// Qt signals. All PA callbacks run on the PA thread; data is marshalled to
// the Qt main thread via QMetaObject::invokeMethod (QueuedConnection).
class VolumeControl : public QObject {
  Q_OBJECT

 public:
  explicit VolumeControl(QObject* parent = nullptr);
  ~VolumeControl() override;

  // Getters — call only from Qt main thread.
  QVariantList GetSinks() const;
  QVariantList GetSources() const;
  QVariantList GetSinkInputs() const;
  QVariantList GetSourceOutputs() const;

  // Volume/mute operations — safe to call from Qt main thread.
  void SetSinkVolume(uint32_t index, int volume_percent);
  void SetSourceVolume(uint32_t index, int volume_percent);
  void SetSinkInputVolume(uint32_t index, int volume_percent);
  void SetSourceOutputVolume(uint32_t index, int volume_percent);
  void SetSinkMute(uint32_t index, bool muted);
  void SetSourceMute(uint32_t index, bool muted);
  void SetSinkInputMute(uint32_t index, bool muted);
  void SetSourceOutputMute(uint32_t index, bool muted);
  void SetDefaultSink(const QString& name);
  void SetDefaultSource(const QString& name);

 signals:
  void SinksChanged();
  void SourcesChanged();
  void SinkInputsChanged();
  void SourceOutputsChanged();

 private:
  pa_threaded_mainloop* mainloop_ = nullptr;
  pa_mainloop_api* mainloop_api_ = nullptr;
  pa_context* context_ = nullptr;

  // All fields below are accessed only on the Qt main thread
  // (updated via QueuedConnection lambdas).
  QVariantList sinks_;
  QVariantList sources_;
  QVariantList sink_inputs_;
  QVariantList source_outputs_;
  QByteArray default_sink_name_;
  QByteArray default_source_name_;

  // ---- PA callback statics ----
  static void OnContextState(pa_context* c, void* userdata);
  static void OnSubscribe(pa_context* c, pa_subscription_event_type_t t,
                          uint32_t index, void* userdata);
  static void OnSinkInfo(pa_context* c, const pa_sink_info* i, int eol,
                         void* userdata);
  static void OnSourceInfo(pa_context* c, const pa_source_info* i, int eol,
                           void* userdata);
  static void OnSinkInputInfo(pa_context* c, const pa_sink_input_info* i,
                              int eol, void* userdata);
  static void OnSourceOutputInfo(pa_context* c,
                                 const pa_source_output_info* i, int eol,
                                 void* userdata);
  static void OnServerInfo(pa_context* c, const pa_server_info* i,
                           void* userdata);

  // ---- Helpers ----
  static int VolumeToPercent(pa_volume_t vol);
  static pa_volume_t PercentToVolume(int percent);
  void RemoveFromList(QVariantList& list, uint32_t index);
};

}  // namespace backend
}  // namespace panel

#endif  // SRC_INFO_SERVER_VOLUME_CONTROL_VOLUME_CONTROL_H_
