#pragma once

#include <cassert>
#include <cstdint>

namespace gw2::compression {

static constexpr std::uint32_t MaxCodeBitsLength =
    32;  // Max number of bits per code
static constexpr std::uint32_t MaxSymbolValue = 285;  // Max value for a symbol
static constexpr std::uint32_t MaxNbBitsHash = 8;

struct HuffmanTree {
  std::uint32_t codeCompTab[MaxCodeBitsLength];
  std::uint16_t symbolValueTabOffsetTab[MaxCodeBitsLength];
  std::uint16_t symbolValueTab[MaxSymbolValue];
  std::uint8_t codeBitsTab[MaxCodeBitsLength];
  std::int16_t symbolValueHashTab[1 << MaxNbBitsHash];
  std::uint8_t codeBitsHashTab[1 << MaxNbBitsHash];
  bool isEmpty;
};

struct State {
  const std::uint32_t* input;
  std::uint32_t inputSize;
  std::uint32_t inputPos;
  std::uint32_t head;
  std::uint32_t buffer;
  std::uint8_t bits;
  bool isEmpty;
};

void buildHuffmanTree(HuffmanTree& ioHuffmanTree, std::int16_t* ioWorkingBitTab,
                      std::int16_t* ioWorkingCodeTab);
void fillWorkingTabsHelper(const std::uint8_t iBits, const std::int16_t iSymbol,
                           std::int16_t* ioWorkingBitTab,
                           std::int16_t* ioWorkingCodeTab);

// Read the next code
void readCode(const HuffmanTree& iHuffmanTree, State& ioState,
              std::uint16_t& ioCode);

// Bits manipulation
inline void pullByte(State& ioState) {
  // checking that we have less than 32 bits available
  assert(ioState.bits < 32 &&
         "Tried to pull a value while we still have 32 bits available.");

  // skip the last element of all 65536 bytes blocks
  if ((ioState.inputPos + 1) % (0x4000) == 0) {
    ++(ioState.inputPos);
  }

  // Fetching the next value
  std::uint32_t aValue = 0;

  // checking that inputPos is not out of bounds
  if (ioState.inputPos >= ioState.inputSize) {
    assert(!ioState.isEmpty &&
           "Reached end of input while trying to fetch a new byte.");
    ioState.isEmpty = true;
  } else {
    aValue = ioState.input[ioState.inputPos];
  }

  // Pulling the data into head/buffer given that we need to keep the relevant
  // bits
  if (ioState.bits == 0) {
    ioState.head = aValue;
    ioState.buffer = 0;
  } else {
    ioState.head |= (aValue >> (ioState.bits));
    ioState.buffer = (aValue << (32 - ioState.bits));
  }

  // Updating state variables
  ioState.bits += 32;
  ++(ioState.inputPos);
}

inline void needBits(State& ioState, const std::uint8_t iBits) {
  // checking that we request at most 32 bits
  assert(iBits <= 32 && "Tried to need more than 32 bits.");

  if (ioState.bits < iBits) {
    pullByte(ioState);
  }
}

inline void dropBits(State& ioState, const std::uint8_t iBits) {
  // checking that we request at most 32 bits
  assert(iBits <= 32 && "Tried to drop more than 32 bits.");

  assert(iBits <= ioState.bits && "Tried to drop more bits than we have.");

  // Updating the values to drop the bits
  if (iBits == 32) {
    ioState.head = ioState.buffer;
    ioState.buffer = 0;
  } else {
    ioState.head <<= iBits;
    ioState.head |= (ioState.buffer) >> (32 - iBits);
    ioState.buffer <<= iBits;
  }

  // update state info
  ioState.bits -= iBits;
}

inline std::uint32_t readBits(const State& iState, const std::uint8_t iBits) {
  return (iState.head) >> (32 - iBits);
}

}  // namespace gw2::compression
