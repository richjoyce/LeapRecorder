#include "Leap.h"
#include "LeapRecorder.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
    Leap::Controller controller = Leap::Controller();
    LeapRecorder recorder;
    if (argc != 2) {
        std::cerr << "usage: play <filename>" << std::endl;
        return 1;
    } else {
        if(!recorder.Load(argv[1])) {
            return 2;
        }
        recorder.Play();
        while (recorder.get_state() != LeapRecorder::STATE_IDLE) {
            Leap::Frame frame = recorder.GetCurrentFrame();
            std::cout << "Got frame id #" << frame.id() << std::endl;
            usleep(13000);
        }
    }
    return 0;
}
