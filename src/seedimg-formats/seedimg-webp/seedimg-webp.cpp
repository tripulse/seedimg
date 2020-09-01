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
// seedimg-webp.cpp : Defines the functions for the static library.
//

#include <cstring>
#include <filesystem>
#include <fstream>

extern "C" {
#include <webp/demux.h>
#include <webp/decode.h>

#include <webp/mux.h>
#include <webp/encode.h>
}

#include <seedimg-formats/seedimg-webp.hpp>

namespace seedimg::modules {
namespace webp {
bool check(const std::string &filename) noexcept {
  std::error_code ec;
  std::size_t size = std::filesystem::file_size(filename, ec);
  if (ec != std::error_code{} || size < 8)
    return false;
  // this does not check the full header as
  // the length is little endian and that would require more code
  std::uint8_t cmp[8] = {'R', 'I', 'F', 'F', 'W', 'E', 'B', 'P'};
  std::uint8_t header[8] = {};
  std::ifstream file(filename, std::ios::binary);
  file.read(reinterpret_cast<char *>(header), 4);
  file.ignore(4);
  file.read(reinterpret_cast<char *>(header + 4), 4);
  return !std::memcmp(cmp, header, 8);
}

anim from(const std::string& filename) {
  anim          images;
  std::ifstream in(filename);

  struct {
    WebPData data;
    size_t image_size;

    struct {
      WebPAnimDecoder* h;
      WebPAnimInfo     info;
    } anim;

    struct {
      int start;
      int end;
    } ts;
  } webp;

  // copy-in the contents from the iterator.
  std::string buf(std::istreambuf_iterator<char>{in},
                  std::istreambuf_iterator<char>());

  webp.data.bytes = reinterpret_cast<const uint8_t*>(buf.data());
  webp.data.size = buf.size();

  webp.anim.h = WebPAnimDecoderNew(&webp.data, NULL);
  WebPAnimDecoderGetInfo(webp.anim.h, &webp.anim.info);

  // amount of data to memcpy into seedimg::img buffer.
  webp.image_size = webp.anim.info.canvas_width * 4 * webp.anim.info.canvas_height;

  auto WebPPushFrame = [&]() -> int {
    uint8_t* buf;
    int      ts;

    if(!WebPAnimDecoderGetNext(webp.anim.h, &buf, &ts))
      return -1;

    auto img = seedimg::make(webp.anim.info.canvas_width,
                             webp.anim.info.canvas_height);

    std::memcpy(img->data(), buf, webp.image_size);
    images.add(img);

    return ts;
  };

  webp.ts.start = WebPPushFrame();
  webp.ts.end = WebPPushFrame();

  // if atleast two consecutive frames were found; calculate the delta,
  // considering it a time step-factor calculate the framerate.
  images.framerate = webp.ts.start == -1 || webp.ts.end == -1 ? 0
                      : 1000 / (webp.ts.end - webp.ts.start);

  // collect all frames until unavailable.
  while(WebPAnimDecoderHasMoreFrames(webp.anim.h))
    if(WebPPushFrame() == -1)
      break;

  WebPAnimDecoderDelete(webp.anim.h);
  return images;
}

bool to(const std::string& filename, anim& images,
        std::pair<std::uint8_t, std::uint8_t> quality,
        bool lossless, bool loop) {
  std::ofstream out(filename);

  if(!images.num_frames())
    return true;

  // since the effect is same and many decoders don't support
  // ANIM and ANMF chunks, they count it as invalid file.
  else if(images.num_frames() == 1) {
    auto img = images[0];
    std::uint8_t* out;

    if(lossless)
      WebPEncodeRGBA(reinterpret_cast<std::uint8_t*>(img->data()),
                     img->width(), img->height(),
                     img->width() * sizeof(seedimg::pixel),
                     quality.first, &out);
    else
      WebPEncodeLosslessRGBA(reinterpret_cast<std::uint8_t*>(img->data()),
                             img->width(), img->height(),
                             img->width() * sizeof(seedimg::pixel), &out);
  }

  struct {
    WebPData data;
    WebPConfig cfg;
    WebPPicture pic;

    struct {
      WebPAnimEncoder*       h;
      WebPAnimEncoderOptions opts;
    } anim;

    struct {
      int counter = 0;
      int step;
    } ts;
  } webp;

  if(!WebPAnimEncoderOptionsInit(&webp.anim.opts) ||
     !WebPConfigInit(&webp.cfg) ||
     !WebPPictureInit(&webp.pic))
    return false;

  webp.cfg.lossless      = lossless;
  webp.cfg.quality       = quality.first;
  webp.cfg.alpha_quality = quality.second;

  webp.pic.width = images[0]->width();
  webp.pic.height = images[0]->height();

  if(!WebPPictureAlloc(&webp.pic))
    return false;

  webp.anim.opts.anim_params.loop_count = !loop;
  webp.ts.step = 1000/images.framerate;

  webp.anim.h = WebPAnimEncoderNew(
        images[0]->width(), images[0]->height(), &webp.anim.opts);

  for(const auto& frame : images) {
    // only rescale picture-buffer if needed.
    if(frame->width() != static_cast<simg_int>(webp.pic.width) ||
       frame->height() != static_cast<simg_int>(webp.pic.height))
      WebPPictureRescale(&webp.pic, frame->width(), frame->height());
    WebPPictureImportRGBA(&webp.pic,
                          reinterpret_cast<std::uint8_t*>(frame->data()),
                          frame->width() * sizeof(seedimg::pixel));

    if(!WebPAnimEncoderAdd(webp.anim.h, &webp.pic, webp.ts.counter, &webp.cfg))
      break;
    webp.ts.counter+= webp.ts.step;
  }

  WebPPictureFree(&webp.pic);
  if(!WebPAnimEncoderAssemble(webp.anim.h, &webp.data))
    return false;
  WebPAnimEncoderDelete(webp.anim.h);

  try {
    out.write(reinterpret_cast<char*>(
                const_cast<std::uint8_t*>(webp.data.bytes)),
              webp.data.size);

    free(const_cast<std::uint8_t*>(webp.data.bytes));
  } catch (std::iostream::failure)
  { return false; }
  return true;
}
} // namespace webp
} // namespace seedimg::modules
