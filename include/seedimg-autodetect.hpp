﻿/**********************************************************************
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
#ifndef SEEDIMG_AUTODETECT_H
#define SEEDIMG_AUTODETECT_H

#include <memory>
#include <optional>
#include <string>

#include <seedimg.hpp>

enum class img_type { unknown = 0, png, jpeg, webp, farbfeld, irdump, gif };

enum img_type match_ext(const std::string &ext) noexcept;

std::optional<enum img_type> imgtype(const std::string &filename) noexcept;

namespace seedimg {
simg load(const std::string &filename);

bool save(const std::string &filename, const simg &image);
} // namespace seedimg

#endif
