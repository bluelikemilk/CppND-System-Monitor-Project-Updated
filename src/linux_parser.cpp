#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;


// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string name = "PRETTY_NAME=";

  std::ifstream stream(kOSPath);

  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      // PRETTY_NAME="Ubuntu 18.04.5 LTS"
      // find the first "="
      std::size_t found = line.find("=");
      // skip one char after "="
      found++;
      // extract the string after " => Ubuntu 18.04.5 LTS"
      string result = line.substr(found);
      // remove the last char " => Ubuntu 18.04.5 LTS
      result.erase(std::remove(result.begin(), result.end(), '"'),
                   result.end());
      return result;
    }
  }
  return "";
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids(){
    vector<int> pids;
    DIR* directory;
    // if path /proc is invalid, throw an error
    if(!(directory = opendir(kProcDirectory.c_str())))
        throw std::runtime_error(strerror(errno));
    
    struct dirent* file;
    // read each file & folder inside dir
    while((file = readdir(directory)) != nullptr){
        // check if the current path is a directory
        if(file->d_type != DT_DIR) continue;
        // check if every character of the name is a digit
        string filename(file->d_name);
				if (all_of(filename.begin(), filename.end(), isdigit)){
						int pid = stoi(filename);
		        pids.push_back(pid);
        }
    }
    // validating process of directory closing
    if(closedir(directory)) // return 0 if successfully close the file, return -1 if fail
        throw std::runtime_error(strerror(errno));
    return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  // read /proc/meminfo and find the line with MemTotal:
  string name = "MemTotal:";
  std::ifstream stream(kProcDirectory.substr(0, kProcDirectory.size()-1) + kMeminfoFilename);
  string line;
  float result = 0.0;
  // search for the line starting with processes
    while(getline(stream, line)){
      if(line.compare(0, name.size(), name) == 0){
        string title, value;
        std::istringstream linestream(line);
        linestream >> title >> value;
        // convert value from kB to MB
        result = float(stoi(value)) / float (1024.0);
        break;
      }
    }
   return result;  
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  // read /proc/uptime and the file contains two numbers (values in seconds)
  // he uptime of the system (including time spent in suspend) and the amount of time spent in the idle process. 
   std::ifstream stream(kProcDirectory.substr(0, kProcDirectory.size()-1) + kUptimeFilename);
   string line;
   getline(stream, line);
   std::istringstream linestream(line);
   string uptime;
   linestream >> uptime;
   return stol(uptime);
  }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  // read /proc/stat and the first line (cpu) is amount of time (in Jiffies) 
  // that the system has spent

  return (ActiveJiffies() + IdleJiffies()); 

}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  // read /proc/[PID]/stat
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string line;
  getline(stream, line);
  // convert line to a string vector
  std::istringstream linstream(line);
  std::istream_iterator<string> beg(linstream), end;
  vector<string> values(beg, end);

    long utime = stol(values[13]);
    long stime = stol(values[14]);
    long cutime = stol(values[15]);
    long cstime = stol(values[16]);
    // long starttime = UpTime(pid);
    // long uptime = UpTime();

    long total_time = utime + stime + cutime + cstime;
    return total_time;
  }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  // load CPU status using CpuUtilization()
  vector<string> values = CpuUtilization();
  // sum up all active cpu time in Jiffies
  /*      user    nice   system  idle      iowait irq   softirq  steal  guest  guest_nice
   cpu  74608   2520   24433   1117073   6176   4054  0        0      0      0
  */
  return (stol(values[kUser_]) +
            stol(values[kNice_]) +
            stol(values[kSystem_]) +
            stol(values[kIRQ_]) +
            stol(values[kSoftIRQ_]) +
            stol(values[kSteal_]) +
            stol(values[kGuest_]) +
            stol(values[kGuestNice_]));
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  // return 10;
    // load CPU status using CpuUtilization()
  vector<string> values = CpuUtilization();
  // sum up all active cpu time in Jiffies
  /*      user    nice   system  idle      iowait irq   softirq  steal  guest  guest_nice
   cpu  74608   2520   24433   1117073   6176   4054  0        0      0      0
  */
  return (stol(values[kIdle_]) + stol(values[kIOwait_]));
  
  }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
