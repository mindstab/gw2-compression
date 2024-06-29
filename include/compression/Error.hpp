#pragma once

#include <expected>

namespace gw2::compression {

enum class Error {
    kInputBufferIsEmpty,
    kOutputBufferIsEmpty,
    kOutputBufferTooSmall,
};

template <typename T>
using Result = std::expected<T, Error>;

}