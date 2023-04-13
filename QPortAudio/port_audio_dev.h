#ifndef PORTAUDIODEV_H
#define PORTAUDIODEV_H
#include <algorithm>
#include <set>
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <portaudio.h>
#include <inttypes.h>
#include <string>
#include <string.h>
#include <vector>
#include <memory>
#include <atomic>
#include "port_audio_dev_info.h"
#include "port_audio_stream_settings.h"

std::ostream& operator<<(std::ostream& o, const PaStreamParameters& p);
namespace PortAudio {
class PortAudioDevStateChangedListener;


class PortAudioDevStateChangedListener{
public:
    virtual ~PortAudioDevStateChangedListener(){}
    virtual void onStateChanged(PortAudioDevInfo::State)=0;
};


class PortAudioDev{
    class PortAudioLibRAII;
    class PortAudioStream;

    static std::unordered_map<PaDeviceIndex, std::unique_ptr<PortAudioStream>> devices__;
    static std::mutex mutex__;

    PortAudioStream *dev__=nullptr;


public:
    using StateListener=PortAudioDevStateChangedListener;
    using Writer=std::function<int64_t(char * , int64_t)>;
    using Reader=std::function<int64_t(const char *, int64_t )>;
    using Sett=PortAudioStreamSettings;
    using Direction=PortAudioDevInfo::Direction;
    using Info=PortAudioDevInfo;
    using State=Info::State;
    static Info getDefaultInputDevice();
    static Info getDefaultOutputDevice();
    static std::vector<Info> getDevicesList();
    static std::vector<Info> getDevicesList(Direction mode_);
    static std::vector<Info> getDevicesList(bool isAudioInput_);

    PortAudioDev(Info devInfo_);
    ~PortAudioDev();
    bool init(Reader r_, int chCnt_, Sett s_ =Sett());
    bool init(Writer w_,int chCnt_, Sett s_ =Sett());
    bool stopInput();
    bool stopOuput();
    bool halt();
    void addStateListener(StateListener* listener_);
    void removeStateListener(StateListener* listener_);
};

class PortAudioDev::PortAudioLibRAII {
    friend class PortAudioStream;
    friend class PortAudioDev;
    PaError result__;
    PortAudioLibRAII();
    ~PortAudioLibRAII();
    bool result() const;
    PaError getErr() const ;
};


class PortAudioDev::PortAudioStream
{
    friend class PortAudioDev;
    PaStream* stream__=nullptr;

    Direction dir__;
    Writer writer__;
    Reader reader__;
    int channelsInput__;
    int channelsOutput__;
    PortAudioStreamSettings settings__;

    std::unique_ptr<PaStreamParameters> outParams__=nullptr,inParams__=nullptr;
    PortAudioLibRAII handler__=PortAudioLibRAII();
    PaError err__=handler__.getErr();
    PortAudioDevInfo devInfo__=PortAudioDevInfo();
    std::atomic_int holdersCount__{0};
    State state__=State::IdleState;
    std::vector<StateListener*> stateListeners__;
    std::mutex listenersMutex__;
    static int playCallback(const void *buffIn_, void *buffOut_, unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData);

    PaStreamCallbackResult callback(const char *buffIn_, char *buffOut_, unsigned long framesPerBuffer,
                                const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags);
    bool isInputInited() const;
    bool isOutputInited() const;
    PortAudioStream(PortAudioDevInfo devInfo_);
    PortAudioStream()=delete;
    PortAudioStream(const PortAudioStream&)=delete;
    bool start();
    void toLog(std::string msg_){ std::cout<<"PortAudio: "<<msg_<<std::endl; }
    PortAudioStream& acquire();
    void onHolderReleased();
    bool isHolderless() const;

    bool init(Reader reader_, int channels_, Sett settings_ =Sett());
    bool init(Writer writer_, int channels_, Sett settings_ =Sett());
    bool stopInput();
    bool stopOutput();
    bool halt();
    bool checkAndPrintIfErr(PaError err_);
    void setState(State state_);
    void addStateListener(StateListener* listener_);
    void removeStateListener(StateListener* listener_);
public:
    ~PortAudioStream();
};


}

#endif // PORTAUDIODEV_H
