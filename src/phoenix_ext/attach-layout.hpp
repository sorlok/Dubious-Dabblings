#pragma once

#include <phoenix.hpp>

//Copyright 2011 by Seth N. Hetu
//Shared under the terms of the ISC license


class Attachment {
private:
	//Type of attachments
	enum class TYPE { UNBOUND, PERCENT, ATTACHED, SPECIAL };

public:
	//Ways to anchor "Attached" attachments
	enum class ANCHOR {
		DEFAULT = 0,   //Default means "right" if it's a left attachment, and "left" if it's a right attachment.
		LEFT=1,  TOP=1, //Semantically the same when it comes to layout.
		RIGHT=2, BOTTOM=2, //Semantically the same when it comes to layout.
		//TODO: I want to allow anchoring on the "center" --it should only require that "left" and "right" are both set.
	};

	//Special Attach cases; currently only "centered"
	enum class SPECIAL {
		CENTERED,
	};

public:
	//Defeault attachment: Figure it out based on the minimum width/height and the diametrically opposed Attachment.
	Attachment(): type(TYPE::UNBOUND) {}

	//Percent attachment: Attach an item at a given % of the parent's total width/height
	Attachment(double percent, int offset=0): percent(percent), offset(offset), type(TYPE::PERCENT) {}

	//Attached attachment: Attach to another component directly, with an offset and possible alignment.
	Attachment(phoenix::Sizable& attachTo, int offset, ANCHOR attachAt=ANCHOR::DEFAULT) : refItem(&attachTo), anchor(attachAt), offset(offset), type(TYPE::ATTACHED) {}

	//Use a special attachment type.
	//"offset" can mean many things. Here, if not zero, it specifies the component's "width" (or whatever) in pixels
	Attachment(SPECIAL specialType, double percent, int offset=0) : special(specialType), percent(percent), offset(offset), type(TYPE::SPECIAL) {}

	//NOTE: If we are clever, we don't have to reset anything; we can just check "done" and "!done" alternatively.
	//      This requires some attention when adding Sizables, but is otherwise easy. I just don't see a real performance boost
	//      here, so I'm using the much-easier-to-understand "reset()" method.
	void reset() {
		done = false;
		waiting = false;
	}

	friend class AttachLayout;

private:
	//Variables used to track which attachments have been computed, and which are pending.
	// Done takes priority over waiting.
	bool done;
	bool waiting;
	int res;

	//Combined state variables
	phoenix::Sizable* refItem;
	ANCHOR anchor;
	SPECIAL special;
	double percent;
	int offset;
	TYPE type;
};


class AttachLayout : public phoenix::Layout {
public:
	//Required functionality: Layout
	void append(phoenix::Sizable& sizable); //Note: calling append() without any attachments might lead to funky effects.
	void append(phoenix::Sizable& sizable, const Attachment& leftA, const Attachment& topA, const Attachment& rightA=Attachment(), const Attachment& bottomA=Attachment());
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
	void setSkipGeomUpdates(bool val) { skipGeomUpdate = val; }

	//Constructor/destructor
	AttachLayout();
	virtual ~AttachLayout();

private:
	phoenix::Geometry lastKnownSize;
	bool skipGeomUpdate;

	struct Children {
		Sizable *sizable;
		Attachment left;
		Attachment top;
		Attachment right;
		Attachment bottom;
	};
	nall::linear_vector<Children> children;

	struct LayoutData {
		int offset;
		unsigned int containerMax;
		unsigned int itemMin;
		int sign;
		bool isHoriz;
		Attachment::ANCHOR defaultAnch;
		nall::linear_vector<Children>& children;

		//sign and defaultAnch aren't set until later, so let's make a slightly more useful constructor
		LayoutData(int offset, unsigned int containerMax, unsigned int itemMin, bool isHoriz, nall::linear_vector<Children>& children) :
			offset(offset), containerMax(containerMax), itemMin(itemMin), isHoriz(isHoriz), children(children)
		{}

		//Helper methods; makes it easier to pass this structure around without manually resetting everything.
		LayoutData& setSign(int sign) {
			this->sign = sign;
			return *this;
		}
		LayoutData& flipSign() {
			sign = -sign;
			return *this;
		}
		LayoutData& setAnchor(const Attachment::ANCHOR& anchor) {
			defaultAnch = anchor;
			return *this;
		}
		LayoutData& flipAnchor() {
			defaultAnch = (defaultAnch==Attachment::ANCHOR::LEFT?Attachment::ANCHOR::RIGHT:Attachment::ANCHOR::LEFT);
			return *this;
		}
	};


	static void ComputeComponent(Attachment& least, Attachment& greatest, int& resOrigin, unsigned int& resMagnitude, LayoutData args);
	static int Get(Attachment& item, Attachment& diam, LayoutData args);
	static int GetUnbound(Attachment& item, Attachment& diam, LayoutData args);
	static int GetPercent(Attachment& item, LayoutData args);
	static int GetCentered(Attachment& item, Attachment& diam, LayoutData args);
	static int GetAttached(Attachment& item, Attachment& diam, LayoutData args);

	struct State {
		bool enabled;
		bool visible;
	} state;
};
