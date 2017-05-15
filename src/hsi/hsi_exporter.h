// The HSIDataExporter handles creating and saving the final hyperspectral
// image data cube to a binary ENVI file. It combines the individual spectra
// and the image layout to generate the large output data file. It can also
// handle adding noise and other "final touches" to the generated data.

#ifndef SRC_HSI_HSI_EXPORTER_H_
#define SRC_HSI_HSI_EXPORTER_H_

#include <QString>

#include <memory>
#include <vector>

#include "hsi/image_layout.h"
#include "hsi/spectrum.h"

namespace hsi_data_generator {

class HSIDataExporter {
 public:
  HSIDataExporter(
      const std::shared_ptr<std::vector<std::shared_ptr<Spectrum>>> spectra,
      const std::shared_ptr<ImageLayout> image_layout)
      : spectra_(spectra), image_layout_(image_layout) {}

  void SaveFile(const QString& file_name) const;

 private:
  const std::shared_ptr<std::vector<std::shared_ptr<Spectrum>>> spectra_;
  const std::shared_ptr<ImageLayout> image_layout_;
};

}  // namespace hsi_data_generator

#endif  // SRC_HSI_HSI_EXPORTER_H_
