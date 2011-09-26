//Copyright 2011 by Seth N. Hetu
//Shared under the terms of the ISC license
#include "anchor-layout.hpp"

#ifdef ANCHOR_LAYOUT_ERRORS_ON
#include <iostream>
#endif

using namespace phoenix;
typedef AnchorPoint::Anchor Anchor;


//I'm not sure how C++ handles floating-point math.
//If you are confident in f1==f2 being accurate for 0.0==0.0 and 1.0==1.0 (which it should be!) then
// you can comment out the following and create a new version of FloatEquals which just checks basic equality
#include <cmath>
#include <cfloat>
namespace {
bool FloatEquals(float f1, float f2)  {
	return fabs(f1-f2) <= FLT_EPSILON * fmax(fmax(1.0F, fabs(f1)), fabs(f2));
}
}

//Here's the alternative, for those who prefer it.
/*namespace {
bool FloatEquals(float f1, float f2)  {
	return f1 == f2;
}
}*/


AnchorLayout::AnchorLayout() : children(0.65)
{
	//Defaults
	state.enabled = true;
	state.visible = true;
	state.margin = 0;
	state.skipGeomUpdate = false;
	state.skippedAnUpdate = false;
}


AnchorLayout::~AnchorLayout()
{
	//Remove all children on exit
	ScopedLayoutLock lock(this);
	while(children.size()>0) {
		remove(*children.rootKey());
		//remove(*children[0].sizable);
	}
}



void AnchorLayout::append(Sizable &sizable) {
	Children res;
	if (children.find(&sizable, res)) {
		return;
	}
	Layout::append(sizable);
	if(window()) {
		window()->synchronize();
	}
}


void AnchorLayout::append(phoenix::Sizable &sizable, const Axis& horizontal, const Axis& vertical)
{
	typedef AnchorPoint::Type Type;

	std::cout <<"Append\n";
	ScopedLayoutLock lock(this);

	//If this child already exists, update its attachment data
	Children res;
	if (children.find(&sizable, res)) {
		res.horiz = horizontal;
		res.vert = vertical;
		setGeometry(state.lastKnownSize); //This might be better as "synchronize"
		return;
	}
	//Else, add a new item
	children.insert(&sizable, { &sizable, horizontal, vertical });
	synchronize();
}


void AnchorLayout::remove(Sizable& sizable)
{
	//TEMP: Just a bit of a hack to handle deletion
	setSkipGeomUpdates(true);

	//Rmove this item if it exists.
	Children res;
	if (children.find(&sizable, res)) {
	//for(unsigned n=0; n<children.size(); n++) {
	//	if(children[n].sizable == &sizable) {
			children.remove(&sizable);
			Layout::remove(sizable);
	//		break;
	//	}
	}
}


void AnchorLayout::synchHack(Sizable* sizable)
{
	//This is required due to a bug in const-casting.
	Layout::append(*sizable);
}
void AnchorLayout::synchronize()
{
	std::cout <<"Synchronize\n";
	ScopedLayoutLock lock(this);

	//Ensure all sizables have been appended to the layout.
	children.for_each([this](Sizable* key, Children& child) {
		synchHack(key);
	});
	/*foreach(child, children) {
		Layout::append(*child.sizable);
	}*/
}


void AnchorLayout::setEnabled(bool enabled)
{
	state.enabled = enabled;
	children.for_each([this](Sizable* key, Children& child) {
		child.sizable->setEnabled(dynamic_cast<Widget*>(child.sizable) ? child.sizable->enabled() : state.enabled);
	});
/*	foreach(child, children) {
		child.sizable->setEnabled(dynamic_cast<Widget*>(child.sizable) ? child.sizable->enabled() : enabled);
	}*/
}

void AnchorLayout::setVisible(bool visible)
{
	state.visible = visible;
	children.for_each([this](Sizable* key, Children& child) {
	//foreach(child, children) {
		child.sizable->setVisible(dynamic_cast<Widget*>(child.sizable) ? child.sizable->visible() : state.visible);
	});
}

bool AnchorLayout::enabled()
{
	if(layout()) {
		return state.enabled && layout()->enabled();
	}
	return state.enabled;
}

bool AnchorLayout::visible()
{
	if(layout()) {
		return state.visible && layout()->visible();
	}
	return state.visible;
}


Geometry AnchorLayout::minimumGeometry()
{
	//By its nature, AttachLayouts take up the entire width/height of the parent.
	// At least, I can't think of a reasonable use case for taking less than the maximum.
	return state.lastKnownSize;
}



