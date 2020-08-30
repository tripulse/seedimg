//#include <seedimg-filters/seedimg-filterchain-ports.hpp>

//namespace seedimg::filters {
//void inversion_filter::operator()(const simg& input, simg& output) {
//  for(simg_int y = 0; y < input->height(); ++y) {
//    for(simg_int x = 0; x < input->width(); ++x) {
//      auto& pix = input->pixel(x, y);
//      output->pixel(x, y) = {static_cast<std::uint8_t>(255-pix.r),
//                              static_cast<std::uint8_t>(255-pix.g),
//                              static_cast<std::uint8_t>(255-pix.b),
//                              static_cast<std::uint8_t>(255-pix.a)};
//    }
//  }
//}
//}
