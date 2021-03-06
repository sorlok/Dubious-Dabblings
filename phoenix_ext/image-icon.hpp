//Copyright 2011 by Seth N. Hetu
//Shared under the terms of the ISC license

#pragma once

#include <phoenix.hpp>
#include <nall/png.hpp>


/*
 * Canvas subclass which is bound directly to an Image, and displays that image at 100% scale
 */
struct ImageIcon : public phoenix::Canvas {
	ImageIcon();
	void setBkgrd(unsigned int argb) { bkgrd = argb; }
	void setImage(const nall::png& image);
	const nall::png& getImage() { return *img; }
	virtual void update();
	virtual phoenix::Geometry minimumGeometry();
	virtual void setGeometry(const phoenix::Geometry &geometry);

private:
	const nall::png* img;
	unsigned int bkgrd;
};







