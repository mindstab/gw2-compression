#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include "Error.hpp"

namespace gw2::compression {

/** @Inputs:
 *    - iWidth: Width of the texture
 *    - iHeight: Height of the texture
 *    - iFormatFourCc: FourCC describing the format of the data
 *    - iInputTab: Pointer to the buffer to inflate
 *    - ioOutputTab: Output buffer
 *  @Return:
 *    - Actual size of the outputBuffer
 */
Result<std::uint32_t> inflateTextureBlockBuffer(
    std::uint16_t iWidth, std::uint16_t iHeight, std::uint32_t iFormatFourCc,
    std::span<const std::byte> iInputTab, std::span<std::byte> ioOutputTab);

}  // namespace gw2::compression