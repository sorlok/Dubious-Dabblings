//Copyright 2011 by Seth N. Hetu
//Shared under the terms of the ISC license
#include "anchor-layout.hpp"

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


void AttachLayout::append(phoenix::Sizable &sizable, const Attachment& leftL, const Attachment& topL, const Attachment& rightL, const Attachment& bottomL)
{
	//First, a quick check: If ONE of any 2 diametrically opposed points is "special", the other should copy it.
	Attachment left = (rightL.type==Attachment::TYPE::SPECIAL_PERCENT||rightL.type==Attachment::TYPE::SPECIAL_ATTACHED) ? rightL : leftL;
	Attachment right = (leftL.type==Attachment::TYPE::SPECIAL_PERCENT||leftL.type==Attachment::TYPE::SPECIAL_ATTACHED) ? leftL : rightL;
	Attachment top = (bottomL.type==Attachment::TYPE::SPECIAL_PERCENT||bottomL.type==Attachment::TYPE::SPECIAL_ATTACHED) ? bottomL : topL;
	Attachment bottom = (topL.type==Attachment::TYPE::SPECIAL_PERCENT||topL.type==Attachment::TYPE::SPECIAL_ATTACHED) ? topL : bottomL;

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


//Compute a single component. "Least" and "Greatest" are essentially "left" and "right".
// Save into "resOrigin", "resMagnitude", which are basically "x" and "width".
void AttachLayout::ComputeComponent(Attachment& least, Attachment& greatest, int& resOrigin, unsigned int& resMagnitude, LayoutData args)
{
	resOrigin = AttachLayout::Get(least, greatest, args.setSign(-1).setAnchor(Attachment::ANCHOR::RIGHT));
	resMagnitude = (unsigned int)(AttachLayout::Get(greatest, least, args.flipSign().flipAnchor())-resOrigin);
}


int AttachLayout::Get(Attachment& item, Attachment& diam, LayoutData args)
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
		item.res = GetUnbound(item, diam, args);
	} else if (item.type==Attachment::TYPE::PERCENT) {
		item.res = GetPercent(item, args);
	} else if (item.type==Attachment::TYPE::ATTACHED) {
		item.res = GetAttached(item, diam, args);
	} else if (item.type==Attachment::TYPE::SPECIAL_PERCENT && item.special==Attachment::SPECIAL::CENTERED) {
		item.res = GetCenteredPercent(item, diam, args);
	} else if (item.type==Attachment::TYPE::SPECIAL_ATTACHED && item.special==Attachment::SPECIAL::CENTERED) {
		item.res = GetCenteredAttached(item, diam, args);
	} else {
		//ERROR (in case we add more types later).
		std::cout <<"ERROR: Unknown type\n";
		return 0;
	}
	item.done = true;
	return item.res;
}



int AttachLayout::GetUnbound(Attachment& item, Attachment& diam, LayoutData args)
{
	//This simply depends on the item diametrically opposed to this one, plus a bit of sign manipulation
	return AttachLayout::Get(diam, item, args.flipAnchor()) + args.sign*args.itemMin;
}


int AttachLayout::GetPercent(Attachment& item, LayoutData args)
{
	//Simple; just remember to include the item's offset.
	return item.percent*args.containerMax + args.offset + item.offset;
}


int AttachLayout::GetCenteredPercent(Attachment& item, Attachment& diam, LayoutData args)
{
	//First, get the centered position and width, then just expand outwards. Remember to set the diametric point to "done".
	int point = item.percent*args.containerMax + args.offset;
	int width = item.offset>0 ? item.offset : args.itemMin;

	diam.res = point + width/2;
	diam.done = true;
	return point - width/2;
}


