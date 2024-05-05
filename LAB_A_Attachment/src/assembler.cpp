/*
 * @Author       : Chivier Humber
 * @Date         : 2021-08-30 15:10:31
 * @LastEditors  : Donghao Guo
 * @LastEditTime : 2024-1-23 11:59:19
 * @Description  : content for samll assembler
 */

#include "assembler.h"
#include <string>

// add label and its address to symbol table
void LabelMapType::AddLabel(const std::string &str, const unsigned address) {
    labels_.insert({str, address});
}

unsigned LabelMapType::GetAddress(const std::string &str) const {
    if (labels_.find(str) == labels_.end()) {
        // not found
        return -1;
    }
    return labels_.at(str);
}

std::string assembler::TranslateOprand(unsigned int current_address, std::string str,
                                       unsigned int line_number, int opcode_length) {
    // Translate the oprand
    str = Trim(str);
    auto item = label_map.GetAddress(str);
    if (item != -1) {
        // str is a label
        int offset = item - current_address - 1;
        if (offset > pow2(opcode_length - 1) || offset < -pow2(opcode_length - 1)) {
            // @ Error offset too large
            throw AssemblyError("Error on line " + std::to_string(line_number) + ": Offset too large");
        }
        std::string offset_str = std::bitset<11>(offset).to_string();
        return offset_str.substr(11 - opcode_length);
    }
    if (str[0] == 'R') {
        // str is a register
        int register_num = stoi(str.substr(1), nullptr, 10);
        if (register_num > 7 || register_num < 0) {
            // @ Error Invalid register number
            throw AssemblyError("Error on line " + std::to_string(line_number) + ": Invalid register number");
        }
        return std::bitset<3>(register_num).to_string();
    } else {
        // str is an immediate number
        int immediate_num = RecognizeNumberValue(str);
        if (immediate_num > pow2(opcode_length - 1) || immediate_num < -pow2(opcode_length - 1)) {
            // @ Error Invalid immediate number
            throw AssemblyError("Error on line " + std::to_string(line_number) + ": Invalid immediate number");
        }
        std::string imm_str = std::bitset<11>(immediate_num).to_string();
        return imm_str.substr(11 - opcode_length);
    }
}

bool isValidLabel(const std::string& label) {
    if (label.length() < 1 || label.length() > 20) {
        return false;
    }
    if (!std::isalpha(label[0])) {
        return false;
    }
    if (!std::all_of(label.begin(), label.end(), ::isalnum)) {
        return false;
    }
    return true;
}

std::string assembler::LineLabelSplit(const std::string &line,
                                      int current_address) {
    // label?
    auto first_whitespace_position = line.find(' ');
    auto first_token = line.substr(0, first_whitespace_position);

    if (IsLC3Pseudo(first_token) == -1 && IsLC3Command(first_token) == -1 &&
        IsLC3TrapRoutine(first_token) == -1) {
        // * This is an label

        // check if the label is valid
        if (!isValidLabel(first_token)) {
            throw AssemblyError("Invalid label: " + first_token);
        }
        // save it in label_map
        label_map.AddLabel(first_token, current_address);

        // remove label from the line
        if (first_whitespace_position == std::string::npos) {
            // nothing else in the line
            return "";
        }
        auto command = line.substr(first_whitespace_position + 1);
        return Trim(command);
    }
    return line;
}

std::string removeQuotesAndHandleEscapeSequences(const std::string& str) {
    // 去除双引号
    std::string noQuotes = str.substr(1, str.size() - 2);

    // 处理转义字符
    std::string result;
    for (size_t i = 0; i < noQuotes.size(); ++i) {
        if (noQuotes[i] == '\\' && i + 1 < noQuotes.size()) {
            switch (noQuotes[i + 1]) {
                case 'n':
                    result.push_back('\n');
                    ++i;  // 跳过下一个字符
                    break;
                case 't':
                    result.push_back('\t');
                    ++i;  // 跳过下一个字符
                    break;
                case 'r':
                    result.push_back('\r');
                    ++i;  // 跳过下一个字符
                    break;
                case '\"':
                    result.push_back('\"');
                    ++i;  // 跳过下一个字符
                    break;
                case '\\':
                    result.push_back('\\');
                    ++i;  // 跳过下一个字符
                    break;
                // 其他转义字符的处理可以在这里添加
                default:
                    result.push_back(noQuotes[i]);
                    break;
            }
        } else {
            result.push_back(noQuotes[i]);
        }
    }

    return result;
}

