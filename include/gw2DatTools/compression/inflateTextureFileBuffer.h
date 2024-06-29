#pragma once

#include <cstdint>
#include <cstddef>
#include <span>

namespace gw2::compression
{

/** @Inputs:
 *    - iWidth: Width of the texture
 *    - iHeight: Height of the texture
 *    - iFormatFourCc: FourCC describing the format of the data
 *    - iInputSize: Size of the input buffer
 *    - iInputTab: Pointer to the buffer to inflate
 *    - ioOutputSize: if the value is 0 then we decode everything
 *                    else we decode until we reach the io_outputSize
 *    - ioOutputTab: Optional output buffer, in case you provide this buffer,
 *                   ioOutputSize shall be inferior or equal to the size of this buffer
 *  @Outputs:
 *    - ioOutputSize: actual size of the outputBuffer
 *  @Return:
 *    - Pointer to the outputBuffer, nullptr if it failed
 *  @Throws:
 *    - gw2dt::exception::Exception or std::exception in case of error
 */
std::uint32_t inflateTextureBlockBuffer(std::uint16_t iWidth, std::uint16_t iHeight, std::uint32_t iFormatFourCc, std::span<const std::byte> iInputTab, std::span<std::byte> ioOutputTab);

}