#ifndef _SURFACE_H_
#define _SURFACE_H_

#ifndef DISABLE_EXR
#include <string>
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/ImathBox.h>
#include <OpenEXR/half.h>

class Surface
{
	void setupFrameBuffer(unsigned int mask, Imf::FrameBuffer & frameBuffer, const Imath::Box2i & dw);
public:
	enum Channel {
		Unknown,
		R = 1,
		G = 2,
		B = 4,
		RGB = 7,
		D = 8,
		M = 16,
		NX = 32,
		NY = 64,
		NZ = 128
	};
	struct Sample {
		half r;
		half g;
		half b; // / 255.0f
		float depth; // check against normal
		float nx, ny, nz; // could use half?
		unsigned int material; // only 0-255 used, though compressed
	};
	Imf::Array2D<Sample> m_framebuffer;
	unsigned int m_channelMask;

	Surface(int width, int height);

	static Surface * load(const std::string & path);
	void save(const std::string & path);

	int getWidth() const { return m_framebuffer.width(); }
	int getHeight() const { return m_framebuffer.height(); }

	void setChannel(Channel channel, void * buffer, size_t size);
	void getChannel(Channel channel, void * buffer, size_t size);
};

#endif
#endif // _SURFACE_H_
