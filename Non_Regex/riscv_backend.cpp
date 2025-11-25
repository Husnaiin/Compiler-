// riscv_backend.cpp
#include "riscv_backend.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

static std::string sanitize(const std::string& s) {
	std::string out;
	out.reserve(s.size());
	for (char c : s) {
		if (std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '.') out.push_back(c);
		else out.push_back('_');
	}
	return out;
}

static void emitPrologue(std::ostringstream& os, int frame_bytes) {
	// Prologue:
	//   addi sp, sp, -frame
	//   sw ra, frame-4(sp)
	//   sw s0, frame-8(sp)
	//   addi s0, sp, frame
	os << "    addi sp, sp, -" << frame_bytes << "\n";
	os << "    sw ra, " << (frame_bytes - 4) << "(sp)\n";
	os << "    sw s0, " << (frame_bytes - 8) << "(sp)\n";
	os << "    addi s0, sp, " << frame_bytes << "\n";
}

static void emitEpilogue(std::ostringstream& os, const std::string& ret_label, int frame_bytes) {
	os << ret_label << ":\n";
	// Epilogue:
	//   lw ra, -4(s0)
	//   lw s0, -8(s0)
	//   addi sp, s0, -frame
	//   ret
	os << "    lw ra, -4(s0)\n";
	os << "    lw s0, -8(s0)\n";
	os << "    addi sp, s0, -" << frame_bytes << "\n";
	os << "    ret\n";
}

