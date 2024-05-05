/*
 * @Author       : Chivier Humber
 * @Date         : 2021-09-14 21:41:49
 * @LastEditors  : Chivier Humber
 * @LastEditTime : 2021-11-24 05:33:45
 * @Description  : file content
 */
#include "simulator.h"

using namespace virtual_machine_nsp;
namespace po = boost::program_options;

bool gIsSingleStepMode = false;
bool gIsDetailedMode = false;
std::string gInputFileName = "input.txt";
std::string gRegisterStatusFileName = "register.txt";
std::string gOutputFileName = "";
int gBeginningAddress = 0x3000;

int main(int argc, char **argv) {
    po::options_description desc{"\e[1mLC3 SIMULATOR\e[0m\n\n\e[1mOptions\e[0m"};
    desc.add_options()                                                                             //
        ("help,h", "Help screen")                                                                  //
        ("file,f", po::value<std::string>()->default_value("input.txt"), "Input file")             //
        ("register,r", po::value<std::string>()->default_value("register.txt"), "Register Status") //
        ("single,s", "Single Step Mode")                                                           //
        ("begin,b", po::value<int>()->default_value(0x3000), "Begin address (0x3000)")
        ("output,o", po::value<std::string>()->default_value(""), "Output file")
        ("detail,d", "Detailed Mode")
        ("setm,m", po::value<std::string>()->default_value(""), "Set memory"); // 新增

    po::variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }
    if (vm.count("file")) {
        gInputFileName = vm["file"].as<std::string>();
    }
    if (vm.count("register")) {
        gRegisterStatusFileName = vm["register"].as<std::string>();
    }
    if (vm.count("single")) {
        gIsSingleStepMode = true;
    }
    if (vm.count("begin")) {
        gBeginningAddress = vm["begin"].as<int>();
    }
    if (vm.count("output")) {
        gOutputFileName = vm["output"].as<std::string>();
    }
    if (vm.count("detail")) {
        gIsDetailedMode = true;
    }

    virtual_machine_tp virtual_machine(gBeginningAddress, gInputFileName, gRegisterStatusFileName);

    if (vm.count("setm")) {
        std::string gSetMemory = vm["setm"].as<std::string>();
        // 解析 gSetMemory，设置指定的内存地址的值
        //gSetMemory = vm["set"].as<std::string>();
        //std::stringstream ss(gSetMemory);
        std::string address, value;
        //std::getline(ss, address, '=');
        //std::getline(ss, value);
        address = gSetMemory.substr(2, 4);
        value = gSetMemory.substr(9, 4);
        int16_t addr = std::stoi(address, nullptr, 16); // 将十六进制字符串转换为整数
        int16_t val = std::stoi(value, nullptr, 16); // 将十六进制字符串转换为整数
        // 设置内存地址 addr 的值为 val
        try {
            virtual_machine.mem.setMemory(addr, val);
        }
        catch (MyError& e) {
            std::cout << e.what() << std::endl;
        }
    }


    int halt_flag = true;
    int time_flag = 0;
    while(halt_flag) {
        // Single step
        if (gIsSingleStepMode) {
            std::cout << "Press any key to continue..." << std::endl;
            getchar();
        }
        virtual_machine.NextStep();
        if (gIsDetailedMode)
            std::cout << virtual_machine.reg << std::endl;
        ++time_flag;
        if (virtual_machine.reg[R_PC] == 0x00) {
            halt_flag = false;
        }
    }

    // Output the memory status to file
    if (gOutputFileName != "") {
        std::ofstream output_file(gOutputFileName);
        if (!output_file) {
            std::cerr << "Unable to open file: " << gOutputFileName << std::endl;
            return 0;
        }
        for (int i = gBeginningAddress; i < kVirtualMachineMemorySize; ++i) {
            output_file << "M[" << std::hex << std::setw(4) << std::setfill('0') << i << "]:\t";
            output_file << std::hex << std::setw(4) << std::setfill('0') << virtual_machine.mem[i] << std::endl;
        }
        output_file.close();
    }

    // Output the register status
    std::cout << "-----------------------HALT-----------------------" << std::endl;
    std::cout << "Register final status:" << std::endl;
    std::cout << virtual_machine.reg << std::endl;
    std::cout << "cycle = " << time_flag << std::endl;
    return 0;
}