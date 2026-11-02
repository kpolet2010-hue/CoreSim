#include "test_memory.h"

#include "coresim/Memory.h"

using namespace coresim;

void MemoryTests::runTests() {
      Memory memory;
      check(memory.size() == 256 * 1024, "memory is 256 KB");

      memory.write32(0x100, 0x12345678);
      check(memory.read8(0x100) == 0x78, "write32 is little-endian");
      check(memory.read32(0x100) == 0x12345678, "read32 round-trips");

      // Out-of-bounds writes
      check(!throwsOutOfRange([&] { memory.write8(kMemorySize - 1, 0xAA); }),
            "write8 at last address is allowed");
      check(throwsOutOfRange([&] { memory.write8(kMemorySize, 0xAA); }),
            "write8 past end throws");
      check(throwsOutOfRange([&] { memory.write8(0xFFFFFFFF, 0xAA); }),
            "write8 at huge address throws");
      check(!throwsOutOfRange([&] { memory.write32(kMemorySize - 4, 0x11223344); }),
            "write32 ending at last byte is allowed");
      check(throwsOutOfRange([&] { memory.write32(kMemorySize - 3, 0x11223344); }),
            "write32 crossing end throws");
      memory.write8(kMemorySize - 3, 0x00);
      throwsOutOfRange([&] { memory.write32(kMemorySize - 3, 0xDEADBEEF); });

      // Out-of-bounds reads
      check(!throwsOutOfRange([&] { memory.read8(kMemorySize - 1); }),
            "read8 at last address is allowed");
      check(throwsOutOfRange([&] { memory.read8(kMemorySize); }),
            "read8 past end throws");
      check(!throwsOutOfRange([&] { memory.read32(kMemorySize - 4); }),
            "read32 ending at last byte is allowed");
      check(throwsOutOfRange([&] { memory.read32(kMemorySize - 3); }),
            "read32 crossing end throws");

      check(memory.read8(kMemorySize - 3) == 0x00,
            "failed write32 leaves memory unchanged");
}


