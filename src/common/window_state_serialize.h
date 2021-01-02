#ifndef WINDOW_STATE_SERIALIZE_H
#define WINDOW_STATE_SERIALIZE_H

class QSettings;
class QCheckBox;
class QLineEdit;
class QComboBox;
class QMainWindow;
class QAction;

#define toSettings(T) QSettings& operator << (QSettings& proto, const T* node)
#define fromSettings(T) const QSettings& operator >> (const QSettings& proto, T* node)

toSettings(QCheckBox);
fromSettings(QCheckBox);

toSettings(QLineEdit);
fromSettings(QLineEdit);

toSettings(QComboBox);
fromSettings(QComboBox);

toSettings(QAction);
fromSettings(QAction);

toSettings(QMainWindow);
fromSettings(QMainWindow);

#endif // WINDOW_STATE_SERIALIZE_H
