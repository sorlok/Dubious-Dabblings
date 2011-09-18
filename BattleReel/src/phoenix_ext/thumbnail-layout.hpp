//Copyright 2011 by Seth N. Hetu
//Shared under the terms of the ISC license
//Original sample code by Kawa

#pragma once

#include <phoenix.hpp>
#include "scapegoat.hpp"


class ThumbnailLayout : public phoenix::Layout {
public:
	//Required functionality: Layout
	void append(phoenix::Sizable& sizable);
	void remove(phoenix::Sizable& sizable);
	void synchronize();

	//Required functionality: Sizable
	virtual void setEnabled(bool enabled = true);
	virtual void setVisible(bool visible = true);
	virtual bool enabled();
	virtual bool visible();
	virtual phoenix::Geometry minimumGeometry();
	virtual void setGeometry(const phoenix::Geometry &geometry);

	//Workaround
	void synchHack(phoenix::Sizable* sizable);

	//Mildly useful
	void removeAll();
	void setSkipGeomUpdates(bool val) { /*state.skipGeomUpdate = val;*/ } //TODO: Enable later

	//Constructor/destructor
	ThumbnailLayout();
	virtual ~ThumbnailLayout();

private:
	struct Children {
		Sizable *sizable;
		unsigned int width;
		unsigned int height;
	};
	lightweight_map<Sizable*, Children> children;

	//static Children* FindChild(nall::linear_vector<Children>& children, const Sizable& find);

	struct State {
		bool enabled;
		bool visible;
		phoenix::Geometry lastKnownSize;
		bool skipGeomUpdate;
	} state;
};
