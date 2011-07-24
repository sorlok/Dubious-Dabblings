#pragma once

#include <phoenix/phoenix.hpp>

#include "PremultImage.hpp"


/*
 * Slightly more powerful canvas class, intended to be used for something more than
 *  basic image display, but less than a full sdl or opengl app (like, e.g., a simple
 *  map editor).
 *
 * Uses a PremultImage to store data to-be rendered. Make sure to call autoresize()
 *  whenever the Window changes size, or the default Canvas rendering engine (black screen)
 *  will appear instead of your buffer.
 */
struct CanvasExt : public phoenix::Canvas {
	CanvasExt();

	///The PremultImage may be larger than, smaller than, or the same size as
	/// the CanvasExt. The offset specifies where to draw it in relation to the
	/// top-left corner of the CanvasExt (it may be negative). The width/height of
	/// the offset are ignored. Defaults to {0,0}
	///NOTE: offset (as a Geometry) can't currently be negative. But the logic
	///      _would_ work; maybe we can use a std::pair<int>
	///The application is expected to request the PremultImage and then modify it.
	PremultImage& getBufferedImage();
	void setImageOffset(const phoenix::Geometry& offset);

	//Update!
	virtual void update();


private:
	void updateCanvasBuffer();

	PremultImage image_;
	phoenix::Geometry offset_;

	//To avoid repainting on a move
	//phoenix::Geometry cached_size_;
};








