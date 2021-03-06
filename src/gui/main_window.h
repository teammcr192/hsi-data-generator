// The main window of the application contains all of the other widgets
// (defined in other files) and provides a uniform global menu and tab system
// for the user to switch between widgets and app functions.

#ifndef SRC_GUI_MAIN_WINDOW_H_
#define SRC_GUI_MAIN_WINDOW_H_

#include <QMainWindow>

#include <memory>
#include <vector>

#include "gui/class_spectra_view.h"
#include "gui/image_layout_view.h"
#include "hsi/image_layout.h"
#include "hsi/spectrum.h"

namespace hsi_data_generator {

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow();

 private slots:  // NOLINT
  // File menu actions:
  void NewActionCalled();
  void OpenActionCalled();
  void ResetActionCalled();
  void SaveActionCalled();

 private:
  // The spectra and image layout are shared between all GUI components of the
  // window. The GUI interacts with them to modify the spectral dictionary and
  // image layout. The number of bands is also shared between the different
  // widgets and needed for saving the project.
  std::shared_ptr<int> num_bands_;
  std::shared_ptr<std::vector<std::shared_ptr<Spectrum>>> spectra_;
  std::shared_ptr<ImageLayout> image_layout_;

  // The widgets contained by this window:
  ClassSpectraView* class_spectra_view_ = nullptr;
  ImageLayoutView* image_layout_view_ = nullptr;
};

}  // namespace hsi_data_generator

#endif  // SRC_GUI_MAIN_WINDOW_H_
