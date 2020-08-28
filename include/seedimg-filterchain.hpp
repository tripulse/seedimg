#ifndef _SEEDIMG_FILTERCHAIN
#define _SEEDIMG_FILTERCHAIN

#include <seedimg.hpp>

/**
 * Filter is an abstract filter when called with an image
 * does somekind of processing then modifies the image.
 */
class filter {
public:
  virtual void operator()(simg& input, simg& output) = 0;

  simg operator()(simg& input) {
    auto output = seedimg::make(input->width(),
                                input->height());

    (*this)(input, output);
    return output;
  }
};

class lazy_filterchain {
private:
  std::vector<std::reference_wrapper<filter>> filters;
public:
  lazy_filterchain() = default;
  lazy_filterchain operator<<(filter& f);
  void operator()(simg& input, simg& output);
};

#endif
