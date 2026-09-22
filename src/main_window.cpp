#include "main_window.h"
#include "qplaintextedit.h"
#include <QGridLayout>
#include <QPlainTextEdit>

// QT widgets are deleted by their parent widget, hence why calling
// 'new QGridLayout' and giving it a parent and not calling 'delete'
// is okay

namespace OpenConfigEditor {

MainWindow::MainWindow(QWidget *parent) {
  QWidget *centralWidget = new QWidget;

  setWindowTitle(tr("ConfigEditor"));

  QGridLayout *layout = new QGridLayout;

  QPlainTextEdit *text_edit =
      new QPlainTextEdit(tr("Test string"), centralWidget);

  layout->addWidget(text_edit, 0, 0);
  centralWidget->setLayout(layout);

  setCentralWidget(centralWidget);

  // QLabel *modelLabel = new QLabel;
  // modelLabel->setText(tr("Model"));
  //
  // modelCombo = new QComboBox;
  // modelCombo->addItem(tr("QFileSystemModel"));
  // modelCombo->addItem(tr("QFileSystemModel that shows full path"));
  // modelCombo->addItem(tr("Country list"));
  // modelCombo->addItem(tr("Word list"));
  // modelCombo->setCurrentIndex(0);
  //
  // QLabel *modeLabel = new QLabel;
  // modeLabel->setText(tr("Completion Mode"));
  // modeCombo = new QComboBox;
  // modeCombo->addItem(tr("Inline"));
  // modeCombo->addItem(tr("Filtered Popup"));
  // modeCombo->addItem(tr("Unfiltered Popup"));
  // modeCombo->setCurrentIndex(1);
  //
  // QLabel *caseLabel = new QLabel;
  // caseLabel->setText(tr("Case Sensitivity"));
  // caseCombo = new QComboBox;
  // caseCombo->addItem(tr("Case Insensitive"));
  // caseCombo->addItem(tr("Case Sensitive"));
  // caseCombo->setCurrentIndex(0);
  // //! [0]
  //
  // //! [1]
  // QLabel *maxVisibleLabel = new QLabel;
  // maxVisibleLabel->setText(tr("Max Visible Items"));
  // maxVisibleSpinBox = new QSpinBox;
  // maxVisibleSpinBox->setRange(3, 25);
  // maxVisibleSpinBox->setValue(10);
  //
  // wrapCheckBox = new QCheckBox;
  // wrapCheckBox->setText(tr("Wrap around completions"));
  // wrapCheckBox->setChecked(true);
  // //! [1]
  //
  // //! [2]
  // contentsLabel = new QLabel;
  // contentsLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  //
  // connect(modelCombo, &QComboBox::activated, this, &MainWindow::changeModel);
  // connect(modeCombo, &QComboBox::activated, this, &MainWindow::changeMode);
  // connect(caseCombo, &QComboBox::activated, this, &MainWindow::changeCase);
  // connect(maxVisibleSpinBox, &QSpinBox::valueChanged, this,
  //         &MainWindow::changeMaxVisible);
  // //! [2]
  //
  // //! [3]
  // lineEdit = new QLineEdit;
  //
  // QGridLayout *layout = new QGridLayout;
  // layout->addWidget(modelLabel, 0, 0);
  // layout->addWidget(modelCombo, 0, 1);
  // layout->addWidget(modeLabel, 1, 0);
  // layout->addWidget(modeCombo, 1, 1);
  // layout->addWidget(caseLabel, 2, 0);
  // layout->addWidget(caseCombo, 2, 1);
  // layout->addWidget(maxVisibleLabel, 3, 0);
  // layout->addWidget(maxVisibleSpinBox, 3, 1);
  // layout->addWidget(wrapCheckBox, 4, 0);
  // layout->addWidget(contentsLabel, 5, 0, 1, 2);
  // layout->addWidget(lineEdit, 6, 0, 1, 2);
  // centralWidget->setLayout(layout);
  // setCentralWidget(centralWidget);
  //
  // changeModel();
}

} // namespace OpenConfigEditor
