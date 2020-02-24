#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "soundlistener.h"
#include "sounddevice.h"
#include "xmlutils.h"
#include <QFileDialog>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFile>
#include <iostream>

void MainWindow::closeEvent(QCloseEvent* event)
{
	if(runDirtyUserCheck())
		event->accept();
	else
		event->ignore();
}

bool MainWindow::runDirtyUserCheck()
{
	if(!m_editorData
	   || !m_editorData->m_dirty)
		return true;
	int rv = QMessageBox::question(this, tr("Do you want to save?"),
		tr("A sound pack has been edited.\r\nDo you want to save it?"),
		QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
		QMessageBox::Cancel);
	switch(rv)
	{
	case QMessageBox::Yes:
		if(saveXML())
			return true;
		return false;
	case QMessageBox::No:
		return true;
	default:
	case QMessageBox::Cancel:
		return false;
	}
}

bool MainWindow::saveXML()
{
	if(!m_editorData || !m_editorData->m_pack.m_valid)
		return false;
	
	QFileInfo qfi(ui->browseSoundPackLineEdit->text());
	
	PackListInfo pack;
	pack.m_filename = qfi.fileName();
	pack.m_basepath = qfi.absolutePath();
	pack.m_displayname = "EDITOR";
	
	if(!SaveXMLPack(pack, m_editorData->m_pack))
	{
		QMessageBox::warning(this, tr("Error saving XML file"), tr("An error happened trying to save the XML file."));
		return false;
	}
	m_editorData->m_dirty = false;
	return true;
}

void MainWindow::on_actionSave_Pack_triggered()
{
	saveXML();
}

void MainWindow::on_loadButton_clicked()
{
	if(!runDirtyUserCheck())
		return;
	ui->eventsList->clear();
	m_editorData.reset();
	ui->saveButton->setEnabled(false);
	ui->eventsGroup->setEnabled(false);
	
	QFileInfo qfi(ui->browseSoundPackLineEdit->text());
	
	PackListInfo pack;
	pack.m_filename = qfi.fileName();
	pack.m_basepath = qfi.absolutePath();
	pack.m_displayname = "EDITOR";
	
	std::unique_ptr<EditorData> newData(new EditorData);
	ParseXMLPack(pack, newData->m_pack, true);
	if(!newData->m_pack.m_valid)
	{
		QMessageBox::warning(this, tr("Error loading XML file"), tr("An error happened trying to load the XML file."));
		return;
	}
	
	newData->m_pack.m_enabled = true;
	newData->m_basedir = qfi.absoluteDir();
	m_editorData.swap(newData);
	
	ui->currentlyLoadedPackLabel->setText(m_editorData->m_pack.m_filename);
	ui->saveButton->setEnabled(true);
	ui->eventsGroup->setEnabled(true);
	
	for(GameLineInfo& gli : m_editorData->m_pack.m_lineinfos)
	{
		ui->eventsList->addItem(gli.m_regex.pattern());
	}
}


void MainWindow::on_browseSoundPackButton_clicked()
{
    QString r = QFileDialog::getOpenFileName(this, tr("Browse XML file..."),
		QString(), QString("XML Files (*.xml);;All Files (*)"), nullptr,
		QFileDialog::DontResolveSymlinks);
	if(!r.isEmpty())
		ui->browseSoundPackLineEdit->setText(r);
	if(!m_editorData)
		on_loadButton_clicked();
}


void MainWindow::on_deleteSelEvent_clicked()
{
	if(!m_editorData)
		return;
    int row = ui->eventsList->currentRow();
	if(row < 0 || row >= m_editorData->m_pack.m_lineinfos.size())
		return;
	delete ui->eventsList->takeItem(row);
	m_editorData->m_pack.m_lineinfos.removeAt(row);
	m_editorData->m_dirty = true;
}

void MainWindow::on_editSelEvent_clicked()
{
	if(!m_editorData)
		return;
	if(m_editorData->m_eventEditor)
		return;
	int row = ui->eventsList->currentRow();
	if(row < 0 || row >= m_editorData->m_pack.m_lineinfos.size())
		return;
	
	QListWidgetItem* item = ui->eventsList->item(row);
	if(!item)
		return;
	
	m_editorData->m_editItem = item;
	m_editorData->m_editGLI = &(m_editorData->m_pack.m_lineinfos[row]);
	m_editorData->m_eventEditor
		= new EventEditor(this, *(m_editorData->m_editGLI), m_editorData->m_basedir);
	connect(m_editorData->m_eventEditor, &QDialog::accepted,
			this, &MainWindow::soundEdited);
	m_editorData->m_eventEditor->show();
}

void MainWindow::soundEdited()
{
	if(!m_editorData
	   || !m_editorData->m_editGLI
	   || !m_editorData->m_editItem
	   || !m_editorData->m_eventEditor)
		return;
	
	m_editorData->m_dirty = true;
	*(m_editorData->m_editGLI) = m_editorData->m_eventEditor->GetGLI();
	m_editorData->m_editItem->setText(m_editorData->m_editGLI->m_regex.pattern());
	
	m_editorData->m_editItem = nullptr;
	m_editorData->m_editGLI = nullptr;
	m_editorData->m_eventEditor = nullptr;
}

void MainWindow::on_createNewEvent_clicked()
{
	if(!m_editorData)
		return;
	GameLineInfo gliNew;
	gliNew.m_regex = QRegExp("^New Event$");
	gliNew.m_channel = "sfx";
	gliNew.m_loop = false;
	gliNew.m_threshold = TH_DEFAULT;
	gliNew.m_delay = 0;
	gliNew.m_concurrency = 0;
	gliNew.m_timeout = 0;
	gliNew.m_probability = 100;
	
	m_editorData->m_pack.m_lineinfos.push_back(gliNew);
	ui->eventsList->addItem(gliNew.m_regex.pattern());
	m_editorData->m_dirty = true;
}
