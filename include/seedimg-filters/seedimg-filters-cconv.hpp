/***********************************************************************
    seedimg - module based image manipulation library written in modern C++
    Copyright (C) 2020 telugu-boy, tripulse

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

#include <seedimg.hpp>

#ifndef SEEDIMG_FILTERS_CCONV_H
#define SEEDIMG_FILTERS_CCONV_H

namespace seedimg::filters {
namespace cconv {

void rgb(simg &inp_img, simg &res_img);
void rgb_i(simg &inp_img);

void hsv(simg &inp_img, simg &res_img);
void hsv_i(simg &inp_img);

void ycbcr(simg &inp_img, simg &res_img,
           seedimg::colourspaces type = seedimg::colourspaces::ycbcr_jpeg);
void ycbcr_i(simg &inp_img,
             seedimg::colourspaces type = seedimg::colourspaces::ycbcr_jpeg);

} // namespace cconv
} // namespace seedimg::filters

#endif
