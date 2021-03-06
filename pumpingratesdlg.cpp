#include "pumpingratesdlg.h"
#include "importdlg.h"
#include "timeseries.h"
#include "well.h"


#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QWidget>

PumpingRatesDlg::PumpingRatesDlg(awfm::Model *model)
{
    wells_ = model->wells();
    setWindowTitle("Edit Pumping Rates");
    initTables();
    initWidgets();
    methodChanged();
    initMethodLayouts();
    initLayout();
}

void PumpingRatesDlg::initWidgets()
{
    recordCountLabel = new QLabel("");
    setRecordCount(0);

    methodLabel = new QLabel("Method");
    methodComboBox = new QComboBox();

    wellsLabel = new QLabel("Well: ");
    wellsComboBox = new QComboBox();
    for (int i = 0; i < wells_.size(); i++) {
        wellsComboBox->addItem(wells_[i].name());
    }
    connect(wellsComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(fillTableWithPumpingRates()));
    fillTableWithPumpingRates();

    applyToAllWellsCheckBox = new QCheckBox("Apply to All Wells");
    applyToAllWellsCheckBox->setChecked(true);


    editWidgets["Erroneous/Missing Data"] = new QWidget();
    errorCodeLabel = new QLabel("Value: ");
    errorCodeLineEdit = new QLineEdit();
    onErrorLabel = new QLabel("Perform on Value: ");
    onErrorComboBox = new QComboBox();
    onErrorComboBox->addItems(QStringList()
                              << "Remove"
                              << "Linear Interpolate");
    applyErrorCodeButton = new QPushButton("Apply");


    editWidgets["Range Constraints"] = new QWidget();
    maxValueLabel = new QLabel("Max Value: ");
    maxValueLineEdit = new QLineEdit();
    minValueLabel = new QLabel("Min Value: ");
    minValueLineEdit = new QLineEdit();
    minMagnitudeLabel = new QLabel("Min Magnitude: ");
    minMagnitudeLineEdit = new QLineEdit();
    applyRangeButton = new QPushButton("Apply");

    editWidgets["Project Onto Line"] = new QWidget();
    t0ProjectLabel = new QLabel("t0: ");
    t0ProjectLineEdit = new QLineEdit();
    tfProjectLabel = new QLabel("tf: ");
    tfProjectLineEdit = new QLineEdit();
    dtProjectLabel = new QLabel("dt: ");
    dtProjectLineEdit = new QLineEdit();
    applyProjectionButton = new QPushButton("Apply");

    editWidgets["Data Reduction"] = new QWidget();
    averagingGroupBox = new QGroupBox("Data Reduction");
    averageByToleranceRadio = new QRadioButton("Average by Tolerance");
    averageToleranceLineEdit = new QLineEdit();
    averageBySignRadio = new QRadioButton("Average by Sign");
    applyAveragingButton = new QPushButton("Apply");


    foreach(QString key, editWidgets.keys()) {
        methodComboBox->addItem(key);
    }


    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    connect(methodComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(methodChanged()));

    connect(buttonBox, &QDialogButtonBox::accepted,
            this, &QDialog::accept);

    connect(buttonBox, &QDialogButtonBox::rejected,
            this, &QDialog::reject);

    connect(applyRangeButton, &QPushButton::clicked,
            this, &PumpingRatesDlg::applyRangeButtonClicked);

    connect(applyAveragingButton, &QPushButton::clicked,
            this, &PumpingRatesDlg::applyAveragingButtonClicked);

    connect(applyErrorCodeButton, &QPushButton::clicked,
            this, &PumpingRatesDlg::applyErrorCodeButtonClicked);
}

void PumpingRatesDlg::initTables()
{
    rawTable = new AWFMTableWidget(0, 2, QStringList() << "t" << "Q");
    rawTable->horizontalHeader()->setStretchLastSection(true);

    connect(rawTable, SIGNAL(importSelected()),
            this, SLOT(import()));
}

