#pragma once

#include <phoenix.hpp>
#include <nall/png.hpp>


/*
 * Canvas subclass which is bound directly to an Image, and displays that image at 100% scale
 */
struct ImageIcon : public phoenix::Canvas {
	ImageIcon();
	void setImage(const nall::png& image);
	virtual void update();
	virtual phoenix::Geometry minimumGeometry();
	virtual void setGeometry(const phoenix::Geometry &geometry);

private:
	const nall::png* img;
};







