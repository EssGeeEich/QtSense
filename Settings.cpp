#include "Settings.h"
#include "ui_Settings.h"
#include <QFileDialog>
#include "qtsense.h"
#include "sounddeviceenumerator.h"

namespace Set {
	QSettings g_settings;
	int intSetting(QString const& key, int defval)
	{
		QVariant var = g_settings.value(key);
		bool rs=false;
		int val = var.toInt(&rs);
		if(rs)
			return val;
		return defval;
	}
	QString stringSetting(QString const& key, QString defval)
	{
		QVariant var = g_settings.value(key, defval);
		return var.toString();
	}
	bool boolSetting(QString const& key, bool defval)
	{
		QVariant var = g_settings.value(key, defval);
		return var.toBool();
	}
}

Settings::Settings(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Settings)
{
	ui->setupUi(this);
	restore();
}

Settings::~Settings()
{
	delete ui;
}

void Settings::restore()
{
	Set::g_settings.sync();
	Set::g_settings.beginGroup("Paths");
	ui->gameLogPathLineEdit->setText(Set::stringSetting("Gamelog"));
	ui->extraGameLogPathLineEdit->setText(Set::stringSetting("GamelogExtra"));
	ui->packsFolderLineEdit->setText(Set::stringSetting("Packs"));
	Set::g_settings.endGroup();
	Set::g_settings.beginGroup("General");
	ui->soundThresholdComboBox->setCurrentIndex(
		std::max<int>(Nothing,
			std::min<int>(Everything,
				Set::intSetting("Threshold",Everything)
			)
		)
	);
	ui->socketIntegrationPortSpinBox->setValue(
		std::max<int>(0,
			std::min<int>(65535,
				Set::intSetting("Port", 27970)
			)
		)
	);
	ui->allowRemoteAccessCheckBox->setCheckState(
		Set::boolSetting("AllowRemote", false) ? Qt::Checked : Qt::Unchecked
	);
	Set::g_settings.endGroup();
	
	ui->soundDeviceComboBox->clear();
	
	{
		SoundDeviceEnumerator sde;
		std::vector<std::string> devices = sde.getSoundDevices();
		for(std::string const& d : devices)
		{
			QString s = QString::fromStdString(d);
			ui->soundDeviceComboBox->addItem(s, s);
		}
	}
	
	if(!ui->soundDeviceComboBox->count())
		ui->soundDeviceComboBox->setEnabled(false);
	else if(ui->soundDeviceComboBox->count() == 1)
	{
		ui->soundDeviceComboBox->setEnabled(false);
		ui->soundDeviceComboBox->setCurrentIndex(0);
	}
	else
	{
		ui->soundDeviceComboBox->setEnabled(true);
		Set::g_settings.beginGroup("Sound");
		QString selDevice = Set::stringSetting("SelectedDevice");
		bool mSel = false;
		for(int i = 0; i < ui->soundDeviceComboBox->count(); ++i)
		{
			if(ui->soundDeviceComboBox->itemData(i) == selDevice)
			{
				ui->soundDeviceComboBox->setCurrentIndex(i);
				mSel = true;
				break;
			}
		}
		if(!mSel)
			ui->soundDeviceComboBox->setCurrentIndex(0);
		Set::g_settings.endGroup();
	}
}

void Settings::save()
{
	Set::g_settings.beginGroup("Paths");
	Set::g_settings.setValue("Gamelog",ui->gameLogPathLineEdit->text());
	Set::g_settings.setValue("GamelogExtra",ui->extraGameLogPathLineEdit->text());
	Set::g_settings.setValue("Packs",ui->packsFolderLineEdit->text());
	Set::g_settings.endGroup();
	Set::g_settings.beginGroup("General");
	Set::g_settings.setValue("Threshold", ui->soundThresholdComboBox->currentIndex());
	Set::g_settings.setValue("Port", ui->socketIntegrationPortSpinBox->value());
	Set::g_settings.setValue("AllowRemote", (ui->allowRemoteAccessCheckBox->checkState() == Qt::Checked));
	Set::g_settings.endGroup();
	Set::g_settings.beginGroup("Sound");
	Set::g_settings.setValue("SelectedDevice", ui->soundDeviceComboBox->currentData().toString());
	Set::g_settings.endGroup();
	Set::g_settings.sync();
}

void Settings::on_browseGameLog_clicked()
{
	QString r = QFileDialog::getOpenFileName(this, tr("Browse gamelog.txt..."), QString(), QString("gamelog.txt (gamelog.txt);;Text Files (*.txt);;All Files (*)"), nullptr, QFileDialog::DontResolveSymlinks | QFileDialog::ReadOnly);
	if(!r.isEmpty())
		ui->gameLogPathLineEdit->setText(r);
}

void Settings::on_browseExtraGameLog_clicked()
{
	QString r = QFileDialog::getOpenFileName(this, tr("Browse for extra gamelog.txt..."), QString(), QString("Text Files (*.txt);;All Files (*)"), nullptr, QFileDialog::DontResolveSymlinks | QFileDialog::ReadOnly);
	if(!r.isEmpty())
		ui->extraGameLogPathLineEdit->setText(r);
}

void Settings::on_browsePacks_clicked()
{
	QString r = QFileDialog::getExistingDirectory(this, tr("Browse for the Packs folder..."), QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks | QFileDialog::ReadOnly);
	if(!r.isEmpty())
		ui->packsFolderLineEdit->setText(r);
}

void Settings::on_Settings_accepted()
{
	save();
}
