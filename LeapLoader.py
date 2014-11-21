import Leap, struct, sys

if len(sys.argv) != 2:
    print "usage:", sys.argv[0], "<filename>"
    exit(1)

filename = sys.argv[1]
controller = Leap.Controller()

frame_list = []
with open(filename, "rb") as file_:
    frame_size_data = file_.read(4)
    while frame_size_data:
        frame_size = struct.unpack('i', frame_size_data)[0]
        frame_data_ptr = Leap.byte_array(frame_size)
        for i in xrange(frame_size):
            frame_data_ptr[i] = struct.unpack('B', file_.read(1))[0]
        frame = Leap.Frame()
        frame.deserialize((frame_data_ptr,frame_size))
        frame_list.append(frame)
        frame_size_data = file_.read(4)

print "Loaded %d frames." % len(frame_list)
for frame in frame_list:
    print frame
