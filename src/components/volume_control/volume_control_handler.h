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

#ifndef SRC_COMPONENTS_VOLUME_CONTROL_VOLUME_CONTROL_HANDLER_H_
#define SRC_COMPONENTS_VOLUME_CONTROL_VOLUME_CONTROL_HANDLER_H_

#include <QObject>
#include <QString>
#include <QVariantList>

#include "src/info_server/volume_control/volume_control.h"

namespace panel {
namespace frontend {

class VolumeControlHandler : public QObject {
  Q_OBJECT

  Q_PROPERTY(QVariantList sinks
    READ GetSinks
    NOTIFY SinksChanged)

  Q_PROPERTY(QVariantList sources
    READ GetSources
    NOTIFY SourcesChanged)

  Q_PROPERTY(QVariantList sinkInputs
    READ GetSinkInputs
    NOTIFY SinkInputsChanged)

  Q_PROPERTY(QVariantList sourceOutputs
    READ GetSourceOutputs
    NOTIFY SourceOutputsChanged)

  Q_PROPERTY(QString volumeIcon
    READ GetVolumeIcon
    NOTIFY SinksChanged)

 public:
  /* ---------- For backend usage ---------- */
  explicit VolumeControlHandler(QObject* parent = nullptr);

  QVariantList GetSinks() const;
  QVariantList GetSources() const;
  QVariantList GetSinkInputs() const;
  QVariantList GetSourceOutputs() const;
  QString GetVolumeIcon() const;

  /* ---------- For frontend usage ---------- */
  Q_INVOKABLE void setSinkVolume(int index, int volume);
  Q_INVOKABLE void setSourceVolume(int index, int volume);
  Q_INVOKABLE void setSinkInputVolume(int index, int volume);
  Q_INVOKABLE void setSourceOutputVolume(int index, int volume);
  Q_INVOKABLE void setSinkMute(int index, bool muted);
  Q_INVOKABLE void setSourceMute(int index, bool muted);
  Q_INVOKABLE void setSinkInputMute(int index, bool muted);
  Q_INVOKABLE void setSourceOutputMute(int index, bool muted);
  Q_INVOKABLE void setDefaultSink(const QString& name);
  Q_INVOKABLE void setDefaultSource(const QString& name);

 private:
  backend::VolumeControl* volume_control_ = nullptr;

 signals:
  void SinksChanged();
  void SourcesChanged();
  void SinkInputsChanged();
  void SourceOutputsChanged();
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_VOLUME_CONTROL_VOLUME_CONTROL_HANDLER_H_
