# ZBrushPluginTest
This is an experiment if I can write a native plugin that can process the ZBrush canvas. The idea was to export the
ZBrush canvas to a native C++ DLL, do some processing, and then send the result back to ZBrush. I wanted to work
on raw pixol data, to preserve depth and normal information.

The code packs the pixol data into memory buffers and then calls the DLL function to copy it into an internal data 
structure. For now this is saved as an EXR image.

The key problems I discovered are that the process to extract the canvas is extremely slow, and there is no way to 
write to the canvas. As far as I can see the only option is to simulate brush strokes.

I found that you have to link the OpenEXR library with delay load, otherwise ZBrush doesn't load the DLL correctly.

## How to build
Well, you probably don't want to, as it is quite useless. You need to get and build the OpenEXR libary, then adjust the paths in the 
project file to pick up the library.

Unfortunately I can't answer many questions about this, as my ZBrush evaluation license has expired.