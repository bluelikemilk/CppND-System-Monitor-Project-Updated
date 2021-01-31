#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    float percentage = 100.0 * (float(LinuxParser::ActiveJiffies()) / float(LinuxParser::Jiffies() ));
    return percentage;

 }