#include <iostream>
#include <QApplication>
#include "echo_window.h"
#include "echo_test.h"
#include <thread>
using namespace std;



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EchoWindow w;
    w.show();
    return a.exec();
    /*
    QPortAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(2);
    format.setSampleSize(QPortAudioFormat::SampleSize::ss16bit);
    format.setSampleType(QPortAudioFormat::SampleType::SignedInt);

    EchoTest echo;
    echo.open(QIODevice::ReadWrite);
    auto devInput=QPortAudioDeviceInfo::defaultInputDevice();
    auto devOutput=QPortAudioDeviceInfo::defaultOutputDevice();
    QPortAudioInput input(devInput,format);
    QPortAudioOutput output(devOutput,format);
    input.start(&echo);
    output.start(&echo);
    std::this_thread::sleep_for(std::chrono::seconds(30));
    return 0;*/
}
