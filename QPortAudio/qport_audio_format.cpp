#include "qport_audio_format.h"

QPortAudioFormat& QPortAudioFormat::operator=(const QPortAudioFormat &other_){
    sampleType__=other_.sampleType__;
    sampleSize__=other_.sampleSize__;
    sampleRate__=other_.sampleRate__;
    channelsCount__=other_.channelsCount__;
    return *this;
}
bool QPortAudioFormat::operator==(const QPortAudioFormat &other_) const{
    return sampleType__==other_.sampleType__ &&
           sampleSize__==other_.sampleSize__ &&
           sampleRate__==other_.sampleRate__ &&
           channelsCount__==other_.channelsCount__;
}
bool QPortAudioFormat::operator!=(const QPortAudioFormat &other_) const {
    return !(other_==*this);
}
