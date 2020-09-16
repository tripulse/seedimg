/***********************************************************************
    seedimg - module based image manipulation library written in modern C++
    Copyright (C) 2020 telugu-boy, <contributor name>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/

#include <cmath>
#include <seedimg-filters/seedimg-filters-cconv.hpp>
#include <seedimg-utils.hpp>

// resource headers. this is for the rgb->ycbcr_jpeg
#include "from_ycbcr_jpeg_lut.rh"
// and this is for bt601
#include "from_ycbcr_bt601_lut.rh"

void hsv2rgb_worker(simg &inp_img, simg &res_img, simg_int start,
                    simg_int end) {
  for (; start < end; start++) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      seedimg::pixel pix_src = inp_img->pixel(x, start);
      struct {
        float h;
        float s;
        float v;
      } pix{static_cast<float>(pix_src.h) * 2,
            static_cast<float>(pix_src.s) / 100,
            static_cast<float>(pix_src.v) / 100};
      float C = pix.v * pix.s;
      float X = C * (1 - std::fabs(std::fmod(pix.h / 60.0f, 2.0f) - 1));

      float m = pix.v - C;

      float componentsp[4] = {m, m, m, 0};
      switch (static_cast<int>(pix.h / 60)) {
      case 0: {
        componentsp[0] += C;
        componentsp[1] += X;
        componentsp[2] += 0;
      } break;
      case 1: {
        componentsp[0] += X;
        componentsp[1] += C;
        componentsp[2] += 0;
      } break;
      case 2: {
        componentsp[0] += 0;
        componentsp[1] += C;
        componentsp[2] += X;
      } break;
      case 3: {
        componentsp[0] += 0;
        componentsp[1] += X;
        componentsp[2] += C;
      } break;
      case 4: {
        componentsp[0] += X;
        componentsp[1] += 0;
        componentsp[2] += C;
      } break;
      case 5: {
        componentsp[0] += C;
        componentsp[1] += 0;
        componentsp[2] += X;
      } break;
      }
      res_img->pixel(x, start) = {
          {{static_cast<std::uint8_t>(componentsp[0] * 255),
            static_cast<std::uint8_t>(componentsp[1] * 255),
            static_cast<std::uint8_t>(componentsp[2] * 255)}},
          pix_src.a};
    }
  }
}

// ry*, gcb*, bcr* definitions are generated by genLUT_ycbcr.py in this
// directory.
// lookup table > actually calculating stuff
void ycbcr_jpeg2rgb_worker(simg &inp_img, simg &res_img, simg_int start,
                           simg_int end) {
  for (; start < end; ++start) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      seedimg::pixel pix = inp_img->pixel(x, start);
      // ry* is all 1. we do not need a lookup because it will all be the same
      // as pix.y
      // gcb1 and bcr3 are zero in the matrix.
      res_img->pixel(x, start).r =
          static_cast<std::uint8_t>(pix.y + 0 + jpeg_bcr1[pix.cr]);
      res_img->pixel(x, start).g = static_cast<std::uint8_t>(
          pix.y + jpeg_gcb2[pix.cb] + jpeg_bcr2[pix.cr]);
      res_img->pixel(x, start).b =
          static_cast<std::uint8_t>(pix.y + jpeg_gcb3[pix.cb] + 0);
    }
  }
}

// same with this function.
void ycbcr_bt6012rgb_worker(simg &inp_img, simg &res_img, simg_int start,
                            simg_int end) {
  for (; start < end; ++start) {
    for (simg_int x = 0; x < inp_img->width(); ++x) {
      seedimg::pixel pix = inp_img->pixel(x, start);
      // gcb1 and bcr3 are zero in the matrix. as well.
      res_img->pixel(x, start).r = static_cast<std::uint8_t>(
          bt601_ry1[pix.y] + 0 + bt601_bcr1[pix.cr] - 222.921f);
      res_img->pixel(x, start).g =
          static_cast<std::uint8_t>(bt601_ry2[pix.y] + bt601_gcb2[pix.cb] +
                                    bt601_bcr2[pix.cr] + 135.576f);
      res_img->pixel(x, start).b = static_cast<std::uint8_t>(
          bt601_ry3[pix.y] + bt601_gcb3[pix.cb] + 0 - 276.836f);
    }
  }
}

namespace seedimg::filters {
namespace cconv {
void rgb(simg &inp_img, simg &res_img) {
  if (inp_img->colourspace() == seedimg::colourspaces::rgb) {
    return;
  } else if (inp_img->colourspace() == seedimg::colourspaces::hsv) {
    seedimg::utils::hrz_thread(hsv2rgb_worker, inp_img, res_img);
    // hsv2rgb_worker(inp_img, res_img, 0, inp_img->height());
  } else if (inp_img->colourspace() == seedimg::colourspaces::ycbcr_jpeg) {
    seedimg::utils::hrz_thread(ycbcr_jpeg2rgb_worker, inp_img, res_img);
  } else if (inp_img->colourspace() == seedimg::colourspaces::ycbcr_bt601) {
    seedimg::utils::hrz_thread(ycbcr_bt6012rgb_worker, inp_img, res_img);
  }
  static_cast<seedimg::uimg *>(res_img.get())
      ->set_colourspace(seedimg::colourspaces::rgb);
}
void rgb_i(simg &inp_img) { rgb(inp_img, inp_img); }
} // namespace cconv
} // namespace seedimg::filters
