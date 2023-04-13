#include "qport_audio_device_info.h"

QPortAudioDeviceInfo::QPortAudioDeviceInfo(const PortAudio::PortAudioDevInfo &info_,Mode mode_):
    info__(info_),
    mode__(mode_),
    deviceName__(nameByMode(info_,mode_))
{

}
QPortAudioDeviceInfo::~QPortAudioDeviceInfo(){

}

QString QPortAudioDeviceInfo::deviceName() const {
    return deviceName__;
}

QPortAudio::Mode QPortAudioDeviceInfo::mode() const {return mode__;}
bool QPortAudioDeviceInfo::isFormatSupported(const QPortAudioFormat &format) const{
    auto maxChannels=mode__==Mode::AudioOutput?info__.maxOutputChannels:info__.maxInputChannels;
    return maxChannels >=format.channelCount();
}

QPortAudioDeviceInfo QPortAudioDeviceInfo::defaultInputDevice(){
    return QPortAudioDeviceInfo(PortAudio::PortAudioDev::getDefaultInputDevice(),Mode::AudioInput);
}
QPortAudioDeviceInfo QPortAudioDeviceInfo::defaultOutputDevice(){
    return QPortAudioDeviceInfo(PortAudio::PortAudioDev::getDefaultOutputDevice(),Mode::AudioOutput);
}

QList<QPortAudioDeviceInfo> QPortAudioDeviceInfo::availableDevices(QPortAudio::Mode mode_){
    auto devs=PortAudio::PortAudioDev::getDevicesList();
    QList<QPortAudioDeviceInfo> result;
    for(auto const&dev:devs){
        if(mode_==Mode::AudioInput && !dev.maxInputChannels)
            continue;
        if(mode_==Mode::AudioOutput && !dev.maxOutputChannels)
            continue;
        result.append(QPortAudioDeviceInfo(dev,mode_));
    }
    return result;
}

QString QPortAudioDeviceInfo::nameByMode(const PortAudio::PortAudioDevInfo &dev_,QPortAudio::Mode mode_){
    return QString(mode_==Mode::AudioInput?"I ":"O ")+dev_.deviceName().c_str();
}
