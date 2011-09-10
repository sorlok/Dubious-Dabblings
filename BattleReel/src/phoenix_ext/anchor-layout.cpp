//Copyright 2011 by Seth N. Hetu
//Shared under the terms of the ISC license
#include "anchor-layout.hpp"

#include <iostream> //Debugging only; comment out if you're not using cout()

using namespace phoenix;
typedef AnchorPoint::Anchor Anchor;


AnchorLayout::AnchorLayout()
{
	//Defaults
	state.enabled = true;
	state.visible = true;
	state.margin = 0;
	state.skipGeomUpdate = false;
}


AnchorLayout::~AnchorLayout()
{
	//Remove all children on exit
	setSkipGeomUpdates(true);
	while(children.size()>0) {
		remove(*children[0].sizable);
	}
}


void AnchorLayout::append(Sizable &sizable) {
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


void AnchorLayout::append(phoenix::Sizable &sizable, const Axis& horizontal, const Axis& vertical)
{
	typedef AnchorPoint::Type Type;

	//If this child already exists, update its attachment data
	foreach(child, children)  {
		if(child.sizable == &sizable) {
			child.horiz = horizontal;
			child.vert = vertical;
			return;
		}
	}
	//Else, add a new item
	children.append({ &sizable, horizontal, vertical });
	synchronize();
}


void AnchorLayout::remove(Sizable& sizable)
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


void AnchorLayout::synchronize()
{
	//Ensure all sizables have been appended to the layout.
	foreach(child, children) {
		Layout::append(*child.sizable);
	}
}


void AnchorLayout::setEnabled(bool enabled)
{
	state.enabled = enabled;
	foreach(child, children) {
		child.sizable->setEnabled(dynamic_cast<Widget*>(child.sizable) ? child.sizable->enabled() : enabled);
	}
}

void AnchorLayout::setVisible(bool visible)
{
	state.visible = visible;
	foreach(child, children) {
		child.sizable->setVisible(dynamic_cast<Widget*>(child.sizable) ? child.sizable->visible() : visible);
	}
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
	//When closing the app (or if the user tells us to) we don't need to redo any internal geometry
	// calculations. Be careful not to leave this flag on before going back to the message loop.
	if (state.skipGeomUpdate) {
		return;
	}

	//Save containerGeometry
	state.lastKnownSize = containerGeometry;

	//First, reset all children.
	foreach(child, children) {
		child.horiz.reset();
		child.vert.reset();
	}

	//Apply alyout rules for each child  individually.
	foreach(child, children) {
		//We compute each component in pairs..
		Geometry res;

		AnchorLayout::ComputeComponent(child.horiz, res.x, res.width, {{containerGeometry.x, containerGeometry.width, state.margin, true, children}, {child.sizable->minimumGeometry().width, true}});
		AnchorLayout::ComputeComponent(child.vert, res.y, res.height, {{containerGeometry.y, containerGeometry.height, state.margin, true, children}, {child.sizable->minimumGeometry().height, false}});
		child.sizable->setGeometry(res);
	}

}




//Compute a single component. "Least" and "Greatest" are essentially "left" and "right".
// Save into "resOrigin", "resMagnitude", which are basically "x" and "width".
void AnchorLayout::ComputeComponent(Axis& axis, int& resOrigin, unsigned int& resMagnitude, LayoutData args)
{
	args.local.ltr = true;
	resOrigin = AnchorLayout::Get(axis.least_, axis.greatest_, args);
	args.local.ltr = false;
	resMagnitude = (unsigned int)(AnchorLayout::Get(axis.greatest_, axis.least_, args) - resOrigin);
}


int AnchorLayout::Get(Axis& axis, LayoutData& args)
{
	typedef AnchorPoint::Type Type;
	typedef AnchorPoint::State State;
	Axis::FullAxis Centered = Axis::FullAxis::Centered;  //For brevity

	//Simple cases
	AnchorPoint item = args.local.ltr ? axis.least_ : axis.greatest_;
	if (item.state==State::Done) {
		return item.res;
	}
	if (item.state==State::Waiting) {
		//ERROR: default to returning 0 (optional approach to problem-solving: throw something)
		std::cout <<"ERROR: Still waiting on item.\n";
		return 0;
	}

	//Have to figure it out
	item.state = State::Waiting;
	if (item.type==Type::Unbound) {
		item.res = GetUnbound(item, diam, args);
	} else if (item.type==Type::Percent) {
		item.res = GetPercent(item, args);
	} else if (item.type==Type::Attached) {
		item.res = GetAttached(item, diam, args);
	} else if (item.type==Type::SpecialPercent && item.special==Attachment::SPECIAL::CENTERED) {
		item.res = GetCenteredPercent(item, diam, args);
	} else if (item.type==Type::SpecialAttached && item.special==Attachment::SPECIAL::CENTERED) {
		item.res = GetCenteredAttached(item, diam, args);
	} else {
		//ERROR (in case we add more types later).
		std::cout <<"ERROR: Unknown type\n";
		return 0;
	}
	item.state = State::Done;
	return item.res;
}



int AnchorLayout::GetUnbound(AnchorPoint& item, AnchorPoint& diam, LayoutData& args)
{
	//This simply depends on the item diametrically opposed to this one, plus a bit of sign manipulation
	return AttachLayout::Get(diam, item, args.flipAnchor()) + args.local.sign*args.local.itemMin;
}


int AnchorLayout::GetPercent(AnchorPoint& item, LayoutData& args)
{
	//Simple; just remember to include the item's offset, and the global margin
	return item.percent*args.containerMax + args.offset + item.offset + ((int)args.margin*-args.sign);
}


int AnchorLayout::GetCenteredPercent(AnchorPoint& item, AnchorPoint& diam, LayoutData& args)
{
	//First, get the centered position and width, then just expand outwards. Remember to set the diametric point to "done".
	//NOTE: The margin has no effect here, because it's (practically speaking) added to the left and removed from the right,
	//      thus having no overall effect on the centered component.
	int point = item.percent*args.containerMax + args.offset;
	int width = item.offset>0 ? item.offset : args.itemMin;

	diam.res = point + width/2;
	diam.done = true;
	return point - width/2;
}


int AnchorLayout::GetCenteredAttached(AnchorPoint& item, AnchorPoint& diam, LayoutData& args)
{
	Axis::FullAxis Centered = Axis::FullAxis::Centered;  //For brevity

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
	Anchor anch = item.anchor==Anchor::Default ? Anchor::Center : item.anchor;
	int baseVal = 0;
	if (anch==Attachment::ANCHOR::CENTER) {
		//The center layout requires both points to be calculatable. Otherwise, it't not very different.
		Attachment* near = args.isHoriz ? &other->left : &other->top;
		Attachment* far = args.isHoriz ? &other->right : &other->bottom;
		baseVal = AttachLayout::Get(*near, *far, args);
		baseVal = (AttachLayout::Get(*far, *near, args)-baseVal)/2 + baseVal;
	} else {
		//For left/right layouts, there's only one point to check. //Not true!
		Attachment* base = nullptr;
		Attachment* apart = nullptr;
		LayoutData ld2 = args;
		if (anch==Attachment::ANCHOR::LEFT) {
			base = args.isHoriz ? &other->left : &other->top;
			apart = args.isHoriz ? &other->right : &other->bottom;
			ld2.setSign(-1).setAnchor(Attachment::ANCHOR::RIGHT);
		} else if (anch==Attachment::ANCHOR::RIGHT) {
			base = args.isHoriz ? &other->right : &other->bottom;
			apart = args.isHoriz ? &other->left : &other->top;
			ld2.setSign(1).setAnchor(Attachment::ANCHOR::LEFT);
		} else {
			//Shouldn't fail, but just to be safe...
			std::cout <<"ERROR: unexpected anchor value.\n";
			return 0;
		}

		baseVal = AttachLayout::Get(*base, *apart, ld2);
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
int AnchorLayout::GetAttached(AnchorPoint& item, AnchorPoint& diam, LayoutData& args)
{
	Axis::FullAxis Centered = Axis::FullAxis::Centered;  //For brevity

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
		baseVal = AttachLayout::Get(*near, *far, args);
		baseVal = (AttachLayout::Get(*far, *near, args)-baseVal)/2 + baseVal;
	} else {
		//For left/right layouts, there's only one point to check.
		Attachment* base = nullptr;
		Attachment* apart = nullptr;
		LayoutData ld2 = args;
		if (anch==Attachment::ANCHOR::LEFT) {
			base = args.isHoriz ? &other->left : &other->top;
			apart = args.isHoriz ? &other->right : &other->bottom;
			ld2.setSign(-1).setAnchor(Attachment::ANCHOR::RIGHT);
		} else if (anch==Attachment::ANCHOR::RIGHT) {
			base = args.isHoriz ? &other->right : &other->bottom;
			apart = args.isHoriz ? &other->left : &other->top;
			ld2.setSign(1).setAnchor(Attachment::ANCHOR::LEFT);
		} else {
			//Shouldn't fail, but just to be safe...
			std::cout <<"ERROR: unexpected anchor value.\n";
			return 0;
		}

		baseVal = AttachLayout::Get(*base, *apart, ld2);
	}

	//Now add the offset
	return baseVal + item.offset;
}
