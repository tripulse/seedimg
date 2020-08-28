/**********************************************************************
    seedimg - module based image manipulation library written in modern
                C++ Copyright(C) 2020 telugu-boy, tripulse

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

#include <seedimg-formats/seedimg-gif.hpp>
#include <fstream>

// #include <gifski.h>
#include <gif_lib.h>

namespace seedimg::modules::gif {
bool check(const std::string& filename) {
  std::ifstream input(filename);
  char gifhdr[6];

  try {
    input.read(gifhdr, 6);
  } catch (std::ios_base::failure) {
    return false;
  }

  return !memcmp("GIF87a", gifhdr, 6) ?  // 87a
         !memcmp("GIF89a", gifhdr, 6) :  // 89a
          false;
}

bool to(const std::string &filename, const simg &input) {
  int  err = 0;
  auto g   = EGifOpenFileName(filename.c_str(), false, NULL);

  if(g == NULL)
    return false;

  err |= EGifPutScreenDesc(g, input->width(), input->height(), 256, 0, NULL);
  err |= EGifPutImageDesc(g, 0, 0, input->width(), input->height(), true, NULL);

  auto rowstride = input->width() * 3;
  auto rowbuf    = new GifByteType[rowstride];

  for(simg_int y = 0; y < input->height(); ++y) {
    for(simg_int x = 0; x < input->width(); ++x) {
      auto& pix = input->pixel(x, y);

      rowbuf[3*x]     = pix.r;
      rowbuf[3*x + 1] = pix.g;
      rowbuf[3*x + 2] = pix.b;
    }
    err |= EGifPutLine(g, rowbuf, rowstride);
  }
  err |= EGifSpew(g);

  return (err | EGifCloseFile(g, NULL)) == GIF_OK;
}

simg from(const std::string &filename) {
  auto g = DGifOpenFileName(filename.c_str(), NULL);
  if(g == NULL || DGifSlurp(g) != 0 || g->ImageCount <= 0)
    return nullptr;

  auto img = seedimg::make(g->Image.Width,
                           g->Image.Height);

  std::memmove(img->data(), g->SavedImages->RasterBits,
               g->Image.Width * g->Image.Height);
  return img;
}
}
