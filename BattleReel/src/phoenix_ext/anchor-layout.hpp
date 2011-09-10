//Copyright 2011 by Seth N. Hetu
//Shared under the terms of the ISC license

#pragma once

#include <phoenix.hpp>

class AnchorPoint {
private:
	//Type of AnchorPoint
	enum class Type { Unbound, Percent, Attached, SpecialPercent, SpecialAttached };

public:
	//Ways to anchor "Attached" AnchorPoints
	enum class Anchor {
		Default = 0,       //Default means "right" if it's a left attachment, and "left" if it's a right attachment, and "centered" if it's centered
		Left=1,  Top=1,    //Semantically the same when it comes to layout.
		Right=2, Bottom=2, //Semantically the same when it comes to layout.
		Center=3,
	};

public:
	//Defeault attachment: Figure it out based on the minimum width/height and the diametrically opposed Attachment.
	AnchorPoint(): type(Type::Unbound) {}

	//Percent attachment: Attach an item at a given % of the parent's total width/height
	AnchorPoint(double percent, int offset=0): percent(percent), offset(offset), type(Type::Percent) {}

	//Attached attachment: Attach to another component directly, with an offset and possible alignment.
	AnchorPoint(phoenix::Sizable& attachTo, int offset=0, Anchor attachAt=Anchor::Default) : refItem(&attachTo), anchor(attachAt), offset(offset), type(Type::Attached) {}

	//NOTE: If we are clever, we don't have to reset anything; we can just check "done" and "!done" alternatively.
	//      This requires some attention when adding Sizables, but is otherwise easy. I just don't see a real performance boost
	//      here, so I'm using the much-easier-to-understand "reset()" method.
	void reset() {
		state= State::Ready;
	}

	friend class AnchorLayout;
	friend class Axis;

private:
	//Use a special attachment type. These can only be set by friend classes.
	//"offset" can mean many things. Here, if not zero, it specifies the component's "width" (or whatever) in pixels
	AnchorPoint(bool isSpecial, double percent, int width=0) : isSpecial(true), percent(percent), offset(width), type(Type::SpecialPercent) {}
	AnchorPoint(bool isSpecial, phoenix::Sizable& attachTo, Anchor attachAt=Anchor::Default, int width=0) : refItem(&attachTo), anchor(attachAt), isSpecial(true), offset(width), type(Type::SpecialAttached) {}

private:
	enum class State {
		Ready, Waiting, Done
	};

	//Variables used to track which attachments have been computed, and which are pending.
	// Done takes priority over waiting.
	State state;
	int res;

	//Combined state variables
	phoenix::Sizable* refItem;
	Anchor anchor;
	bool isSpecial;
	double percent;
	int offset;
	Type type;
};


//An Axis contains attachment points for either the horizontal or the vertical axis.
// It can generally be constructed as either a pair of points (left/right or top/bottom) or as
// a single entity (horiz) spanning the entire axis.
class Axis {
private:
	//Actual data
	AnchorPoint least_;
	AnchorPoint greatest_;
	bool isFullAxis;

	//Default constructor; used by AnchorLayout. Puts a component at 0,0 with 0,0 width/height
	Axis() : least_({}), greatest_({}), isFullAxis(false) {}

public:
	//Whole-axis attachment types
	enum class FullAxis {
		Centered,
	};

	//Construct an axis composed of a left/right or top/bottom pair of Attachments.
	//Either one of these Attachments is optional, and may be set to {} to use the component's
	// default width/height.
	Axis(const AnchorPoint& least, const AnchorPoint& greatest=AnchorPoint()) : least_(least), greatest_(greatest), isFullAxis(false) {}

	//Construct an axis composed of a full-axis attachment, such as "centered"
	Axis(const FullAxis& type, const AnchorPoint& full) : least_(full), greatest_({}), isFullAxis(true) {}

	//For convenience
	AnchorPoint& left()    { return least_; }
	AnchorPoint& right()   { return greatest_; }
	AnchorPoint& top()     { return least_; }
	AnchorPoint& bottom()  { return greatest_; }
	AnchorPoint& horiz()   { return least_; }
	AnchorPoint& vert()    { return least_; }

	void reset() {
		least_.reset();
		greatest_.reset();
	}

	friend class AnchorLayout;
};



class AnchorLayout : public phoenix::Layout {
public:
	//Append a component with the given horizontal/vertical Axis data. This data is used to
	//  determine where the component should appear and what size it should be.
	void append(phoenix::Sizable& sizable, const Axis& horizontal, const Axis& vertical);

	//Required functionality: Layout
	void append(phoenix::Sizable& sizable); //Calling this is not recommended; I think it'll create default Attachments?
	void remove(phoenix::Sizable& sizable);
	void synchronize();

	//Required functionality: Sizable
	virtual void setEnabled(bool enabled = true);
	virtual void setVisible(bool visible = true);
	virtual bool enabled();
	virtual bool visible();
	virtual phoenix::Geometry minimumGeometry();
	virtual void setGeometry(const phoenix::Geometry &geometry);

	//Mildly useful
	void setSkipGeomUpdates(bool val) { state.skipGeomUpdate = val; }
	void setMargin(size_t margin) { state.margin = margin; }

	//Constructor/destructor
	AnchorLayout();
	virtual ~AnchorLayout();

private:
	struct Children {
		Sizable *sizable;
		Axis horiz;
		Axis vert;
	};
	nall::linear_vector<Children> children;

	//Global layout data doesn't change based on each component.
	struct LayoutData {
		int containerOffset;
		unsigned int containerMax;
		size_t containerMargin;
		bool isHoriz;
		nall::linear_vector<Children>& children;
	};

	static Children* FindChild(nall::linear_vector<Children>& children, const Sizable& find);

	static void ComputeComponent(Axis& axis, int& resOrigin, unsigned int& resMagnitude, LayoutData args, phoenix::Sizable& comp);
	static int Get(Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp);
	static nall::linear_vector<int> GetBoth(Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp);
	static int GetUnbound(Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp);
	static int GetPercent(Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp);
	static int GetCenteredPercent(nall::linear_vector<int>& res, Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp);
	static int GetCenteredAttached(nall::linear_vector<int>& res, Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp);
	static int GetAttached(Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp);

	struct State {
		bool enabled;
		bool visible;
		size_t margin;
		phoenix::Geometry lastKnownSize;
		bool skipGeomUpdate;
	} state;
};
