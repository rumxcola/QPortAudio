#ifndef QPORTAUDIOOUTPUT_H
#define QPORTAUDIOOUTPUT_H
#include <QObject>
#include <QIODevice>
#include <chrono>

#include "qport_audio.h"
#include "qport_audio_format.h"
#include "port_audio_dev.h"
#include "qport_audio_device_info.h"

class QPortAudioOutput : public QObject, public PortAudio::PortAudioDevStateChangedListener
{
    Q_OBJECT
    static const int DEFAULT_BUFSIZE=-1;
    using Format=QPortAudioFormat;
    qint64 processedUSecs__;
    qint64 elapsedUSecs__;
    QPortAudio::Error error__;
    QPortAudio::State state__,prevState__;
    QPortAudioFormat fmt__;
    QPortAudioDeviceInfo devInfo__;
    PortAudio::PortAudioDev dev__;
    PortAudio::PortAudioStreamSettings settings__;
    QIODevice *device__=nullptr;
    qreal volume__=1.0;
    int notifyIntervalInMillis__=1000,millisSinceLastNotification__=0;
    int bufSize__=DEFAULT_BUFSIZE;
    int pollings__=0;

    bool wasActive__=false;
    std::chrono::steady_clock::time_point started__;
    PortAudio::PortAudioDev::Writer w__=[this](char * buf_, int64_t len_){return read(buf_,len_);};

    void onStateChanged(PortAudio::PortAudioDevInfo::State state_);

    void volumize(char *buf_,int64_t len_);
    int64_t read(char * buf_, int64_t len_);
    void setState(QPortAudio::State);
public:
    explicit QPortAudioOutput(const QPortAudioDeviceInfo &audioDeviceInfo=QPortAudioDeviceInfo::defaultOutputDevice(), const QPortAudioFormat &format_ = QPortAudioFormat(), QObject *parent_ = nullptr);
    ~QPortAudioOutput();
    /**
     * @brief format
     * @return Returns the QAudioFormat being used.
     */
    QPortAudioFormat format() const ;

    /**
     * Returns a pointer to the internal QIODevice being used to transfer data
     * to the system's audio output. The device will already be open and write()
     * can write data directly to it.
     *
     * Note: The pointer will become invalid after the stream is stopped or if you start another stream.
     *
     * If the QAudioOutput is able to access the system's audio device, state()
     * returns QPortAudio::IdleState, error() returns QPortAudio::NoError and
     * the stateChanged() signal is emitted.
     *
     * If a problem occurs during this process, error() returns QAudio::OpenError,
     * state() returns QAudio::StoppedState and the stateChanged() signal is emitted.
    */
    void start(QIODevice *device);

    /**
     * @brief Starts transferring audio data from the device to the system's audio output.
     * The device must have been opened in the ReadOnly or ReadWrite modes.
     *
     * If the QAudioOutput is able to successfully output audio data, state()
     * returns QAudio::ActiveState, error() returns QAudio::NoError and
     * the stateChanged() signal is emitted.
     *
     * If a problem occurs during this process, error() returns QAudio::OpenError,
     * state() returns QAudio::StoppedState and the stateChanged() signal is emitted.
     * @return
     */
    QIODevice* start();

    /**
     * @brief stop
     * Stops the audio output, detaching from the system resource.
     * Sets error() to QAudio::NoError, state() to QAudio::StoppedState and emit stateChanged() signal.
     */
    void stop();

    /**
     * @brief reset
     * Drops all audio data in the buffers, resets buffers to zero.
     */
    void reset();

    /**
     * @brief suspend
     * Stops processing audio data, preserving buffered audio data.
     * Sets error() to QAudio::NoError, state() to QAudio::SuspendedState and
     * emits stateChanged() signal.
     */
    void suspend();

    /**
     * Resumes processing audio data after a suspend().
     * Sets error() to QAudio::NoError. Sets state() to QAudio::ActiveState if you previously called start(QIODevice*). Sets state() to QAudio::IdleState if you previously called start(). emits stateChanged() signal.
    */
    void resume();

    /**
     * Sets the audio buffer size to value in bytes.
     * Note: This function can be called anytime before start(). Calls to this are ignored
     * after start(). It should not be assumed that the buffer size set is the actual buffer
     * size used - call bufferSize() anytime after start() to return the actual buffer size being used.
     */
    void setBufferSize(int bytes);
    /**
     * Returns the audio buffer size in bytes.
     * If called before start(), returns platform default value. If called before start()
     * but setBufferSize() was called prior, returns value set by setBufferSize(). If called
     * after start(), returns the actual buffer size being used. This may not be what was set
     * previously by setBufferSize().
     */
    int bufferSize() const;

    /**
     * Returns the number of free bytes available in the audio buffer.
     * Note: The returned value is only valid while in QAudio::ActiveState
     * or QAudio::IdleState state, otherwise returns zero.
    */
    int bytesFree() const ;

    /**
     * Returns the period size in bytes. This is the amount of data required
     * each period to prevent buffer underrun, and to ensure uninterrupted playback.
     * Note: It is recommended to provide at least enough data for a full period with
     *  each write operation.
    */
    int periodSize() const;

    /**
     * Sets the interval for notify() signal to be emitted.
     * This is based on the ms of audio data processed, not on wall clock time.
     * The minimum resolution of the timer is platform specific and values
     * should be checked with notifyInterval() to confirm the actual value being used.
     */
    void setNotifyInterval(int milliSeconds);

    /**
     * Returns the notify interval in milliseconds.
    */
    int notifyInterval() const;

    /**
     * Returns the amount of audio data processed since start()
     * was called (in microseconds).
     */
    qint64 processedUSecs() const;

    /**
     *Returns the microseconds since start() was called,
     *including time in Idle and Suspend states.
     */
    qint64 elapsedUSecs() const;

    /**
     * Returns the error state.
     */
    QPortAudio::Error error() const ;

    /**
     * Returns the state of audio processing.
     */
    QPortAudio::State state() const ;

    /**
     * Sets the output volume to volume.
     * The volume is scaled linearly from 0.0 (silence) to 1.0 (full volume). Values outside this range will be clamped.
     *
     * The default volume is 1.0.
     * Note: Adjustments to the volume will change the volume of this audio stream, not the global volume.     *
     * UI volume controls should usually be scaled nonlinearly. For example, using a logarithmic scale will produce linear changes in perceived loudness, which is what a user would normally expect from a volume control. See QAudio::convertVolume() for more details.
    */
    void setVolume(qreal volume_);
    /**
     * @brief volume
     * @return Returns the volume between 0.0 and 1.0 inclusive.
     */
    qreal volume() const ;

    QString category() const ;
    /**
     * Returns the audio category of this audio stream.
     * Some platforms can group audio streams into categories
     * and manage their volumes independently, or display them
     * in a system mixer control. You can set this property to
     * allow the platform to distinguish the purpose of your streams.
     *
     */
    void setCategory(const QString &category);

Q_SIGNALS:

    /**
     * This signal is emitted when the device state has changed.
     * This is the current state of the audio output.
     */
    void stateChanged(QPortAudio::State state);
    //void stateChanged(int);

    /**
     * This signal is emitted when a certain interval of milliseconds of audio data has been processed.
     * The interval is set by setNotifyInterval().
     */
    void notify();
private:
    Q_DISABLE_COPY(QPortAudioOutput)
};
#endif // QPORTAUDIOOUTPUT_H