// Scan #1: save commands and labels with their addresses
int assembler::firstPass(std::string &input_filename) {
    std::string line;
    std::ifstream input_file(input_filename);
    bool orig_started = false;
    int last_end_address = 0;
    if (!input_file.is_open()) {
        std::cout << "Unable to open file" << std::endl;
        // @ Input file read error
        throw AssemblyError("Error on line 0: unable to open file");
    }

    int orig_address = -1;
    int current_address = -1;
    int line_number = 0;

    while (std::getline(input_file, line)) {
        line_number += 1;
        line = FormatLine(line);
        if (line.empty()) {
            continue;
        }

        auto command = LineLabelSplit(line, current_address);
        if (command.empty()) {
            continue;
        }

        // OPERATION or PSEUDO?
        auto first_whitespace_position = command.find(' ');
        auto first_token = command.substr(0, first_whitespace_position);

        // Special judge .ORIG and .END
        if (first_token == ".ORIG") {
            if (orig_started) {
                // @ Error: .ORIG appears before .END
                throw AssemblyError("Error on line " + std::to_string(line_number) + ": .ORIG appears before .END");
            }
            std::string orig_value =
                command.substr(first_whitespace_position + 1);
            orig_value = Trim(orig_value);
            orig_address = RecognizeNumberValue(orig_value);
            if (orig_address == std::numeric_limits<int>::max()) {
                // @ Error address
                throw AssemblyError("Error on line " + std::to_string(line_number) + ": invalid address");
            }
            current_address = orig_address;
            orig_started = true;
            if(last_end_address != 0 && orig_address < last_end_address){
                // @ Error address
                throw AssemblyError("Error on line " + std::to_string(line_number) + ": invalid .ORIG address. You should put .ORIG in order");
            }else if(last_end_address != 0 && orig_address >= last_end_address){
                for (int address = last_end_address; address < orig_address; ++address) {
                    commands.push_back({address, std::bitset<16>(0).to_string(), CommandType::ZERO, line_number});
                }
            }
            continue;
        }

        if (!orig_started) {
            // @ Error: Code appears before .ORIG
            throw AssemblyError("Error on line " + std::to_string(line_number) + ": code appears before .ORIG");
        }

        if (first_token == ".END") {
            last_end_address = current_address;
            orig_started = false;
            continue;
        }

        // For LC3 Operation
        if (IsLC3Command(first_token) != -1 ||
            IsLC3TrapRoutine(first_token) != -1) {
            commands.push_back(
                {current_address, command, CommandType::OPERATION, line_number});
            current_address += 1;
            continue;
        }

        // For Pseudo code
        commands.push_back({current_address, command, CommandType::PSEUDO, line_number});
        auto operand = command.substr(first_whitespace_position + 1);
        operand = Trim(operand);
        if (first_token == ".FILL") {
            auto num_temp = RecognizeNumberValue(operand);
            if (num_temp == std::numeric_limits<int>::max()) {
                // @ Error Invalid Number input @ FILL
                throw AssemblyError("Error on line " + std::to_string(line_number) + ": invalid number");
            }
            if (num_temp > 65535 || num_temp < -65536) {
                // @ Error Too large or too small value  @ FILL
                throw AssemblyError("Error on line " + std::to_string(line_number) + ": too large or too small value");
            }
            current_address += 1;
        }
        if (first_token == ".BLKW") {
            // modify current_address
            auto num_temp = RecognizeNumberValue(operand);
            if (num_temp == std::numeric_limits<int>::max()) {
                // @ Error Invalid Number input @ BLKW
                throw AssemblyError("Error on line " + std::to_string(line_number) + ": invalid number");
            }
            if (num_temp > 65535 || num_temp < -65536) {
                // @ Error Too large or too small value  @ BLKW
                throw AssemblyError("Error on line " + std::to_string(line_number) + ": too large or too small value");
            }
            current_address += num_temp;
        }
        if (first_token == ".STRINGZ") {
            // modify current_address
            std::string processed_operand = removeQuotesAndHandleEscapeSequences(operand);
            current_address += processed_operand.length() + 1;
        }
    }

    if (orig_started) {
        // @ Error: .END is missing
        throw AssemblyError("Error on line " + std::to_string(line_number) + ": .END is missing");
    }

    // OK flag
    return 0;
}