void PumpingRatesDlg::initMethodLayouts()
{
    // Range Constraints
    QHBoxLayout *rangeLayout = new QHBoxLayout();
    rangeLayout->addWidget(minValueLabel);
    rangeLayout->addWidget(minValueLineEdit);
    rangeLayout->addWidget(maxValueLabel);
    rangeLayout->addWidget(maxValueLineEdit);
    rangeLayout->addWidget(minMagnitudeLabel);
    rangeLayout->addWidget(minMagnitudeLineEdit);

    QHBoxLayout *rangeApplyLayout = new QHBoxLayout();
    rangeApplyLayout->addStretch();
    rangeApplyLayout->addWidget(applyRangeButton);

    QVBoxLayout *rangeLayoutWithApply = new QVBoxLayout();
    rangeLayoutWithApply->addLayout(rangeLayout);
    rangeLayoutWithApply->addLayout(rangeApplyLayout);
    editWidgets["Range Constraints"]->setLayout(rangeLayoutWithApply);

    // Erroneous/Missing Data
    QHBoxLayout *errorCodeLayout = new QHBoxLayout();
    errorCodeLayout->addWidget(errorCodeLabel);
    errorCodeLayout->addWidget(errorCodeLineEdit);
    errorCodeLayout->addWidget(onErrorLabel);
    errorCodeLayout->addWidget(onErrorComboBox);

    QHBoxLayout *errorCodeApplyLayout = new QHBoxLayout();
    errorCodeApplyLayout->addStretch();
    errorCodeApplyLayout->addWidget(applyErrorCodeButton);

    QVBoxLayout *errorCodeLayoutWithApply = new QVBoxLayout();
    errorCodeLayoutWithApply->addLayout(errorCodeLayout);
    errorCodeLayoutWithApply->addLayout(errorCodeApplyLayout);
    editWidgets["Erroneous/Missing Data"]->setLayout(errorCodeLayoutWithApply);

    // Project Onto Line
    QHBoxLayout *projectOntoLineLayout = new QHBoxLayout();
    projectOntoLineLayout->addWidget(t0ProjectLabel);
    projectOntoLineLayout->addWidget(t0ProjectLineEdit);
    projectOntoLineLayout->addWidget(tfProjectLabel);
    projectOntoLineLayout->addWidget(tfProjectLineEdit);
    projectOntoLineLayout->addWidget(dtProjectLabel);
    projectOntoLineLayout->addWidget(dtProjectLineEdit);

    QHBoxLayout *projectOntoLineApplyLayout = new QHBoxLayout();
    projectOntoLineApplyLayout->addStretch();
    projectOntoLineApplyLayout->addWidget(applyProjectionButton);

    QVBoxLayout *projectOntoLineLayoutWithApply = new QVBoxLayout();
    projectOntoLineLayoutWithApply->addLayout(projectOntoLineLayout);
    projectOntoLineLayoutWithApply->addLayout(projectOntoLineApplyLayout);
    editWidgets["Project Onto Line"]->setLayout(projectOntoLineLayoutWithApply);

    // Data Reduction
    QHBoxLayout *averageByToleranceLayout = new QHBoxLayout();
    averageByToleranceLayout->addWidget(averageByToleranceRadio);
    averageByToleranceLayout->addWidget(averageToleranceLineEdit);
    QVBoxLayout *averagingLayout = new QVBoxLayout();
    averagingLayout->addWidget(averageBySignRadio);
    averagingLayout->addLayout(averageByToleranceLayout);

    QHBoxLayout *averagingApplyLayout = new QHBoxLayout();
    averagingApplyLayout->addStretch();
    averagingApplyLayout->addWidget(applyAveragingButton);

    QVBoxLayout *averagingLayoutWithApply = new QVBoxLayout();
    averagingLayoutWithApply->addLayout(averagingLayout);
    averagingLayoutWithApply->addLayout(averagingApplyLayout);
    editWidgets["Data Reduction"]->setLayout(averagingLayoutWithApply);
}

void PumpingRatesDlg::initLayout()
{
    QVBoxLayout *leftLayout = new QVBoxLayout();
    QVBoxLayout *rightLayout = new QVBoxLayout();

    leftLayout->addWidget(rawTable);
    rawTable->setMinimumWidth(350);
    leftLayout->addWidget(recordCountLabel);

    QHBoxLayout *applyToAllWellsLayout = new QHBoxLayout();
    applyToAllWellsLayout->addWidget(methodLabel);
    applyToAllWellsLayout->addWidget(methodComboBox);
    applyToAllWellsLayout->addStretch();
    applyToAllWellsLayout->addWidget(wellsLabel);
    applyToAllWellsLayout->addWidget(wellsComboBox);
    applyToAllWellsLayout->addWidget(applyToAllWellsCheckBox);
    rightLayout->addLayout(applyToAllWellsLayout);

    rightLayout->addWidget(editWidgets["Range Constraints"]);
    rightLayout->addWidget(editWidgets["Erroneous/Missing Data"]);
    rightLayout->addWidget(editWidgets["Project Onto Line"]);
    rightLayout->addWidget(editWidgets["Data Reduction"]);


    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    bottomLayout->addWidget(buttonBox);
    rightLayout->addLayout(bottomLayout);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addLayout(leftLayout);
    topLayout->addLayout(rightLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(topLayout);

    setLayout(mainLayout);
}

void PumpingRatesDlg::fillTableWithPumpingRates()
{
    awfm::Timeseries ts = wells_[wellsComboBox->currentIndex()].q();
    rawTable->setRowCount(ts.size());
    for (int i = 0; i < ts.size(); i++) {
        double t = ts.t(i);
        double v = ts.v(i);
        rawTable->setItem(i, 0, new QTableWidgetItem(QString("%1").arg(t, 0, 'f', 6)));
        rawTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(v, 0, 'f', 6)));
    }
    recordCountLabel->setText(QString("%1 records").arg(ts.size()));
}