bool RiscVBackend::emitAssembly(const IRProgram& program, const std::string& asm_output_path) {
	std::ostringstream os;
	os << "    .option nopic\n";

	// Data: globals
	if (!program.global_vars.empty()) {
		os << "    .data\n";
		for (const auto& gv : program.global_vars) {
			os << "    .globl " << sanitize(gv.first) << "\n";
			os << sanitize(gv.first) << ":\n";
			os << "    .word 0\n";
		}
		os << "\n";
	}

	// Read-only data: string literals
	if (!program.string_literals.empty()) {
		os << "    .section .rodata\n";
		for (const auto& kv : program.string_literals) {
			os << sanitize(kv.first) << ":\n";
			os << "    .asciz " << kv.second << "\n";
		}
		os << "\n";
	}

	// Text: functions
	os << "    .text\n";
	for (const auto& func : program.functions) {
		std::string fname = sanitize(func.name);
		os << "    .globl " << fname << "\n";
		os << fname << ":\n";

		// Collect variables/temps used
		std::unordered_set<std::string> syms;
		for (const auto& ins : func.instructions) {
			auto acc = [&](const TACOperand& op) {
				if (!op.isValid()) return;
				if (op.kind == TACOperand::Kind::VAR || op.kind == TACOperand::Kind::TEMP) {
					syms.insert(op.value);
				}
			};
			acc(ins.dest); acc(ins.src1); acc(ins.src2);
		}
		// Add parameter names too (to store a0.. into stack slots)
		for (const auto& p : func.parameters) syms.insert(p);

		// Assign stack offsets (4 bytes each), starting at -12, -16, ...
		std::unordered_map<std::string,int> offset;
		int next_off = -12; // reserve -4: ra, -8: s0
		for (const auto& s : syms) {
			offset[s] = next_off;
			next_off -= 4;
		}
		int frame_size = -next_off - 12 + 12; // total multiple of 16 recommended; keep simple
		if (frame_size % 16 != 0) frame_size += (16 - (frame_size % 16));

		emitPrologue(os, frame_size);

		// Store parameters from a0.. into their stack slots
		for (size_t i = 0; i < func.parameters.size(); ++i) {
			std::string p = func.parameters[i];
			if (!p.empty()) {
				int off = offset[p];
				if (i == 0) os << "    sw a0, " << off << "(s0)\n";
				else if (i == 1) os << "    sw a1, " << off << "(s0)\n";
				else if (i == 2) os << "    sw a2, " << off << "(s0)\n";
				else if (i == 3) os << "    sw a3, " << off << "(s0)\n";
				else if (i == 4) os << "    sw a4, " << off << "(s0)\n";
				else if (i == 5) os << "    sw a5, " << off << "(s0)\n";
				else if (i == 6) os << "    sw a6, " << off << "(s0)\n";
				else if (i == 7) os << "    sw a7, " << off << "(s0)\n";
			}
		}

		auto loadValue = [&](const TACOperand& op, const char* rd) {
			if (op.kind == TACOperand::Kind::CONST) {
				os << "    li " << rd << ", " << op.value << "\n";
			} else if (op.kind == TACOperand::Kind::VAR || op.kind == TACOperand::Kind::TEMP) {
				int off = offset[op.value];
				os << "    lw " << rd << ", " << off << "(s0)\n";
			} else {
				os << "    li " << rd << ", 0\n";
			}
		};
		auto storeValue = [&](const std::string& name, const char* rs) {
			int off = offset[name];
			os << "    sw " << rs << ", " << off << "(s0)\n";
		};

		std::string ret_label = fname + "_ret";

		for (size_t i = 0; i < func.instructions.size(); ++i) {
			const auto& ins = func.instructions[i];
			switch (ins.opcode) {
				case TACOpcode::LABEL:
					os << sanitize(ins.dest.value) << ":\n";
					break;
				case TACOpcode::GOTO:
					os << "    j " << sanitize(ins.dest.value) << "\n";
					break;
				case TACOpcode::IF_TRUE: {
					loadValue(ins.src1, "t0");
					os << "    bne t0, x0, " << sanitize(ins.dest.value) << "\n";
					break;
				}
				case TACOpcode::IF_FALSE: {
					loadValue(ins.src1, "t0");
					os << "    beq t0, x0, " << sanitize(ins.dest.value) << "\n";
					break;
				}
				case TACOpcode::RETURN: {
					loadValue(ins.src1, "a0");
					os << "    j " << ret_label << "\n";
					break;
				}
				case TACOpcode::RETURN_VOID: {
					os << "    j " << ret_label << "\n";
					break;
				}
				case TACOpcode::COPY: {
					loadValue(ins.src1, "t0");
					storeValue(ins.dest.value, "t0");
					break;
				}
				case TACOpcode::ADD:
				case TACOpcode::SUB:
				case TACOpcode::MUL:
				case TACOpcode::DIV:
				case TACOpcode::MOD:
				case TACOpcode::AND:
				case TACOpcode::OR:
				case TACOpcode::XOR:
				case TACOpcode::SHL:
				case TACOpcode::SHR:
				case TACOpcode::EQ:
				case TACOpcode::NE:
				case TACOpcode::LT:
				case TACOpcode::LE:
				case TACOpcode::GT:
				case TACOpcode::GE: {
					loadValue(ins.src1, "t0");
					loadValue(ins.src2, "t1");
					if (ins.opcode == TACOpcode::ADD) os << "    add t2, t0, t1\n";
					else if (ins.opcode == TACOpcode::SUB) os << "    sub t2, t0, t1\n";
					else if (ins.opcode == TACOpcode::MUL) os << "    mul t2, t0, t1\n";
					else if (ins.opcode == TACOpcode::DIV) os << "    div t2, t0, t1\n";
					else if (ins.opcode == TACOpcode::MOD) os << "    rem t2, t0, t1\n";
					else if (ins.opcode == TACOpcode::AND) os << "    and t2, t0, t1\n";
					else if (ins.opcode == TACOpcode::OR)  os << "    or  t2, t0, t1\n";
					else if (ins.opcode == TACOpcode::XOR) os << "    xor t2, t0, t1\n";
					else if (ins.opcode == TACOpcode::SHL) os << "    sll t2, t0, t1\n";
					else if (ins.opcode == TACOpcode::SHR) os << "    sra t2, t0, t1\n";
					else if (ins.opcode == TACOpcode::EQ)  { os << "    sub t2, t0, t1\n"; os << "    seqz t2, t2\n"; }
					else if (ins.opcode == TACOpcode::NE)  { os << "    sub t2, t0, t1\n"; os << "    snez t2, t2\n"; }
					else if (ins.opcode == TACOpcode::LT)  os << "    slt t2, t0, t1\n";
					else if (ins.opcode == TACOpcode::LE)  { os << "    slt t2, t1, t0\n"; os << "    xori t2, t2, 1\n"; }
					else if (ins.opcode == TACOpcode::GT)  os << "    slt t2, t1, t0\n";
					else if (ins.opcode == TACOpcode::GE)  { os << "    slt t2, t0, t1\n"; os << "    xori t2, t2, 1\n"; }
					storeValue(ins.dest.value, "t2");
					break;
				}
				case TACOpcode::NEG: {
					loadValue(ins.src1, "t0");
					os << "    neg t2, t0\n";
					storeValue(ins.dest.value, "t2");
					break;
				}
				case TACOpcode::LNOT: {
					loadValue(ins.src1, "t0");
					os << "    seqz t2, t0\n";
					storeValue(ins.dest.value, "t2");
					break;
				}
				case TACOpcode::NOT: {
					loadValue(ins.src1, "t0");
					os << "    not t2, t0\n";
					storeValue(ins.dest.value, "t2");
					break;
				}
				case TACOpcode::PARAM:
					// For simplicity, ignore; CALL will pass zeros for args
					break;
				case TACOpcode::CALL: {
					int nargs = std::atoi(ins.src2.value.c_str());
					// Pass zeros in a0.. per nargs (placeholder)
					if (nargs > 0) os << "    li a0, 0\n";
					if (nargs > 1) os << "    li a1, 0\n";
					if (nargs > 2) os << "    li a2, 0\n";
					if (nargs > 3) os << "    li a3, 0\n";
					if (nargs > 4) os << "    li a4, 0\n";
					if (nargs > 5) os << "    li a5, 0\n";
					if (nargs > 6) os << "    li a6, 0\n";
					if (nargs > 7) os << "    li a7, 0\n";
					os << "    call " << sanitize(ins.src1.value) << "\n";
					// Return in a0
					storeValue(ins.dest.value, "a0");
					break;
				}
				default:
					// Unhandled ops (arrays, memory, prints) -> no-op comments
					os << "    # unhandled opcode\n";
					break;
			}
		}

		emitEpilogue(os, ret_label, frame_size);
		os << "\n";
	}

	std::ofstream out(asm_output_path);
	if (!out.is_open()) return false;
	out << os.str();
	out.close();
	return true;
}


