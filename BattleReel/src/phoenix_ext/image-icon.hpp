#pragma once

#include <phoenix.hpp>
#include <nall/png.hpp>


/*
 * Canvas subclass which is bound directly to an Image, and displays that image at 100% scale
 */
struct ImageIcon : public phoenix::Canvas {
	ImageIcon();
	void setImage(const nall::string& filename);
	virtual void update();

private:
	nall::png img;
};







