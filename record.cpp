#include "Leap.h"
#include "LeapRecorder.h"

int main(int argc, char *argv[])
{
    Leap::Controller controller = Leap::Controller();
    LeapRecorder recorder;
    controller.addListener(recorder);
}
