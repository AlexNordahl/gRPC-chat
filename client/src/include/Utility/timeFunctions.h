#ifndef TIME_FUNCTIONS
#define TIME_FUNCTIONS

#include "google/protobuf/timestamp.pb.h"
#include <string>

std::string convertProtoTime(const google::protobuf::Timestamp& timeStamp);
std::string getTimestamp();

#endif