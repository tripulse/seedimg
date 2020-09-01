#include <seedimg-filters/seedimg-filterchain.hpp>

namespace seedimg::filters {
lazy_filterchain& lazy_filterchain::evaluate(const simg& input, simg& output) {
  output = input;

  if(as_backwards)
    for(auto f = filters.rbegin(); f != filters.rend(); ++f)
      (*f)(output, output);
  else
    for(const auto& f: filters)
      f(output, output);
  
  return *this;
}

lazy_filterchain& lazy_filterchain::evaluate(simg& image) {
  evaluate(image, image);
  return *this;
}

lazy_filterchain& lazy_filterchain::pop_end() {
  filters.pop_back();
  return *this;
}

lazy_filterchain& lazy_filterchain::reverse() {
  as_backwards = true;
  return *this;
}
};
