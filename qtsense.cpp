#include "qtsense.h"
#include "settings.h"
#include "xmlutils.h"
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QTimer>
#include <iostream>

QtSense::QtSense(QObject *parent)
	: QObject(parent),
	  m_threshold(Nothing),
	  m_skipping(false)
{}

void QtSense::addMonitor(QString filename)
{
	if(filename.isEmpty())
		return;
	QtSenseFileMonitor* fm = new QtSenseFileMonitor(this, filename);
	if(!fm->isValid())
	{
		delete fm;
		return;
	}

	m_monitors << fm;
	connect(this, &QtSense::onUpdate,
			fm, &QtSenseFileMonitor::update);
	connect(fm, &QtSenseFileMonitor::onNewLine,
			this, &QtSense::monitorLineFeed);
}

void QtSense::monitorLineFeed(QString line)
{
	if(m_skipping)
		return;
	processCommand(QStringList() << "MSG" << line);
}

void QtSense::updatePaths(QString gamelog, QString gamelogExtra, QString packs)
{
	for(QtSenseFileMonitor* fm : m_monitors)
	{ delete fm; }
	m_monitors.clear();

	m_gamelog = gamelog;
	m_gamelog_extra = gamelogExtra;
	m_packs = packs;

	addMonitor(m_gamelog);
	addMonitor(m_gamelog_extra);
}

QMap<QString, PackProperties> const& QtSense::Packs() const { return m_loadedPacks; }
void QtSense::SetPackEnabled(QString const& packName, bool enabled)
{
	auto it = m_loadedPacks.find(packName);
	if(it == m_loadedPacks.end())
		return;
	if(it->m_enabled != enabled)
	{
		it->m_enabled = enabled;

		Set::g_settings.beginGroup("EnabledPacks");
		Set::g_settings.setValue(packName, enabled);
		Set::g_settings.endGroup();

		emit onPackStatusChanged(packName, enabled);
	}
}


void QtSense::reloadPacks()
{
	QMap<QString, PackProperties> oldPacks;
	oldPacks.swap(m_loadedPacks);

	QList<PackListInfo> packsList;
	QDirIterator it(m_packs, QDir::NoDotAndDotDot | QDir::Dirs, QDirIterator::NoIteratorFlags);
	while(it.hasNext())
	{
		QDir packSection = QDir(it.next());
		QString dn = packSection.dirName();

		QStringList packFiles = packSection.entryList(QStringList() << "*.xml", QDir::NoDotAndDotDot | QDir::Readable | QDir::Files, QDir::Name | QDir::IgnoreCase);
		for(QString pack : packFiles)
		{
			packsList.push_back(PackListInfo{pack, it.path() + '/' + dn, dn + '/' + pack});
		}
		continue;
	}

	Set::g_settings.beginGroup("EnabledPacks");
	for(PackListInfo pack : packsList)
	{
		PackProperties pp;

		ParseXMLPack(pack, pp);
		
		if(pp.m_valid)
		{
			auto oldIt = oldPacks.find(pack.m_displayname);
			if(oldIt != oldPacks.end())
			{
				pp.m_enabled = oldIt->m_enabled;
			}
		}

		m_loadedPacks.insert(pack.m_displayname, pp);
	}
	Set::g_settings.endGroup();

	emit onPacksReloaded();
}

void QtSense::setThreshold(Threshold thres)
{
	if(thres < m_threshold)
		stopAllSounds();

	m_threshold = thres;
}

bool QtSense::isReady() const
{
	return QDir(m_packs).exists();
}

bool QtSense::isModern() const
{
	return (!QFile(m_gamelog).exists())
			&& !(QFile(m_gamelog_extra).exists());
}

void QtSense::update()
{
	emit onUpdate();
}

void QtSense::skipAll()
{
	m_skipping = true;
	try {
		emit onUpdate();
	}
	catch (...) {
		m_skipping = false;
		throw;
	}

	m_skipping = false;
}

void QtSense::stopAllSounds()
{
	for(auto it = m_loadedPacks.begin(); it != m_loadedPacks.end(); ++it)
	{
		for(GameLineInfo& gli : it->m_lineinfos)
		{
			for(SoundVariant& sv : gli.m_variants)
			{
				if(sv.m_sound)
					sv.m_sound->stopAll();
			}
		}
	}
}

void QtSense::processCommand(QStringList args)
{
	if(!args.size())
		return;
	
	if(args[0] == "MSG")
	{
		for(int i = 1; i < args.size(); ++i)
		{
			processLine(args[i]);
		}
	}
	
	emit onCommandProcessed(args);
}

void QtSense::processLine(QString line)
{
	for(auto it = m_loadedPacks.begin(); it != m_loadedPacks.end(); ++it)
	{
		if(!it->m_enabled || !it->m_valid)
			continue;
		for(GameLineInfo& gli : it->m_lineinfos)
		{
			// Contains variants
			if(gli.m_variants.isEmpty())
				continue;
			
			// Threshold Match
			if(gli.m_threshold > m_threshold)
				continue;

			// Regex Match
			if(gli.m_regex.indexIn(line) < 0)
				continue;

			// Concurrency Match
			if(gli.m_concurrency > 0)
			{
				int concurrent = 0;
				for(SoundVariant& sv : gli.m_variants)
				{
					if(sv.m_sound)
						concurrent += sv.m_sound->concurPlays();
				}
				if(concurrent >= gli.m_concurrency)
					break;
			}

			// Probability Match
			if(gli.m_probability < gli.m_rngProbability.draw())
				continue;

			// Select a Variant
			SoundVariant* selVariant = &gli.m_variants[0];
			if(gli.m_variants.size() > 1)
			{
				int var = gli.m_rngVariant.draw();
				int ixVariant = 0;
				do
				{
					++ixVariant;
					ixVariant = ixVariant % gli.m_variants.size();
					var -= gli.m_variants[ixVariant].m_weight;
				} while(var > 0);
				selVariant = &gli.m_variants[ixVariant];
			}

			if(!selVariant->m_sound)
			{
				if(!InitializeVariantSound(gli, *selVariant))
				{
					std::cerr << "Trying to play invalid sound variant:" << std::endl
							  << " File: " << selVariant->m_filename.toStdString() << std::endl
							  << " Event: " << line.toStdString() << std::endl;
				}
			}
			
			if(selVariant->m_sound)
			{
				SoundInstance* instance =
					InitializeVariantSoundInst(gli, *selVariant);
				if(!instance)
				{
					std::cerr << "Unable to create a new instance of sound variant:" << std::endl
							  << " File: " << selVariant->m_filename.toStdString() << std::endl
							  << " Event: " << line.toStdString() << std::endl;
				}
				else
				{
					instance->start(gli.m_channel);
				}
			}
		}
	}
	emit onLineProcessed(line);
}
