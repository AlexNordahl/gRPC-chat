#ifndef TIME_FUNCTIONS
#define TIME_FUNCTIONS

#include "google/protobuf/timestamp.pb.h"
#include <string>

std::string convertProtobufTime(const google::protobuf::Timestamp& timeStamp, const std::string& format);
std::string getTimestampFormatted(const std::string &format);

#endif