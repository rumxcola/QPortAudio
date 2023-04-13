#ifndef QPORTAUDIO_H
#define QPORTAUDIO_H
#include <QMetaType>

#include <string>

class QPortAudio{
public:
    enum class Error { NoError, OpenError, IOError, UnderrunError, FatalError };
    /**
     * ActiveState	    0 Audio data is being processed, this state is set after start()
     * is called and while audio data is available to be processed.
     *
     * SuspendedState   1 The audio stream is in a suspended state. Entered after suspend()
     * is called or when another stream takes control of the audio device. In the later case,
     * a call to resume will return control of the audio device to this stream. This should
     * usually only be done upon user request.
     *
     * StoppedState	    2 The audio device is closed, and is not processing any audio data
     *
     * IdleState	    3 The QIODevice passed in has no data and audio system's buffer is empty,
     * this state is set after start() is called and while no audio data is available to be processed.
     *
     * InterruptedState	4 This stream is in a suspended state because another higher priority stream
     * currently has control of the audio device. Playback cannot resume until the higher priority
     * stream relinquishes control of the audio device.
     */
    enum class State {
        ActiveState=0,
        SuspendedState=1,
        StoppedState=2,
        IdleState=3,
        InterruptedState=4
    };
    static std::string stateToString(State s){
        switch(s){
        case State::ActiveState: return "ActiveState";
        case State::SuspendedState: return "SuspendedState";
        case State::StoppedState:return "StoppedState";
        case State::IdleState:return "IdleState";
        case State::InterruptedState:return "InterruptedState";
        default: return "";
        }
    }
    enum class Mode { AudioInput, AudioOutput };
};

#endif // QPORTAUDIO_H
