#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include "Error.hpp"

namespace gw2::compression {

/** @Inputs:
 *    - iInputTab: Pointer to the buffer to inflate
 *    - ioOutputTab: Output buffer
 *  @Return:
 *    - Actual size of the outputBuffer
 */
Result<std::uint32_t> inflateDatFileBuffer(std::span<const std::byte> iInputTab,
                                           std::span<std::byte> ioOutputTab);

}  // namespace gw2::compression