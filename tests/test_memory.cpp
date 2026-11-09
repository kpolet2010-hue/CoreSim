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

      // 16-bit accesses, little-endian
      memory.write8(0x1FFFF, 0xAA);
      memory.write8(0x20002, 0xBB);
      memory.write16(0x20000, 0x1234);
      check(memory.read8(0x20000) == 0x34, "write16 stores the low byte first");
      check(memory.read8(0x20001) == 0x12, "write16 stores the high byte second");
      check(memory.read16(0x20000) == 0x1234, "read16 round-trips");
      check(memory.read8(0x1FFFF) == 0xAA, "write16 keeps the byte before");
      check(memory.read8(0x20002) == 0xBB, "write16 keeps the byte after");

      memory.write8(0x300, 0xCD);
      memory.write8(0x301, 0xAB);
      check(memory.read16(0x300) == 0xABCD, "read16 assembles two bytes written by write8");

      memory.write16(0x401, 0xBEEF);
      check(memory.read16(0x401) == 0xBEEF, "write16/read16 at an odd address");

      memory.write16(0x500, 0xFFFF);
      check(memory.read16(0x500) == 0xFFFF, "read16 returns the full 16-bit range");

      // 16-bit out-of-bounds accesses
      check(!throwsOutOfRange([&] { memory.write16(kMemorySize - 2, 0x1122); }),
            "write16 ending at last byte is allowed");
      check(throwsOutOfRange([&] { memory.write16(kMemorySize - 1, 0x1122); }),
            "write16 crossing end throws");
      check(!throwsOutOfRange([&] { memory.read16(kMemorySize - 2); }),
            "read16 ending at last byte is allowed");
      check(throwsOutOfRange([&] { memory.read16(kMemorySize - 1); }),
            "read16 crossing end throws");

      memory.write8(kMemorySize - 1, 0x00);
      throwsOutOfRange([&] { memory.write16(kMemorySize - 1, 0xDEAD); });
      check(memory.read8(kMemorySize - 1) == 0x00,
            "failed write16 leaves memory unchanged");
}


