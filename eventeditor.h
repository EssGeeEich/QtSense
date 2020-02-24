#ifndef EVENTEDITOR_H
#define EVENTEDITOR_H

#include <QDialog>
#include <QDir>
#include "qtsense_structs.h"

namespace Ui {
class EventEditor;
}

class EventEditor : public QDialog
{
	Q_OBJECT
	
public:
	explicit EventEditor(QWidget *parent, GameLineInfo const& gli, QDir base);
	~EventEditor();
	
	GameLineInfo& GetGLI();
	
private slots:
	void on_randomLRBalanceCheckBox_stateChanged(int);
	
	void on_fileLineEdit_textChanged(QString const&);
	void on_weightSpinBox_valueChanged(int);
	void on_volumeDoubleSpinBox_valueChanged(double);
	void on_lRBalanceDoubleSpinBox_valueChanged(double);
	
	void on_deleteVariantButton_clicked();
	void on_newVariantButton_clicked();
	void on_browseFileButton_clicked();
	void on_variantComboBox_currentIndexChanged(int);
	
private:
	Ui::EventEditor *ui;
	QDir m_basePath;
	GameLineInfo m_gli;
	SoundVariant* m_variant;
	bool m_refreshing;
};

#endif // EVENTEDITOR_H
