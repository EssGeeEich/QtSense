#include "xmlutils.h"
#include "settings.h"
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QFile>
#include <QDir>
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

bool SaveXMLPack(PackListInfo const& pack, PackProperties const& pp)
{
	QDir basePath(pack.m_basepath);
	try {
		QFile xmlFile(pp.m_basepath + '/' + pp.m_filename);
		if(!xmlFile.open(QFile::WriteOnly | QFile::Text))
			throw std::runtime_error("Cannot create the XML file.");
		QDomDocument xml(pack.m_displayname);
		
		QDomElement sounds = xml.createElement("sounds");
		
		for(GameLineInfo const& gli : pp.m_lineinfos)
		{
			QDomElement sound = xml.createElement("sound");
			sound.setAttribute("pattern", gli.m_regex.pattern());
			sound.setAttribute("channel", gli.m_channel.toLower());
			if(gli.m_loop)
				sound.setAttribute("loop", "true");
			if(gli.m_threshold != TH_DEFAULT)
				sound.setAttribute("threshold", static_cast<int>(gli.m_threshold));
			if(gli.m_delay != 0)
				sound.setAttribute("delay", gli.m_delay);
			if(gli.m_concurrency > 0)
				sound.setAttribute("concurrency", gli.m_concurrency);
			if(gli.m_timeout > 0)
				sound.setAttribute("timeout", gli.m_timeout);
			if(gli.m_probability < 100)
				sound.setAttribute("probability", gli.m_probability);
			
			for(SoundVariant const& variant : gli.m_variants)
			{
				QDomElement soundFile = xml.createElement("soundFile");
				
				soundFile.setAttribute("fileName", variant.m_relfilename);
				if(variant.m_weight != DEFAULT_SOUND_WEIGHT)
					soundFile.setAttribute("weight", variant.m_weight);
				if(variant.m_volume != 1.f)
					soundFile.setAttribute("volume", variant.m_volume);
				if(variant.m_balance != 0.f)
					soundFile.setAttribute("balance", variant.m_balance);
				if(variant.m_rngBalance.get())
					soundFile.setAttribute("randomBalance", "true");
				
				sound.appendChild(soundFile);
			}
			
			sounds.appendChild(sound);
		}
		
		xml.appendChild(sounds);
		QTextStream textStream(&xmlFile);
		textStream << xml.toString();
		if(textStream.status() != QTextStream::Ok)
			throw std::runtime_error("Error writing data to the XML file.");
	}  catch (std::exception& e) {
		std::cerr << "Exception saving to an XML file: " << e.what() << std::endl
				  << "(Pack '" << pack.m_displayname.toStdString() << "')"
				  << std::endl;
		return false;
	} catch(...) {
		std::cerr << "Unknown exception saving to an XML file." << std::endl
				  << "(Pack '" << pack.m_displayname.toStdString() << "')"
				  << std::endl;
		return false;
	}
	return true;
}

bool ParseXMLPack(PackListInfo const& pack, PackProperties& pp, bool loadAll)
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
				if(!loadAll && !gli.m_regex.isValid())
				{
					std::cerr << "Invalid regex pattern: '" << logPattern.toStdString()
							  << "' in pack '" << pack.m_displayname.toStdString()
							  << "'. Entry ignored." << std::endl;
					continue;
				}

				gli.m_channel = domElStr(soundEl,"channel", "sfx").toLower();
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
					variant.m_relfilename = soundFileEl.attribute("fileName");
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
				if(!loadAll && gli.m_variants.isEmpty())
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
		pp.m_valid = false;
		return false;
	}
	catch(...) {
		std::cerr << "Unknown exception parsing an XML file." << std::endl
				  << "Pack '" << pack.m_displayname.toStdString()
					<< "' will be disabled." << std::endl;
		pp.m_lineinfos.clear();
		pp.m_valid = false;
		return false;
	}

	if(!loadAll && pp.m_lineinfos.isEmpty())
		pp.m_valid = false;
	
	return pp.m_valid;
}
