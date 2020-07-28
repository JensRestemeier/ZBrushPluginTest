#include <string>

#include "surface.h"

#include <OpenEXR/OpenEXRConfig.h>
#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfOutputFile.h>
#include <OpenEXR/ImfTestFile.h>
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImathBox.h>

Surface::Surface(int width, int height)
	: m_channelMask(0)
{
	m_framebuffer.resizeErase(height, width);
}

void Surface::setupFrameBuffer(unsigned int mask, Imf::FrameBuffer & frameBuffer, const Imath::Box2i & dw)
{
	int width = m_framebuffer.width();
	int height = m_framebuffer.height();
	frameBuffer.insert("R", // name
		Imf::Slice(Imf::HALF, // type
		(char *)(&(&m_framebuffer[0][0] - // base
			dw.min.x -
			dw.min.y * width)->r),
			sizeof(Sample) * 1, // xStride
			sizeof(Sample) * width,// yStride
			1, 1, // x/y sampling
			0.0)); // fillValue
	frameBuffer.insert("G", // name
		Imf::Slice(Imf::HALF, // type
		(char *)(&(&m_framebuffer[0][0] - // base
			dw.min.x -
			dw.min.y * width)->g),
			sizeof(Sample) * 1, // xStride
			sizeof(Sample) * width,// yStride
			1, 1, // x/y sampling
			0.0)); // fillValue
	frameBuffer.insert("B", // name
		Imf::Slice(Imf::HALF, // type
		(char *)(&(&m_framebuffer[0][0] - // base
			dw.min.x -
			dw.min.y * width)->b),
			sizeof(Sample) * 1, // xStride
			sizeof(Sample) * width,// yStride
			1, 1, // x/y sampling
			0.0)); // fillValue
	frameBuffer.insert("Depth.Z", // name
		Imf::Slice(Imf::FLOAT, // type
		(char *)(&(&m_framebuffer[0][0] - // base
			dw.min.x -
			dw.min.y * width)->depth),
			sizeof(Sample) * 1, // xStride
			sizeof(Sample) * width,// yStride
			1, 1, // x/y sampling
			FLT_MAX)); // fillValue
	frameBuffer.insert("Normal.X", // name
		Imf::Slice(Imf::FLOAT, // type
		(char *)(&(&m_framebuffer[0][0] - // base
			dw.min.x -
			dw.min.y * width)->nx),
			sizeof(Sample) * 1, // xStride
			sizeof(Sample) * width,// yStride
			1, 1, // x/y sampling
			FLT_MAX)); // fillValue
	frameBuffer.insert("Normal.Y", // name
		Imf::Slice(Imf::FLOAT, // type
		(char *)(&(&m_framebuffer[0][0] - // base
			dw.min.x -
			dw.min.y * width)->ny),
			sizeof(Sample) * 1, // xStride
			sizeof(Sample) * width,// yStride
			1, 1, // x/y sampling
			FLT_MAX)); // fillValue
	frameBuffer.insert("Normal.Z", // name
		Imf::Slice(Imf::FLOAT, // type
		(char *)(&(&m_framebuffer[0][0] - // base
			dw.min.x -
			dw.min.y * width)->nz),
			sizeof(Sample) * 1, // xStride
			sizeof(Sample) * width,// yStride
			1, 1, // x/y sampling
			FLT_MAX)); // fillValue
	frameBuffer.insert("M", // name
		Imf::Slice(Imf::UINT, // type
		(char *)(&(&m_framebuffer[0][0] - // base
			dw.min.x -
			dw.min.y * width)->material),
			sizeof(Sample) * 1, // xStride
			sizeof(Sample) * width,// yStride
			1, 1, // x/y sampling
			FLT_MAX)); // fillValue

}

Surface * Surface::load(const std::string & path)
{
	Imf::InputFile file(path.c_str());
	Imath::Box2i dw = file.header().dataWindow();
	int width = dw.max.x - dw.min.x + 1;
	int height = dw.max.y - dw.min.y + 1;
	Surface * surface = new Surface(width, height);

	unsigned int mask = 0;
	const Imf::ChannelList &channels = file.header().channels();
	if (channels.findChannel("R") != 0) { mask |= Surface::R; }
	if (channels.findChannel("G") != 0) { mask |= Surface::G; }
	if (channels.findChannel("B") != 0) { mask |= Surface::B; }
	if (channels.findChannel("Depth.Z") != 0) { mask |= Surface::D; }
	if (channels.findChannel("Normal.X") != 0) { mask |= Surface::NX; }
	if (channels.findChannel("Normal.Y") != 0) { mask |= Surface::NY; }
	if (channels.findChannel("Normal.Z") != 0) { mask |= Surface::NZ; }
	if (channels.findChannel("M") != 0) { mask |= Surface::M; }

	Imf::FrameBuffer frameBuffer;
	surface->setupFrameBuffer(mask, frameBuffer, dw);
	file.setFrameBuffer(frameBuffer);
	file.readPixels(dw.min.y, dw.max.y);

	return surface;
}

