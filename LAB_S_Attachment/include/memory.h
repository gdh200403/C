/*
 * @Author       : Chivier Humber
 * @Date         : 2021-09-15 21:15:30
 * @LastEditors  : Chivier Humber
 * @LastEditTime : 2021-09-22 20:02:31
 * @Description  : file content
 */
#include "common.h"

namespace virtual_machine_nsp {
const int kInstructionLength = 16;

inline int16_t TranslateInstruction(std::string &line) {
    // TODO: translate hex mode to int16_t
    if(line.size() != 4){
        throw MyError("Instruction length is not 4");
    }
    try {
        return static_cast<int16_t>(std::stoi(line, nullptr, 16));
    } catch (const std::invalid_argument& ia) {
        std::cerr << "Invalid argument: " << ia.what() << '\n';
        return 0;
    } catch (const std::out_of_range& oor) {
        std::cerr << "Out of range: " << oor.what() << '\n';
        return 0;
    }
}

const int kVirtualMachineMemorySize = 0xFFFF;

class memory_tp {
    private:
    int16_t memory[kVirtualMachineMemorySize];

    public:
    memory_tp() {
        memset(memory, 0, sizeof(int16_t) * kVirtualMachineMemorySize);
    }
    // Managements
    void ReadMemoryFromFile(std::string filename, int beginning_address=0x3000);
    void setMemory(int16_t address, int16_t value);
    int16_t GetContent(int address, bool IsUserMode) const;
    int16_t& operator[](int address);
};

}; // virtual machine nsp