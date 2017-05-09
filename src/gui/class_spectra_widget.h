// This widget provides a GUI for creating and manipulating new spectral
// signatures. These spectra serve as the core spectral classes to construct
// the synthesized HSI data.
//
// As a very simple example, for an RGB image, each class would be defined as a
// unique color, and the image would be constructed by combining these colors
// in a particular spatial layout across the image.

#ifndef SRC_GUI_CLASS_SPECTRA_WIDGET_H_
#define SRC_GUI_CLASS_SPECTRA_WIDGET_H_

#include <QLayout>
#include <QLineEdit>
#include <QString>
#include <QWidget>

#include <vector>

#include "gui/class_spectrum_row.h"

namespace hsi_data_generator {

class ClassSpectraWidget : public QWidget {
  Q_OBJECT

 public:
  ClassSpectraWidget();

 private slots:  // NOLINT
  void NumberOfBandsInputChanged();

 private:
  // The layout used by this widget.
  QLayout* layout_;

  // The input field where the user can set the number of bands.
  QLineEdit* number_of_bands_input_;

  // A list of all added ClassSpectrumRow widgets. These are referenced to
  // modify the number of bands or to get the spectra.
  std::vector<ClassSpectrumRow*> class_spectrum_rows_;

  // Adds a new ClassSpectrumRow to the widget. It will be displayed and
  // tracked in the class_spectrum_rows_ list.
  void AddClassSpectrumRow(const QString& name, const int num_bands);
};

}  // namespace hsi_data_generator

#endif  // SRC_GUI_CLASS_SPECTRA_WIDGET_H_
