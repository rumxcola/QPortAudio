#include "port_audio_dev_info.h"

std::ostream& operator<<(std::ostream& os, const PortAudio::PortAudioDevInfo& dev){
    return os<<dev.toString();
}

namespace PortAudio{

PortAudioDevInfo::PortAudioDevInfo(int num_, const   PaDeviceInfo *deviceInfo):
    devNum(num_),
    structVersion(deviceInfo->structVersion),
    name(deviceInfo->name),
    hostApi(deviceInfo->hostApi),
    maxInputChannels(deviceInfo->maxInputChannels),
    maxOutputChannels(deviceInfo->maxOutputChannels),
    defaultLowInputLatency(deviceInfo->defaultLowInputLatency),
    defaultLowOutputLatency(deviceInfo->defaultLowOutputLatency),
    defaultHighInputLatency(deviceInfo->defaultHighInputLatency),
    defaultHighOutputLatency(deviceInfo->defaultHighOutputLatency),
    defaultSampleRate(deviceInfo->defaultSampleRate){

}
std::string PortAudioDevInfo::toString()const {
    return
            "{idx="+std::to_string(devNum)
            //+" sVer="+std::to_string(dev.structVersion)
            //+" hApi="+std::to_string(dev.hostApi)
            +" SR=" +std::to_string(defaultSampleRate)
            +" maxI/O="+std::to_string(maxInputChannels)+"/"+std::to_string(maxOutputChannels)
            +"\tdefLatency LI/LO/HI/HO="
            +std::to_string(defaultLowInputLatency)+"/"+std::to_string(defaultLowOutputLatency)
            +std::to_string(defaultHighInputLatency)+"/"+std::to_string(defaultHighOutputLatency)
            +"\t"+name+"}";
}
std::string PortAudioDevInfo::toShortString() const{
    return "{idx="+std::to_string(devNum)+" "+name+"}";
}

bool PortAudioDevInfo::operator==(const PortAudioDevInfo& other_){
    return
            other_.devNum==devNum
            &&other_.structVersion==structVersion
            &&other_.name==name
            &&other_.hostApi==hostApi
            &&other_.maxInputChannels==maxInputChannels
            &&other_.maxOutputChannels==maxOutputChannels
            &&other_.defaultLowInputLatency==defaultLowInputLatency
            &&other_.defaultLowOutputLatency==defaultLowOutputLatency
            &&other_.defaultHighInputLatency==defaultHighInputLatency
            &&other_.defaultHighOutputLatency==defaultHighOutputLatency
            &&other_.defaultSampleRate==defaultSampleRate;

}
bool PortAudioDevInfo::isDirectionSupported(bool isAudioInput_) const {
    return isAudioInput_? maxInputChannels:maxOutputChannels;
}
}

