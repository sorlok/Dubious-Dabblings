#include "attach-layout.hpp"

#include <iostream> //Debugging only; comment out if you're not using cout()

using namespace phoenix;


AttachLayout::AttachLayout()
{
	//Defaults
	state.enabled = true;
	state.visible = true;
	skipGeomUpdate = false;
}


AttachLayout::~AttachLayout()
{
	//Remove all children on exit
	setSkipGeomUpdates(true);
	while(children.size()>0) {
		remove(*children[0].sizable);
	}
}


void AttachLayout::append(Sizable &sizable) {
	foreach(child, children) {
		if(child.sizable == &sizable) {
			return;
		}
	}
	Layout::append(sizable);
	if(window()) {
		window()->synchronize();
	}
}


void AttachLayout::append(phoenix::Sizable &sizable, const Attachment& left, const Attachment& top, const Attachment& right, const Attachment& bottom)
{
	//If this child already exists, update its attachment data
	foreach(child, children)  {
		if(child.sizable == &sizable) {
			child.left = left;
			child.top = top;
			child.right = right;
			child.bottom = bottom;
			return;
		}
	}
	//Else, add a new item
	children.append({ &sizable, left, top, right, bottom });
	synchronize();
}


void AttachLayout::remove(Sizable& sizable)
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


void AttachLayout::synchronize()
{
	//Ensure all sizables have been appended to the layout.
	foreach(child, children) {
		Layout::append(*child.sizable);
	}
}


void AttachLayout::setEnabled(bool enabled)
{
	state.enabled = enabled;
	foreach(child, children) {
		child.sizable->setEnabled(dynamic_cast<Widget*>(child.sizable) ? child.sizable->enabled() : enabled);
	}
}

void AttachLayout::setVisible(bool visible)
{
	state.visible = visible;
	foreach(child, children) {
		child.sizable->setVisible(dynamic_cast<Widget*>(child.sizable) ? child.sizable->visible() : visible);
	}
}

bool AttachLayout::enabled()
{
	if(layout()) {
		return state.enabled && layout()->enabled();
	}
	return state.enabled;
}

bool AttachLayout::visible()
{
	if(layout()) {
		return state.visible && layout()->visible();
	}
	return state.visible;
}


Geometry AttachLayout::minimumGeometry()
{
	//By its nature, AttachLayouts take up the entire width/height of the parent.
	// At least, I can't think of a reasonable use case for taking less than the maximum.
	return lastKnownSize;
}


//Compute a single component.
// NOTE: I am using "left" and "right" rather than generic names, since I find it easier to reason about these terms.
void AttachLayout::ComputeComponent(Attachment& left, Attachment& right, int offsetX, unsigned int maxWidth, int& resX, unsigned int& resWidth)
{
	resX = (int)(left.percent*maxWidth + offsetX + left.offset);
	resWidth = (int)(right.percent*maxWidth + offsetX + right.offset) - resX;
}




void AttachLayout::setGeometry(const Geometry& containerGeometry)
{
	//When closing the app (or if the user tells us to) we don't need to redo any internal geometry
	// calculations. Be careful not to leave this flag on before going back to the message loop.
	if (skipGeomUpdate) {
		return;
	}

	//Save containerGeometry
	lastKnownSize = containerGeometry;

	//std::cout <<"SET GEOM: " <<containerGeometry.width <<"x" <<containerGeometry.height <<"\n";
	//First, resize all children who have set their width/height to MinimumSize
	/*auto children = this->children;
	foreach(child, children) {
		if(child.width  == MinimumSize) child.width  = child.sizable->minimumGeometry().width;
		if(child.height == MinimumSize) child.height = child.sizable->minimumGeometry().height;
	}*/


	//Apply percentage-based layout rules
	foreach(child, children) {
		//We compute each component in pairs..
		Geometry res;
		AttachLayout::ComputeComponent(child.left, child.right, containerGeometry.x, containerGeometry.width, res.x, res.width);
		AttachLayout::ComputeComponent(child.top, child.bottom, containerGeometry.y, containerGeometry.height, res.y, res.height);
		child.sizable->setGeometry(res);
	}



	//Apply margins.
	/*Geometry geometry = containerGeometry;
	geometry.x      += state.margin;
	geometry.y      += state.margin;
	geometry.width  -= state.margin * 2;
	geometry.height -= state.margin * 2;*/


	//Determine how many children have set the "maximum width" counter.
	/*unsigned minimumWidth = 0, maximumWidthCounter = 0;
	foreach(child, children) {
		if(child.width == MaximumSize) {
			maximumWidthCounter++;
		}
		if(child.width != MaximumSize) {
			minimumWidth += child.width;
		}
		minimumWidth += child.spacing;
	}*/

	//Apply the "maximum width" to children, dividing if it's been set more than once.
	/*foreach(child, children) {
		if(child.width  == MaximumSize) child.width  = (geometry.width - minimumWidth) / maximumWidthCounter;
		if(child.height == MaximumSize) child.height = geometry.height;
	}*/

	//Get the maximum height of any child component.
	/*unsigned maximumHeight = 0;
	foreach(child, children) {
		maximumHeight = max(maximumHeight, child.height);
	}*/

	//Apply children from left to right. Use pivot+alignment for items which are not as tall as the
	//   maximum component. Update local copy of geometry as we go along.
	/*foreach(child, children) {
		unsigned pivot = (maximumHeight - child.height) * state.alignment;
		Geometry childGeometry = { geometry.x, geometry.y + pivot, child.width, child.height };
		child.sizable->setGeometry(childGeometry);

		geometry.x += child.width + child.spacing;
		geometry.width -= child.width + child.spacing;
	}*/
}












