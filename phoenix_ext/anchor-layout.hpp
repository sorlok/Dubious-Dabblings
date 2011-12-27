//Copyright 2011 by Seth N. Hetu
//Shared under the terms of the ISC license

#pragma once

#include <phoenix.hpp>
#include "scapegoat.hpp"


class AnchorPoint {
private:
	//Type of AnchorPoint
	enum class Type { Unbound, Percent, Attached };

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
	AnchorPoint(): type(Type::Unbound), superSpecial(false) {}

	//Percent attachment: Attach an item at a given % of the parent's total width/height
	AnchorPoint(double percent, int offset=0): percent(percent), offset(offset), type(Type::Percent), superSpecial(false) {}

	//Attached attachment: Attach to another component directly, with an offset and possible alignment.
	AnchorPoint(phoenix::Sizable& attachTo, int offset=0, Anchor attachAt=Anchor::Default) : refItem(&attachTo), anchor(attachAt), offset(offset), type(Type::Attached), superSpecial(false) {}

	//NOTE: If we are clever, we don't have to reset anything; we can just check "done" and "!done" alternatively.
	//      This requires some attention when adding Sizables, but is otherwise easy. I just don't see a real performance boost
	//      here, so I'm using the much-easier-to-understand "reset()" method.
	void reset() {
		state= State::Ready;
	}

	friend class AnchorLayout;
	friend class Axis;

private:
	//Hackish constructor: "Special" type. Only usable internally to prevent abuse.
	//explicit AnchorPoint(bool isSpecial, bool dummy, bool dummy2, bool dummy3) : superSpecial(true) {}

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
	double percent;
	int offset;
	Type type;
	bool superSpecial;
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
	unsigned int forceSize;

	//Default constructor; used by AnchorLayout. Puts a component at 0,0 with 0,0 width/height
	Axis() : least_({}), greatest_({}), isFullAxis(false) {}

public:
	//Whole-axis attachment types. (Note: This is mildly hackish, but only in the backend)
	static AnchorPoint Centered() { AnchorPoint res; res.superSpecial=true; return res; }


	//Construct an axis composed of a left/right or top/bottom pair of Attachments.
	//Either one of these Attachments is optional, and may be set to {} to use the component's
	// default width/height.
	//NOTE: forceSize only applies to "Centered" components
	Axis(const AnchorPoint& least, const AnchorPoint& greatest=AnchorPoint(), unsigned int forceSize=0) {
		isFullAxis = least.superSpecial;
		this->forceSize = forceSize;
		if (isFullAxis) {
			least_ = greatest;
			greatest_ = {};
		} else {
			least_ = least;
			greatest_ = greatest;
		}
	}

	//Construct an axis composed of a full-axis attachment, such as "centered"
	//Explicit helps prevent usage errors
	//explicit Axis(const FixedAxis& type, const AnchorPoint& full) : least_(full), greatest_({}), isFullAxis(true) {}

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
	void synchronizeLayout();

	//Workaround
	void synchHack(Sizable* sizable);

	//Required functionality: Sizable
	virtual void setEnabled(bool enabled = true);
	virtual void setVisible(bool visible = true);
	virtual bool enabled();
	virtual bool visible();
	virtual phoenix::Geometry minimumGeometry();
	virtual void setGeometry(const phoenix::Geometry &geometry);

	//For clarity.
	void setMargin(size_t margin) { state.margin = margin; }

	//Constructor/destructor
	AnchorLayout();
	virtual ~AnchorLayout();

	//Currently needs to be public until we get a better "overall" phoenix solution.
	void setSkipGeomUpdates(bool val) { state.skipGeomUpdate = val; }

private:
	//For "geometry locking"
	friend class ScopedLayoutLock;

	struct Children {
		Sizable *sizable;
		Axis horiz;
		Axis vert;
	};
	lightweight_map<phoenix::Sizable*, Children> children;

	//Global layout data doesn't change based on each component.
	struct LayoutData {
		int containerOffset;
		unsigned int containerMax;
		size_t containerMargin;
		bool isHoriz;
		lightweight_map<phoenix::Sizable*, Children>& children;
	};

	static void ComputeComponent(Axis& axis, int& resOrigin, unsigned int& resMagnitude, LayoutData args, phoenix::Sizable& comp);
	static int Get(Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp);
	static nall::linear_vector<int> GetBoth(Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp);
	static int GetUnbound(Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp);
	static int GetPercent(Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp);
	static int GetAttached(Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp);
	static void GetCenteredPercent(nall::linear_vector<int>& res, Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp);
	static void GetCenteredAttached(nall::linear_vector<int>& res, Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp);
	static void CenterItem(nall::linear_vector<int>& res, int center, Axis& axis, LayoutData& args, bool ltr, phoenix::Sizable& comp);

	struct State {
		bool enabled;
		bool visible;
		size_t margin;
		phoenix::Geometry lastKnownSize;
		bool skipGeomUpdate;
		bool skippedAnUpdate;
	} state;
};



//Used to help manage layout changes
class ScopedLayoutLock {
public:
	ScopedLayoutLock(AnchorLayout* layout) {
		append(layout);
	}

	~ScopedLayoutLock() {
		for(auto& layout : layouts) {
			layout->setSkipGeomUpdates(false);
			if (layout->state.skippedAnUpdate) {
				layout->setGeometry(layout->state.lastKnownSize);
			}
		}
	}

	void append(AnchorLayout* layout) {
		if (layout->state.skipGeomUpdate) {
			return;
		}

		layout->state.skippedAnUpdate = false;
		layout->setSkipGeomUpdates(true);
		layouts.append(layout);
	}

private:
	//Non-copyable
	ScopedLayoutLock (const ScopedLayoutLock &)  = delete;
	ScopedLayoutLock& operator= (const ScopedLayoutLock &)  = delete;

	nall::linear_vector<AnchorLayout*> layouts;
};

