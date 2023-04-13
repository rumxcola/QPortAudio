#include "port_audio_stream_settings.h"
namespace PortAudio {

std::ostream& operator<<(std::ostream& os, const PortAudioStreamSettings::SampleFormat& fmt_){
    return os<<PortAudioStreamSettings::sfToString(fmt_);
}
std::ostream& operator<<(std::ostream& os, const PortAudioStreamSettings& settings_){
    return os<<settings_.toString();
}

PortAudioStreamSettings::PortAudioStreamSettings(SampleFormat sf_,size_t bytesInFrame_):
    sf(sf_),
    bytesInFrame(bytesInFrame_){

    using SF=SampleFormat;
    switch(sf_){
    case SF::SFFloat32:
    case SF::SFInt32:
        bytesInFrame=sizeof(int32_t);
        break;
    case SF::SFInt24:
        bytesInFrame= 3;
        break;
    case SF::SFInt16:
        bytesInFrame= sizeof(int16_t);
        break;
    case SF::SFInt8:
    case SF::SFUint8:
        bytesInFrame= sizeof(uint8_t);
        break;
    default:
        ;
    }
}

PortAudioStreamSettings::PortAudioStreamSettings():
    PortAudioStreamSettings(SampleFormat::SFInt16){
}

std::string PortAudioStreamSettings::sfToString(PortAudioStreamSettings::SampleFormat fmt_){
    using SF=PortAudioStreamSettings::SampleFormat;
    switch(fmt_){
    case SF::SFFloat32:         return "SFFloat32";
    case SF::SFInt32:           return "SFInt32";
    case SF::SFInt24:           return "SFInt24";
    case SF::SFInt16:           return "SFInt16";
    case SF::SFInt8:            return "SFInt8";
    case SF::SFUint8:           return "SFUint8";
    case SF::SFCustomFormat:    return "SFCustomFormat";
    }
}
bool PortAudioStreamSettings::operator==(const PortAudioStreamSettings& other) const {
    return &other==this ||
            other.bytesInFrame==bytesInFrame &&
            other.framesPerBuffer==framesPerBuffer&&
            other.sampleRate==sampleRate &&
            other.sf==sf;
            //0==memcmp(this,&other,sizeof(PortAudioStreamSettings));
}
bool PortAudioStreamSettings::operator!=(const PortAudioStreamSettings& other) const {
    return !(other==*this);
}

std::string PortAudioStreamSettings::toString() const{
    return "{"+sfToString(sf)+"("+std::to_string(bytesInFrame)+"),sr:"
            +std::to_string(sampleRate)
            +","+std::to_string(framesPerBuffer)+ "frames per buffer";
}

}
