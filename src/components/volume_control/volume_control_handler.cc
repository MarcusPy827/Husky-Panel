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

#include "src/components/volume_control/volume_control_handler.h"

namespace panel {
namespace frontend {

/**
 * @brief The constructor for @c VolumeControlHandler.
 * 
 * @details Creates the underlying @c backend::VolumeControl instance, connects
 *          the signals needed (including sinks/sources/sink inputs/source
 *          outputs) to the corresponding signals of the handler.
 * @param parent (QObject*) The parent QObject.
 * @return void.
 */

VolumeControlHandler::VolumeControlHandler(QObject* parent) : QObject(parent) {
  volume_control_ = new backend::VolumeControl(this);

  connect(volume_control_, &backend::VolumeControl::SinksChanged,
    this, &VolumeControlHandler::SinksChanged);
  connect(volume_control_, &backend::VolumeControl::SourcesChanged,
    this, &VolumeControlHandler::SourcesChanged);
  connect(volume_control_, &backend::VolumeControl::SinkInputsChanged,
    this, &VolumeControlHandler::SinkInputsChanged);
  connect(volume_control_, &backend::VolumeControl::SourceOutputsChanged,
    this, &VolumeControlHandler::SourceOutputsChanged);
}


/**
 * @brief Gets the list of available audio sinks (output devices).
 * 
 * @see @c volume_control.h for the function @c VolumeControl::GetSinks().
 * @return (QVariantList) A list of available audio sinks.
 */

QVariantList VolumeControlHandler::GetSinks() const {
  return volume_control_->GetSinks();
}


/**
 * @brief Gets the list of available audio sources (input devices).
 * 
 * @see @c volume_control.h for the function @c VolumeControl::GetSources().
 * @return (QVariantList) A list of available audio sources.
 */

QVariantList VolumeControlHandler::GetSources() const {
  return volume_control_->GetSources();
}


/**
 * @brief Gets the list of active sink inputs (application audio streams).
 *
 * @see @c volume_control.h for the function @c VolumeControl::GetSinkInputs().
 * @return (QVariantList) A list of active sink input streams.
 */

QVariantList VolumeControlHandler::GetSinkInputs() const {
  return volume_control_->GetSinkInputs();
}


/**
 * @brief Gets the list of active source outputs (application capture streams).
 *
 * @see @c volume_control.h for the function @c VolumeControl::GetSourceOutputs().
 * @return (QVariantList) A list of active source output streams.
 */

QVariantList VolumeControlHandler::GetSourceOutputs() const {
  return volume_control_->GetSourceOutputs();
}


/**
 * @brief Returns the material icon name reflecting the state of the default
 *        audio output sink.
 *
 * @note Icon mapping reflects the active (default) output device:
 *         muted     → @c volume_mute
 *         0%        → @c volume_off
 *         1 – 50%   → @c volume_down
 *         51 – 100% → @c volume_up
 * @return (QString) One of: volume_mute, volume_off, volume_down, volume_up.
 */

QString VolumeControlHandler::GetVolumeIcon() const {
  for (const QVariant& v : volume_control_->GetSinks()) {
    const QVariantMap m = v.toMap();
    if (!m[QStringLiteral("is_default")].toBool()) continue;

    if (m[QStringLiteral("muted")].toBool())
      return QStringLiteral("volume_mute");

    const int vol = m[QStringLiteral("volume")].toInt();
    if (vol == 0)  return QStringLiteral("volume_off");
    if (vol <= 50) return QStringLiteral("volume_down");
    return QStringLiteral("volume_up");
  }
  return QStringLiteral("volume_off");
}


/**
 * @brief Sets the volume of a sink (output device) by index.
 *
 * @param index (int) The sink index.
 * @param volume (int) The desired volume level in percent (0–100).
 * @return void.
 */

void VolumeControlHandler::setSinkVolume(int index, int volume) {
  volume_control_->SetSinkVolume(static_cast<uint32_t>(index), volume);
}


/**
 * @brief Sets the volume of a source (input device) by index.
 *
 * @param index (int) The source index.
 * @param volume (int) The desired volume level in percent (0–100).
 * @return void.
 */

void VolumeControlHandler::setSourceVolume(int index, int volume) {
  volume_control_->SetSourceVolume(static_cast<uint32_t>(index), volume);
}


/**
 * @brief Sets the volume of a sink input (application output stream) by index.
 *
 * @param index (int) The sink input index.
 * @param volume (int) The desired volume level in percent (0–100).
 * @return void.
 */

void VolumeControlHandler::setSinkInputVolume(int index, int volume) {
  volume_control_->SetSinkInputVolume(static_cast<uint32_t>(index), volume);
}


/**
 * @brief Sets the volume of a source output (application capture stream) by index.
 *
 * @param index (int) The source output index.
 * @param volume (int) The desired volume level in percent (0–100).
 * @return void.
 */

void VolumeControlHandler::setSourceOutputVolume(int index, int volume) {
  volume_control_->SetSourceOutputVolume(static_cast<uint32_t>(index), volume);
}


/**
 * @brief Sets the mute state of a sink (output device) by index.
 *
 * @param index (int) The sink index.
 * @param muted (bool) @c true to mute, @c false to unmute.
 * @return void.
 */

void VolumeControlHandler::setSinkMute(int index, bool muted) {
  volume_control_->SetSinkMute(static_cast<uint32_t>(index), muted);
}


/**
 * @brief Sets the mute state of a source (input device) by index.
 *
 * @param index (int) The source index.
 * @param muted (bool) @c true to mute, @c false to unmute.
 * @return void.
 */

void VolumeControlHandler::setSourceMute(int index, bool muted) {
  volume_control_->SetSourceMute(static_cast<uint32_t>(index), muted);
}


/**
 * @brief Sets the mute state of a sink input (application output stream) by index.
 *
 * @param index (int) The sink input index.
 * @param muted (bool) @c true to mute, @c false to unmute.
 * @return void.
 */

void VolumeControlHandler::setSinkInputMute(int index, bool muted) {
  volume_control_->SetSinkInputMute(static_cast<uint32_t>(index), muted);
}


/**
 * @brief Sets the mute state of a source output (application capture stream) by index.
 *
 * @param index (int) The source output index.
 * @param muted (bool) @c true to mute, @c false to unmute.
 * @return void.
 */

void VolumeControlHandler::setSourceOutputMute(int index, bool muted) {
  volume_control_->SetSourceOutputMute(static_cast<uint32_t>(index), muted);
}


/**
 * @brief Sets the default audio sink (output device) by name.
 *
 * @param name (const QString&) The PulseAudio sink name to set as default.
 * @return void.
 */

void VolumeControlHandler::setDefaultSink(const QString& name) {
  volume_control_->SetDefaultSink(name);
}


/**
 * @brief Sets the default audio source (input device) by name.
 *
 * @param name (const QString&) The PulseAudio source name to set as default.
 * @return void.
 */

void VolumeControlHandler::setDefaultSource(const QString& name) {
  volume_control_->SetDefaultSource(name);
}

}  // namespace frontend
}  // namespace panel
