#ifndef QPORTAUDIODEVICEINFO_H
#define QPORTAUDIODEVICEINFO_H

#include <QString>
#include <QList>
#include "port_audio_dev.h"
#include "qport_audio.h"
#include "qport_audio_format.h"

class QPortAudioDeviceInfo{
    using Mode=QPortAudio::Mode;
    friend class QPortAudioOutput;
    friend class QPortAudioInput;
public:
    QPortAudioDeviceInfo()=default;
    QPortAudioDeviceInfo(const QPortAudioDeviceInfo&)=default;
    QPortAudioDeviceInfo(const PortAudio::PortAudioDevInfo &info_,Mode mode_);
    ~QPortAudioDeviceInfo();

    QString deviceName() const;
    QPortAudio::Mode mode() const;
    bool isFormatSupported(const QPortAudioFormat &format) const;

    static QPortAudioDeviceInfo defaultInputDevice();
    static QPortAudioDeviceInfo defaultOutputDevice();

    static QList<QPortAudioDeviceInfo> availableDevices(QPortAudio::Mode mode_);
private :
    static QString nameByMode(const PortAudio::PortAudioDevInfo &dev_,QPortAudio::Mode mode_);

    PortAudio::PortAudioDevInfo info__;
    Mode mode__=Mode::AudioOutput;
    QString deviceName__="";
};

#endif // QPORTAUDIODEVICEINFO_H
