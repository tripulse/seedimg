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
#include <numeric>
#include <seedimg-filters/seedimg-filters-cconv.hpp>
#include <seedimg-utils.hpp>

inline bool feq(float a, float b) {
  return std::fabs(a - b) < std::numeric_limits<float>::epsilon();
}

void rgb2hsv_worker(simg &inp_img, simg &res_img, simg_int start,
                    simg_int end) {
  for (; start < end; ++start) {
    for (simg_int x = 0; x < res_img->width(); ++x) {
      // the p suffix in this sense stands for prime. normally we use R' G' B'
      // to represent normalized colour.
      float rp = static_cast<float>(inp_img->pixel(x, start).r) /
                 static_cast<float>(seedimg::img::MAX_PIXEL_VALUE);
      float gp = static_cast<float>(inp_img->pixel(x, start).g) /
                 static_cast<float>(seedimg::img::MAX_PIXEL_VALUE);
      float bp = static_cast<float>(inp_img->pixel(x, start).b) /
                 static_cast<float>(seedimg::img::MAX_PIXEL_VALUE);
      float cmax = std::max(rp, std::max(gp, bp));
      float cmin = std::min(rp, std::min(gp, bp));
      float delta = cmax - cmin;

      std::uint8_t hue = static_cast<std::uint8_t>(delta);
      std::uint8_t sat = 0;
      std::uint8_t val = static_cast<std::uint8_t>(cmax * 100.0f);

      if (feq(rp, cmax)) {
        hue = static_cast<std::uint8_t>(60 * std::fmod((gp - bp) / delta, 3));
      } else if (feq(gp, cmax)) {
        hue = static_cast<std::uint8_t>(60 * ((bp - rp) / delta + 1));
      } else if (feq(bp, cmax)) {
        hue = static_cast<std::uint8_t>(60 * ((rp - gp) / delta + 2));
      }

      // saturation
      if (!feq(cmax, 0)) {
        sat = static_cast<std::uint8_t>((delta / cmax) * 100.0f);
      }
      res_img->pixel(x, start) = {{{hue, sat, val}},
                                  inp_img->pixel(x, start).a};
    }
  }
}

namespace seedimg::filters {
namespace cconv {

void hsv(simg &inp_img, simg &res_img) {
  if (inp_img->colourspace() == seedimg::colourspaces::hsv) {
    return;
  } else if (inp_img->colourspace() == seedimg::colourspaces::rgb) {
    seedimg::utils::hrz_thread(rgb2hsv_worker, inp_img, res_img);
  } else if (inp_img->colourspace() == seedimg::colourspaces::ycbcr) {
    rgb(inp_img, res_img);
    hsv(inp_img, res_img);
  }
  std::static_pointer_cast<seedimg::uimg>(res_img)->set_colourspace(
      seedimg::colourspaces::hsv);
}
void hsv_i(simg &inp_img) { hsv(inp_img, inp_img); }
} // namespace cconv
} // namespace seedimg::filters
