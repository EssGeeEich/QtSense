#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QStringListModel>
#include "qtsense.h"
#include "networkaccess.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();
private:
	void createChannelSlider(QString ch, int position);
	void prepareChannelSlider(QString ch);
	
	void onSettingsChanged();
	void onPacksChanged();
	void onPackListItemChanged(QListWidgetItem*);
	void onPackListItemChangedExternally(QString, bool);
	void onSliderValueChanged(QString const& ch, int position);
	void onLineProcessed(QString);
	void onCommandProcessed(QStringList);
	
private slots:
	void on_actionSettings_triggered();
	void on_actionReload_Packs_triggered();
	void on_testGamelogPushButton_clicked();
	void on_masterSlider_valueChanged(int value);
	
private:
	Ui::MainWindow *ui;

	QtSense* m_sense;
	NetworkAccess* m_network;
	QStringListModel* m_logging;
	bool m_loadingChannels;
};
#endif // MAINWINDOW_H
