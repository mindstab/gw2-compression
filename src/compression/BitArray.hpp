#pragma once

#include <bit>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>

namespace gw2::utils {

template <std::integral IntType>
class BitArray {
 public:
  BitArray(std::span<const std::byte> ipBuffer, std::uint32_t iSkippedBytes = 0)
      : _pBufferStartPos(ipBuffer.data()),
        _pBufferPos(ipBuffer.data()),
        _bytesAvail(ipBuffer.size()),
        _skippedBytes(iSkippedBytes),
        _head(0),
        _buffer(0),
        _bitsAvail(0) {
    assert(ipBuffer.size() % sizeof(IntType) == 0);

    pull(_head, _bitsAvail);
  }

  void readLazy(std::uint8_t iBitNumber, std::integral auto& oValue) const {
    assert((iBitNumber <= sizeof(oValue) * 8) &&
           "Invalid number of bits requested.");
    assert((iBitNumber <= sizeof(IntType) * 8) &&
           "Invalid number of bits requested.");

    readImpl(iBitNumber, oValue);
  }

  template <std::uint8_t isBitNumber>
  void readLazy(std::integral auto& oValue) const {
    static_assert(
        isBitNumber <= sizeof(oValue) * 8,
        "isBitNumber must be inferior to the size of the requested type.");
    static_assert(
        isBitNumber <= sizeof(IntType) * 8,
        "isBitNumber must be inferior to the size of the internal type.");

    readImpl(isBitNumber, oValue);
  }

  void readLazy(std::integral auto& oValue) const {
    readLazy<sizeof(oValue) * 8>(oValue);
  }

  void read(std::uint8_t iBitNumber, std::integral auto& oValue) const {
    assert(_bitsAvail >= iBitNumber &&
           "Not enough bits available to read the value.");
    readLazy(iBitNumber, oValue);
  }

  template <std::uint8_t isBitNumber>
  void read(std::integral auto& oValue) const {
    assert(_bitsAvail >= isBitNumber &&
           "Not enough bits available to read the value.");
    readLazy<isBitNumber>(oValue);
  }

  void read(std::integral auto& oValue) const {
    read<sizeof(oValue) * 8>(oValue);
  }

  void drop(std::uint8_t iBitNumber) {
    assert((iBitNumber <= sizeof(IntType) * 8) &&
           "Invalid number of bits to be dropped.");
    dropImpl(iBitNumber);
  }

  template <std::uint8_t isBitNumber>
  void drop() {
    static_assert(
        isBitNumber <= sizeof(IntType) * 8,
        "isBitNumber must be inferior to the size of the internal type.");
    dropImpl(isBitNumber);
  }

  template <std::integral OutputType>
  void drop() {
    drop<sizeof(OutputType) * 8>();
  }

 private:
  void readImpl(std::uint8_t iBitNumber, std::integral auto& oValue) const {
    oValue = (_head >> ((sizeof(IntType) * 8) - iBitNumber));
  }

  void dropImpl(std::uint8_t iBitNumber) {
    assert(_bitsAvail >= iBitNumber &&
           "Too much bits were asked to be dropped.");

    std::uint8_t aNewBitsAvail = _bitsAvail - iBitNumber;
    if (aNewBitsAvail >= sizeof(IntType) * 8) {
      if (iBitNumber == sizeof(IntType) * 8) {
        _head = _buffer;
        _buffer = 0;
      } else {
        _head = (_head << iBitNumber) |
                (_buffer >> ((sizeof(IntType) * 8) - iBitNumber));
        _buffer = _buffer << iBitNumber;
      }
      _bitsAvail = aNewBitsAvail;
    } else {
      IntType aNewValue;
      std::uint8_t aNbPulledBits;
      pull(aNewValue, aNbPulledBits);

      if (iBitNumber == sizeof(IntType) * 8) {
        _head = 0;
      } else {
        _head = _head << iBitNumber;
      }
      _head |= (_buffer >> ((sizeof(IntType) * 8) - iBitNumber)) |
               (aNewValue >> (aNewBitsAvail));
      if (aNewBitsAvail > 0) {
        _buffer = aNewValue << ((sizeof(IntType) * 8) - aNewBitsAvail);
      }
      _bitsAvail = aNewBitsAvail + aNbPulledBits;
    }
  }

  void pull(IntType& oValue, std::uint8_t& oNbPulledBits) {
    if (_bytesAvail >= sizeof(IntType)) {
      if (_skippedBytes != 0) {
        if (_skippedBytes == 0xffff) {
          auto distance = std::distance(_pBufferStartPos, _pBufferPos);
          if (distance > 0 && ((distance + 12) % 0x10000 == 0)) {
            _bytesAvail -= sizeof(IntType);
            _pBufferPos += sizeof(IntType);
          }
        } else if (_pBufferPos != _pBufferStartPos &&
                   (((_pBufferPos - _pBufferStartPos) / sizeof(IntType)) + 1) %
                           _skippedBytes ==
                       0) {
          _bytesAvail -= sizeof(IntType);
          _pBufferPos += sizeof(IntType);
        }
      }
      oValue = *(std::bit_cast<const IntType*>(_pBufferPos));
      _bytesAvail -= sizeof(IntType);
      _pBufferPos += sizeof(IntType);
      oNbPulledBits = sizeof(IntType) * 8;
    } else {
      oValue = 0;
      oNbPulledBits = 0;
    }
  }

  const std::byte* const _pBufferStartPos;
  const std::byte* _pBufferPos;
  std::uint32_t _bytesAvail;

  std::uint32_t _skippedBytes;

  IntType _head;
  IntType _buffer;
  std::uint8_t _bitsAvail;
};

}  // namespace gw2::utils