#pragma once

#include <phoenix.hpp>

#include <vector>
#include <stdexcept>


/**
 * Simple buffer of pixel data (with an additional width/height) that stores pre-multiplied
 *  pixel values. Implicitly supports alpha transparency, fairly fast to composite multiple
 *  images, and provides a useful platform-agnostic buffer.
 *
 * Contains some helper functions to convert ARGB to premultiplied alpha, since I assume most
 *  programmers are more familiar with the former.
 *
 * Based loosely on the Image and Font classes from Pulp Core:
 *  http://www.interactivepulp.com/pulpcore/
 * ...which is licensed under the terms of the New BSD License (and an excellent project).
 */
class PremultImage {
public:
	///By default, makes an image of zero size
	PremultImage();
	~PremultImage();

	void resetSize(const phoenix::Geometry& size);
	const phoenix::Geometry& getSize() const;

	///Simple drawing. (Really, you should use images, or write to the buffer directly)
	///The color can be {R,G,B}, or {A,R,G,B}; the premultiplication will be done automatically.
	///The color can _also_ be {G} or {A, G} for grayscale components. Fun!
	void fillRect(const phoenix::Geometry& rectangle, std::vector<uint8_t> color);


	///Useful if you want to access array elements directly.
	///Remember! These will be premultiplied values!
	uint32_t& operator[] (size_t x);


	///Helper: convert A,R,G,B to premultiplied RGB
	static uint32_t Premultiply(uint8_t a, uint8_t r, uint8_t g, uint8_t b);

	//Helper: combine two pixel values into one:
	static void Combine(uint32_t& base, uint32_t brush);


private:
	uint32_t* buffer_;
	phoenix::Geometry size_;

};




