#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int SS = seconds % 60;
    int MM = (seconds / 60) % 60;
    int HH = seconds / 60 / 60;
    string result = std::to_string(HH) + ":" + std::to_string(MM) + ":" + std::to_string(SS);
    return result; 
    
}