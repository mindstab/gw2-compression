#pragma once

#include <array>
#include <cstdint>
#include <concepts>

#include "../utils/BitArray.h"

namespace gw2::compression {

template <std::integral SymbolType, std::uint8_t sMaxCodeBitsLength,
          std::uint16_t sMaxSymbolValue>
class HuffmanTreeBuilder;

// Assumption: code length <= 32
template <std::integral SymbolType, std::uint8_t sNbBitsHash,
          std::uint8_t sMaxCodeBitsLength, std::uint16_t sMaxSymbolValue>
class HuffmanTree {
 public:
  friend class HuffmanTreeBuilder<SymbolType, sMaxCodeBitsLength,
                                  sMaxSymbolValue>;

  template <std::integral IntType>
  void readCode(utils::BitArray<IntType>& iBitArray,
                SymbolType& oSymbol) const {
    std::uint32_t aHashValue;
    iBitArray.readLazy<sNbBitsHash>(aHashValue);

    if (_symbolValueHashExistenceArray[aHashValue]) {
      oSymbol = _symbolValueHashArray[aHashValue];
      iBitArray.drop(_codeBitsHashArray[aHashValue]);
    } else {
      iBitArray.readLazy(aHashValue);

      std::uint16_t anIndex = 0;
      while (aHashValue < _codeComparisonArray[anIndex]) {
        ++anIndex;
      }

      std::uint8_t aNbBits = _codeBitsArray[anIndex];
      oSymbol =
          _symbolValueArray[_symbolValueArrayOffsetArray[anIndex] -
                            ((aHashValue - _codeComparisonArray[anIndex]) >>
                             (32 - aNbBits))];
      iBitArray.drop(aNbBits);
    }
  }

 private:
  void clear() {
    _codeComparisonArray.fill(0);
    _symbolValueArrayOffsetArray.fill(0);
    _symbolValueArray.fill(0);
    _codeBitsArray.fill(0);

    _symbolValueHashExistenceArray.fill(false);
    _symbolValueHashArray.fill(0);
    _codeBitsHashArray.fill(0);
  }

  std::array<std::uint32_t, sMaxCodeBitsLength> _codeComparisonArray;
  std::array<std::uint16_t, sMaxCodeBitsLength> _symbolValueArrayOffsetArray;
  std::array<SymbolType, sMaxSymbolValue> _symbolValueArray;
  std::array<std::uint8_t, sMaxCodeBitsLength> _codeBitsArray;

  std::array<bool, (1 << sNbBitsHash)> _symbolValueHashExistenceArray;
  std::array<SymbolType, (1 << sNbBitsHash)> _symbolValueHashArray;
  std::array<std::uint8_t, (1 << sNbBitsHash)> _codeBitsHashArray;
};

template <std::integral SymbolType, std::uint8_t sMaxCodeBitsLength,
          std::uint16_t sMaxSymbolValue>
class HuffmanTreeBuilder {
 public:
  void clear() {
    _symbolListByBitsHeadExistenceArray.fill(false);
    _symbolListByBitsHeadArray.fill(0);

    _symbolListByBitsBodyExistenceArray.fill(false);
    _symbolListByBitsBodyArray.fill(0);
  }

  void addSymbol(SymbolType iSymbol, std::uint8_t iNbBits) {
    if (_symbolListByBitsHeadExistenceArray[iNbBits]) {
      _symbolListByBitsBodyArray[iSymbol] = _symbolListByBitsHeadArray[iNbBits];
      _symbolListByBitsBodyExistenceArray[iSymbol] = true;
      _symbolListByBitsHeadArray[iNbBits] = iSymbol;
    } else {
      _symbolListByBitsHeadArray[iNbBits] = iSymbol;
      _symbolListByBitsHeadExistenceArray[iNbBits] = true;
    }
  }

  template <std::uint8_t sNbBitsHash>
  bool buildHuffmanTree(HuffmanTree<SymbolType, sNbBitsHash, sMaxCodeBitsLength,
                                    sMaxSymbolValue>& oHuffmanTree) {
    if (empty()) {
      return false;
    }

    oHuffmanTree.clear();

    // Building the HuffmanTree
    std::uint32_t aCode = 0;
    std::uint8_t aNbBits = 0;

    // First part, filling hashTable for codes that are of less than 8 bits
    while (aNbBits <= sNbBitsHash) {
      bool anExistence = _symbolListByBitsHeadExistenceArray[aNbBits];

      if (anExistence) {
        SymbolType aCurrentSymbol = _symbolListByBitsHeadArray[aNbBits];

        while (anExistence) {
          // Processing hash values
          std::uint16_t aHashValue = aCode << (sNbBitsHash - aNbBits);
          std::uint16_t aNextHashValue = (aCode + 1) << (sNbBitsHash - aNbBits);

          while (aHashValue < aNextHashValue) {
            oHuffmanTree._symbolValueHashExistenceArray[aHashValue] = true;
            oHuffmanTree._symbolValueHashArray[aHashValue] = aCurrentSymbol;
            oHuffmanTree._codeBitsHashArray[aHashValue] = aNbBits;
            ++aHashValue;
          }

          anExistence = _symbolListByBitsBodyExistenceArray[aCurrentSymbol];
          aCurrentSymbol = _symbolListByBitsBodyArray[aCurrentSymbol];
          --aCode;
        }
      }

      aCode = (aCode << 1) + 1;
      ++aNbBits;
    }

    std::uint16_t aCodeComparisonArrayIndex = 0;
    std::uint16_t aSymbolOffset = 0;

    // Second part, filling classical structure for other codes
    while (aNbBits < sMaxCodeBitsLength) {
      bool anExistence = _symbolListByBitsHeadExistenceArray[aNbBits];

      if (anExistence) {
        SymbolType aCurrentSymbol = _symbolListByBitsHeadArray[aNbBits];

        while (anExistence) {
          // Registering the code
          oHuffmanTree._symbolValueArray[aSymbolOffset] = aCurrentSymbol;

          ++aSymbolOffset;
          anExistence = _symbolListByBitsBodyExistenceArray[aCurrentSymbol];
          aCurrentSymbol = _symbolListByBitsBodyArray[aCurrentSymbol];
          --aCode;
        }

        // Minimum code value for aNbBits bits
        oHuffmanTree._codeComparisonArray[aCodeComparisonArrayIndex] =
            ((aCode + 1) << (32 - aNbBits));

        // Number of bits for l_codeCompIndex index
        oHuffmanTree._codeBitsArray[aCodeComparisonArrayIndex] = aNbBits;

        // Offset in symbolValueTab table to reach the value
        oHuffmanTree._symbolValueArrayOffsetArray[aCodeComparisonArrayIndex] =
            aSymbolOffset - 1;

        ++aCodeComparisonArrayIndex;
      }

      aCode = (aCode << 1) + 1;
      ++aNbBits;
    }

    return true;
  }

 private:
  bool empty() const {
    for (auto it = _symbolListByBitsHeadExistenceArray.begin();
         it != _symbolListByBitsHeadExistenceArray.end(); ++it) {
      if (*it == true) {
        return false;
      }
    }
    return true;
  }

  std::array<bool, sMaxCodeBitsLength> _symbolListByBitsHeadExistenceArray;
  std::array<SymbolType, sMaxCodeBitsLength> _symbolListByBitsHeadArray;

  std::array<bool, sMaxSymbolValue> _symbolListByBitsBodyExistenceArray;
  std::array<SymbolType, sMaxSymbolValue> _symbolListByBitsBodyArray;
};

}  // namespace gw2::compression
