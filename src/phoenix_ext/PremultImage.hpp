#pragma once

#include <phoenix/phoenix.hpp>

#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <fstream>


//Allows Rotations
enum class ROTATION {
	NONE,
	FLIP_HORIZ,
	FLIP_VERT,
	CW_90_DEG
};


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

	//Note: We need a better PNG class.
	static uint32_t* LoadPNGFile(const std::string& path, unsigned int& imgWidth, unsigned int& imgHeigh);

	//For loading from a file
	void initFromImage(const std::string& path);
	void resetSize(const phoenix::Geometry& size, uint32_t* newBuffer);

	//For making it ourselves
	void resetSize(const phoenix::Geometry& size, std::vector<uint8_t> fill=std::vector<uint8_t>({0x00, 0x00, 0x00}));
	const phoenix::Geometry& getSize() const;

	//For basing it off an existing buffer
	void initFromBuffer(const PremultImage& src, ROTATION modify=ROTATION::NONE);

	///Simple drawing. (Really, you should use images, or write to the buffer directly)
	///The color can be {R,G,B}, or {A,R,G,B}; the premultiplication will be done automatically.
	///The color can _also_ be {G} or {A, G} for grayscale components. Fun!
	void fillRect(const phoenix::Geometry& rectangle, std::vector<uint8_t> color);
	void paintImage(const PremultImage& img, const phoenix::Geometry& position);


	//An undo buffer is something of a hack for displaying buttons on a Canvas
	//  It is deleted every time resetSize() is called.
	void createUndoBuffer();


	///Useful if you want to access array elements directly.
	///Remember! These will be premultiplied values!
	uint32_t& operator[] (size_t x);
	uint32_t* getPixels();
	bool isEmpty();


	///Helper: convert A,R,G,B to premultiplied RGB
	static uint32_t Premultiply(uint8_t a, uint8_t r, uint8_t g, uint8_t b);
	static uint32_t Premultiply(uint32_t argb);

	//Helper: combine two pixel values into one:
	static void Combine(uint32_t& base, uint32_t brush);

	//Helper: convert a vector (likely an initializer list) to ARGB
	static uint32_t DecodeColor(const std::vector<uint8_t>& color);


private:
	bool resetSize_(const phoenix::Geometry& size);

	uint32_t* buffer_;
	uint32_t* undoBuffer_;
	phoenix::Geometry size_;

};




