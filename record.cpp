#include "Leap.h"
#include "LeapRecorder.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "usage: record <filename>" << std::endl;
        return 1;
    }
    FILE* test = fopen(argv[1], "wb");
    if (!test) {
        std::cerr << "can not open " << argv[1] << " for writing..." << std::endl;
        return 2;
    }
    fclose(test);
    Leap::Controller controller;
    LeapRecorder recorder;
    controller.addListener(recorder);
    controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

    if (!controller.isConnected()) {
        std::cerr << "error no LeapMotion found!" << std::endl;
        return 3;
    }

    // Record until Enter is pressed
    recorder.Record();
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();
    recorder.Stop();

    if (!recorder.Save(argv[1])) {
        std::cerr << "error during saving." << std::endl;
        return 2;
    }

    controller.removeListener(recorder);
    return 0;
}
