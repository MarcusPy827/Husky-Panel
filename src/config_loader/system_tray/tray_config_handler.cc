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

#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <QTemporaryFile>

#include <vector>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "absl/log/log.h"
#include "absl/log/check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"

#include "src/config_loader/system_tray/tray_config_handler.h"
#include "src/config_loader/path_finder/path_finder.h"

namespace panel {
namespace loader {

/**
 * @brief The constructor of the system tray configuration handler.
 * 
 * @note It is here so we could register this one to the QML engine providers.
 * @return void
 */

TrayConfigHandler::TrayConfigHandler(QObject* parent) : QObject(parent) {
  LOG(INFO) << absl::StrCat("TrayConfigHandler constructor invoked!!");
}


/**
 * @brief The destructor of the system tray configuration handler.
 * 
 * @return void
 */
TrayConfigHandler::~TrayConfigHandler() {
  LOG(INFO) << absl::StrCat("TrayConfigHandler is being deleted...");
}


/**
 * @brief Get a vector of the configurations for recorded system tray icons.
 * 
 * @note Don't ask me why using @c fcntl instead of Qt's own implementation, if
 *       you asked, then my answer is: This is something I like!!!! And don't
 *       judge me out of this...
 * @return (std::vector<TrayConfig>) The vector of the configurations.
 */

std::vector<TrayConfig> TrayConfigHandler::GetTrayConfigs() {
  std::vector<TrayConfig> result;
  QString config_path = PathFinder::GetTrayConfigPath();

  // I know Qt has its own lock and file handling, but this is for Linux only
  // so nvm I am going to use the standard UNIX way...
  int fd = open(config_path.toLocal8Bit().constData(), O_RDWR);
  if (fd == -1) {
    LOG(ERROR) << absl::StrCat("Failed to lock the system tray config file!!");
    return result;
  }

  // Lock the whole file
  struct flock lock;
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;

  LOG(INFO) << absl::StrCat("Attempting to acquire the lock...");
  if (fcntl(fd, F_SETLKW, &lock) == -1) {
    LOG(ERROR) << absl::StrCat("Failed to acquire the lock!!");
    close(fd);
    return result;
  }

  // Read the file
  LOG(INFO) << absl::StrCat("Successfully acquired the lock, now reading...");
  struct stat st;
  fstat(fd, &st);
  QByteArray data(st.st_size, '\0');
  pread(fd, data.data(), st.st_size, 0);

  // To buffer
  QTemporaryFile tmp;
  tmp.open();
  tmp.write(data);
  tmp.flush();

  QSettings settings(tmp.fileName(), QSettings::IniFormat);
  QStringList sections = settings.childGroups();
  for (const QString &section : sections) {
    if (section.isEmpty()) {
      continue;
    }

    TrayConfig config_gen;
    config_gen.id = section;
    config_gen.friendly_name = settings.value(section + "/friendly_name",
      "Unknown").toString();
    config_gen.is_visible = settings.value(section + "/visible", "false")
      .toBool();
    result.push_back(config_gen);
  }

  LOG(INFO) << absl::StrCat("Successfully read the system tray. ",
    "Hey, should I say \"configurations\" just because it does what it is",
    " supposed to do?");
  LOG(INFO) << absl::StrCat("Now releasing the lock...");
  lock.l_type = F_UNLCK;
  fcntl(fd, F_SETLK, &lock);
  close(fd);

  return result;
}


/**
 * @brief Get if the system tray preference for the given id exists.
 * 
 * @param (const QString &) id The id of the system tray icon.
 * @note Don't ask me why using @c fcntl instead of Qt's own implementation, if
 *       you asked, then my answer is: This is something I like!!!! And don't
 *       judge me out of this...
 * @note Defaulting to @c false if opreation failed.
 * @return (bool) True if the preference exists, false otherwise.
 */

bool TrayConfigHandler::IsTrayIconPrefrenceExists(const QString& id) {
  bool result = false;
  QString config_path = PathFinder::GetTrayConfigPath();

  // I know Qt has its own lock and file handling, but this is for Linux only
  // so nvm I am going to use the standard UNIX way...
  int fd = open(config_path.toLocal8Bit().constData(), O_RDWR);
  if (fd == -1) {
    LOG(ERROR) << absl::StrCat("Failed to lock the system tray config file!!");
    return false;
  }

  // Lock the whole file
  struct flock lock;
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;

  LOG(INFO) << absl::StrCat("Attempting to acquire the lock...");
  if (fcntl(fd, F_SETLKW, &lock) == -1) {
    LOG(ERROR) << absl::StrCat("Failed to acquire the lock!!");
    close(fd);
    return false;
  }

  // Read the file
  LOG(INFO) << absl::StrCat("Successfully acquired the lock, now reading...");
  struct stat st;
  fstat(fd, &st);
  QByteArray data(st.st_size, '\0');
  pread(fd, data.data(), st.st_size, 0);

  // To buffer
  QTemporaryFile tmp;
  tmp.open();
  tmp.write(data);
  tmp.flush();

  QSettings settings(tmp.fileName(), QSettings::IniFormat);
  if (settings.childGroups().contains(id)) {
    result = true;
  }

  LOG(INFO) << absl::StrCat("Successfully read the system tray config. ",
    "Hey, should I say \"configurations\" just because it does what it is",
    " supposed to do?");
  LOG(INFO) << absl::StrCat("Now releasing the lock...");
  lock.l_type = F_UNLCK;
  fcntl(fd, F_SETLK, &lock);
  close(fd);

  return result;
}


/**
 * @brief Get if a system tray icon is visible according to the preference.
 * 
 * @param (const QString &) id The id of the system tray icon.
 * @note Don't ask me why using @c fcntl instead of Qt's own implementation, if
 *       you asked, then my answer is: This is something I like!!!! And don't
 *       judge me out of this...
 * @note Defaulting to @c true if opreation failed...
 * @return (bool) True if the preference exists, false otherwise.
 */

bool TrayConfigHandler::IsTrayIconVisible(const QString& id) {
  bool result = true;
  QString config_path = PathFinder::GetTrayConfigPath();

  // I know Qt has its own lock and file handling, but this is for Linux only
  // so nvm I am going to use the standard UNIX way...
  int fd = open(config_path.toLocal8Bit().constData(), O_RDWR);
  if (fd == -1) {
    LOG(ERROR) << absl::StrCat("Failed to lock the system tray config file!!");
    return true;
  }

  // Lock the whole file
  struct flock lock;
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;

  LOG(INFO) << absl::StrCat("Attempting to acquire the lock...");
  if (fcntl(fd, F_SETLKW, &lock) == -1) {
    LOG(ERROR) << absl::StrCat("Failed to acquire the lock!!");
    close(fd);
    return false;
  }

  // Read the file
  LOG(INFO) << absl::StrCat("Successfully acquired the lock, now reading...");
  struct stat st;
  fstat(fd, &st);
  QByteArray data(st.st_size, '\0');
  pread(fd, data.data(), st.st_size, 0);

  // To buffer
  QTemporaryFile tmp;
  tmp.open();
  tmp.write(data);
  tmp.flush();

  QSettings settings(tmp.fileName(), QSettings::IniFormat);
  if (!settings.childGroups().contains(id)) {
    result = true;
  } else {
    bool is_visible = settings.value(id + "/visible", "false").toBool();
    result = is_visible;
  }

  LOG(INFO) << absl::StrCat("Successfully read the system tray config. ",
    "Hey, should I say \"configurations\" just because it does what it is",
    " supposed to do?");
  LOG(INFO) << absl::StrCat("Now releasing the lock...");
  lock.l_type = F_UNLCK;
  fcntl(fd, F_SETLK, &lock);
  close(fd);

  return result;
}


/**
 * @brief Set if a system tray icon is visible.
 * 
 * @param (const QString &) id The id of the system tray icon.
 * @param (bool) visible Whether the system tray icon is visible.
 * @note Don't ask me why using @c fcntl instead of Qt's own implementation, if
 *       you asked, then my answer is: This is something I like!!!! And don't
 *       judge me out of this...
 * @return void.
 */

void TrayConfigHandler::SetTrayIconVisible(const QString& id, bool visible) {
  QString config_path = PathFinder::GetTrayConfigPath();

  // I know Qt has its own lock and file handling, but this is for Linux only
  // so nvm I am going to use the standard UNIX way...
  int fd = open(config_path.toLocal8Bit().constData(), O_RDWR);
  if (fd == -1) {
    LOG(ERROR) << absl::StrCat("Failed to lock the system tray config file!!");
    return;
  }

  // Lock the whole file
  struct flock lock;
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;

  LOG(INFO) << absl::StrCat("Attempting to acquire the lock...");
  if (fcntl(fd, F_SETLKW, &lock) == -1) {
    LOG(ERROR) << absl::StrCat("Failed to acquire the lock!!");
    close(fd);
    return;
  }

  // Read the file
  LOG(INFO) << absl::StrCat("Successfully acquired the lock, now reading...");
  struct stat st;
  fstat(fd, &st);
  QByteArray data(st.st_size, '\0');
  pread(fd, data.data(), st.st_size, 0);

  // To buffer
  QTemporaryFile tmp;
  tmp.open();
  tmp.write(data);
  tmp.flush();

  QSettings settings(tmp.fileName(), QSettings::IniFormat);
  settings.setValue(id + "/visible", visible);
  settings.sync();

  // QSettings::sync() may atomically rename the file (new inode), so we
  // must re-open by path rather than seeking the original FD.
  QFile synced(tmp.fileName());
  synced.open(QIODevice::ReadOnly);
  QByteArray new_data = synced.readAll();
  synced.close();
  ftruncate(fd, 0);
  pwrite(fd, new_data.constData(), new_data.size(), 0);

  LOG(INFO) << absl::StrCat("Successfully wrote the system tray config. ",
    "Hey, should I say \"configurations\" just because it does what it is",
    " supposed to do?");
  LOG(INFO) << absl::StrCat("Now releasing the lock...");
  lock.l_type = F_UNLCK;
  fcntl(fd, F_SETLK, &lock);
  close(fd);
}


/**
 * @brief Get if the expanding icon should be placed on the left-hand side of
 *        the tray icons.
 * 
 * @note Don't ask me why using @c fcntl instead of Qt's own implementation, if
 *       you asked, then my answer is: This is something I like!!!! And don't
 *       judge me out of this...
 * @note Defaulting to @c false if opreation failed.
 * @return (bool) True if the expanding icon should be placed on the left-hand
 *                side of the tray icons, false otherwise.
 */

bool TrayConfigHandler::IsExpandIconOnLeftHandSide() {
  bool result = false;
  QString config_path = PathFinder::GetTrayExpandingIconConfigPath();

  // I know Qt has its own lock and file handling, but this is for Linux only
  // so nvm I am going to use the standard UNIX way...
  int fd = open(config_path.toLocal8Bit().constData(), O_RDWR);
  if (fd == -1) {
    LOG(ERROR) << absl::StrCat("Failed to lock the system tray config file!!");
    return false;
  }

  // Lock the whole file
  struct flock lock;
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;

  LOG(INFO) << absl::StrCat("Attempting to acquire the lock...");
  if (fcntl(fd, F_SETLKW, &lock) == -1) {
    LOG(ERROR) << absl::StrCat("Failed to acquire the lock!!");
    close(fd);
    return false;
  }

  // Read the file
  LOG(INFO) << absl::StrCat("Successfully acquired the lock, now reading...");
  struct stat st;
  fstat(fd, &st);
  QByteArray data(st.st_size, '\0');
  pread(fd, data.data(), st.st_size, 0);

  // To buffer
  QTemporaryFile tmp;
  tmp.open();
  tmp.write(data);
  tmp.flush();

  QSettings settings(tmp.fileName(), QSettings::IniFormat);
  if (!settings.childGroups().contains("ExpandingIcon")) {
    result = false;
  } else {
    result = settings.value("ExpandingIcon/display_on_left_hand_side",
      "false").toBool();
  }

  LOG(INFO) << absl::StrCat("Successfully read the system tray config. ",
    "Hey, should I say \"configurations\" just because it does what it is",
    " supposed to do?");
  LOG(INFO) << absl::StrCat("Now releasing the lock...");
  lock.l_type = F_UNLCK;
  fcntl(fd, F_SETLK, &lock);
  close(fd);

  return result;
}


/**
 * @brief Set if the expanding icon should be placed on the left-hand side of
 *        the tray icons.
 *
 * @param (bool) on_left Whether the expanding icon should be on the left-hand
 *               side.
 * @note Don't ask me why using @c fcntl instead of Qt's own implementation, if
 *       you asked, then my answer is: This is something I like!!!! And don't
 *       judge me out of this...
 * @return void.
 */

void TrayConfigHandler::SetExpandIconOnLeftHandSide(bool on_left) {
  QString config_path = PathFinder::GetTrayExpandingIconConfigPath();

  // I know Qt has its own lock and file handling, but this is for Linux only
  // so nvm I am going to use the standard UNIX way...
  int fd = open(config_path.toLocal8Bit().constData(), O_RDWR);
  if (fd == -1) {
    LOG(ERROR) << absl::StrCat("Failed to lock the system tray config file!!");
    return;
  }

  // Lock the whole file
  struct flock lock;
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;

  LOG(INFO) << absl::StrCat("Attempting to acquire the lock...");
  if (fcntl(fd, F_SETLKW, &lock) == -1) {
    LOG(ERROR) << absl::StrCat("Failed to acquire the lock!!");
    close(fd);
    return;
  }

  // Read the file
  LOG(INFO) << absl::StrCat("Successfully acquired the lock, now reading...");
  struct stat st;
  fstat(fd, &st);
  QByteArray data(st.st_size, '\0');
  pread(fd, data.data(), st.st_size, 0);

  // To buffer
  QTemporaryFile tmp;
  tmp.open();
  tmp.write(data);
  tmp.flush();

  QSettings settings(tmp.fileName(), QSettings::IniFormat);
  settings.setValue("ExpandingIcon/display_on_left_hand_side", on_left);
  settings.sync();

  // Write modified data back to the actual config file.
  // QSettings::sync() on Linux atomically renames (new inode), so the old
  // QTemporaryFile FD is stale — re-open by path to get the updated content.
  QFile synced(tmp.fileName());
  synced.open(QIODevice::ReadOnly);
  QByteArray new_data = synced.readAll();
  synced.close();
  ftruncate(fd, 0);
  pwrite(fd, new_data.constData(), new_data.size(), 0);

  LOG(INFO) << absl::StrCat("Successfully wrote the system tray config. ",
    "Hey, should I say \"configurations\" just because it does what it is",
    " supposed to do?");
  LOG(INFO) << absl::StrCat("Now releasing the lock...");
  lock.l_type = F_UNLCK;
  fcntl(fd, F_SETLK, &lock);
  close(fd);
}

}  // namespace loader
}  // namespace panel
