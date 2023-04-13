#ifndef ECHOTEST_H
#define ECHOTEST_H

#include <QIODevice>
#include "qport_audio_input.h"
#include "qport_audio_output.h"
class EchoTest : public QIODevice
{
    Q_OBJECT
public:
    EchoTest();

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

    //int channelsIn_,channelsOut_;
    //QPortAudioFormat::SampleSize size_;
    //QPortAudioFormat::SampleType type_;
    std::list<std::vector<char>> data_;
    std::mutex mutex__;
};

#endif // ECHOTEST_H
