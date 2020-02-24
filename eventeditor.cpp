#include "eventeditor.h"
#include "ui_eventeditor.h"
#include "xmlutils.h"
#include <QFileDialog>

EventEditor::EventEditor(QWidget *parent, GameLineInfo const& gli,
						 QDir base) :
	QDialog(parent),
	ui(new Ui::EventEditor),
	m_basePath(std::move(base)),
	m_gli(gli),
	m_variant(nullptr),
	m_refreshing(false)
{
	ui->setupUi(this);
	
	ui->patternLineEdit->setText(m_gli.m_regex.pattern());
	ui->channelLineEdit->setText(m_gli.m_channel);
	ui->loopingEnabledCheckBox->setChecked(m_gli.m_loop);
	ui->thresholdComboBox->setCurrentIndex(
		MkEnum<Threshold>(Nothing, Everything, m_gli.m_threshold)
	);
	ui->delaySpinBox->setValue(m_gli.m_delay);
	ui->concurrencySpinBox->setValue(m_gli.m_concurrency);
	ui->timeoutSpinBox->setValue(m_gli.m_timeout);
	ui->probabilitySpinBox->setValue(m_gli.m_probability);
	
	m_refreshing = true;
	for(SoundVariant& sv : m_gli.m_variants)
	{
		ui->variantComboBox->addItem(sv.m_relfilename);
	}
	m_refreshing = false;
	
	on_variantComboBox_currentIndexChanged(ui->variantComboBox->currentIndex());
}

GameLineInfo& EventEditor::GetGLI()
{
	return m_gli;
}

EventEditor::~EventEditor()
{
	delete ui;
}

void EventEditor::on_randomLRBalanceCheckBox_stateChanged(int)
{
	ui->lRBalanceDoubleSpinBox->setEnabled(!ui->randomLRBalanceCheckBox->isChecked());
	if(m_variant)
		//										These values don't matter
		m_variant->m_rngBalance = std::make_shared<RandomDouble>(0.0,0.0);
	else
		m_variant->m_rngBalance.reset();
}

void EventEditor::on_variantComboBox_currentIndexChanged(int index)
{
    if(index >= 0 && index < m_gli.m_variants.size())
	{
		m_variant = &(m_gli.m_variants[index]);
		m_refreshing = true;
		ui->fileLineEdit->setText(m_variant->m_relfilename);
		ui->weightSpinBox->setValue(m_variant->m_weight);
		ui->volumeDoubleSpinBox->setValue(m_variant->m_volume);
		ui->lRBalanceDoubleSpinBox->setValue(m_variant->m_balance);
		ui->randomLRBalanceCheckBox->setChecked(
			m_variant->m_rngBalance.get() != nullptr
		);
		m_refreshing = false;
		ui->variantGroupBox->setEnabled(true);
	}
	else
	{
		ui->variantGroupBox->setEnabled(false);
	}
}

void EventEditor::on_fileLineEdit_textChanged(QString const& arg1)
{
    if(m_variant && !m_refreshing)
	{
		m_variant->m_relfilename = arg1;
		int ix = ui->variantComboBox->currentIndex();
		if(ix >= 0 && ix < ui->variantComboBox->count())
			ui->variantComboBox->setItemText(ix, arg1);
	}
}

void EventEditor::on_weightSpinBox_valueChanged(int arg1)
{
    if(m_variant && !m_refreshing)
		m_variant->m_weight = arg1;
}

void EventEditor::on_volumeDoubleSpinBox_valueChanged(double arg1)
{
    if(m_variant && !m_refreshing)
		m_variant->m_volume = arg1;
}

void EventEditor::on_lRBalanceDoubleSpinBox_valueChanged(double arg1)
{
    if(m_variant && !m_refreshing)
		m_variant->m_balance = arg1;
}

void EventEditor::on_deleteVariantButton_clicked()
{
	int row = ui->variantComboBox->currentIndex();
	if(row < 0 || row >= m_gli.m_variants.size())
		return;
	m_variant = nullptr;
	ui->variantComboBox->removeItem(row);
	m_gli.m_variants.removeAt(row);
}

void EventEditor::on_newVariantButton_clicked()
{
    SoundVariant sv;
	sv.m_relfilename = "New Sound";
	sv.m_weight = DEFAULT_SOUND_WEIGHT;
	sv.m_volume = 1.f;
	sv.m_balance = 0.f;
	
	m_gli.m_variants.push_back(sv);
	ui->variantComboBox->addItem(sv.m_relfilename);
	ui->variantComboBox->setCurrentIndex(ui->variantComboBox->count() - 1);
}

void EventEditor::on_browseFileButton_clicked()
{
	if(!m_variant)
		return;
    QString r = QFileDialog::getOpenFileName(this, tr("Browse Sound File..."),
		m_basePath.canonicalPath(),
		QString("Sound files (*.wav *.mp3 *.ogg *.flac;;All Files (*)")
	);
	if(!r.isEmpty())
	{
		ui->fileLineEdit->setText(m_basePath.relativeFilePath(r));
	}
}
