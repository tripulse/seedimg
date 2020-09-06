/**********************************************************************
    seedimg - module based image manipulation library written in modern
                C++ Copyright(C) 2020 tripulse

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

#ifndef _SEEDIMG_FILTERCHAIN
#define _SEEDIMG_FILTERCHAIN

#include <seedimg.hpp>
#include <functional>
#include <list>


namespace seedimg::filters {
/**
 * A linear chain of multiple filters which is lazily evaluated making it
 * reusable multiple times. It contains both inplace and I/O-based filters.
 */
class filterchain {
protected:
  std::list<std::function<void(simg&)>> filters;
public:
  /**
   * @brief Push a function to end of the queue that follows
   * the idioms of seedimg filter-function definitions. It
   * will bind any number of additional arguments to the
   * function (if given).
   *
   * @param func a callable object.
   * @param args custom arguments to pass to callable.
   *
   * @typeparam inplace whether callback object does
   * inplace image transformation. inplace functions have
   * mostly "_i" as their suffix.
   */
  template<bool inplace = false, class F, class... Args>
  filterchain& add(F&& func, Args&&... args) {
    filters.push_back([&](simg& input) -> void {
      if constexpr(inplace)
        func(input, args...);
      else
        func(input, input, args...);
    });

    return *this;
  }

  /**
   * @brief Pop-off the most recently added filter in queue.
   */
  filterchain& pop() {
    filters.pop_back();
    return *this;
  }

  /**
   * @brief Apply enqueued filters accumulatively from start to end.
   *
   * @param input image to apply filters on.
   * @param output output of the accumulated result.
   */
  filterchain& eval(const simg& in, simg& out) {
    out = in;
    for(const auto& f: filters)
      f(out);

    return *this;
  }

  filterchain& eval(simg& img) {
    eval(img, img);
    return *this;
  }
};
}
#endif
