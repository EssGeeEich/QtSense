#include "mainwindow.h"
#include "sounddevice.h"
#include "sounddeviceenumerator.h"
#include "settings.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
	SoundDeviceEnumerator sde;
	std::vector<std::string> devices = sde.getSoundDevices();
	if(devices.empty())
	{
		QMessageBox::critical(nullptr,
			QObject::tr("Sound System error."),
			QObject::tr("No sound devices have been found."));
		return a.exec();
	}
	
	Set::g_settings.beginGroup("Sound");
	std::string prefDevice = Set::stringSetting("SelectedDevice").toStdString();
	Set::g_settings.endGroup();
	
	std::string selDevice = devices.front();
	for(auto& dev : devices)
	{
		if(dev == prefDevice)
		{
			selDevice = std::move(prefDevice);
			break;
		}
	}
	
	SoundDevice sd(nullptr, selDevice);
	if(!sd.isReady())
	{
		QMessageBox::critical(nullptr,
			QObject::tr("Sound System error."),
			sd.error());
		return a.exec();
	}
	
	if(!sd.select())
	{
		QMessageBox::critical(nullptr,
			QObject::tr("Sound System error."),
			sd.error());
		return a.exec();
	}

	MainWindow w;
	w.show();
	return a.exec();
}
