#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "soundlistener.h"
#include "sounddevice.h"
#include "xmlutils.h"
#include <QFileDialog>
#include <QFile>
#include <iostream>

void MainWindow::on_actionSave_Pack_triggered()
{
	if(!m_editorData || !m_editorData->m_pack.m_valid)
		return;
	
	QFileInfo qfi(ui->browseSoundPackLineEdit->text());
	
	PackListInfo pack;
	pack.m_filename = qfi.fileName();
	pack.m_basepath = qfi.absolutePath();
	pack.m_displayname = "EDITOR";
	
	SaveXMLPack(pack, m_editorData->m_pack);
}

void MainWindow::on_loadButton_clicked()
{
	m_editorData.reset();
	QFileInfo qfi(ui->browseSoundPackLineEdit->text());
	
	PackListInfo pack;
	pack.m_filename = qfi.fileName();
	pack.m_basepath = qfi.absolutePath();
	pack.m_displayname = "EDITOR";
	
	std::unique_ptr<EditorData> newData(new EditorData);
	ParseXMLPack(pack, newData->m_pack);
	if(!newData->m_pack.m_valid)
		return;
	
	newData->m_pack.m_enabled = true;
	m_editorData.swap(newData);
}


void MainWindow::on_browseSoundPackButton_clicked()
{
    QString r = QFileDialog::getOpenFileName(this, tr("Browse XML file..."),
		QString(), QString("XML Files (*.xml);;All Files (*)"), nullptr,
		QFileDialog::DontResolveSymlinks);
	if(!r.isEmpty())
		ui->browseSoundPackLineEdit->setText(r);
}
