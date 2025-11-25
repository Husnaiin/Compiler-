// riscv_backend.h
#ifndef RISCV_BACKEND_H
#define RISCV_BACKEND_H

#include "ir_generator.h"
#include <string>

// Minimal RISC-V (RV32I/M) text assembly emitter for TAC
class RiscVBackend {
public:
	// Emit GNU assembler RISC-V source (.s) from TAC program
	// rv32im is assumed (for mul/div). If you only have rv32i, avoid MUL/DIV/MOD in programs.
	bool emitAssembly(const IRProgram& program, const std::string& asm_output_path);
};

#endif // RISCV_BACKEND_H