void AnchorLayout::setGeometry(const Geometry& containerGeometry)
{
	//Save containerGeometry
	state.lastKnownSize = containerGeometry;

	//When closing the app (or if the user tells us to) we don't need to redo any internal geometry
	// calculations. Be careful not to leave this flag on before going back to the message loop.
	if (state.skipGeomUpdate) {
		state.skippedAnUpdate = true;
		return;
	}

	std::cout <<"setGeometry()\n";

	//First, reset all children.
	//foreach(child, children) {
	children.for_each([this](Sizable* key, Children& child) {
		child.horiz.reset();
		child.vert.reset();
	});

	//Apply alyout rules for each child  individually.
	children.for_each([this, &containerGeometry](Sizable* key, Children& child) {
	//foreach(child, children) {
		//We compute each component in pairs..
		Geometry res;
		AnchorLayout::ComputeComponent(child.horiz, res.x, res.width, {containerGeometry.x, containerGeometry.width, state.margin, true, children}, *child.sizable);
		AnchorLayout::ComputeComponent(child.vert, res.y, res.height, {containerGeometry.y, containerGeometry.height, state.margin, false, children}, *child.sizable);
		child.sizable->setGeometry(res);
	});

}




//Compute a single component. "Least" and "Greatest" are essentially "left" and "right".
// Save into "resOrigin", "resMagnitude", which are basically "x" and "width".
void AnchorLayout::ComputeComponent(Axis& axis, int& resOrigin, unsigned int& resMagnitude, LayoutData args, phoenix::Sizable& comp)
{
	resOrigin = AnchorLayout::Get(axis, args, true, comp);
	resMagnitude = (unsigned int)(AnchorLayout::Get(axis, args, false, comp) - resOrigin);
}


int AnchorLayout::Get(Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp)
{
	typedef AnchorPoint::Type Type;
	typedef AnchorPoint::State State;

	//Make sure we check the opposing point if we the axis is computed in one go.
	AnchorPoint& item = ltr ? axis.least_ : axis.greatest_;
	AnchorPoint* other = nullptr;
	if (axis.isFullAxis) {
		other = ltr ? &axis.greatest_ : &axis.least_;
	}

	//Simple cases
	if (item.state==State::Done) {
		return item.res;
	}
	if (item.state==State::Waiting || (other && other->state==State::Waiting)) {
		//ERROR: All errors position the component at 0 X/Y.
#ifdef ANCHOR_LAYOUT_ERRORS_ON
		std::cout <<"Error: Already waiting on component.\n";
#endif
		return 0;
	}
	item.state = State::Waiting;
	if (other) {
		other->state = State::Waiting;
	}

	if (axis.isFullAxis) {
		//Some items require figuring out both points at the same time.
		nall::linear_vector<int> res = GetBoth(axis, args, ltr, comp);
		if (ltr) {
			item.res = res[0];
			other->res = res[1];
		} else {
			item.res = res[1];
			other->res = res[0];
		}
	} else {
		//Others can be computed independently.
		if (item.type==Type::Unbound) {
			item.res = GetUnbound(axis, args, ltr, comp);
		} else if (item.type==Type::Percent) {
			item.res = GetPercent(axis, args, ltr, comp);
		} else if (item.type==Type::Attached) {
			item.res = GetAttached(axis, args, ltr, comp);
		} else {
#ifdef ANCHOR_LAYOUT_ERRORS_ON
			std::cout <<"Error: Unknown independent type.\n";
#endif
			return 0;
		}
	}

	//Mark done, return result
	item.state = State::Done;
	if (other) {
		other->state = State::Done;
	}
	return item.res;
}


nall::linear_vector<int> AnchorLayout::GetBoth(Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp)
{
	typedef AnchorPoint::Type Type;

	//Dispatch
	nall::linear_vector<int> res = {0, 0};
	if (axis.least_.type==Type::Percent) {
		GetCenteredPercent(res, axis, args, true, comp);
	} else if (axis.least_.type==Type::Attached) {
		GetCenteredAttached(res, axis, args, true, comp);
	} else {
		//Error.
#ifdef ANCHOR_LAYOUT_ERRORS_ON
		std::cout <<"Error: Unknown dependent type.\n";
#endif
		return {0, 0};
	}

	return res;
}


int AnchorLayout::GetPercent(Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp)
{
	//Note: Margin only applies if percent is 0.0 or 1.0
	AnchorPoint& item = ltr ? axis.least_ : axis.greatest_;
	int sign = 0;
	if (FloatEquals(item.percent,0.0) || FloatEquals(item.percent,1.0)) {
		sign = ltr ? 1 : -1;
	}

	//Simple; just remember to include the item's offset, and the global margin (+/- sign)
	return item.percent*args.containerMax + args.containerOffset + item.offset + ((int)args.containerMargin*sign);
}


