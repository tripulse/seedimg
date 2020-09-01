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
#ifndef SEEDIMG_WEBP_H
#define SEEDIMG_WEBP_H

#include <memory>
#include <optional>
#include <seedimg.hpp>

namespace seedimg::modules {
namespace webp {
bool check(const std::string &filename) noexcept;

/**
 * @brief Decodes a (possibly) animated WebP file.
 * @param filename location to read from.
 * @return an empty array on error/file being empty.
 */
anim from(const std::string& filename);

/**
 * @brief Encodes a (possibly) animated WebP file.
 * @param filename location to write to.
 * @param images animated sequence to write in.
 * @param quality quality of RGB + alpha channels,
 *                first = RGB, second = alpha.
 * @param lossless whether to use the lossless mode.
 * @param loop infinitely loop or not (GIF semantics).
 */
bool to(
    const std::string& filename, anim& images,
    std::pair<std::uint8_t, std::uint8_t> quality = {100, 100},
    bool lossless = false,
    bool loop = false);
} // namespace webp
} // namespace seedimg::modules
#endif
