#include "hsi/image_layout.h"

#include <QImage>
#include <QColor>
#include <QtGlobal>  // qrand()

#include <algorithm>
#include <utility>
#include <vector>

namespace hsi_data_generator {
namespace {

// This is the default stripe width in pixels for generating the stripe and
// grid layouts. The actual assigned stripe width can be smaller if the given
// number of classes cannot fit into the width of the image unless each stripe
// is thinner than this value.
constexpr int kDefaultMaxStripeWidth = 25;

static const std::vector<std::pair<int, int>> kCoordinateNeighborOffsets = {
    std::make_pair(0, -1),  // left
    std::make_pair(0, 1),   // right
    std::make_pair(-1, 0),  // top
    std::make_pair(1, 0),   // bottom
};

}  // namespace

ImageLayout::ImageLayout(const int image_width, const int image_height)
    : image_width_(image_width),
      image_height_(image_height),
      previous_layout_(LAYOUT_TYPE_NONE),
      previous_num_classes_(0),
      previous_size_parameter_(0) {

  // All pixels will be mapped to 0 (the default class index) initially.
  // TODO: Fill with 0 or a "non-class" id, such as -1?
  spectral_class_map_.resize(image_width_ * image_height_);
}

void ImageLayout::GenerateHorizontalStripesLayout(
    const int num_classes, const int stripe_width) {

  int pixels_per_class = stripe_width;
  if (stripe_width <= 0) {
    pixels_per_class =
        std::min(image_height_ / num_classes, kDefaultMaxStripeWidth);
  }
  for (int row = 0; row < image_height_; ++row) {
    const int class_index = (row / pixels_per_class) % num_classes;
    std::fill(
        spectral_class_map_.begin() + (row * image_width_),
        spectral_class_map_.begin() + ((row + 1) * image_width_),
        class_index);
  }

  previous_layout_ = LAYOUT_TYPE_HORIZONTAL_STRIPES;
  previous_num_classes_ = num_classes;
  previous_size_parameter_ = stripe_width;
}

void ImageLayout::GenerateVerticalStripesLayout(
    const int num_classes, const int stripe_width) {

  int pixels_per_class = stripe_width;
  if (stripe_width <= 0) {
    pixels_per_class =
        std::min(image_width_ / num_classes, kDefaultMaxStripeWidth);
  }
  for (int col = 0; col < image_width_; ++col) {
    const int class_index = (col / pixels_per_class) % num_classes;
    for (int row = 0; row < image_height_; ++row) {
      const int index = row * image_width_ + col;
      spectral_class_map_[index] = class_index;
    }
  }

  previous_layout_ = LAYOUT_TYPE_VERTICAL_STRIPES;
  previous_num_classes_ = num_classes;
  previous_size_parameter_ = stripe_width;
}

void ImageLayout::GenerateGridLayout(
    const int num_classes, const int square_width) {

  int pixels_per_class = square_width;
  if (square_width <= 0) {
    pixels_per_class =
        std::min(image_width_ / num_classes, kDefaultMaxStripeWidth);
  }
  for (int row = 0; row < image_height_; ++row) {
    for (int col = 0; col < image_width_; ++col) {
      const int index = row * image_width_ + col;
      const int class_index =
          ((row / pixels_per_class) + (col / pixels_per_class)) % num_classes;
      spectral_class_map_[index] = class_index;
    }
  }

  previous_layout_ = LAYOUT_TYPE_GRID;
  previous_num_classes_ = num_classes;
  previous_size_parameter_ = square_width;
}

void ImageLayout::GenerateRandomLayout(
    const int num_classes, const int random_blob_size) {

  const int width = GetWidth();
  const int height = GetHeight();
  int num_pixels_remaining = spectral_class_map_.size();
  std::vector<bool> filled_in_pixels(num_pixels_remaining, false);
  while (num_pixels_remaining > 0) {
    // Select a random class for the next blob:
    const int current_class = qrand() % num_classes;
    // Select a random pixel that isn't part of a blob:
    int start_index = qrand() % num_pixels_remaining;
    while (filled_in_pixels[start_index]) {
      start_index++;
      if (start_index >= filled_in_pixels.size()) {
        start_index = 0;
      }
    }
    // Fill in the randomly selected pixel:
    spectral_class_map_[start_index] = current_class;
    num_pixels_remaining--;
    filled_in_pixels[start_index] = true;
    // Add this pixel as the only initial edge pixel in the blob:
    std::vector<int> edge_pixels;
    edge_pixels.push_back(start_index);
    // Fill in pixels for the rest of the blob:
    for (int i = 1; i < random_blob_size; ++i) {
      // Stop if there no more available edges or if all pixels were filled in.
      if (num_pixels_remaining <= 0 || edge_pixels.empty()) {
        break;
      }
      // Randomly select one of the blob's edge pixels:
      const int edge_index = qrand() % edge_pixels.size();
      const int expand_index = edge_pixels[edge_index];
      const int expand_row = expand_index / width;
      const int expand_col = expand_index % width;
      // Select all valid candidate pixels adjacent (left, right, top, or
      // bottom) to the selected egde pixel to be filled in next. A candidate
      // is only valid if it is inside the bounds of the image and if it hasn't
      // already been filled in before.
      std::vector<int> neighbor_candidates;
      for (const std::pair<int, int>& offset : kCoordinateNeighborOffsets) {
        const int row = expand_row + offset.first;
        if (row < 0 || row >= height) {
          continue;
        }
        const int col = expand_col + offset.second;
        if (col < 0 || col >= width) {
          continue;
        }
        const int neighbor_index =  row * width + col;
        if (!filled_in_pixels[neighbor_index]) {
          neighbor_candidates.push_back(neighbor_index);
        }
      }
      // If no neighbors are available, this edge pixel has no valid neighbors,
      // so remove it from the set of neighbors.
      if (neighbor_candidates.size() == 0) {
        edge_pixels.erase(edge_pixels.begin() + edge_index);
      } else {
        // Randomly select one of the neighbors and fill it in as part of the
        // blob. This new filled-in pixel becomes a new edge pixel.
        const int neighbor_index =
            neighbor_candidates[qrand() % neighbor_candidates.size()];
        spectral_class_map_[neighbor_index] = current_class;
        filled_in_pixels[neighbor_index] = true;
        edge_pixels.push_back(neighbor_index);
        num_pixels_remaining -= 1;
      }
    }
  }

  previous_layout_ = LAYOUT_TYPE_RANDOM;
  previous_num_classes_ = num_classes;
  previous_size_parameter_ = random_blob_size;
}

void ImageLayout::GenerateLayoutFromImage(
    const int num_classes, const QImage& layout_image) {

  // Resize the image to fit the layout's size.
  QImage image = layout_image.scaled(image_width_, image_height_);

  // Convert the image to grayscale if it isn't already. Code taken from:
  // https://stackoverflow.com/questions/27949569/convert-a-qimage-to-grayscale
  // Pixel depth is 1 (grayscale), 3 (rgb), or 4 (rgba), etc. QImage.depth is
  // given in bits (8 per byte).
  const int pixel_depth = image.depth() / 8;
  for (int row = 0; row < image_height_; ++row) {
    unsigned char* scanline = image.scanLine(row);
    for (int col = 0; col < image_width_; ++col) {
      QRgb* rgb_pixel = reinterpret_cast<QRgb*>(scanline + col * pixel_depth);
      const int gray_value = qGray(*rgb_pixel);
      *rgb_pixel = QColor(gray_value, gray_value, gray_value).rgba();
    }
  }
  image = image.convertToFormat(QImage::Format_Indexed8);

  // Set the class map values by pixel intensity.
  // TODO: Possibly, do pixel intensity clustering instead of this hack.
  for (int row = 0; row < image_height_; ++row) {
    for (int col = 0; col < image_width_; ++col) {
      const int index = row * image_width_ + col;
      const int gray_value = qGray(image.pixel(row, col));
      const int class_index = gray_value / (256 / num_classes);
      spectral_class_map_[index] = class_index;
    }
  }
}

void ImageLayout::ResetLayout() {
  // TODO: Fill with 0 or a "non-class" id, such as -1?
  std::fill(spectral_class_map_.begin(), spectral_class_map_.end(), 0);
}

void ImageLayout::SetImageSize(const int width, const int height) {
  image_width_ = width;
  image_height_ = height;
  spectral_class_map_.resize(image_width_ * image_height_);
  switch (previous_layout_) {
  case LAYOUT_TYPE_HORIZONTAL_STRIPES:
    GenerateHorizontalStripesLayout(
        previous_num_classes_, previous_size_parameter_);
    break;
  case LAYOUT_TYPE_VERTICAL_STRIPES:
    GenerateVerticalStripesLayout(
        previous_num_classes_, previous_size_parameter_);
    break;
  case LAYOUT_TYPE_GRID:
    GenerateGridLayout(previous_num_classes_, previous_size_parameter_);
    break;
  case LAYOUT_TYPE_RANDOM:
    GenerateRandomLayout(previous_num_classes_, previous_size_parameter_);
    break;
  case LAYOUT_TYPE_NONE:
  default:
    break;
  }
}

int ImageLayout::GetClassAtPixel(const int x_col, const int y_row) const {
  const int map_index = GetMapIndex(x_col, y_row);
  // TODO: Some range checking?
  return spectral_class_map_[map_index];
}

int ImageLayout::GetMapIndex(const int x_col, const int y_row) const {
  // TODO: Some range checking?
  return y_row * image_width_ + x_col;
}

}  // namespace hsi_data_generator
