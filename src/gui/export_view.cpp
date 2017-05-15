#include "gui/export_view.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

#include <memory>
#include <vector>

#include "hsi/hsi_exporter.h"
#include "hsi/image_layout.h"
#include "hsi/spectrum.h"
#include "util/util.h"

namespace hsi_data_generator {
namespace {

static const QString kQtExportViewStyle = "qt_stylesheets/export_view.qss";

static const QString kExportButtonString = "Export HSI";
static const QString kSaveFileDialogName = "Save HSI File";
static const QString kSaveFileErrorDialogName = "File Save Error";

}  // namespace

ExportView::ExportView(
    std::shared_ptr<std::vector<std::shared_ptr<Spectrum>>> spectra,
    std::shared_ptr<ImageLayout> image_layout)
    : spectra_(spectra), image_layout_(image_layout) {

  setStyleSheet(util::GetStylesheetRelativePath(kQtExportViewStyle));

  QVBoxLayout* layout = new QVBoxLayout();
  layout->setAlignment(Qt::AlignTop);
  setLayout(layout);

  QPushButton* export_button = new QPushButton(kExportButtonString);
  layout->addWidget(export_button);
  layout->setAlignment(export_button, Qt::AlignCenter);
  connect(export_button, SIGNAL(released()), this, SLOT(ExportButtonPressed()));
}

void ExportView::ExportButtonPressed() {
  const QString file_name = QFileDialog::getSaveFileName(
      this,
      kSaveFileDialogName,           // Dialog save caption.
      util::GetRootCodeDirectory(),  // Default directory.
      "All Files (*)");              // File filter
  if (!file_name.isEmpty()) {
    HSIDataExporter exporter(spectra_, image_layout_);
    if (!exporter.SaveFile(file_name)) {
      QMessageBox::critical(
          this,
          kSaveFileErrorDialogName,
          exporter.GetErrorMessage());
    }
  }
}

}  // namespace hsi_data_generator
