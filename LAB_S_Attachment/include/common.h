/*
 * @Author       : Chivier Humber
 * @Date         : 2021-09-14 21:44:05
 * @LastEditors  : Chivier Humber
 * @LastEditTime : 2021-10-31 15:59:54
 * @Description  : file content
 */
#pragma once

#include <iostream>
#include <fstream>
#include <cstdio>

#include <array>
#include <vector>
#include <cmath>
#include <climits>
#include <cstdlib>
#include <string>
#include <cstring>
#include <algorithm>
#include <bitset>
#include <iomanip>

// Boost library
#include <boost/program_options.hpp>

// Application global variables
extern bool gIsSingleStepMode;
extern bool gIsDetailedMode;
extern std::string gInputFileName;
extern std::string gRegisterStatusFileName;
extern std::string gOutputFileName;
extern int gBeginningAddress;

//定义错误类型
class MyError : public std::runtime_error {
public:
    MyError(const std::string& message)
            : std::runtime_error(message) {}
};