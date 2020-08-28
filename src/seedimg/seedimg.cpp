/**********************************************************************
    seedimg - module based image manipulation library written in modern
                C++ Copyright(C) 2020 telugu-boy

    This program is free software : you can redistribute it and /
    or modify it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation,
    either version 3 of the License,
    or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/
// seedimg.cpp : Defines the functions for the static library.
//

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>

#include <seedimg.hpp>

namespace seedimg {

img::img() : width_(0), height_(0), data_(nullptr) {}

img::img(simg_int w, simg_int h) : width_{w}, height_{h} {
  data_ = reinterpret_cast<seedimg::pixel *>(std::malloc(
      static_cast<std::size_t>(height_ * width_) * sizeof(seedimg::pixel *)));
  if (data_ == nullptr)
    throw std::bad_alloc();
}

img::img(simg_int w, simg_int h, seedimg::pixel *u_data)
    : width_{w}, height_{h}, data_{u_data} {}

img::img(seedimg::img const &img_) : img(img_.width_, img_.height_) {
  std::memcpy(this->data_, img_.data_,
              static_cast<std::size_t>(img_.width_ * img_.height_) *
                  sizeof(seedimg::pixel));
}

img::img(seedimg::img &&other) : width_{0}, height_{0}, data_{nullptr} {
  this->width_ = other.width_;
  this->height_ = other.height_;
  this->data_ = other.data_;
  other.width_ = 0;
  other.height_ = 0;
  other.data_ = nullptr;
}

img &img::operator=(img other) {
  std::swap(this->data_, other.data_);
  std::swap(this->width_, other.width_);
  std::swap(this->height_, other.height_);
  return *this;
}
img &img::operator=(img &&other) {
  if (&other != this) {
    delete data_;
    data_ = other.data_;
    other.data_ = nullptr;
    width_ = other.width_;
    height_ = other.height_;
  }
  return *this;
}

img::~img() { std::free(data_); }

std::vector<std::pair<simg_int, simg_int>> img::start_end_rows() {
  std::vector<std::pair<simg_int, simg_int>> res;
  auto processor_count =
      std::min(this->height(),
               static_cast<simg_int>(std::thread::hardware_concurrency()));
  if (processor_count == 0)
    processor_count = 1;
  res.reserve(static_cast<std::size_t>(processor_count));
  simg_int rows_per_thread = this->height() / processor_count;
  for (simg_int i = 0; i < processor_count * rows_per_thread;
       i += rows_per_thread)
    res.emplace_back(i, i + rows_per_thread);
  res[res.size() - 1].second += this->height() % processor_count;
  return res;
}

std::vector<std::pair<simg_int, simg_int>> img::start_end_cols() {
  std::vector<std::pair<simg_int, simg_int>> res;
  auto processor_count =
      std::min(this->width(),
               static_cast<simg_int>(std::thread::hardware_concurrency()));
  if (processor_count == 0)
    processor_count = 1;
  res.reserve(static_cast<std::size_t>(processor_count));
  simg_int cols_per_thread = this->width() / processor_count;
  for (simg_int i = 0; i < processor_count * cols_per_thread;
       i += cols_per_thread)
    res.emplace_back(i, i + cols_per_thread);
  res[res.size() - 1].second += this->width() % processor_count;
  return res;
}

seedimg::pixel &img::pixel(simg_int x, simg_int y) noexcept {
  return data_[y * this->width_ + x];
}

seedimg::pixel &img::pixel(seedimg::point p) noexcept {
  return pixel(p.first, p.second);
}

seedimg::pixel &img::pixel(simg_int x) {
  if (x > this->width() * this->height() - 1)
    std::terminate();
  return pixel(x / this->width(), x % this->width());
}

seedimg::pixel *img::row(simg_int y) noexcept {
  return data_ + y * this->width_;
}

seedimg::pixel *img::data() noexcept { return data_; }
simg_int img::width() noexcept { return width_; }
simg_int img::height() noexcept { return height_; }

std::shared_ptr<seedimg::img> make(simg_int width, simg_int height) {
  return std::make_shared<seedimg::img>(width, height);
}

std::shared_ptr<seedimg::img>
make(const std::shared_ptr<seedimg::img> &inp_img) {
  return std::make_shared<seedimg::img>(*inp_img);
}

anim::anim() { framerate = 0; }

anim::anim(std::size_t size, std::size_t framerate) {
  this->framerate = framerate;
  data = std::vector<simg>(size);
}

anim::anim(seedimg::anim const &anim_) {
  data = std::vector<simg>();
  data.reserve(anim_.data.size());
}

anim::anim(seedimg::anim &&other) {
  framerate = other.framerate;
  other.framerate = 0;
  data = std::move(other.data);
}

anim &anim::operator=(anim other) {
  this->data.swap(other.data);
  return *this;
}
anim &anim::operator=(anim &&other) {
  if (&other != this) {
    this->data = std::move(other.data);
  }
  return *this;
}

simg &anim::operator[](std::size_t i) { return data[i]; }

void anim::add(simg img) { data.push_back(img); }
bool anim::insert(simg img, std::size_t index) {
  if (index >= data.size())
    return false;
  data.insert(data.begin() + static_cast<int>(index), img);
  return true;
}
bool anim::remove(std::size_t index) {
  if (index >= data.size())
    return false;
  data.erase(data.begin() + static_cast<int>(index));
  return true;
}
bool anim::trim(std::size_t start, std::size_t end) {
  if (start > end || end >= data.size())
    return false;
  data.erase(data.begin(), data.begin() + static_cast<int>(start));
  data.erase(data.begin() + static_cast<int>(end - start), data.end());
  return true;
}

auto anim::begin() { return data.begin(); }
auto anim::end() { return data.end(); }
} // namespace seedimg
