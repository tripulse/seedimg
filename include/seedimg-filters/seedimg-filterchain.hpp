#ifndef _SEEDIMG_FILTERCHAIN
#define _SEEDIMG_FILTERCHAIN

#include <seedimg.hpp>
#include <list>


namespace seedimg::filters {
//class lazy_filterchain;

//class filter {
//public:
//  virtual void operator()(const simg& input, simg& output) = 0;

//  std::reference_wrapper<filter> operator>>(simg& input) {
//    (*this)(input, input);
//    return *this;
//  }

//  lazy_filterchain operator>>(filter&& f);

//  //// this can enable eager evaluation of filters:
//  //  simg operator+(const simg& input) {
//  //    auto output = seedimg::make(input->width(),
//  //                                input->height());
//  //    (*this)(input, output);
//  //    return output;
//  //  }
//};

/** 
 * A linear chain of multiple filters which is lazily evaluated. For it being
 * lazily evaluated it can be re-used. The order of evaluating filters can be
 * customized by using boolean-negotiation operator.
 */
class lazy_filterchain {
protected:
//  std::list<std::reference_wrapper<filter>> filters;

  std::list<std::function<void(simg&, simg&)>> filters;
  bool as_backwards;
public:
  /**
   * @brief Push a function to end of the queue that follows
   * the idioms of seedimg filter-function definitions. It
   * will bind any number of additional arguments to the
   * function (if given).
   *
   * @param func a callable object.
   * @param args custom arguments to pass to callable.
   */

  // Why inlining?
  // https://stackoverflow.com/q/10978642

  // NOTE: excluding rvalue in 'args' causes the elements to be
  // uninitialised for a very strange reason.
  template<class F, class... Args>
  lazy_filterchain& push_end(F&& func, Args&&... args) {
    filters.push_back(
          [&](simg& input, simg& output) -> void
          { func(input, output, args...); });
    return *this;
  }

  /**
   * @brief Evaluate the filters in the queue in a defined order,
   * by default it does in forward order backwards can be set by
   * .reverse() method. If the queue was empty the resulting image
   * is the same as the given one.
   *
   * @param input the initial image to apply the filters.
   * @param output dump the resulting image.
   */
  lazy_filterchain& evaluate(const simg& input, simg& output);

  /**
   * @brief Same as the other overload, except its done inplace.
   */
  lazy_filterchain& evaluate(simg& image);

  /**
   * @brief Pop-off a filter from the very end of the queue.
   */
  lazy_filterchain& pop_end();

  /**
   * @brief Reverses the evaulation order, but doesn't reverse the
   * actual ordering of filters in the internal queue. Sets a flag.
   */
  lazy_filterchain& reverse();
};
}

#endif
