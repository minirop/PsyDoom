//------------------------------------------------------------------------------------------------------------------------------------------
// VM interface functions.
// Functionality which helps us emulate the original hardware environment of the program.
//------------------------------------------------------------------------------------------------------------------------------------------

#define PSX_VM_NO_REGISTER_MACROS 1     // Because they cause conflicts with Avocado
#include "PsxVm.h"

#include <cstdlib>
#include <system.h>

namespace PsxVm {
    const uint32_t* gpReg_zero;
    uint32_t* gpReg_at;
    uint32_t* gpReg_v0;
    uint32_t* gpReg_v1;
    uint32_t* gpReg_a0;
    uint32_t* gpReg_a1;
    uint32_t* gpReg_a2;
    uint32_t* gpReg_a3;
    uint32_t* gpReg_t0;
    uint32_t* gpReg_t1;
    uint32_t* gpReg_t2;
    uint32_t* gpReg_t3;
    uint32_t* gpReg_t4;
    uint32_t* gpReg_t5;
    uint32_t* gpReg_t6;
    uint32_t* gpReg_t7;
    uint32_t* gpReg_s0;
    uint32_t* gpReg_s1;
    uint32_t* gpReg_s2;
    uint32_t* gpReg_s3;
    uint32_t* gpReg_s4;
    uint32_t* gpReg_s5;
    uint32_t* gpReg_s6;
    uint32_t* gpReg_s7;
    uint32_t* gpReg_t8;
    uint32_t* gpReg_t9;
    uint32_t* gpReg_k0;
    uint32_t* gpReg_k1;
    uint32_t* gpReg_gp;
    uint32_t* gpReg_sp;
    uint32_t* gpReg_fp;
    uint32_t* gpReg_ra;
    uint32_t* gpReg_hi;
    uint32_t* gpReg_lo;
}

static void notImplementedError() noexcept {
    std::abort();
}

void tge(
    [[maybe_unused]] const uint32_t r1,
    [[maybe_unused]] const uint32_t r2,
    [[maybe_unused]] const uint16_t i
) noexcept {
    // Shouldn't ever be invoking this instruction!
    std::abort();
}

uint32_t add(const uint32_t r1, const uint32_t r2) noexcept {
    notImplementedError();
    return 0;
}

uint32_t addi(const uint32_t r1, const int16_t i) noexcept {
    notImplementedError();
    return 0;
}

void div(const uint32_t r1, const uint32_t r2) noexcept {
    notImplementedError();
}

void divu(const uint32_t r1, const uint32_t r2) noexcept {
    notImplementedError();
}

void mult(const uint32_t r1, const uint32_t r2) noexcept {
    notImplementedError();
}

void multu(const uint32_t r1, const uint32_t r2) noexcept {
    notImplementedError();
}

uint32_t sub(const uint32_t r1, const uint32_t r2) noexcept {
    notImplementedError();
    return 0;
}

uint32_t lb(const uint32_t addr) noexcept {
    return PsxVm::gpCpu->sys->readMemory8(addr);
}

uint32_t lbu(const uint32_t addr) noexcept {
    return (uint32_t)(int32_t)(int8_t) PsxVm::gpCpu->sys->readMemory8(addr);
}

uint32_t lh(const uint32_t addr) noexcept {
    return (uint32_t)(int32_t)(int16_t) PsxVm::gpCpu->sys->readMemory16(addr);
}

uint32_t lhu(const uint32_t addr) noexcept {
    return PsxVm::gpCpu->sys->readMemory16(addr);
}

uint32_t lw(const uint32_t addr) noexcept {
    return PsxVm::gpCpu->sys->readMemory32(addr);
}

uint32_t lwl(const uint32_t r1, const uint32_t addr) noexcept {
    notImplementedError();
    return 0;
}

uint32_t lwr(const uint32_t r1, const uint32_t addr) noexcept {
    notImplementedError();
    return 0;
}

void sb(const uint32_t r1, const uint32_t addr) noexcept {
    PsxVm::gpCpu->sys->writeMemory8(addr, (uint8_t) r1);
}

void sh(const uint32_t r1, const uint32_t addr) noexcept {
    PsxVm::gpCpu->sys->writeMemory16(addr, (uint16_t) r1);
}

void sw(const uint32_t r1, const uint32_t addr) noexcept {
    PsxVm::gpCpu->sys->writeMemory32(addr, r1);
}

void swl(const uint32_t r1, const uint32_t addr) noexcept {
    notImplementedError();
}

void swr(const uint32_t r1, const uint32_t addr) noexcept {
    notImplementedError();
}

uint32_t mfc0(const uint8_t s) noexcept {
    notImplementedError();
    return 0;
}

void mtc0(const uint32_t r1, const uint8_t d) noexcept {
    notImplementedError();
}

void cop2(const uint32_t i) noexcept {
    notImplementedError();
}

uint32_t cfc2(const uint8_t s) noexcept {
    notImplementedError();
    return 0;
}

void ctc2(const uint32_t r1, const uint8_t d) noexcept {
    notImplementedError();
}

uint32_t mfc2(const uint8_t s) noexcept {
    notImplementedError();
    return 0;
}

void mtc2(const uint32_t r1, const uint8_t d) noexcept {
    notImplementedError();
}

void lwc2(const uint8_t d, const uint32_t addr) noexcept {
    notImplementedError();
}

void swc2(const uint8_t s, const uint32_t addr) noexcept {
    notImplementedError();
}

void _break(const uint32_t i) noexcept {
    notImplementedError();
}

void syscall(const uint32_t i) noexcept {
    notImplementedError();
}

void pcall(const uint32_t addr) noexcept {
    notImplementedError();
}

void bios_call(const uint32_t func) noexcept {
    notImplementedError();
}

void jump_table_err() noexcept {
    notImplementedError();
}