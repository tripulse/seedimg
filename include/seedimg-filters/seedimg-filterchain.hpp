#ifndef _SEEDIMG_FILTERCHAIN
#define _SEEDIMG_FILTERCHAIN

#include <seedimg.hpp>
#include <list>


namespace seedimg::filters {
class lazy_filterchain;

class filter {
public:
  virtual void operator()(const simg& input, simg& output) = 0;

  std::reference_wrapper<filter> operator>>(simg& input) {
    (*this)(input, input);
    return *this;
  }

  lazy_filterchain operator>>(filter&& f);

  //// this can enable eager evaluation of filters:
  //  simg operator+(const simg& input) {
  //    auto output = seedimg::make(input->width(),
  //                                input->height());
  //    (*this)(input, output);
  //    return output;
  //  }
};

/** 
 * A linear chain of multiple filters which is lazily evaluated. For it being
 * lazily evaluated it can be re-used. The order of evaluating filters can be
 * customized by using boolean-negotiation operator.
 */
class lazy_filterchain {
protected:
  std::list<std::reference_wrapper<filter>> filters;
  bool as_backwards;
public:
  lazy_filterchain() = default;
  lazy_filterchain operator>>(filter&& f);
  lazy_filterchain operator>>(simg& image);
  lazy_filterchain operator()(const simg& input, simg& second);
  lazy_filterchain operator--();
  lazy_filterchain operator!();
};
}

#endif