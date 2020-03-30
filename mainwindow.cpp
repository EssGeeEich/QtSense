#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "soundlistener.h"
#include "sounddevice.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	  ui(new Ui::MainWindow),
	  m_sense(new QtSense(this)),
	  m_network(new NetworkAccess(this)),
	  m_logging(nullptr),
	  m_loadingChannels(false)
{
	ui->setupUi(this);
	
#ifdef LOGGING
	m_logging = new QStringListModel(this);
	ui->logText->setModel(m_logging);
#endif
	
	if(!m_logging)
	{
		ui->debugTab->setEnabled(false);
		ui->debugTab->setVisible(false);
		ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->debugTab));
	}
	
#ifdef NOEDITOR
	{
		ui->editorTab->setEnabled(false);
		ui->editorTab->setVisible(false);
		ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->editorTab));
	}
#endif
	
	
	connect(m_sense, &QtSense::onPacksReloaded,
			this, &MainWindow::packsChanged);
	connect(ui->packsListWidget, &QListWidget::itemChanged,
			this, &MainWindow::packListItemChanged);
	connect(g_soundDevice->channelMgr(), &SoundChannelManager::onChannelDiscovered,
			this, &MainWindow::prepareChannelSlider);
	connect(m_network, &NetworkAccess::newEvent,
			m_sense, &QtSense::processCommand);
	
	connect(m_sense, &QtSense::onCommandProcessed,
			this, &MainWindow::commandProcessed);
	
	settingsChanged();
	m_sense->reloadPacks();
}

MainWindow::~MainWindow()
{
	Set::g_settings->sync();
	delete ui;
}

void MainWindow::commandProcessed(QStringList args)
{
	if(args.isEmpty() || args[0] == "MSG")
		return;
	for(int i = 1; i < args.size(); ++i)
	{
		if(m_logging && m_logging->insertRow(m_logging->rowCount()))
		{
			QModelIndex index = m_logging->index(m_logging->rowCount() - 1, 0);
			m_logging->setData(index, args[i]);
			
			ui->logText->scrollToBottom();
		}
	}
}

void MainWindow::settingsChanged()
{
	Set::g_settings->beginGroup("Paths");
	QString gamelog = Set::stringSetting("Gamelog");
	QString gamelog_extra = Set::stringSetting("GamelogExtra");
	QString packs = Set::stringSetting("Packs");
	Set::g_settings->endGroup();
	Set::g_settings->beginGroup("General");
	Threshold thres = static_cast<Threshold>(Set::intSetting("Threshold", Everything));
	int port = Set::intSetting("Port", 27970);
	bool allowRemote = Set::boolSetting("AllowRemote", false);
	Set::g_settings->endGroup();
	Set::g_settings->beginGroup("Volume");
	QStringList keys = Set::g_settings->childKeys();
	keys.sort(Qt::CaseInsensitive);
	m_loadingChannels = true;
	for(QString const& channel : keys)
	{
		if(channel == "Master")
		{
			ui->masterSlider->setValue(Set::intSetting("Master", 100));
		}
		else if(channel.startsWith("Ch_"))
		{
			QString channelName = channel.right(channel.size() - 3);
			QSlider* slider = findChild<QSlider*>(QString::fromUtf8("Ch_%1_Slider").arg(channelName));
			if(slider)
			{
				slider->setValue(Set::intSetting(channel, 100));
			}
			else
			{
				createChannelSlider(
					channelName,
					Set::intSetting(channel, 100)
				);
			}
		}
	}
	m_loadingChannels = false;
	Set::g_settings->endGroup();

	if(port > 0 && port <= 65535)
	{
		if((!m_network->isRunning()) ||
		   port != m_network->port() ||
		   allowRemote != m_network->allowRemoteAccess())
		{
			m_network->stop();
			m_network->setPort(port);
			m_network->setAllowRemoteAccess(allowRemote);
			m_network->start();
		}
	}
	else
		m_network->stop();
	
	m_sense->setThreshold(thres);
	m_sense->updatePaths(gamelog, gamelog_extra, packs);

	ui->statusBar->clearMessage();
	if(!m_sense->isReady())
	{
		ui->statusBar->showMessage(tr("Not Ready: Missing Sounds folder."), 5000);
	}
	else if(m_sense->isModern())
	{
		ui->statusBar->showMessage(tr("Ready: Modern Mode."), 5000);
	}
	else
	{
		ui->statusBar->showMessage(tr("Ready: Legacy/Hybrid Mode."), 5000);
	}
	m_sense->skipAll();
}

