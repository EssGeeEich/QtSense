#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QStringListModel>
#include "qtsense.h"
#include "networkaccess.h"
#include "mainwindow_editor.h"

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
	
	void settingsChanged();
	void soundEdited();
	void packsChanged();
	void packListItemChanged(QListWidgetItem*);
	void packListItemChangedExternally(QString, bool);
	void sliderValueChanged(QString const& ch, int position);
	void commandProcessed(QStringList);
	
	bool runDirtyUserCheck();
	bool saveXML();
protected:
	void closeEvent(QCloseEvent* event);
private slots:
	void on_actionSettings_triggered();
	void on_actionReload_Packs_triggered();
	void on_testGamelogPushButton_clicked();
	void on_masterSlider_valueChanged(int value);
	void on_actionSave_Pack_triggered();
	void on_browseSoundPackButton_clicked();
	void on_loadButton_clicked();
	void on_deleteSelEvent_clicked();
	void on_editSelEvent_clicked();
	void on_createNewEvent_clicked();
	
private:
	Ui::MainWindow *ui;

	QtSense* m_sense;
	NetworkAccess* m_network;
	QStringListModel* m_logging;
	std::unique_ptr<EditorData> m_editorData;
	bool m_loadingChannels;
};
#endif // MAINWINDOW_H
