#ifndef QPORTAUDIOFORMAT_H
#define QPORTAUDIOFORMAT_H
#include "port_audio_stream_settings.h"

class QPortAudioFormat{
public:
    enum class SampleType { SignedInt, UnSignedInt, Float };
    enum class SampleSize {ss8bit=8,ss16bit=16, ss32bit=32};
    void setSampleRate(int sampleRate_){sampleRate__=sampleRate_;}
    int sampleRate() const{return sampleRate__;}

    void setChannelCount(int channelCount_){channelsCount__=channelCount_;}
    int channelCount() const{return channelsCount__;}

    void setSampleSize(SampleSize sampleSize_){sampleSize__=sampleSize_;}
    SampleSize sampleSize() const {return sampleSize__;}

    void setSampleType(SampleType sampleType_){sampleType__=sampleType_;}
    SampleType sampleType() const {return sampleType__;}

    QPortAudioFormat& operator=(const QPortAudioFormat &other);
    QPortAudioFormat(const QPortAudioFormat &other)=default;
    QPortAudioFormat()=default;
    bool operator==(const QPortAudioFormat &other) const;
    bool operator!=(const QPortAudioFormat &other) const;
    PortAudio::PortAudioStreamSettings toStreamSettings(int bufSize_=-1) const {
        using SF=PortAudio::PortAudioStreamSettings::SampleFormat;

        PortAudio::PortAudioStreamSettings settings;
        switch (sampleType__){
        case SampleType::SignedInt:
            switch (sampleSize__){
            case SampleSize::ss8bit:
                settings.sf=SF::SFInt8;
                settings.bytesInFrame=1;
                break;
            case SampleSize::ss16bit:
                settings.sf=SF::SFInt16;
                settings.bytesInFrame=2;
                break;
            case SampleSize::ss32bit:
                settings.sf=SF::SFInt32;
                settings.bytesInFrame=4;
                break;
            }
            break;
        case SampleType::UnSignedInt:
            settings.sf=SF::SFUint8;
            settings.bytesInFrame=1;
            break;
        case SampleType::Float:
            settings.sf=SF::SFFloat32;
            settings.bytesInFrame=4;
            break;
        }
        settings.sampleRate=sampleRate__;
        if(bufSize_>=static_cast<int>(channelCount()*settings.bytesInFrame))
            settings.framesPerBuffer=bufSize_/( channelCount()*settings.bytesInFrame);
        else
            settings.framesPerBuffer=sampleRate__*DEFAULT_PERIOD_POLLING_IN_MS/1000;
        return settings;
    }
private:
    static const int DEFAULT_PERIOD_POLLING_IN_MS=20;
    SampleType sampleType__=SampleType::SignedInt;
    SampleSize sampleSize__=SampleSize::ss16bit;
    int sampleRate__=0;
    int channelsCount__=1;
};



#endif // QPORTAUDIOFORMAT_H
