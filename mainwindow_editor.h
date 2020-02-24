#ifndef MAINWINDOW_EDITOR_H
#define MAINWINDOW_EDITOR_H
#include <QString>
#include <QStringList>
#include <QDir>
#include <QListWidgetItem>
#include <QVector>
#include "qtsense_structs.h"
#include "eventeditor.h"

struct EditorData {
	inline EditorData() :
		m_editItem(nullptr),
		m_editGLI(nullptr),
		m_eventEditor(nullptr),
		m_dirty(false)
	{}
	PackProperties m_pack;
	QDir m_basedir;
	
	QListWidgetItem* m_editItem;
	GameLineInfo* m_editGLI;
	EventEditor* m_eventEditor;
	bool m_dirty;
};

#endif // MAINWINDOW_EDITOR_H
