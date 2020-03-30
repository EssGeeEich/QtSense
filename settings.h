#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QSettings>
#include "simpleton.h"

namespace Ui {
class Settings;
}

namespace Set {
	extern Simpleton<QSettings> g_settings;
	int intSetting(QString const&, int =0);
	QString stringSetting(QString const&, QString =QString());
	bool boolSetting(QString const&, bool =false);
}

class Settings : public QDialog
{
	Q_OBJECT

public:
	explicit Settings(QWidget *parent = nullptr);
	~Settings();

protected:
	void restore();
	void save();

private slots:
	void on_browseGameLog_clicked();
	void on_browseExtraGameLog_clicked();
	void on_browsePacks_clicked();
	void on_Settings_accepted();

private:
	Ui::Settings *ui;
};

#endif // SETTINGS_H
