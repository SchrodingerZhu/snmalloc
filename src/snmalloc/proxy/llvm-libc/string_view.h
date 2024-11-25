#pragma once

#ifndef LIBC_NAMESPACE
#  define LIBC_NAMESPACE __llvm_libc
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "src/__support/CPP/string_view.h"
#pragma GCC diagnostic pop

namespace cpp
{
  using namespace LIBC_NAMESPACE::cpp;
} // namespace cpp
