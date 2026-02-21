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

#ifndef SRC_TRANSLATION_LOADER_TRANSLATION_LOADER_H_
#define SRC_TRANSLATION_LOADER_TRANSLATION_LOADER_H_

#include <QObject>
#include <QString>
#include <QWidget>
#include <QSettings>

#include "src/theme_loader/theme_conf.h"

namespace panel {
namespace loader {

enum class LanguageType {
  EN_US,
  ZH_CN
};

class TranslationLoader: public QObject {
  Q_OBJECT

 public:
  explicit TranslationLoader(const QString& file,
    const LanguageType& fallback_lng);
  ~TranslationLoader() = default;

  QString GetLocaleSuffix(const LanguageType& type);
  QString GetLocaleDisplayName(const LanguageType& type);
  QString GetTranslation(const QString& message);

 private:
  void RecordCurrentLanguage(const QString& lng);

  LanguageType fallback_lng_ = LanguageType::EN_US;
  LanguageType current_lng_ = LanguageType::EN_US;
  QSettings * translation_ = nullptr;
};

}  // namespace loader
}  // namespace panel

#endif  // SRC_TRANSLATION_LOADER_TRANSLATION_LOADER_H_
