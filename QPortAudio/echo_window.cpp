#include "echo_window.h"
#include "ui_echo_window.h"
#include <QVariant>
#include "qport_audio_device_info.h"
Q_DECLARE_METATYPE(QPortAudioDeviceInfo)
template<>
inline QVariant QVariant::fromValue(const QPortAudioDeviceInfo &s)
{
    QVariant variant;
    variant.setValue(s);
    return variant;
}



EchoWindow::EchoWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EchoWindow)
{
    ui->setupUi(this);
    echo_.open(QIODevice::ReadWrite);
    auto defaultIn=QPortAudioDeviceInfo::defaultInputDevice();
    auto defaultOut=QPortAudioDeviceInfo::defaultOutputDevice();
    for(auto &dev :QPortAudioDeviceInfo::availableDevices(QPortAudio::Mode::AudioInput)){
        ui->devIn->addItem(dev.deviceName(),QVariant::fromValue(dev));
        if(dev.deviceName()==defaultIn.deviceName())
            ui->devIn->setCurrentText(dev.deviceName());
    }
    for(auto &dev :QPortAudioDeviceInfo::availableDevices(QPortAudio::Mode::AudioOutput)){
        ui->devOut->addItem(dev.deviceName(),QVariant::fromValue(dev));
        if(dev.deviceName()==defaultOut.deviceName())
            ui->devOut->setCurrentText(dev.deviceName());
    }
    ui->size->setCurrentText("16bit");
    ui->chOut->setVisible(false);
    ui->chOutLbl->setVisible(false);
    ui->chInLbl->setText("Channels");
}

EchoWindow::~EchoWindow()
{
    delete ui;
}

void EchoWindow::on_onOffBtn_clicked()
{
    isOn_=!isOn_;
    if(isOn_){
        auto devIn=ui->devIn->itemData(ui->devIn->currentIndex()).value<QPortAudioDeviceInfo>();
        auto chIn=ui->chIn->value();
        auto devOut=ui->devOut->itemData(ui->devOut->currentIndex()).value<QPortAudioDeviceInfo>();
        //auto chOut=ui->chOut->value();
        double rate=8000;
        switch(ui->rate->currentIndex()){
            case 0: rate=8000; break;
            case 1: rate=11025; break;
            case 2: rate=16000; break;
            case 3: rate=22050; break;
            case 4: rate=32000; break;
            case 5: rate=44100; break;
            case 6: rate=48000; break;
            case 7: rate=96000; break;
            case 8: rate=192000; break;
            case 9: rate=2822400; break;
            case 10: rate=5644800; break;
        }
        auto size=QPortAudioFormat::SampleSize::ss16bit;
        switch(ui->size->currentIndex()){
        case 0: size=QPortAudioFormat::SampleSize::ss8bit;break;
        case 1: size=QPortAudioFormat::SampleSize::ss16bit;break;
        case 2: size=QPortAudioFormat::SampleSize::ss32bit;break;
        }
        auto type=QPortAudioFormat::SampleType::SignedInt;
        switch(ui->type->currentIndex()){
        case 0:type=QPortAudioFormat::SampleType::SignedInt; break;
        case 1:type=QPortAudioFormat::SampleType::UnSignedInt; break;
        case 2:type=QPortAudioFormat::SampleType::Float; break;
        }
        QPortAudioFormat fmt;
        fmt.setSampleRate(rate);
        fmt.setSampleSize(size);
        fmt.setSampleType(type);
        fmt.setChannelCount(chIn);
        input_.reset(new QPortAudioInput(devIn,fmt));
        //fmt.setChannelCount(chOut);
        output_.reset(new QPortAudioOutput(devOut,fmt));
        input_->start(&echo_);
        output_->start(&echo_);
    } else{
        input_.reset();
        output_.reset();
    }

    updateView();
}

void EchoWindow::updateView(){
    ui->chIn->setEnabled(!isOn_);
    ui->chOut->setEnabled(!isOn_);
    ui->type->setEnabled(!isOn_);
    ui->size->setEnabled(!isOn_);
    ui->devIn->setEnabled(!isOn_);
    ui->devOut->setEnabled(!isOn_);
    ui->rate->setEnabled(!isOn_);
    ui->onOffBtn->setText(isOn_?"Stop echo":"Start echo");
}

