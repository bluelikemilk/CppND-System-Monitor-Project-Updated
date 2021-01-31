#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;
using std::ifstream;
using std::istringstream;
using std::istream_iterator;
using namespace LinuxParser;


// TODO: Return this process's ID
int Process::Pid() { return this->pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {  // variables for file stream 
    string line; 
    // the result Cpu percentage
    float result;

    // filename proc/[PID]/stat
    ifstream stream(kProcDirectory + to_string(this->pid_) + kStatFilename);
    // file proc/[PID]/stat is just a long line with fixed format
    getline(stream, line);
    // convert line into vector
    istringstream buf(line);
    // get interator of buf
    istream_iterator<string> beg(buf), end;
    // convert to a vector
    vector<string> values(beg, end);

    // acquring relevant times to calculate active occupation of CPU
    long utime = stol(values[13]);
    long stime = stol(values[14]);
    long cutime = stol(values[15]);
    long cstime = stol(values[16]);
    long starttime = LinuxParser::UpTime(this->pid_);
    long uptime = LinuxParser::UpTime();
    float freq = sysconf(_SC_CLK_TCK);
    float total_time = float(utime + stime + cutime + cstime);
    float seconds = float(uptime)/freq - float(starttime)/freq;
    result = 100.0 * ((total_time/freq)/seconds);

    return result;
}

// TODO: Return the command that generated this process
string Process::Command() { 
    return LinuxParser::Command(this -> pid_);
}

// TODO: Return this process's memory utilization
string Process::Ram() { 
    return LinuxParser::Ram(this -> pid_); 
}

// TODO: Return the user (name) that generated this process
string Process::User() { 
    return LinuxParser::User(this->pid_);
 }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
    long startTimeJiff = LinuxParser::UpTime(this->pid_);
    // convert clock ticks to seconds
    long uptime = startTimeJiff / sysconf(_SC_CLK_TCK);
    return uptime;
 }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    if(this->pid_ < a.pid_) return true;
    return false; 
}