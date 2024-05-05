/*
 * @Author       : Chivier Humber
 * @Date         : 2021-09-15 21:15:24
 * @LastEditors  : Chivier Humber
 * @LastEditTime : 2021-11-23 16:08:51
 * @Description  : file content
 */
#include "common.h"
#include "memory.h"

namespace virtual_machine_nsp {
    void memory_tp::ReadMemoryFromFile(std::string filename, int beginning_address) {
        // read the memory from file
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Unable to open file: " << filename << std::endl;
            return;
        }

        int16_t value;
        int address = beginning_address;
        std::string line;
        while (file >> line) {
            value = TranslateInstruction(line);
            if (address >= kVirtualMachineMemorySize) {
                throw MyError("Memory overflow");
                break;
            }
            memory[address++] = value;
        }

        file.close();
    }

    void memory_tp::setMemory(int16_t address, int16_t value) {
        // set the memory
        if(address >= kVirtualMachineMemorySize || address < 0){
            throw MyError("Address out of range");
        }
        memory[address] = value;
    }

    int16_t memory_tp::GetContent(int address, bool IsUserMode) const {
        // get the content
        if(address >= kVirtualMachineMemorySize || address < 0){
            throw MyError("Address out of range");
        }
        if(IsUserMode && address > 0xFDFF || address < 0x3000){
            throw MyError("ACV violation");
        }
        return memory[address];
    }

    int16_t& memory_tp::operator[](int address) {
        // get the content
        if(address >= kVirtualMachineMemorySize || address < 0){
            throw MyError("Address out of range");
        }
        return memory[address];
    }    
}; // virtual machine namespace