int AttachLayout::GetCenteredAttached(Attachment& item, Attachment& diam, LayoutData args)
{
	//First, retrieve the component
	Children* other = nullptr;
	foreach(child, args.children)  {
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

	//This is only mildly more complex than the normal "attached" case.
	//NOTE: The "default" for items centered this way is ANCHOR::CENTERED; otherwise, it behaves just like GetAttached()
	//TODO: Combine code; modularity could be much better.
	Attachment::ANCHOR anch = item.anchor==Attachment::ANCHOR::DEFAULT ? Attachment::ANCHOR::CENTER : item.anchor;
	int baseVal = 0;
	if (anch==Attachment::ANCHOR::CENTER) {
		//The center layout requires both points to be calculatable. Otherwise, it't not very different.
		Attachment* near = args.isHoriz ? &other->left : &other->top;
		Attachment* far = args.isHoriz ? &other->right : &other->bottom;
		baseVal = AttachLayout::Get(*near, diam, args);
		baseVal = (AttachLayout::Get(*far, diam, args)-baseVal)/2 + baseVal;
	} else {
		//For left/right layouts, there's only one point to check.
		Attachment* base = nullptr;
		if (anch==Attachment::ANCHOR::LEFT) {
			base = args.isHoriz ? &other->left : &other->top;
		} else if (anch==Attachment::ANCHOR::RIGHT) {
			base = args.isHoriz ? &other->right : &other->bottom;
		} else {
			//Shouldn't fail, but just to be safe...
			std::cout <<"ERROR: unexpected anchor value.\n";
			return 0;
		}

		baseVal = AttachLayout::Get(*base, diam, args);
	}

	//Now center it as expected, setting diam's value too.
	//NOTE: This code is nearly the same as GetCenteredPercent(), except for how "Point" is calculated. Surely we can combine them....
	//NOTE: There might be an error with this code if we calculate the Right-anchor first. Need a test case.... or we can just be more 
	//      responsible with how we declare Centered layouts. 
	int point = baseVal + item.offset;
	int width = item.offset>0 ? item.offset : args.itemMin;

	diam.res = point + width/2;
	diam.done = true;
	return point - width/2;
}


//Only slightly more complex. Most of the math (figuring out the sign & default anchor) has already been done for us.
int AttachLayout::GetAttached(Attachment& item, Attachment& diam, LayoutData args)
{
	//First, retrieve the component
	Children* other = nullptr;
	foreach(child, args.children)  {
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

	//Handle anchors; get the result
	//TODO: Cleanup isHoriz if possible
	Attachment::ANCHOR anch = item.anchor==Attachment::ANCHOR::DEFAULT ? args.defaultAnch : item.anchor;
	int baseVal = 0;
	if (anch==Attachment::ANCHOR::CENTER) {
		//The center layout requires both points to be calculatable. Otherwise, it't not very different.
		Attachment* near = args.isHoriz ? &other->left : &other->top;
		Attachment* far = args.isHoriz ? &other->right : &other->bottom;
		baseVal = AttachLayout::Get(*near, diam, args);
		baseVal = (AttachLayout::Get(*far, diam, args)-baseVal)/2 + baseVal;
	} else {
		//For left/right layouts, there's only one point to check.
		Attachment* base = nullptr;
		if (anch==Attachment::ANCHOR::LEFT) {
			base = args.isHoriz ? &other->left : &other->top;
		} else if (anch==Attachment::ANCHOR::RIGHT) {
			base = args.isHoriz ? &other->right : &other->bottom;
		} else {
			//Shouldn't fail, but just to be safe...
			std::cout <<"ERROR: unexpected anchor value.\n";
			return 0;
		}

		baseVal = AttachLayout::Get(*base, diam, args);
	}

	//Now add the offset
	return baseVal + item.offset;
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

	//First, reset all children.
	foreach(child, children) {
		child.left.reset();
		child.right.reset();
		child.top.reset();
		child.bottom.reset();
	}

	//Apply alyout rules for each child  individually.
	foreach(child, children) {
		//We compute each component in pairs..
		Geometry res;
		AttachLayout::ComputeComponent(child.left, child.right, res.x, res.width, {containerGeometry.x, containerGeometry.width, child.sizable->minimumGeometry().width, true, children});
		AttachLayout::ComputeComponent(child.top, child.bottom, res.y, res.height, {containerGeometry.y, containerGeometry.height, child.sizable->minimumGeometry().height, false, children});
		child.sizable->setGeometry(res);
	}

}












