#include "echo_test.h"
#include <thread>
EchoTest::EchoTest()
{

}

qint64 EchoTest::readData(char *data, qint64 maxlen){
    if(data_.empty()){
        memset(data,0,maxlen);
        return maxlen;
    }
    const auto& echo=data_.front();
    auto len=echo.size();
    memcpy(data,echo.data(),len);
    std::unique_lock<std::mutex> locker(mutex__);
    data_.pop_front();
    return len;
};
qint64 EchoTest::writeData(const char *data, qint64 len){
    /*switch(type_){
        case QPortAudioFormat::SampleType::SignedInt:

        case QPortAudioFormat::SampleType::UnSignedInt:
        case QPortAudioFormat::SampleType::Float:
        break;
    }*/

    std::unique_lock<std::mutex> locker(mutex__);
    data_.emplace_back(data,data+len);
    return len;
}
