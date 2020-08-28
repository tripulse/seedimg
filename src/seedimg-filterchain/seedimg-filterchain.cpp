#include <seedimg-filterchain.hpp>
#include <numeric>

class inversion_filter : filter {
  inversion_filter() = default;

  void operator()(simg& input, simg& output) {
    for(simg_int y = 0; y < input->height(); ++y) {
      for(simg_int x = 0; x < input->width(); ++x) {
        auto& pix = input->pixel(x, y);
        output->pixel(x, y) = {static_cast<std::uint8_t>(255-pix.r),
                               static_cast<std::uint8_t>(255-pix.g),
                               static_cast<std::uint8_t>(255-pix.b),
                               static_cast<std::uint8_t>(255-pix.a)};
      }
    }
  }
};


lazy_filterchain lazy_filterchain::operator<<(filter& f) {
  filters.push_back(f);
  return *this;
}

void lazy_filterchain::operator()(simg& input, simg& output) {
  output = std::accumulate(filters.begin(), filters.end(), input,
      [&](simg img, std::reference_wrapper<filter> f) -> std::shared_ptr<seedimg::img>
      { return f(img); });
}
