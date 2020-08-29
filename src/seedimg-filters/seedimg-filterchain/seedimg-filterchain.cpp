#include <seedimg-filters/seedimg-filterchain.hpp>
#include <seedimg-filters/seedimg-filterchain-ports.hpp>

namespace seedimg::filters {
lazy_filterchain filter::operator>>(filter&& f) {
    return lazy_filterchain()
        >> std::move(*this)
        >> std::move(f);
}

lazy_filterchain lazy_filterchain::operator>>(filter&& f) {
  filters.push_back(f);
  return *this;
}

lazy_filterchain lazy_filterchain::operator()(const simg& input, simg& output) {
  output = input;

  if(as_backwards)
    for(auto f = filters.rbegin(); f != filters.rend(); ++f)
      (*f).get() >> output;
  else
    for(const auto& f: filters)
      f.get() >> output;
  
  return *this;
}

lazy_filterchain lazy_filterchain::operator>>(simg& image) {
  (*this)(image, image);
  return *this;
}

lazy_filterchain lazy_filterchain::operator--() {
  filters.pop_back();
  return *this;
}

lazy_filterchain lazy_filterchain::operator!() {
  as_backwards = true;
  return *this;
}
};