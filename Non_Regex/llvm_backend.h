// llvm_backend.h
#ifndef LLVM_BACKEND_H
#define LLVM_BACKEND_H

#include "ir_generator.h"
#include <string>

// A lightweight LLVM IR text generator that lowers TAC (IRProgram) to LLVM IR (.ll)
// Focuses on integer-centric programs (int/bool/char) and basic control flow.
// Outputs a textual module you can feed into llvm-as/llc/clang.
class LLVMBackend {
public:
	// Generate LLVM IR text from TAC program and write to ll_output_path.
	// module_name: used as the source_filename in the LLVM module.
	// Returns true on success.
	bool emitModule(const IRProgram& program,
	                const std::string& module_name,
	                const std::string& ll_output_path);

private:
	// Internal helpers
	std::string mapBaseType(const std::string& base) const;   // "int" -> "i32"
	std::string mapValueType(const TACOperand& op) const;      // operand to LLVM type
	std::string sanitizeLabel(const std::string& label) const; // make valid LLVM label
	std::string sanitizeName(const std::string& name) const;   // valid LLVM identifier
};

#endif // LLVM_BACKEND_H


