#include "LeapRecorder.h"
#include <chrono>

void LeapRecorder::Play()
{
    switch (state) {
    case STATE_PLAY:
        return;
    case STATE_PAUSE:
        Pause();
        return;
    case STATE_RECORDING:
        Stop();
    case STATE_IDLE:
        state = STATE_PLAY;
    }

    /* Only get here if playing from beginning */
    if (frames.size() == 0) {
        std::cout << "Nothing to play!" << std::endl;
        state = STATE_IDLE;
        return;
    }

    playback_index = 0;
    playback_start_system = GetSystemMicroseconds();
    playback_start_leap = GetFrameAtIndex(playback_index).timestamp();
}

void LeapRecorder::Pause()
{
    /* Can't pause recording... */
    if (state == STATE_PLAY) {
        state = STATE_PAUSE;
    } else if (state == STATE_PAUSE) {
        state = STATE_PLAY;
        /* Reset the playback timers to now/current frame */
        playback_start_system = GetSystemMicroseconds();
        playback_start_leap = GetFrameAtIndex(playback_index).timestamp();
    }
}

void LeapRecorder::Record()
{
    Reset();
    state = STATE_RECORDING;
}

void LeapRecorder::Reset()
{
    frames_lock.lock();
    frames.clear();
    frames_lock.unlock();
    state = STATE_IDLE;
}

void LeapRecorder::Stop()
{
    if (state == STATE_PLAY) {
        playback_index = 0;
    } else if (state == STATE_RECORDING) {
        /* don't need to do anything ? */
        /* do i want to save? */
    }
    state = STATE_IDLE;
}

bool LeapRecorder::Save(std::string filename)
{
    FILE* fp = fopen(filename.c_str(), "wb");
    if (fp) {
        frames_lock.lock();
        for (auto it = frames.begin(); it != frames.end(); it++) {
            std::string frame_data = *it;
            frame_data.data();
            u_int32_t frame_size = frame_data.length();
            fwrite(&frame_size, sizeof(u_int32_t), 1, fp);
            fwrite(frame_data.data(), sizeof(char), frame_size, fp);
        }
        frames_lock.unlock();
        fclose(fp);
        return true;
    } else {
        std::cerr << "Error saving recorded data: Can not write to file: " << filename << std::endl;
        return false;
    }
}

bool LeapRecorder::Load(std::string filename)
{
    FILE* fp = fopen(filename.c_str(), "rb");
    if (fp) {
        Reset();
        frames_lock.lock();
        u_int32_t frame_size;
        while (fread(&frame_size, sizeof(u_int32_t), 1, fp) == 1) {
            char* data = (char*)malloc(frame_size*sizeof(char));
            if (fread(data, sizeof(char), frame_size, fp) != frame_size) {
                std::cerr << "Error reading recorded data: unexpected end of file: " << filename << std::endl;
                frames_lock.unlock();
                Reset();
                return false;
            }
            frames.push_back(std::string(data, frame_size));
        }
        std::cout << "Loaded " << frames.size() << " frames successfully." << std::endl;
        frames_lock.unlock();
        fclose(fp);
        return true;
    } else {
        std::cerr << "Error loading recorded data: Can not read file " << filename.c_str() << std::endl;
        return false;
    }
}

Leap::Frame LeapRecorder::GetCurrentFrame()
{
    if (state == STATE_PAUSE) {
        return GetFrameAtIndex(playback_index);
    } else if (state == STATE_PLAY) {
        /* Load newest frame, based on timestamp */
        /* Loop until next frame is past the current time */
        Leap::Frame last = GetFrameAtIndex(playback_index);
        Leap::Frame next = last;
        int64_t system_seconds = GetSystemMicroseconds() - playback_start_system;
        int64_t leap_seconds = next.timestamp()-playback_start_leap;

        while (system_seconds > leap_seconds) {
            if (playback_index+1 >= frames.size()) { /* Check if we will go too far */
                std::cout << "Reached end of playback." << std::endl;
                Stop();
                if (loop) Play(); // Restart if looping
                return GetCurrentFrame(); // Returns first frame if looping or invalid if stopped
            }
            playback_index++;
            last = next;
            next = GetFrameAtIndex(playback_index);
            leap_seconds = next.timestamp() - playback_start_leap;
        }
        return last;
    } else {
        return Leap::Frame();
    }
}

Leap::Frame LeapRecorder::GetFrameAtIndex(unsigned int index)
{
    Leap::Frame frame = Leap::Frame();
    frames_lock.lock();
    try {
        frame.deserialize(frames.at(index));
    } catch (const std::out_of_range& oor) {
        std::cerr << "Error getting frame number " << index << std::endl;
    }
    frames_lock.unlock();
    if (!frame.isValid()) {
        std::cout << "Error deserializing frame number " << index << std::endl;
    }
    return frame;
}

int64_t LeapRecorder::GetSystemMicroseconds()
{
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    return (int64_t)(std::chrono::duration_cast<std::chrono::microseconds>(now).count());
}

void LeapRecorder::onExit(const Leap::Controller &)
{
    if (state == STATE_RECORDING) {
        Stop();
    }
}

void LeapRecorder::onDisconnect(const Leap::Controller &)
{
    if (state == STATE_RECORDING) {
        Stop();
    }
}

void LeapRecorder::onFrame(const Leap::Controller &cont)
{
    if (state == STATE_RECORDING) {
        frames_lock.lock();
        frames.push_back(cont.frame().serialize());
        frames_lock.unlock();
    }
}
