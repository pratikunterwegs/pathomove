
#pragma once

#define _USE_MATH_DEFINES

#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

// TODO: move to different files or include headers as needed

template <typename T>
const inline T square(T value) {
  return value * value;
}