void Surface::save(const std::string & path)
{
	long width = m_framebuffer.width();
	long height = m_framebuffer.height();

	Imf::Header header(width, height);
	if (m_channelMask & Surface::R)
	{
		header.channels().insert("R", Imf::Channel(Imf::HALF));
	}
	if (m_channelMask & Surface::G)
	{
		header.channels().insert("G", Imf::Channel(Imf::HALF));
	}
	if (m_channelMask & Surface::B)
	{
		header.channels().insert("B", Imf::Channel(Imf::HALF));
	}
	if (m_channelMask & Surface::D)
	{
		header.channels().insert("Depth.Z", Imf::Channel(Imf::FLOAT));
	}
	if (m_channelMask & Surface::NX)
	{
		header.channels().insert("Normal.X", Imf::Channel(Imf::FLOAT));
	}
	if (m_channelMask & Surface::NY)
	{
		header.channels().insert("Normal.Y", Imf::Channel(Imf::FLOAT));
	}
	if (m_channelMask & Surface::NZ)
	{
		header.channels().insert("Normal.Z", Imf::Channel(Imf::FLOAT));
	}
	if (m_channelMask & Surface::M)
	{
		header.channels().insert("M", Imf::Channel(Imf::UINT));
	}
	Imf::OutputFile file(path.c_str(), header);
	Imf::FrameBuffer frameBuffer;
	Imath::Box2i dw(Imath::V2i(0, 0), Imath::V2i(width - 1, height - 1));
	setupFrameBuffer(m_channelMask, frameBuffer, dw);
	file.setFrameBuffer(frameBuffer);
	file.writePixels(dw.max.y + 1);
}

void Surface::setChannel(Surface::Channel channel, void * buffer, size_t size)
{
	m_channelMask |= channel;
	int width = getWidth();
	int height = getHeight();
	switch (channel)
	{
	case Surface::RGB:
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				unsigned int ofs = (y * width + x) * 4;
				unsigned int val = *(unsigned int *)((char*)buffer + ofs);

				m_framebuffer[y][x].b = (half)((float)(val & 0xFF) / 255.0f);
				m_framebuffer[y][x].g = (half)((float)((val >> 8) & 0xFF) / 255.0f);
				m_framebuffer[y][x].r = (half)((float)((val >> 16) & 0xFF) / 255.0f);
			}
		}
		break;
	case Surface::D:
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				unsigned int ofs = (y * width + x) * 2;
				m_framebuffer[y][x].depth = (float)*(unsigned short *)((char*)buffer + ofs);
			}
		}
		break;
	case Surface::M:
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				unsigned int ofs = (y * width + x) * 1;
				m_framebuffer[y][x].material = *(unsigned char *)((char*)buffer + ofs);
			}
		}
		break;
	case Surface::NX:
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				unsigned int ofs = (y * width + x) * 4;
				m_framebuffer[y][x].nx = *(float *)((char*)buffer + ofs);
			}
		}
		break;
	case Surface::NY:
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				unsigned int ofs = (y * width + x) * 4;
				m_framebuffer[y][x].ny = *(float *)((char*)buffer + ofs);
			}
		}
		break;
	case Surface::NZ:
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				unsigned int ofs = (y * width + x) * 4;
				m_framebuffer[y][x].nz = *(float *)((char*)buffer + ofs);
			}
		}
		break;
	}
}

static int clamp(int low, int high, int val)
{
	if (val < low) {
		return low;
	}
	else if (val > high)
	{
		return high;
	}
	else
	{
		return val;
	}
}

void Surface::getChannel(Surface::Channel channel, void * buffer, size_t size)
{
	int width = getWidth();
	int height = getHeight();
	switch (channel)
	{
	case Surface::RGB:
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				unsigned int ofs = (y * width + x) * 4;

				unsigned int r = clamp(0, 255, round(m_framebuffer[y][x].r * 255.0f));
				unsigned int g = clamp(0, 255, round(m_framebuffer[y][x].g * 255.0f));
				unsigned int b = clamp(0, 255, round(m_framebuffer[y][x].b * 255.0f));

				unsigned int val = (r << 16) | (g << 8) | b;
				*(unsigned int *)((char*)buffer + ofs) = val;
			}
		}
		break;
	case Surface::D:
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				unsigned int ofs = (y * width + x) * 2;
				*(unsigned short *)((char*)buffer + ofs) = clamp(0, 65535, round(m_framebuffer[y][x].depth));
			}
		}
		break;
	case Surface::M:
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				unsigned int ofs = (y * width + x) * 1;
				*(unsigned char *)((char*)buffer + ofs) = m_framebuffer[y][x].material;
			}
		}
		break;
	case Surface::NX:
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				unsigned int ofs = (y * width + x) * 4;
				*(float *)((char*)buffer + ofs) = m_framebuffer[y][x].nx;
			}
		}
		break;
	case Surface::NY:
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				unsigned int ofs = (y * width + x) * 4;
				*(float *)((char*)buffer + ofs) = m_framebuffer[y][x].ny;
			}
		}
		break;
	case Surface::NZ:
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				unsigned int ofs = (y * width + x) * 4;
				*(float *)((char*)buffer + ofs) = m_framebuffer[y][x].nz;
			}
		}
		break;
	}
}