void PumpingRatesDlg::setRecordCount(int records)
{
    recordCountLabel->setText(QString("%1 records").arg(records));
}

void PumpingRatesDlg::methodChanged()
{
    foreach (QString key, editWidgets.keys()) {
        editWidgets[key]->setVisible(false);
    }
    editWidgets[methodComboBox->currentText()]->setVisible(true);
}

void PumpingRatesDlg::applyAveragingButtonClicked()
{
    QString selected_well_name = wellsComboBox->currentText();
    for (size_t i = 0; i < wells_.size(); i++) {
        if (averageBySignRadio->isChecked()) {
            if (applyToAllWellsCheckBox->isChecked()
                    || wells_[i].name() == selected_well_name) {
                awfm::Timeseries q = wells_[i].q();
                q.averageBySign();
                wells_[i].setQ(q);
            }
        }
    }

    fillTableWithPumpingRates();
}

void PumpingRatesDlg::applyErrorCodeButtonClicked()
{
    if (errorCodeLineEdit->text().isEmpty()) {
        return;
    }
    double error_code = errorCodeLineEdit->text().toDouble();
    QString on_error_text = onErrorComboBox->currentText();

    QString selected_well_name = wellsComboBox->currentText();
    for (size_t i = 0; i < wells_.size(); i++) {
        if (applyToAllWellsCheckBox->isChecked()
                || wells_[i].name()==selected_well_name) {
            awfm::Timeseries q = wells_[i].q();
            if (on_error_text == "Linear Interpolate") {
                q.interpolateOverValue(error_code);
            } else if (on_error_text == "Remove") {
                q.removeByValue(error_code);
            } else {
                // This line should not be reached.
            }
            wells_[i].setQ(q);
        }
    }
    fillTableWithPumpingRates();
}

void PumpingRatesDlg::applyRangeButtonClicked()
{
    QString selected_well_name = wellsComboBox->currentText();
    for (size_t i = 0; i < wells_.size(); i++) {
        if (!minValueLineEdit->text().isEmpty()) {
            double min_value = minValueLineEdit->text().toDouble();
            if (applyToAllWellsCheckBox->isChecked()
                || wells_[i].name() == selected_well_name) {
                awfm::Timeseries q = wells_[i].q();
                q.setMinValue(min_value);
                wells_[i].setQ(q);
            }
        }
        if (!maxValueLineEdit->text().isEmpty()) {
            double max_value = maxValueLineEdit->text().toDouble();
            if (applyToAllWellsCheckBox->isChecked()
                || wells_[i].name() == selected_well_name) {
                awfm::Timeseries q = wells_[i].q();
                q.setMaxValue(max_value);
                wells_[i].setQ(q);
            }
        }
        if (!minMagnitudeLineEdit->text().isEmpty()) {
            double min_magnitude = minMagnitudeLineEdit->text().toDouble();
            if (applyToAllWellsCheckBox->isChecked()
                || wells_[i].name() == selected_well_name) {
                awfm::Timeseries q = wells_[i].q();
                q.setMinMagnitude(min_magnitude);
                wells_[i].setQ(q);
            }
        }
    }
    fillTableWithPumpingRates();

}

void PumpingRatesDlg::cellChanged(QTableWidgetItem *item)
{

}

void PumpingRatesDlg::import()
{
    int well_idx = wellsComboBox->currentIndex();

    QStringList targets;
    targets << "t" << "Q";
    ImportDlg *dlg = new ImportDlg(targets);

    if (!dlg->exec()) {
        return;
    }

    awfm::AbstractDataFrame *df = dlg->df();
    QMap<QString,QString> target_map = dlg->getTargetMap();
    df->windUp();

    if (df->hasError()) {
        QMessageBox::warning(this, tr("awfm"),
            df->error(), QMessageBox::Ok);
        return;
    }

    awfm::Timeseries ts;

    bool reading_values = false;
    bool values_out_of_order = false;
    while (df->nextRow()) {

        double t, v;

        if (df->isNull(df->columnIndex(target_map["t"]))) {
            continue;
        } else {
            t = df->getDouble(df->columnIndex(target_map["t"]));
        }

        if (!df->isNull(df->columnIndex(target_map["Q"]))) {
            reading_values = true;
            v = df->getDouble(df->columnIndex(target_map["Q"]));
        } else {
            v = -9999;
        }

        if (reading_values) {
            if (!ts.append(t, v))
            {
                values_out_of_order = true;
            }
        }
    }

    if (values_out_of_order) {
        QMessageBox::warning(this, "Source File Error",
            "Not all values were imported. Time column is not"
            " in chronological order or contains duplicates.");
    }

    wells_[well_idx].setQ(ts);

    fillTableWithPumpingRates();
    delete df;
}

void PumpingRatesDlg::insertAbove(QList<int> selected_rows)
{

}

void PumpingRatesDlg::insertBelow(QList<int> selected_rows)
{

}

void PumpingRatesDlg::deleteRows(QList<int> selected_rows)
{

}
