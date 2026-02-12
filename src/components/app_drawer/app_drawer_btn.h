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

#ifndef SRC_COMPONENTS_APP_DRAWER_APP_DRAWER_BTN_H_
#define SRC_COMPONENTS_APP_DRAWER_APP_DRAWER_BTN_H_

#include <memory>
#include <QWidget>
#include <QPushButton>

namespace panel {
namespace frontend {

class AppDrawerBtn : public QWidget {
  Q_OBJECT

 public:
  explicit AppDrawerBtn(QWidget *parent = nullptr);
  ~AppDrawerBtn();
  QPushButton * GetBtn();

 private:
  QPushButton * btn_ = nullptr;
};

}  // namespace frontend
}  // namespace panel

#endif  // SRC_COMPONENTS_APP_DRAWER_APP_DRAWER_BTN_H_
