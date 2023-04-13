#include "qport_audio_input.h"
Q_DECLARE_METATYPE(QPortAudio::State)

QPortAudioInput::QPortAudioInput(const QPortAudioDeviceInfo &audioDeviceInfo, const QPortAudioFormat &format_, QObject *parent_):
    QObject(parent_),
    fmt__(format_),
    devInfo__(audioDeviceInfo),
    dev__(devInfo__.info__)
{
    qRegisterMetaType<QPortAudio::State>();
    dev__.addStateListener(this);
}

QPortAudioInput::~QPortAudioInput(){
    dev__.removeStateListener(this);
    stop();
}

void QPortAudioInput::onStateChanged(PortAudio::PortAudioDevInfo::State state_){
    auto newState=state__;
    using PAState=decltype(state_);
    switch(state_){
    case PAState::IdleState:
        newState=QPortAudio::State::IdleState;
        break;
    case PAState::InterruptedState:
        newState=QPortAudio::State::InterruptedState;
        break;
    case PAState::StoppedState:
        if(state__!=QPortAudio::State::IdleState)
            newState=QPortAudio::State::StoppedState;
        break;
    case PAState::SuspendedState:
        newState=QPortAudio::State::SuspendedState;
        break;
    case PAState::ActiveBoth:
    case PAState::ActiveInput:
        if(state__!=QPortAudio::State::IdleState)
            newState=QPortAudio::State::ActiveState;
    case PAState::ActiveOutput:
        break;
    }
    setState(newState);
}
void QPortAudioInput::volumize(char *buf_,int64_t len_){
    int64_t i=0;
    switch (fmt__.sampleSize()){
    case Format::SampleSize::ss8bit:{
        switch(fmt__.sampleType()){
        case Format::SampleType::SignedInt:
            for(i=0;i<len_;++i)
                buf_[i]*=volume__;
            break;
        case Format::SampleType::UnSignedInt:
            for(auto*b=reinterpret_cast<uint8_t*>(buf_);i<len_;++i)
                b[i]*=volume__;
            break;
        default:break;
        }
    }
    case Format::SampleSize::ss16bit:{
        switch(fmt__.sampleType()){
        case Format::SampleType::SignedInt:
            for(auto*b=reinterpret_cast<int16_t*>(buf_);i<len_/sizeof(int16_t);++i)
                b[i]*=volume__;
            break;
        case Format::SampleType::UnSignedInt:
            for(auto*b=reinterpret_cast<uint16_t*>(buf_);i<len_/sizeof(uint16_t);++i)
                b[i]*=volume__;
            break;
        default:break;
        }
    }
    case Format::SampleSize::ss32bit:{
        switch(fmt__.sampleType()){
        case Format::SampleType::SignedInt:
            for(auto*b=reinterpret_cast<int32_t*>(buf_);i<len_/sizeof(int32_t);++i)
                b[i]*=volume__;
            break;
        case Format::SampleType::UnSignedInt:
            for(auto*b=reinterpret_cast<uint32_t*>(buf_);i<len_/sizeof(uint32_t);++i)
                b[i]*=volume__;
            break;
        case Format::SampleType::Float:
            for(auto*b=reinterpret_cast<float*>(buf_);i<len_/sizeof(float);++i)
                b[i]*=volume__;
            break;
        default:
            break;
        }
    }
    default:
        break;
    }

}

void QPortAudioInput::setState(QPortAudio::State s){
    if(s==state__)
        return;
    state__=s;
    Q_EMIT stateChanged(state__);
}

int64_t QPortAudioInput::write(const char * buf_, int64_t len_){
    if(state__!=QPortAudio::State::ActiveState){
        return len_;
    }
    std::vector<char> volumized(buf_,buf_+len_);
    volumize(volumized.data(),len_);
    auto res=device__->write(volumized.data(),len_);
    if(res<len_)
        setState(QPortAudio::State::IdleState);

    ++pollings__;
    millisSinceLastNotification__+=pollings__*1000*settings__.framesPerBuffer/settings__.sampleRate;
    if(millisSinceLastNotification__>=notifyIntervalInMillis__){
        millisSinceLastNotification__=0;
        Q_EMIT notify();
    }
    return res;
}

QPortAudioFormat QPortAudioInput::format() const {
    return fmt__;
}

void QPortAudioInput::start(QIODevice *device){

    state__=QPortAudio::State::InterruptedState;
    error__=QPortAudio::Error::NoError;
    dev__.stopInput();
    device__=device;
    settings__=fmt__.toStreamSettings(bufSize__);
    pollings__=0;
    started__=std::chrono::steady_clock::now();
    if(devInfo__.mode()!=QPortAudio::Mode::AudioInput
            ||!dev__.init(r__,fmt__.channelCount(),settings__)){
        error__=QPortAudio::Error::OpenError;
        setState(QPortAudio::State::StoppedState);
        return;
    }
}

QIODevice* QPortAudioInput::start(){
    start(device__);
    return device__;
};

void QPortAudioInput::stop(){
    state__=QPortAudio::State::StoppedState;
    dev__.stopInput();
};

void QPortAudioInput::reset(){
    stop();
    start();
};

void QPortAudioInput::suspend(){
    prevState__=state__;
    state__=QPortAudio::State::SuspendedState;
}

void QPortAudioInput::resume(){
    state__=prevState__;
}

void QPortAudioInput::setBufferSize(int bytes){
    bufSize__=bytes;
};

int QPortAudioInput::bufferSize() const{
    return settings__.bytesInFrame*settings__.framesPerBuffer*fmt__.channelCount();
};

int QPortAudioInput::bytesFree() const {
    return 0;
}

int QPortAudioInput::periodSize() const{
    return 1000*settings__.framesPerBuffer/settings__.sampleRate;
}

void QPortAudioInput::setNotifyInterval(int milliSeconds){
    notifyIntervalInMillis__=milliSeconds;
}

int QPortAudioInput::notifyInterval() const{
    return notifyIntervalInMillis__;
}


qint64 QPortAudioInput::processedUSecs() const{
    return pollings__*1000000*settings__.framesPerBuffer/settings__.sampleRate;
}

qint64 QPortAudioInput::elapsedUSecs() const{
    return std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now()-started__).count();
}


QPortAudio::Error QPortAudioInput::error() const {
    return error__;
};

QPortAudio::State QPortAudioInput::state() const {
    return state__;
}

void QPortAudioInput::setVolume(qreal volume_){
    volume__=volume_;
}
qreal QPortAudioInput::volume() const {
    return volume__;
}

QString QPortAudioInput::category() const {
    return "";
}
void QPortAudioInput::setCategory(const QString &category){
   // not supported
};
