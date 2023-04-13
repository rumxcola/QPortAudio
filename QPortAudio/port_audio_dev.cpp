#include "port_audio_dev.h"

std::ostream& operator<<(std::ostream& o, const PaStreamParameters& p){
    std::string str=
            "{idx="+std::to_string(p.device)
            +" channels="+std::to_string(p.channelCount)
            +"\tlatency ="
            +std::to_string(p.suggestedLatency)
            +"\t sf="+std::to_string(p.sampleFormat)+"}"
            ;
    return o<<str;
}

namespace PortAudio {

std::unordered_map<PaDeviceIndex, std::unique_ptr<PortAudioDev::PortAudioStream>> PortAudioDev::devices__;
std::mutex PortAudioDev::mutex__;

PortAudioDevInfo PortAudioDev::getDefaultInputDevice(){
    PortAudioLibRAII handler;
    if(!handler.result())
        return PortAudioDevInfo();
    auto idx=Pa_GetDefaultInputDevice();
    return PortAudioDevInfo(idx,Pa_GetDeviceInfo(idx));
}

PortAudioDev::Info PortAudioDev::getDefaultOutputDevice(){
    PortAudioLibRAII handler;
    if(!handler.result())
        return PortAudioDevInfo();
    auto idx=Pa_GetDefaultOutputDevice();
    return PortAudioDevInfo(idx,Pa_GetDeviceInfo(idx));
}

std::vector<PortAudioDev::Info> PortAudioDev::getDevicesList(){
    std::vector<PortAudioDevInfo> devices;
    PortAudioLibRAII handler;
    if(!handler.result())
        return devices;
    PaDeviceIndex numDevices__=Pa_GetDeviceCount();
    for(PaDeviceIndex idx=0;idx<numDevices__;++idx)
        devices.emplace_back(idx,Pa_GetDeviceInfo(idx));
    return devices;
}

std::vector<PortAudioDev::Info> PortAudioDev::getDevicesList(Direction mode_){
    return getDevicesList(mode_!=Direction::Output);
}

std::vector<PortAudioDev::Info> PortAudioDev::getDevicesList(bool isAudioInput_){
    std::vector<Info> devices;
    PortAudioLibRAII handler;
    if(!handler.result())
        return devices;
    PaDeviceIndex numDevices__=Pa_GetDeviceCount();
    for(PaDeviceIndex idx=0;idx<numDevices__;++idx){
        auto info=Pa_GetDeviceInfo(idx);
        auto channelsCnt=isAudioInput_?info->maxInputChannels:info->maxOutputChannels;
        if(channelsCnt!=0)
            devices.emplace_back(idx,Pa_GetDeviceInfo(idx));
    }
    return devices;
}


PortAudioDev::PortAudioDev(PortAudioDevInfo devInfo_){
    std::unique_lock<std::mutex> locker(mutex__);
    if(PortAudioDev::devices__.find(devInfo_.devNum)==devices__.end())
        devices__[devInfo_.devNum]=std::unique_ptr<PortAudioStream>(new PortAudioStream(devInfo_));
    dev__=&devices__[devInfo_.devNum]->acquire();
}

PortAudioDev::~PortAudioDev(){
    dev__->onHolderReleased();
    if(!dev__->isHolderless())
        return;
    std::unique_lock<std::mutex> locker(mutex__);
    devices__.erase(dev__->devInfo__.devNum);
}

bool PortAudioDev::init(Reader r_, int chCnt_, Sett s_){
    return dev__->init(r_,chCnt_,s_);
}

bool PortAudioDev::init(Writer w_,int chCnt_, Sett s_){
    return dev__->init(w_,chCnt_,s_);
}

bool PortAudioDev::stopInput(){
    return dev__->stopInput();
}

bool PortAudioDev::stopOuput(){
    return dev__->stopOutput();
}
void PortAudioDev::addStateListener(StateListener* listener_){
    dev__->addStateListener(listener_);
}
void PortAudioDev::removeStateListener(StateListener* listener_){
    dev__->removeStateListener(listener_);
}

bool PortAudioDev::halt(){
    return dev__->halt();
}


PortAudioDev::PortAudioLibRAII::PortAudioLibRAII() {
    result__=Pa_Initialize();
}

PortAudioDev::PortAudioLibRAII::~PortAudioLibRAII(){
    if (result__ != paNoError)
        return;
    Pa_Terminate();
}

bool PortAudioDev::PortAudioLibRAII::result() const {
    return result__==paNoError;
}

PaError PortAudioDev::PortAudioLibRAII::getErr() const {
    return result__;
}


PortAudioDev::PortAudioStream::PortAudioStream(PortAudioDevInfo devInfo_):devInfo__(devInfo_){

}

PortAudioDev::PortAudioStream::~PortAudioStream(){
    if(!stream__)
        return;
    halt();
}

bool PortAudioDev::PortAudioStream::init(Reader reader_, int channels_, Sett settings_){
    if(isInputInited() || !reader_ || channels_>devInfo__.maxInputChannels)
        return false;
    if(isOutputInited()){
        if(!halt())
            return false;
        if(settings__!=settings_)
            toLog("Changing stream settings cause of reinit");
    }

    auto prevSettings=settings__;// std::move(sett__);


    settings__=settings_;//std::unique_ptr<Sett>(new Sett(settings_));

    inParams__=std::unique_ptr<PaStreamParameters>(new PaStreamParameters());
    inParams__->device=devInfo__.devNum;
    inParams__->suggestedLatency=devInfo__.defaultLowOutputLatency;
    inParams__->hostApiSpecificStreamInfo=nullptr;
    inParams__->channelCount=channelsInput__=channels_;

    inParams__->sampleFormat=static_cast<PaSampleFormat>(settings_.sf);
    reader__=reader_;
    if(start())
        return true;

    inParams__=nullptr;
    reader__=nullptr;
    settings__=prevSettings;
    start();
    return false;
}

bool PortAudioDev::PortAudioStream::init(Writer writer_, int channels_, Sett settings_){
    if(isOutputInited() || !writer_ || channels_>devInfo__.maxOutputChannels)
        return false;
    if(isInputInited()){
        if(!halt())
            return false;
        if(settings_!=settings__)
            toLog("Changing stream settings cause of reinit");
    }
    auto prevSettings=settings__;

    settings__=settings_;

    outParams__=std::unique_ptr<PaStreamParameters>(new PaStreamParameters());
    outParams__->device=devInfo__.devNum;
    outParams__->suggestedLatency=devInfo__.defaultLowInputLatency;
    outParams__->hostApiSpecificStreamInfo=nullptr;
    outParams__->channelCount=channelsOutput__=channels_;
    outParams__->sampleFormat=static_cast<PaSampleFormat>(settings_.sf);
    writer__=writer_;

    if(start())
        return true;

    outParams__=nullptr;
    writer__=nullptr;
    settings__=prevSettings;
    start();
    return false;
}

bool PortAudioDev::PortAudioStream::isOutputInited() const{
    return handler__.result() && stream__ && outParams__&& writer__;
}

bool PortAudioDev::PortAudioStream::isInputInited() const {
    return handler__.result() && stream__ && inParams__ && reader__ ;
}

bool PortAudioDev::PortAudioStream::halt(){
    if(!isInputInited() && !isOutputInited())
        return false;
    checkAndPrintIfErr(Pa_CloseStream(stream__));
    auto &out=std::cout<<devInfo__.toShortString()<<" halted ";
    if(writer__ && reader__)
        out <<"intput and output";
    else
        out << (writer__?std::string("output"):std::string("input"));
    out//<<"\n"
            <<std::endl;
    setState(State::StoppedState);
    stream__=nullptr;
    return true;
}

bool PortAudioDev::PortAudioStream::start(){
    if(!writer__ && !reader__)
        return false;
    /*if(inParams__)
        std::cout<<"starting in:"<<*inParams__.get()<<std::endl;
    if(outParams__)
        std::cout<<"starting in:"<<*outParams__.get()<<std::endl;*/

    if(!checkAndPrintIfErr(Pa_OpenStream(&stream__,inParams__.get(),outParams__.get(),
                                settings__.sampleRate,settings__.framesPerBuffer,
                                paNoFlag
                                ,playCallback,this))){
        setState(State::StoppedState);
        return false;
    }
    if(!checkAndPrintIfErr(Pa_StartStream(stream__))){
        setState(State::StoppedState);
        return false;
    }
    auto &out=std::cout<<devInfo__.toShortString()
                      <<" with settings "
                     <<settings__.toString()
                     <<" started ";
    if(writer__ && reader__)
        out <<"input("<<channelsInput__<<")"
            <<"and "
            <<"output("<<channelsOutput__<<")";
    else
        out << (writer__?(std::string("output(")+std::to_string(channelsOutput__)+")")
                           : (std::string("input(")+std::to_string(channelsInput__)+")"));

    out//<<"\n"
        <<std::endl;
    setState(writer__ && reader__?State::ActiveBoth:writer__?State::ActiveOutput:State::ActiveInput);
    return true;
}

int PortAudioDev::PortAudioStream::playCallback(const void *buffIn_, void *buffOut_, unsigned long frames_,
                 const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData){
    auto res=paAbort;
    if(!userData)
        return paAbort;
    return static_cast<PortAudioStream*>(userData)->callback(static_cast<const char*>(buffIn_),
                      static_cast<char *>(buffOut_),
                      frames_,timeInfo,statusFlags);
}

PaStreamCallbackResult PortAudioDev::PortAudioStream::callback(const char *buffIn_, char *buffOut_, unsigned long frames_,
              const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags){
    int64_t readed=0,writed=0;
    if(buffIn_ && inParams__ && reader__)
        readed=reader__(buffIn_,frames_*settings__.bytesInFrame*inParams__->channelCount);
    if(buffOut_ && outParams__ && writer__)
        writed=writer__(buffOut_,frames_*settings__.bytesInFrame*outParams__->channelCount);
    return paContinue;
}

PortAudioDev::PortAudioStream& PortAudioDev::PortAudioStream::acquire(){
    holdersCount__.fetch_add(1,std::memory_order_relaxed);
    return *this;
}

void PortAudioDev::PortAudioStream::onHolderReleased(){
    holdersCount__.fetch_sub(1,std::memory_order_relaxed);
}

bool PortAudioDev::PortAudioStream::isHolderless() const{
    return holdersCount__==0;
}

bool PortAudioDev::PortAudioStream::checkAndPrintIfErr(PaError err_){
    err__=err_;
    if(err__!=paNoError)
        std::cout<<Pa_GetErrorText(err__)<<"\n"<<std::endl;
    return err__==paNoError;
}



void PortAudioDev::PortAudioStream::addStateListener(StateListener* listener_){
    std::unique_lock<decltype(listenersMutex__)> locker(listenersMutex__);
    stateListeners__.push_back(listener_);
}

void PortAudioDev::PortAudioStream::removeStateListener(StateListener* listener_){
    std::unique_lock<decltype(listenersMutex__)> locker(listenersMutex__);
    for(auto it=stateListeners__.begin();it!=stateListeners__.end();++it){
        if(*it==listener_){
            stateListeners__.erase(it);
            return;
        }
    }
}
void PortAudioDev::PortAudioStream::setState(State state_){
    if(state__==state_)
        return;
    auto prevState=state__;
    state__=state_;
    auto listeneresCopy=std::vector<StateListener*>();
    {
        std::unique_lock<decltype(listenersMutex__)> locker(listenersMutex__);
        std::copy(stateListeners__.begin(),stateListeners__.end(),std::back_inserter(listeneresCopy));
    }
    if(prevState==PortAudioDevInfo::ActiveOutput
            && state__==PortAudioDevInfo::StoppedState)
        std::cout<<"";
    std::cout<<devInfo__.toShortString()<<" "
            <<PortAudioDevInfo::stateToString(prevState)
           <<"->"
          <<PortAudioDevInfo::stateToString(state__)<<std::endl;
    for(auto &l:listeneresCopy)
        l->onStateChanged(state__);
}

bool PortAudioDev::PortAudioStream::stopInput(){
    bool isRestartNeed=isOutputInited();
    if(!isInputInited() || !halt())
        return false;
    inParams__=nullptr;
    reader__=nullptr;
    if(isRestartNeed)
        start();
    return true;
}

bool PortAudioDev::PortAudioStream::stopOutput(){
    bool isRestartNeed=isInputInited();
    if(!isOutputInited() || !halt())
        return false;
    outParams__=nullptr;
    writer__=nullptr;
    if(isRestartNeed)
        start();
    return true;
}

}
