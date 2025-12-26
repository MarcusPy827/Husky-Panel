/*
 * Copyright (C) 2025 MarcusPy827
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

#ifndef SRC_QUICK_KDE_SU_QUICK_KDE_SU_H_
#define SRC_QUICK_KDE_SU_QUICK_KDE_SU_H_

#include <QWidget>

namespace panel {
namespace frontend {

namespace Ui {
class QuickKDESU;
}

class QuickKDESU : public QWidget {
  Q_OBJECT

 public:
  explicit QuickKDESU(QWidget *parent = nullptr);
  ~QuickKDESU();

 private:
  void ExecWithKDESU(const QString &prog, const QStringList &args);
  Ui::QuickKDESU * ui_;

 private slots:
  void OpenDoplhinWithKDESU();
  void OpenKonsoleWithKDESU();
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_QUICK_KDE_SU_QUICK_KDE_SU_H_
