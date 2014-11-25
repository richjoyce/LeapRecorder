#ifndef LEAPRECORDER_H
#define LEAPRECORDER_H

#include <vector>
#include <mutex>
#include "Leap.h"

class LeapRecorder : public Leap::Listener
{
public:
    enum LeapRecorderState {
        STATE_IDLE,
        STATE_RECORDING,
        STATE_PLAY,
        STATE_PAUSE,
    };

    LeapRecorder() : state(STATE_IDLE), loop(false) {}

    void Play();
    void Pause();
    void Record();
    void Stop();
    void Reset();

    bool Save(std::string filename);
    bool Load(std::string filename);

    Leap::Frame GetCurrentFrame();

    LeapRecorderState get_state() const { return state; }

    volatile bool loop;
private:

    volatile LeapRecorderState state;

    std::vector<std::string> frames;
    std::mutex frames_lock;

    // playback params
    int64_t playback_start_leap;
    int64_t playback_start_system;
    int64_t GetSystemMicroseconds();

    unsigned int playback_index;
    Leap::Frame GetFrameAtIndex(unsigned int index);

    // Listener interface
public:
    void onDisconnect(const Leap::Controller &);
    void onExit(const Leap::Controller &);
    void onFrame(const Leap::Controller &);
};

#endif // LEAPRECORDER_H
