/**********************************************************************
    seedimg - module based image manipulation library written in modern
                C++ Copyright(C) 2020 telugu-boy

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

#ifndef SEEDIMG_OCL_SINGLETON_H
#define SEEDIMG_OCL_SINGLETON_H

#include <CL/cl.hpp>

class ocl_singleton {
public:
  std::vector<cl::Platform> all_platforms;
  cl::Platform platform;
  std::vector<cl::Device> all_devices;
  cl::Context context;
  cl::Device device;
  cl::Program::Sources sources;
  cl::Program program;

  static ocl_singleton &instance(std::size_t plat = 0, std::size_t dev = 0);

  ocl_singleton(ocl_singleton const &) = delete;
  void operator=(ocl_singleton const &) = delete;

private:
  ocl_singleton(std::size_t plat, std::size_t dev);
};

#endif
