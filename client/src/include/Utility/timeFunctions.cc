#include "timeFunctions.h"
#include "google/protobuf/timestamp.pb.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

std::string convertProtobufTime(const google::protobuf::Timestamp& timeStamp, const std::string& format)
{
    time_t t = static_cast<time_t>(timeStamp.seconds());
    std::tm tm{};
    localtime_r(&t, &tm);

    std::ostringstream oss;
    oss << std::put_time(&tm, format.c_str());

    return oss.str();
}

std::string getTimestampFormatted(const std::string &format)
{
    time_t t = static_cast<time_t>(std::time(nullptr));
    std::tm tm{};
    localtime_r(&t, &tm);

    std::ostringstream oss;
    oss << std::put_time(&tm, format.c_str());

    return oss.str();
}