// read the file in /proc/stat and return the first line as a vector of string, 
// the information is as below
/*      user    nice   system  idle      iowait irq   softirq  steal  guest  guest_nice
   cpu  74608   2520   24433   1117073   6176   4054  0        0      0      0
*/
    string line;
    string name = "cpu";
    std::ifstream stream(kProcDirectory.substr(0, kProcDirectory.size()-1) + kStatFilename);
    while (getline(stream, line)) {
      if (line.compare(0, name.size(), name) == 0) {
        std::istringstream buf(line);
        std::istream_iterator<string> beg(buf), end;
        // skip the first word cpu
        vector<string> values(++beg, end);
        // return all usage data of cpu in a vector of string
        return values;
      }
    }
    return (vector<string>());
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
   // read /proc/meminfo file and find the line starting with processes
   string name = "processes";
   std::ifstream stream(kProcDirectory.substr(0, kProcDirectory.size()-1) + kStatFilename);
   string line;
    // search for the line starting with processes
    while(getline(stream, line)){
      if(line.compare(0, name.size(), name) == 0){
        string title, value;
        std::istringstream linestream(line);
        linestream >> title >> value;
        // return the Ram usage in kB
        return stoi(value);
      }
    }
   return 0;  
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  // read /proc/stat and find the line starting with procs_running
  string name = "procs_running";
  std::ifstream stream(kProcDirectory.substr(0, kProcDirectory.size()-1) + kStatFilename);
   string line;
    // search for the line starting with processes
    while(getline(stream, line)){
      if(line.compare(0, name.size(), name) == 0){
        string title, value;
        std::istringstream linestream(line);
        linestream >> title >> value;
        // return the Ram usage in kB
        return stoi(value);
      }
    }
   return 0;   
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  // read /proc/[PID]/cmdline
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string line;
  getline(stream, line);
  return line; 

}

// TODO: Read and return the memory used by a process in kB
string LinuxParser::Ram(int pid) {
  // read /proc/[PID]/status and find the line starting with VmData
    string name = "VmData";
    std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
    string line;

    // search for the line starting with VmData
    while(getline(stream, line)){
      if(line.compare(0, name.size(), name) == 0){
        string title, value;
        std::istringstream linestream(line);
        linestream >> title >> value;
        // convert the Ram usage in MB
        float ram = float(stol(value)) / float(1024.0);
        return to_string(ram);
      }
    }
  // if no VmData is found, return empty
   return string(); 
   }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  // read /proc/[PID]/status and find the line starting with Uid:
  string name = "Uid:";
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string line;
  while(getline(stream, line)){
    if(line.compare(0, name.size(), name) == 0){
      std::istringstream linestream(line);
      string title, value;
      linestream >> title >> value;
      return value;
    }
  }
  return string(); 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  // call Uid() to find the uid string of current pid
  string uid = Uid(pid);
  // /etc/passwd contains the information necessary to match the UID to a username.
  // with the format <username>:x:<uid>...
  string name = "x:" + uid;
  std::ifstream stream(kPasswordPath);
  string line;
  while(getline(stream, line)){
    if(line.find(name)!=string::npos){
      // extract the username which is the word before the first comma
      string username = line.substr(0, line.find(":"));
      return username;
    }
  }
  return string(); 
}

// TODO: Read and return the uptime (in Jiffies)of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  // read /proc/pid/stat, this is a long line and the 22nd word is uptime expressed in jiffies
  // uptime = startJiff / sysconf(_SC_CLK_TCK)
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string line;
  getline(stream, line);
  // convert line to a string vector
  std::istringstream linstream(line);
  std::istream_iterator<string> beg(linstream), end;
  vector<string> values(beg, end);
  long startTimeJiff = stol(values[21]);
  return startTimeJiff; 
  
}