int AnchorLayout::GetUnbound(Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp)
{
	//This simply depends on the item diametrically opposed to this one, plus a bit of sign manipulation
	int itemMin = args.isHoriz ? comp.minimumGeometry().width : comp.minimumGeometry().height;
	int other = AnchorLayout::Get(axis, args, !ltr, comp);
	int sign = ltr ? -1 : 1;

	return other + sign*itemMin;

}


//Only slightly more complex. Centering requires a bit of careful math; the rest is a matter of remembering to flip "ltr"
int AnchorLayout::GetAttached(Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp)
{
	//Axis::FixedAxis Centered = Axis::Centered();  //For brevity

	//First, retrieve the components
	AnchorPoint& item = ltr ? axis.least_ : axis.greatest_;

	//Children* other = AnchorLayout::FindChild(args.children, *item.refItem);

	//Did we retrieve anything? (this should also check for a null item.refItem)
	Children other;
	if (!args.children.find(item.refItem, other)) {
		//TODO: We _could_ probably allow attaching to components in a fixed/horizontal/vertical layout
		//      manager. But I think the complexity won't gain us much, and I'd rather get this working first.
#ifdef ANCHOR_LAYOUT_ERRORS_ON
		std::cout <<"Error: Attaching to unmanaged component.\n";
#endif
		return 0;
	}

	//Retrive the anchor; translate "default" value
	Anchor anch = item.anchor;
	if (anch==Anchor::Default) {
		if (axis.isFullAxis) {  //TODO: This might be assuming too much!
			anch = Anchor::Center;
		} else {
			anch = ltr ? Anchor::Right : Anchor::Left;
		}
	}

	//Compute the result, based on either the first or second point, or both.
	Axis & otherAxis = args.isHoriz ? other.horiz : other.vert;
	if (anch==Anchor::Center) {
		//The center layout requires both points to be calculatable. Otherwise, it't not very different.
		int baseVal = AnchorLayout::Get(otherAxis, args, true, *other.sizable);
		baseVal = (AnchorLayout::Get(otherAxis, args, false, *other.sizable)-baseVal)/2 + baseVal;
		return baseVal + item.offset;
	} else {
		//For left/right layouts, there's only one point to check.
		int baseVal = 0;
		if (anch==Anchor::Left) {
			baseVal = AnchorLayout::Get(otherAxis, args, true, *other.sizable);
		} else if (anch==Anchor::Right) {
			baseVal = AnchorLayout::Get(otherAxis, args, false, *other.sizable);
		} else {
			//Shouldn't fail, but just to be safe...
#ifdef ANCHOR_LAYOUT_ERRORS_ON
			std::cout <<"Error: Unexpected Anchor.\n";
#endif
			return 0;
		}

		return baseVal + item.offset;
	}
}


void AnchorLayout::CenterItem(nall::linear_vector<int>& res, int center, Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp)
{
	//Retrieve the item's width, divide by 2, and save the result
	AnchorPoint& item = ltr ? axis.least_ : axis.greatest_;
	int itemMin = args.isHoriz ? comp.minimumGeometry().width : comp.minimumGeometry().height;
	int width = axis.forceSize>0 ? axis.forceSize : itemMin;
	res[0] = center + item.offset - width/2;
	res[1] = center + item.offset + width/2;
}


void AnchorLayout::GetCenteredPercent(nall::linear_vector<int>& res, Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp)
{
	//Retrieve the item, stash "offset" (since we use it for the width here.)
	AnchorPoint& item = ltr ? axis.least_ : axis.greatest_;
	int offset = item.offset;
	item.offset = 0;

	//Call the relevant sibling function, restore the offset
	int center = AnchorLayout::GetPercent(axis, args, ltr, comp);
	item.offset = offset;

	//Now factor in the item's width
	AnchorLayout::CenterItem(res, center, axis, args, ltr, comp);
}


void AnchorLayout::GetCenteredAttached(nall::linear_vector<int>& res, Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp)
{
	//Retrieve the item, call the sibling function
	AnchorPoint& item = ltr ? axis.least_ : axis.greatest_;
	int offset = item.offset;
	item.offset = 0;
	int center = AnchorLayout::GetAttached(axis, args, ltr, comp);
	item.offset = offset;

	//Now factor in the item's width
	AnchorLayout::CenterItem(res, center, axis, args, ltr, comp);
}

