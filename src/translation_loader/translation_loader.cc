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

#include <QDebug>
#include <QApplication>
#include <QStyleHints>
#include <QFile>
#include <QLocale>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/translation_loader/translation_loader.h"

namespace panel {
namespace loader {

TranslationLoader::TranslationLoader(const QString& file,
    const LanguageType& fallback_lng) {
  LOG(INFO) << absl::StrCat("Initializing translator...");
  if (!QFile::exists(file)) {
    LOG(ERROR) << absl::StrCat("Invalid locale file!! Translator halted...");
  }


  LOG(INFO) << absl::StrCat(absl::StrFormat(
    "Fallback language is set to %s...", GetLocaleDisplayName(fallback_lng)
    .toStdString()));
  fallback_lng_ = fallback_lng;

  LOG(INFO) << absl::StrCat("Initializing translation reader...");
  translation_ = new QSettings(file, QSettings::IniFormat);

  QLocale qlocale_instance = QLocale();
  QString current_lng = QLocale::languageToString(qlocale_instance.language())
    .toLower();
  RecordCurrentLanguage(current_lng);
}

QString TranslationLoader::GetLocaleSuffix(const LanguageType& type) {
  switch (type) {
    case LanguageType::EN_US:
      return "en_us";

    case LanguageType::ZH_CN:
      return "zh_cn";

    default:
      LOG(ERROR) << absl::StrCat("Unknown type, defaulting to EN-US...");
      return "en_us";
  }
}

QString TranslationLoader::GetLocaleDisplayName(const LanguageType& type) {
  switch (type) {
    case LanguageType::EN_US:
      return "English (United States)";

    case LanguageType::ZH_CN:
      return "简体中文（中国）";

    default:
      LOG(ERROR) << absl::StrCat("Unknown type, defaulting to EN-US...");
      return "English (United States)";
  }
}

void TranslationLoader::RecordCurrentLanguage(const QString& lng) {
  LOG(INFO) << absl::StrCat("Getting current language...");
  LOG(INFO) << absl::StrCat(absl::StrFormat("QLocale raw value: %s.",
    lng.toStdString()));
  if (lng.contains("zh") || lng.contains("chinese")) {
    LOG(INFO) << absl::StrCat(absl::StrFormat("Language set to %s!",
      GetLocaleDisplayName(LanguageType::ZH_CN).toStdString()));
    current_lng_ = LanguageType::ZH_CN;
  } else if (lng.contains("en")) {
    LOG(INFO) << absl::StrCat(absl::StrFormat("Language set to %s!",
      GetLocaleDisplayName(LanguageType::EN_US).toStdString()));
    current_lng_ = LanguageType::EN_US;
  } else {
    LOG(INFO) << absl::StrCat(absl::StrFormat(
      "Unknown language type, now setting language to fallback %s...",
    GetLocaleDisplayName(fallback_lng_).toStdString()));
    current_lng_ = fallback_lng_;
  }
}

QString TranslationLoader::GetTranslation(const QString& message) {
  if (translation_ == nullptr) {
    LOG(ERROR) << absl::StrCat("Translator is NOT set up properly!! ",
      "Now returning original message...");
    return message;
  }

  QString search_path_base = "/" + message + "/";
  QString search_path_current = search_path_base +
    GetLocaleSuffix(current_lng_);

  QString result_current_lng = translation_->value(search_path_current,
    "%TRANSLATION_NOT_FOUND").toString();
  if (result_current_lng != "%TRANSLATION_NOT_FOUND") {
    return result_current_lng;
  }

  LOG(ERROR) << absl::StrCat(absl::StrFormat(
    "The message %s is NOT found in translation file, now trying fallback...",
    message.toStdString()));
  QString search_path_fallback = search_path_base +
    GetLocaleSuffix(fallback_lng_);
  QString result_fallback_lng = translation_->value(search_path_fallback,
    "%TRANSLATION_NOT_FOUND").toString();
  if (result_fallback_lng != "%TRANSLATION_NOT_FOUND") {
    return result_fallback_lng;
  }

  LOG(ERROR) << absl::StrCat(absl::StrFormat(
    "The message %s is even NOT found in fallback, aborting...",
    message.toStdString()));
  return message;
}

}  // namespace loader
}  // namespace panel