void MainWindow::packsChanged()
{
	ui->packsListWidget->clear();

	QMap<QString, PackProperties> Packs = m_sense->Packs();
	for(auto it = Packs.begin(); it != Packs.end(); ++it)
	{
		QString const& k = it.key();
		PackProperties const& v = it.value();

		QListWidgetItem* item = new QListWidgetItem(k, ui->packsListWidget);

		if(v.m_valid)
		{
			item->setFlags(item->flags()
						   | Qt::ItemIsUserCheckable
						   | Qt::ItemIsEnabled);
		}
		else
		{
			item->setFlags(item->flags()
						   & ~Qt::ItemIsEnabled);
		}
		item->setCheckState(v.m_enabled ? Qt::Checked : Qt::Unchecked);
		ui->packsListWidget->addItem(item);
	}
	return;
}

void MainWindow::packListItemChanged(QListWidgetItem* item)
{
	m_sense->SetPackEnabled(item->text(), item->checkState() == Qt::Checked);
}

void MainWindow::packListItemChangedExternally(QString item, bool status)
{
	auto items = ui->packsListWidget->findItems(item, Qt::MatchExactly);
	for(auto item : items)
	{
		item->setCheckState(status ? Qt::Checked : Qt::Unchecked);
	}
}

void MainWindow::on_actionSettings_triggered()
{
	Settings* s = new Settings(this);
	connect(s, &QDialog::finished, this, &MainWindow::settingsChanged);
	s->show();
}

void MainWindow::on_actionReload_Packs_triggered()
{
	m_sense->reloadPacks();
}

void MainWindow::on_testGamelogPushButton_clicked()
{
    m_sense->processCommand(QStringList() << "MSG" << ui->testGamelogStringLineEdit->text());
}

void MainWindow::createChannelSlider(QString ch, int position)
{
	int row = ui->formLayout_2->rowCount();
	
	QLabel* nLabel = new QLabel(ui->volumeTab);
	nLabel->setObjectName(QString::fromUtf8("Ch_%1_Label").arg(ch));
	{
		QString capitalized = ch;
		for(int i = 0; i < capitalized.size(); ++i)
		{
			if(!i || (capitalized.at(i).isLetter() && !capitalized.at(i-1).isLetter()))
			{
				capitalized[i] = capitalized[i].toTitleCase();
			}
		}
		nLabel->setText(capitalized);
	}
	ui->formLayout_2->setWidget(row, QFormLayout::LabelRole, nLabel);
	
	QSlider* nSlider = new QSlider(ui->volumeTab);
	nSlider->setObjectName(QString::fromUtf8("Ch_%1_Slider").arg(ch));
	nSlider->setMaximum(100);
	nSlider->setValue(position);
	nSlider->setOrientation(Qt::Horizontal);
	nSlider->setTickPosition(QSlider::TicksBelow);
	nSlider->setTickInterval(10);
	ui->formLayout_2->setWidget(row, QFormLayout::FieldRole, nSlider);
	
	g_soundDevice->channelMgr()->SetChannelVolume(
		ch,
		static_cast<float>(position) * 0.01f
	);
	
	MainWindow* mw = this;
	connect(nSlider, &QSlider::valueChanged,
	[=](int position){
		mw->sliderValueChanged(ch, position);
	});
}

void MainWindow::prepareChannelSlider(QString ch)
{
	if(m_loadingChannels)
		return;
	
	Set::g_settings->beginGroup("Volume");
	int position = Set::intSetting(QString("Ch_") + ch, 100);
	Set::g_settings->endGroup();
	createChannelSlider(std::move(ch), position);
}

void MainWindow::sliderValueChanged(QString const& ch, int position)
{
	g_soundDevice->channelMgr()->SetChannelVolume(ch, static_cast<float>(position) * 0.01f);
	
	Set::g_settings->beginGroup("Volume");
	Set::g_settings->setValue(QString("Ch_") + ch, position);
	Set::g_settings->endGroup();
}

void MainWindow::on_masterSlider_valueChanged(int position)
{
	g_currentListener->setGain(static_cast<float>(position) * 0.01f);
	
	Set::g_settings->beginGroup("Volume");
	Set::g_settings->setValue("Master", position);
	Set::g_settings->endGroup();
}
