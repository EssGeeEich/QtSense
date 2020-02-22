#include "xmlutils.h"
#include "settings.h"
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QFile>
#include <iostream>

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

void SaveXMLPack(PackListInfo const&, PackProperties const&)
{
	std::cerr << "SaveXMLPack: Not implemented" << std::endl;
}

void ParseXMLPack(PackListInfo const& pack, PackProperties& pp)
{
	pp.m_enabled = Set::boolSetting(pack.m_displayname, true);
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
}
