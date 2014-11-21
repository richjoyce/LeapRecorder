LeapRecorder
============

Examples to record and playback LeapMotion serialized data.
This is loosely based off of the C# Unity example from the LeapMotion team.
The major difference is that my playback is based off of timestamp instead of iterating through each frame, so that recording fps does not have to equal playback fps.

My use case is to record data in my C++ application that can later be analyzed in a Python script (i.e. make pretty plots), so hopefully this helps someone.

It uses a small amount of C++11 that could be easily stripped out as it is mostly in the ``LeapRecorder::GetSystemMicroseconds()`` function, so you just need to replace it with your way of getting microseconds of execution time.
This is only needed for playback though, so if you just want to record, you can strip it all out.
The other use is simply an `auto` to save keystrokes in ``LeapRecorder::Save``.

Files
=====

* **LeapRecorder.{cpp,h}** C++ class that acts as a Leap::Listener to record all frames, and provides a playback ability
* **record.cpp** sample of how to use the record function
* **play.cpp** sample of playing back recorded data
* **LeapLoader.py** python script that loads in the recorded data (could be used for anaylzing recorded data)

Data Format
===========

I've stuck to the convention used by the Unity package, and described [here](https://community.leapmotion.com/t/serialize-deserialize-sample-request/1748/2):

> In this sample, we first save the byte array size as a 4 Bytes Int, then we save the byte array. We repeat this process for the duration of the recording, and save all the data into 1 .byte file.
> 
> For example, if you save two frames, the file format could look like:  
> [ frame data size 1 (4 B) ] [ frame data 1 (5 KB) ] [ frame data size 2 (4 B) ] [ frame data 2 (5 KB) ]

Unfortunately I don't know if what I implemented works the same as they didn't provide any sample data and I do not have Unity to test. If someone could test that for me that would be great!

Compilation
===========

There is no `make` file or anything, sorry! It's a fairly simple compilation though.