std::string assembler::TranslatePseudo(std::stringstream &command_stream) {
    std::string pseudo_opcode;
    std::string output_line;
    command_stream >> pseudo_opcode;
    if (pseudo_opcode == ".FILL") {
        std::string number_str;
        command_stream >> number_str;
        output_line = NumberToAssemble(number_str);
        if (gIsHexMode)
            output_line = ConvertBin2Hex(output_line);
    } else if (pseudo_opcode == ".BLKW") {
        // Fill 0 here
        std::string number_str;
        command_stream >> number_str;
        int number = RecognizeNumberValue(number_str);
        for(int i = 0; i < number - 1; ++i) {
            if(gIsHexMode)
                output_line += ConvertBin2Hex(std::bitset<16>(0).to_string()) + "\n";
            else
                output_line += std::bitset<16>(0).to_string() + "\n";
        }
        //append the last 0 without a "\n" at the end
        if(gIsHexMode)
            output_line += ConvertBin2Hex(std::bitset<16>(0).to_string());
        else
            output_line += std::bitset<16>(0).to_string();
    } else if (pseudo_opcode == ".STRINGZ") {
    // Fill string here
    std::string str;
    std::getline(command_stream, str);  // read the rest of the line
    str = Trim(str);
    str = removeQuotesAndHandleEscapeSequences(str);  // remove the quotes
    for (size_t i = 0; i < str.size(); ++i) {
        std::string bin_str = std::bitset<16>(str[i]).to_string();  // convert each char to binary
        if (gIsHexMode)
            bin_str = ConvertBin2Hex(bin_str);
        output_line += bin_str + "\n";
    }
    // append a null terminator without a "\n" at the end
    if(gIsHexMode)
        output_line += ConvertBin2Hex(std::bitset<16>(0).to_string());
    else
        output_line += std::bitset<16>(0).to_string();
    }
    return output_line;
}

