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
void AttachLayout::ComputeComponent(Attachment& left, Attachment& right, int offsetX, unsigned int maxWidth, int& resX, unsigned int& resWidth, unsigned int minWidth,bool isHoriz, nall::linear_vector<Children>& children)
{
	resX = AttachLayout::Get(left, right, offsetX, maxWidth, minWidth, -1, isHoriz, Attachment::ANCHOR::RIGHT, children);
	resWidth = (unsigned int)(AttachLayout::Get(right, left, offsetX, maxWidth, minWidth, 1, isHoriz, Attachment::ANCHOR::LEFT, children)-resX);
}


int AttachLayout::Get(Attachment& item, Attachment& diam, int offset, unsigned int maximum, unsigned int minimum, int sign, bool isHoriz, Attachment::ANCHOR defaultAnch, nall::linear_vector<Children>& children)
{
	//Simple cases
	if (item.done) {
		return item.res;
	}
	if (item.waiting) {
		//ERROR: default to returning 0 (optional approach to problem-solving: throw something)
		std::cout <<"ERROR: Still waiting on item.\n";
		return 0;
	}

	//Have to figure it out
	item.waiting = true;
	if (item.type==Attachment::TYPE::UNBOUND) {
		item.res = GetUnbound(item, diam, offset, maximum, minimum, sign, isHoriz, defaultAnch, children);
	} else if (item.type==Attachment::TYPE::PERCENT) {
		item.res = GetPercent(item, offset, maximum);
	} else if (item.type==Attachment::TYPE::ATTACHED) {
		item.res = GetAttached(item, diam, offset, maximum, minimum, sign, isHoriz, defaultAnch, children);
	} else {
		//ERROR (in case we add more types later).
		std::cout <<"ERROR: Unknown type\n";
		return 0;
	}
	item.done = true;
	return item.res;
}



int AttachLayout::GetUnbound(Attachment& item, Attachment& diam, int offset, unsigned int maximum, unsigned int minimum, int sign, bool isHoriz, Attachment::ANCHOR defaultAnch, nall::linear_vector<Children>& children)
{
	//This simply depends on the item diametrically opposed to this one, plus a bit of sign manipulation
	std::cout <<"   Unbound: " <<AttachLayout::Get(diam, item, offset, maximum, minimum, -sign, isHoriz, defaultAnch, children) <<" + " <<sign << "*" <<minimum <<"\n";
	return AttachLayout::Get(diam, item, offset, maximum, minimum, -sign, isHoriz, defaultAnch, children) + sign*minimum;
}


int AttachLayout::GetPercent(Attachment& item, int offset, unsigned int maximum)
{
	//Simple; just remember to include the item's offset.
	std::cout <<"   Percent: " <<item.percent <<" of " <<maximum <<" and offset: " <<offset <<"," <<item.offset <<"\n";
	return item.percent*maximum + offset + item.offset;
}


//Only slightly more complex. Most of the math (figuring out the sign & default anchor) has already been done for us.
int AttachLayout::GetAttached(Attachment& item, Attachment& diam, int offset, unsigned int maximum, unsigned int minimum, int sign, bool isHoriz, Attachment::ANCHOR defaultAnch, nall::linear_vector<Children>& children)
{
	//First, retrieve the component
	Children* other = nullptr;
	foreach(child, children)  {
		if(child.sizable == item.refItem) {
			other = &child;
			break;
		}
	}

	//Did we retrieve anything? (this should also check for a null item.refItem)
	if (!other) {
		//TODO: We _could_ probably allow attaching to components in a fixed/horizontal/vertical layout
		//      manager. But I think the complexity won't gain us much, and I'd rather get this working first.
		std::cout <<"ERROR: attached item is not managed by this layout manager.\n";
		return 0;
	}

	//Now, it's just a matter of retrieving the value we're looking for...
	//TODO: This is the only place that "isHoriz" is used... I'd like to try to remove it if possible since it feels like a hack.
	//      Then again, most of this code feels like a hack in its present state. (Better TODO: clean up code!)
	Attachment* base = nullptr;
	Attachment::ANCHOR anch = item.anchor==Attachment::ANCHOR::DEFAULT ? defaultAnch : item.anchor;
	if (anch==Attachment::ANCHOR::LEFT) {
		base = isHoriz ? &other->left : &other->top;
	} else if (anch==Attachment::ANCHOR::RIGHT) {
		base = isHoriz ? &other->right : &other->bottom;
	} else {
		//Leaving this check in here for when we add ANCHOR::CENTER
		std::cout <<"ERROR: unexpected anchor value.\n";
		return 0;
	}

	//...and adding the offset
	return AttachLayout::Get(*base, diam, offset, maximum, minimum, sign, isHoriz, defaultAnch, children) + item.offset;
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

	//First, reset all
	foreach(child, children) {
		child.left.reset();
		child.right.reset();
		child.top.reset();
		child.bottom.reset();
	}


	//Apply percentage-based layout rules
	std::cout <<"CHILD LAYOUT\n";
	foreach(child, children) {
		//First, compute the minimum width/height, in case they're needed.
		//  (We can cheat a bit on space by stuffing these values into the result temporarily)
		Geometry res = {0, 0, child.sizable->minimumGeometry().width, child.sizable->minimumGeometry().height};

		std::cout <<"Child geometry: " <<child.sizable <<"\n";

		//We compute each component in pairs..
		AttachLayout::ComputeComponent(child.left, child.right, containerGeometry.x, containerGeometry.width, res.x, res.width, res.width, true, children);
		AttachLayout::ComputeComponent(child.top, child.bottom, containerGeometry.y, containerGeometry.height, res.y, res.height, res.height, false, children);
		child.sizable->setGeometry(res);

		std::cout <<"   Res: " <<res.x <<"," <<res.y <<" -> " <<res.width <<"," <<res.height <<"\n";
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












