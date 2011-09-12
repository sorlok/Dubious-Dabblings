//Copyright 2011 by Seth N. Hetu
//Shared under the terms of the ISC license
//Original sample code by Kawa

#include "thumbnail-layout.hpp"

using namespace phoenix;



ThumbnailLayout::ThumbnailLayout()
{
	//Defaults
	state.enabled = true;
	state.visible = true;
	state.skipGeomUpdate = false;
}


ThumbnailLayout::~ThumbnailLayout()
{
	//Remove all children on exit
	setSkipGeomUpdates(true);
	removeAll();
}

void ThumbnailLayout::removeAll()
{
	while(children.size()>0) {
		remove(*children[0].sizable);
	}
}


ThumbnailLayout::Children* ThumbnailLayout::FindChild(nall::linear_vector<Children>& children, const Sizable& find)
{
	foreach(child, children) {
		if(child.sizable == &find) {
			return &child;
		}
	}
	return nullptr;
}


void ThumbnailLayout::append(Sizable &sizable) {
	if (FindChild(children, sizable)) {
		return;
	} else {
		children.append({ &sizable, 0, 0 });
	}
	Layout::append(sizable);
	if(window()) {
		window()->synchronize();
	}
}


void ThumbnailLayout::remove(Sizable& sizable)
{
	//Rmove this item if it exists.
	for(unsigned n=0; n<children.size(); n++) {
		if(children[n].sizable == &sizable) {
			children.remove(n);
			Layout::remove(sizable);
			break;
		}
	}
}


void ThumbnailLayout::synchronize()
{
	if (children.size()==0) {
		return;
	}

	//Ensure all sizables have been appended to the layout.
	state.skipGeomUpdate = true;
	size_t i = 0;
	foreach(child, children) {
		if (i+1==children.size()) {
			state.skipGeomUpdate = false;
		}
		Layout::append(*child.sizable);
		i++;
	}
}


void ThumbnailLayout::setEnabled(bool enabled)
{
	state.enabled = enabled;
	foreach(child, children) {
		child.sizable->setEnabled(dynamic_cast<Widget*>(child.sizable) ? child.sizable->enabled() : enabled);
	}
}

void ThumbnailLayout::setVisible(bool visible)
{
	state.visible = visible;
	foreach(child, children) {
		child.sizable->setVisible(dynamic_cast<Widget*>(child.sizable) ? child.sizable->visible() : visible);
	}
}

bool ThumbnailLayout::enabled()
{
	if(layout()) {
		return state.enabled && layout()->enabled();
	}
	return state.enabled;
}

bool ThumbnailLayout::visible()
{
	if(layout()) {
		return state.visible && layout()->visible();
	}
	return state.visible;
}


Geometry ThumbnailLayout::minimumGeometry()
{
	//By its nature, a ThumbnailLayout take up the entire width/height of the parent.
	// At least, I can't think of a reasonable use case for taking less than the maximum.
	return state.lastKnownSize;
}


#include <iostream>

void ThumbnailLayout::setGeometry(const Geometry& containerGeometry)
{
	//When closing the app (or if the user tells us to) we don't need to redo any internal geometry
	// calculations. Be careful not to leave this flag on before going back to the message loop.
	if (state.skipGeomUpdate) {
		return;
	}

	std::cout <<"--------------------\n";

	//Save containerGeometry
	state.lastKnownSize = containerGeometry;

	//First, save each child's minimum width, and get the maximum width/height of all children
	Geometry referenceGeom = {0, 0, 0, 0};
	foreach(child, children) {
		child.width = child.sizable->minimumGeometry().width;
		child.height = child.sizable->minimumGeometry().height;

		if (child.width>referenceGeom.width) {
			referenceGeom.width = child.width;
		}
		if (child.height>referenceGeom.height) {
			referenceGeom.height = child.height;
		}
	}

	std::cout <<"Max: " <<referenceGeom.width <<"," <<referenceGeom.height <<"\n";

	//Apply layout rules for each child  individually.
	foreach(child, children) {
		//Reset?
		if (referenceGeom.x+referenceGeom.width > containerGeometry.width) {
			referenceGeom.x = 0;
			referenceGeom.y += referenceGeom.height; //Might scroll offscreen
		}

		//std::cout <<"Ref geom: " <<referenceGeom.x <<"," <<referenceGeom.y <<" : " <<referenceGeom.width <<"," <<referenceGeom.height <<"\n";

		//Pivot
		double alignment = 0.5; //This can be customized later.
		int pivotW = (referenceGeom.width - child.width) * alignment;
		int pivotH = (referenceGeom.height - child.height) * alignment;

		//Set
	    Geometry childGeometry = {referenceGeom.x+pivotW, referenceGeom.y+pivotH, child.width, child.height};
	    child.sizable->setGeometry(childGeometry);

	    //Increment
	    referenceGeom.x += referenceGeom.width;
	}

}

