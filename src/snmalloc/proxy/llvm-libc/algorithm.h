#pragma once

#ifndef LIBC_NAMESPACE
#  define LIBC_NAMESPACE __llvm_libc
#endif

#include "src/__support/CPP/algorithm.h"

namespace cpp
{
  using namespace LIBC_NAMESPACE::cpp;
} // namespace cpp
