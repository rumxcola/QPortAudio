#include "qport_audio_output.h"
Q_DECLARE_METATYPE(QPortAudio::State)

QPortAudioOutput::QPortAudioOutput(const QPortAudioDeviceInfo &audioDeviceInfo, const QPortAudioFormat &format_, QObject *parent_):
    QObject(parent_),
    fmt__(format_),
    devInfo__(audioDeviceInfo),
    dev__(devInfo__.info__)
{    
    qRegisterMetaType<QPortAudio::State>();
    dev__.addStateListener(this);
}

QPortAudioOutput::~QPortAudioOutput(){
    dev__.removeStateListener(this);
    stop();
}

void QPortAudioOutput::onStateChanged(PortAudio::PortAudioDevInfo::State state_){
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
    case PAState::ActiveOutput:
        if(state__!=QPortAudio::State::IdleState)
            newState=QPortAudio::State::ActiveState;
    case PAState::ActiveInput:
        break;
    }
    setState(newState);
}
void QPortAudioOutput::volumize(char *buf_, int64_t len_){
    uint64_t i=0;
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
        default:break;
        }
    }
    default:
        break;
        return;
    }

}

void QPortAudioOutput::setState(QPortAudio::State s){
    if(s==state__)
        return;
    state__=s;
    Q_EMIT stateChanged(state__);
}

int64_t QPortAudioOutput::read(char * buf_, int64_t len_){
    if(state__!=QPortAudio::State::ActiveState){
        memset(buf_,0,len_);
        return len_;
    }
    auto res=device__->read(buf_,len_);
    if(res<len_)
        setState(QPortAudio::State::IdleState);

    volumize(buf_,len_);
    ++pollings__;
    millisSinceLastNotification__+=pollings__*1000*settings__.framesPerBuffer/settings__.sampleRate;
    if(millisSinceLastNotification__>=notifyIntervalInMillis__){
        millisSinceLastNotification__=0;
        Q_EMIT notify();
    }
    return res;
}

QPortAudioFormat QPortAudioOutput::format() const {
    return fmt__;
}

void QPortAudioOutput::start(QIODevice *device){

    if(!device)
        return;
    state__=QPortAudio::State::InterruptedState;
    error__=QPortAudio::Error::NoError;
    dev__.stopOuput();
    device__=device;
    settings__=fmt__.toStreamSettings(bufSize__);
    pollings__=0;
    started__=std::chrono::steady_clock::now();
    if(devInfo__.mode()!=QPortAudio::Mode::AudioOutput
            ||!dev__.init(w__,fmt__.channelCount(),settings__)){
        error__=QPortAudio::Error::OpenError;
        setState(QPortAudio::State::StoppedState);
        return;
    }
}

QIODevice* QPortAudioOutput::start(){
    start(device__);
    return device__;
};

void QPortAudioOutput::stop(){
    state__=QPortAudio::State::StoppedState;
    dev__.stopOuput();
};

void QPortAudioOutput::reset(){
    stop();
    start();
};

void QPortAudioOutput::suspend(){
    prevState__=state__;
    state__=QPortAudio::State::SuspendedState;
}

void QPortAudioOutput::resume(){
    state__=prevState__;
}

void QPortAudioOutput::setBufferSize(int bytes){
    bufSize__=bytes;
};

int QPortAudioOutput::bufferSize() const{
    return settings__.bytesInFrame*settings__.framesPerBuffer*fmt__.channelCount();
};

int QPortAudioOutput::bytesFree() const {
    return 0;
}

int QPortAudioOutput::periodSize() const{
    return 1000*settings__.framesPerBuffer/settings__.sampleRate;
}

void QPortAudioOutput::setNotifyInterval(int milliSeconds){
    notifyIntervalInMillis__=milliSeconds;
}

int QPortAudioOutput::notifyInterval() const{
    return notifyIntervalInMillis__;
}


qint64 QPortAudioOutput::processedUSecs() const{
    return pollings__*1000000*settings__.framesPerBuffer/settings__.sampleRate;
}

qint64 QPortAudioOutput::elapsedUSecs() const{
    return std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now()-started__).count();
}


QPortAudio::Error QPortAudioOutput::error() const {
    return error__;
};

QPortAudio::State QPortAudioOutput::state() const {
    return state__;
}

void QPortAudioOutput::setVolume(qreal volume_){
    volume__=volume_;
}
qreal QPortAudioOutput::volume() const {
    return volume__;
}

QString QPortAudioOutput::category() const {
    return "";
}
void QPortAudioOutput::setCategory(const QString &category){
   // not supported
};
