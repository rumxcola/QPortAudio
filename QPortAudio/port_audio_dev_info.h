#ifndef PORTAUDIODEVINFO_H
#define PORTAUDIODEVINFO_H

#include <portaudio.h>
#include <string>
#include <ostream>

namespace PortAudio {
struct PortAudioDevInfo{
    enum class Direction{ Input, Output, Both};
    enum State { ActiveInput,ActiveOutput, ActiveBoth, SuspendedState, StoppedState, IdleState, InterruptedState };
    static std::string stateToString(State s){
        switch(s){
        case ActiveInput: return "ActiveInput";
        case ActiveOutput: return "ActiveOutput";
        case ActiveBoth: return "ActiveBoth";
        case SuspendedState: return "SuspendedState";
        case StoppedState:return "StoppedState";
        case IdleState:return "IdleState";
        case InterruptedState:return "InterruptedState";
        }
    }
    PaDeviceIndex devNum=-1;
    int structVersion=2;  /* this is struct version 2 */
    std::string name="no device";
    PaHostApiIndex hostApi=0; // note this is a host API index, not a type id
    int maxInputChannels=0;
    int maxOutputChannels=0;
    PaTime defaultLowInputLatency=0;//Default latency values for interactive performance.
    PaTime defaultLowOutputLatency=0;//Default latency values for interactive performance.
    PaTime defaultHighInputLatency=0;//Default latency values for robust non-interactive applications (eg. playing sound files).
    PaTime defaultHighOutputLatency=0;//Default latency values for robust non-interactive applications (eg. playing sound files).
    double defaultSampleRate=0;
    bool operator==(const PortAudioDevInfo& other_);
    PortAudioDevInfo()=default;
    PortAudioDevInfo(int num_, const   PaDeviceInfo *deviceInfo);
    std::string toString()const ;
    std::string toShortString() const;
    std::string deviceName() const {return name;}
    bool isDirectionSupported(bool isAudioInput_) const ;
};
}
std::ostream& operator<<(std::ostream& os, const PortAudio::PortAudioDevInfo& dev);

#endif // PORTAUDIODEVINFO_H