std::string assembler::TranslateCommand(std::stringstream &command_stream,
                                        unsigned int current_address, unsigned int line_number) {
    std::string opcode;
    command_stream >> opcode;
    auto command_tag = IsLC3Command(opcode);

    std::vector<std::string> operand_list;
    std::string operand;
    while (command_stream >> operand) {
        operand_list.push_back(operand);
    }
    auto operand_list_size = operand_list.size();

    std::string output_line;

    if (command_tag == -1) {
        // This is a trap routine
        command_tag = IsLC3TrapRoutine(opcode);
        output_line = kLC3TrapMachineCode[command_tag];
    } else {
        // This is a LC3 command
        switch (command_tag) {
        case 0:
            // "ADD"
            output_line += "0001";
            if (operand_list_size != 3) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number);
            output_line += TranslateOprand(current_address, operand_list[1], line_number);
            if (operand_list[2][0] == 'R') {
                // The third operand is a register
                output_line += "000";
                output_line +=
                    TranslateOprand(current_address, operand_list[2], line_number);
            } else {
                // The third operand is an immediate number
                output_line += "1";
                output_line +=
                    TranslateOprand(current_address, operand_list[2], line_number, 5);
            }
            break;
        case 1:
            // "AND"
            output_line += "0101";
            if (operand_list_size != 3) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number);
            output_line += TranslateOprand(current_address, operand_list[1], line_number);
            if (operand_list[2][0] == 'R') {
                // The third operand is a register
                output_line += "000";
                output_line +=
                    TranslateOprand(current_address, operand_list[2], line_number);
            } else {
                // The third operand is an immediate number
                output_line += "1";
                output_line +=
                    TranslateOprand(current_address, operand_list[2], line_number, 5);
            }
            break;
        case 2:
            // "BR"
            output_line += "0000111";//BRNZP
            if (operand_list_size != 1) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number, 9);
            break;
        case 3:
            // "BRN"
            output_line += "0000100";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number, 9);
            break;
        case 4:
            // "BRZ"
            output_line += "0000010";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number, 9);
            break;
        case 5:
            // "BRP"
            output_line += "0000001";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number, 9);
            break;
        case 6:
            // "BRNZ"
            output_line += "0000110";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number, 9);
            break;
        case 7:
            // "BRNP"
            output_line += "0000101";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number, 9);
            break;
        case 8:
            // "BRZP"
            output_line += "0000011";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number, 9);
            break;
        case 9:
            // "BRNZP"
            output_line += "0000111";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number, 9);
            break;
        case 10:
            // "JMP"
            output_line += "1100000";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number);
            output_line += "000000";
            break;
        case 11:
            // "JSR"
            output_line += "01001";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number, 11);
            break;
        case 12:
            // "JSRR"
            output_line += "0100000";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number);
            output_line += "000000";
            break;
        case 13:
            // "LD"
            output_line += "0010";
            if (operand_list_size != 2) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number);
            output_line += TranslateOprand(current_address, operand_list[1], line_number, 9);
            break;
        case 14:
            // "LDI"
            output_line += "1010";
            if (operand_list_size != 2) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number);
            output_line += TranslateOprand(current_address, operand_list[1], line_number, 9);
            break;
        case 15:
            // "LDR"
            output_line += "0110";
            if (operand_list_size != 3) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number);
            output_line += TranslateOprand(current_address, operand_list[1], line_number);
            output_line += TranslateOprand(current_address, operand_list[2], line_number, 6);   
            break;
        case 16:
            // "LEA"
            output_line += "1110";
            if (operand_list_size != 2) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number);
            output_line += TranslateOprand(current_address, operand_list[1], line_number, 9);
            break;
        case 17:
            // "NOT"
            output_line += "1001";
            if (operand_list_size != 2) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number);
            output_line += TranslateOprand(current_address, operand_list[1], line_number);
            output_line += "111111";
            break;
        case 18:
            // RET
            output_line += "1100000111000000";
            if (operand_list_size != 0) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            break;
        case 19:
            // RTI
            output_line += "1000000000000000";
            if (operand_list_size != 0) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            break;
        case 20:
            // ST
            output_line += "0011";
            if (operand_list_size != 2) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number);
            output_line += TranslateOprand(current_address, operand_list[1], line_number, 9);
            break;
        case 21:
            // STI
            output_line += "1011";
            if (operand_list_size != 2) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number);
            output_line += TranslateOprand(current_address, operand_list[1], line_number, 9);
            break;
        case 22:
            // STR
            output_line += "0111";
            if (operand_list_size != 3) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number);
            output_line += TranslateOprand(current_address, operand_list[1], line_number);
            output_line += TranslateOprand(current_address, operand_list[2], line_number, 6);
            break;
        case 23:
            // TRAP
            output_line += "11110000";
            if (operand_list_size != 1) {
                // @ Error operand numbers
                throw AssemblyError("Error on line " + std::to_string(line_number) 
                                        + ": incorrect number of operands");
            }
            output_line += TranslateOprand(current_address, operand_list[0], line_number, 8);
            break;
        default:
            // Unknown opcode
            // @ Error
            throw AssemblyError("Error on line " + std::to_string(line_number) + ": Unknown opcode");
            break;
        }
    }

    if (gIsHexMode)
        output_line = ConvertBin2Hex(output_line);

    return output_line;
}

int assembler::secondPass(std::string &output_filename) {
    // Scan #2:
    // Translate
    std::ofstream output_file;
    // Create the output file
    output_file.open(output_filename);
    if (!output_file) {
        // @ Error at output file
        return -20;
    }

    for (const auto &command : commands) {
        const unsigned address = std::get<0>(command);
        const std::string command_content = std::get<1>(command);
        const CommandType command_type = std::get<2>(command);
        const unsigned line_number = std::get<3>(command);
        auto command_stream = std::stringstream(command_content);

        if (command_type == CommandType::PSEUDO) {
            // Pseudo
            output_file << TranslatePseudo(command_stream) << std::endl;
        } else if(command_type == CommandType::ZERO) {
            // Zero
            if(gIsHexMode)
                output_file << ConvertBin2Hex(std::bitset<16>(0).to_string()) << std::endl;
            else
                output_file << std::bitset<16>(0).to_string() << std::endl;
        } else{
            // LC3 command
            output_file << TranslateCommand(command_stream, address, line_number) << std::endl;
        }
    }

    // Close the output file
    output_file.close();
    // OK flag
    return 0;
}

// assemble main function
int assembler::assemble(std::string &input_filename, std::string &output_filename) {
    auto first_scan_status = firstPass(input_filename);
    if (first_scan_status != 0) {
        return first_scan_status;
    }
    auto second_scan_status = secondPass(output_filename);
    if (second_scan_status != 0) {
        return second_scan_status;
    }
    // OK flag
    return 0;
}
