#ifndef TIME_FUNCTIONS
#define TIME_FUNCTIONS

#include "google/protobuf/timestamp.pb.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

constexpr std::string_view format {"[%Y-%m-%d %H:%M] "};

std::string convertProtoTime(const google::protobuf::Timestamp& timeStamp)
{
    time_t t = static_cast<time_t>(timeStamp.seconds());
    std::tm tm{};
    localtime_r(&t, &tm);

    std::ostringstream oss;
    oss << std::put_time(&tm, format.data());

    return oss.str();
}

std::string getTimestamp()
{
    time_t t = static_cast<time_t>(std::time(nullptr));
    std::tm tm{};
    localtime_r(&t, &tm);

    std::ostringstream oss;
    oss << std::put_time(&tm, format.data());

    return oss.str();
}

#endif