#pragma once

#include <phoenix.hpp>



class Attachment {
private:
	//Type of attachments
	enum class TYPE { UNBOUND, PERCENT, ATTACHED };

public:
	//Ways to anchor "Attached" attachments
	enum class ANCHOR {
		DEFAULT = 0,   //Default means "right" if it's a left attachment, and "left" if it's a right attachment.
		LEFT=1,  TOP=1, //Semantically the same when it comes to layout.
		RIGHT=2, BOTTOM=2, //Semantically the same when it comes to layout.
		//TODO: I want to allow anchoring on the "center" --it should only require that "left" and "right" are both set.
	};

public:
	//Defeault attachment: Figure it out based on the minimum width/height and the diametrically opposed Attachment.
	Attachment(): type(TYPE::UNBOUND) {}

	//Percent attachment: Attach an item at a given % of the parent's total width/height
	Attachment(double percent, int offset=0): percent(percent), offset(offset), type(TYPE::PERCENT) {}

	//Attached attachment: Attach to another component directly, with an offset and possible alignment.
	Attachment(phoenix::Sizable& attachTo, int offset, ANCHOR attachAt=ANCHOR::DEFAULT) : refItem(&attachTo), anchor(attachAt), offset(offset), type(TYPE::ATTACHED) {}

	//TODO: I want a "special" Attachment type which allows things like "centering" a component within the
	//      parent component. Call it a version 2.0 feature.

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
	double percent;
	int offset;
	TYPE type;
};


class AttachLayout : public phoenix::Layout {
public:
	//Required functionality: Layout
	void append(phoenix::Sizable& sizable); //Note: calling append() without any attachments might lead to funky effects.
	void append(phoenix::Sizable& sizable, const Attachment& left, const Attachment& top, const Attachment& right=Attachment(), const Attachment& bottom=Attachment());
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

	//TODO: These all share a bunch of annoying attributes like "offset", etc., which are passed from the first call to the last.
	//      Perhaps we can group these into a common class called "args"?
	static void ComputeComponent(Attachment& left, Attachment& right, int offsetX, unsigned int maxWidth, int& resX, unsigned int& resWidth, unsigned int minWidth, bool isHoriz, nall::linear_vector<Children>& children);
	static int Get(Attachment& item, Attachment& diam, int offset, unsigned int maximum, unsigned int minimum, int sign, bool isHoriz, Attachment::ANCHOR defaultAnch, nall::linear_vector<Children>& children);
	static int GetUnbound(Attachment& item, Attachment& diam, int offset, unsigned int maximum, unsigned int minimum, int sign, bool isHoriz, Attachment::ANCHOR defaultAnch, nall::linear_vector<Children>& children);
	static int GetPercent(Attachment& item, int offset, unsigned int maximum);
	static int GetAttached(Attachment& item, Attachment& diam, int offset, unsigned int maximum, unsigned int minimum, int sign, bool isHoriz, Attachment::ANCHOR defaultAnch, nall::linear_vector<Children>& children);

	struct State {
		bool enabled;
		bool visible;
	} state;
};
