#include "qtsense.h"
#include "Settings.h"
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
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

struct PackListInfo {
	QString m_filename;
	QString m_basepath;
	QString m_displayname;
};

int domElInt(QDomElement& el, QString const& key, int defval)
{
	QString s = el.attribute(key, QString::number(defval));
	bool ss = false;
	int r = s.toInt(&ss);
	if(!ss)
		r = defval;
	return r;
}
float domElFloat(QDomElement& el, QString const& key, float defval)
{
	QString s = el.attribute(key, QString::number(defval));
	bool ss = false;
	float r = s.toFloat(&ss);
	if(!ss)
		r = defval;
	return r;
}
QString domElStr(QDomElement& el, QString const& key, QString defval)
{
	return el.attribute(key, defval);
}
bool domElBool(QDomElement& el, QString const& key, bool defval, QStringList const& trues = QStringList())
{
	QString s = el.attribute(key, defval ? "true" : "false");
	return (s == "true" || trues.contains(s));
}

template <typename T>
T MkEnum(T min, T max, int val)
{
	return static_cast<T>(std::max<int>(min, std::min<int>(max, val)));
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

		auto oldIt = oldPacks.find(pack.m_displayname);
		if(oldIt != oldPacks.end())
		{
			pp.m_enabled = oldIt->m_enabled;
		}
		else
		{
			pp.m_enabled = Set::boolSetting(pack.m_displayname, true);
		}
		
		pp.m_basepath = pack.m_basepath;
		pp.m_filename = pack.m_filename;

		// Cache all regex entries here
		try
		{
			pp.m_valid = true;

			QFile xmlFile(pp.m_basepath + '/' + pp.m_filename);
			if(!xmlFile.open(QFile::ReadOnly | QFile::Text))
				throw std::runtime_error("Cannot open the XML file.");
			QDomDocument xml(pack.m_displayname);
			if(!xml.setContent(&xmlFile))
				throw std::runtime_error("Cannot parse the XML file.");
			xmlFile.close();
			QDomElement root = xml.documentElement();
			for(QDomNode sounds = root; !sounds.isNull(); sounds = sounds.nextSibling())
			{
				if(sounds.nodeName() != "sounds")
					continue;

				for(QDomNode sound = sounds.firstChild();
					!sound.isNull();
					sound = sound.nextSibling())
				{
					if(sound.nodeName() != "sound")
						continue;
					QDomElement soundEl = sound.toElement();
					if(soundEl.isNull())
						continue;

					QString logPattern = soundEl.attribute("pattern");
					if(logPattern.isEmpty())
						continue;

					GameLineInfo gli;
					gli.m_regex.setPattern(logPattern);
					if(!gli.m_regex.isValid())
					{
						std::cerr << "Invalid regex pattern: '" << logPattern.toStdString()
								  << "' in pack '" << pack.m_displayname.toStdString()
								  << "'. Entry ignored." << std::endl;
						continue;
					}

					gli.m_channel = domElStr(soundEl,"channel", "sfx");
					if(gli.m_channel.isEmpty())
						gli.m_channel = "sfx";

					gli.m_loop = domElBool(soundEl, "loop", false,
										   QStringList() << "start" << "true");

					gli.m_threshold = MkEnum<Threshold>(Nothing, Everything, domElInt(soundEl, "threshold", TH_DEFAULT));
					gli.m_delay = domElInt(soundEl, "delay", 0);
					gli.m_concurrency = domElInt(soundEl, "concurrency", 0);
					gli.m_timeout = domElInt(soundEl, "timeout", 0);
					gli.m_probability = domElInt(soundEl, "probability", 100);

					int fullweight = 0;
					for(QDomNode soundFile = sound.firstChild();
						!soundFile.isNull();
						soundFile = soundFile.nextSibling())
					{
						if(soundFile.nodeName() != "soundFile")
							continue;
						QDomElement soundFileEl = soundFile.toElement();
						if(soundFileEl.isNull())
							continue;

						SoundVariant variant;

						QString fileName = pack.m_basepath + '/' + soundFileEl.attribute("fileName");
						// QString url = soundFileEl.attribute(QString("URL"));

						variant.m_weight = domElInt(soundFileEl, "weight", DEFAULT_SOUND_WEIGHT);
						variant.m_volume = domElFloat(soundFileEl, "volume", 1.f);
						variant.m_balance = domElFloat(soundFileEl, "balance", 0.f);
						bool randomBalance = domElBool(soundFileEl, "randomBalance", false);

						if(randomBalance)
							variant.m_rngBalance = std::make_shared<RandomDouble>(-0.5,0.5);
						variant.m_filename = fileName;
						/*
							variant.m_url = url;
							QUrl realUrl;
							if(url.isEmpty())
								realUrl = QUrl::fromLocalFile(fileName);
							else
								realUrl = QUrl(url);
							variant.m_chosenUrl = realUrl;
						*/
						fullweight += variant.m_weight;
						gli.m_variants.push_back(variant);
					}
					if(gli.m_variants.isEmpty())
					{
						std::cerr << "No valid variants for the pattern: '" << logPattern.toStdString()
								  << "' in pack '" << pack.m_displayname.toStdString()
								  << "'. Entry ignored." << std::endl;
						continue;
					}

					gli.m_rngProbability.changeMinMax(1,100);
					gli.m_rngVariant.changeMinMax(0, fullweight);
					pp.m_lineinfos.push_back(gli);
				}
			}
		}
		catch(std::exception& e) {
			std::cerr << "Exception parsing an XML file: " << e.what() << std::endl
					  << "Pack '" << pack.m_displayname.toStdString()
						<< "' will be disabled." << std::endl;
			pp.m_lineinfos.clear();
		}
		catch(...) {
			std::cerr << "Unknown exception parsing an XML file." << std::endl
					  << "Pack '" << pack.m_displayname.toStdString()
						<< "' will be disabled." << std::endl;
			pp.m_lineinfos.clear();
		}

		if(pp.m_lineinfos.isEmpty())
			pp.m_valid = false;

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
	return QFile(m_gamelog).exists()
			&& QDir(m_packs).exists();
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
	if(m_skipping)
		return;
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
