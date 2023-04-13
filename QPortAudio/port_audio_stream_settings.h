#ifndef PORTAUDIOSTREAMSETTINGS_H
#define PORTAUDIOSTREAMSETTINGS_H
#include <portaudio.h>
#include <functional>
#include <string.h>
#include <string>
#include "port_audio_dev_info.h"
namespace PortAudio {

struct PortAudioStreamSettings{
    enum class SampleFormat{ SFFloat32=paFloat32,SFInt32=paInt32, SFInt24=paInt24,
                             SFInt16=paInt16, SFInt8=paInt8,SFUint8=paUInt8, SFCustomFormat=paCustomFormat};

    static const unsigned DEFAULT_BYTES_IN_FRAME=4;
    static const unsigned DEFAULT_SAMPLE_RATE=8000;
    static const unsigned DEFAULT_PERIOD_POLLING_IN_MS=20;
    static const unsigned DEFAULT_FRAMES_PER_BUFFER= DEFAULT_SAMPLE_RATE*DEFAULT_PERIOD_POLLING_IN_MS/1000;
    static const SampleFormat DEFAULT_SAMPLE_FORMAT=SampleFormat::SFInt16;

    SampleFormat sf=DEFAULT_SAMPLE_FORMAT;
    double sampleRate=DEFAULT_SAMPLE_RATE;
    unsigned framesPerBuffer=DEFAULT_FRAMES_PER_BUFFER;
    unsigned bytesInFrame=DEFAULT_BYTES_IN_FRAME;

    static std::string sfToString(SampleFormat f);

    PortAudioStreamSettings(SampleFormat sf_,size_t bytesInFrame_=4);
    PortAudioStreamSettings();
    std::string toString() const;
    bool operator==(const PortAudioStreamSettings& other) const ;
    bool operator!=(const PortAudioStreamSettings& other) const ;
};
std::ostream& operator<<(std::ostream& os, const PortAudioStreamSettings::SampleFormat& fmt_);
std::ostream& operator<<(std::ostream& os, const PortAudioStreamSettings& dev_);

/*struct PortAudioStreamFormat{
    using Direction=PortAudioDevInfo::Direction;
    using Writer=std::function<int64_t(char * , int64_t)>;
    using Reader=std::function<int64_t(const char *, int64_t )>;

    Direction dir;
    Writer writer;
    Reader reader;
    int channelsInput;
    int channelsOutput;
    PortAudioStreamSettings settings;
};*/
}


#endif // PORTAUDIOSTREAMSETTINGS_H
