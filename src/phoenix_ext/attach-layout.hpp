#pragma once

#include <phoenix.hpp>



class Attachment {
private:
	//Type of attachments
	enum class TYPE { UNBOUND, PERCENT, ATTACHED };

public:
	//Attach an item at a given % of the parent's total width/height
	Attachment(): type(TYPE::UNBOUND) {}
	Attachment(double percent, int offset=0): percent(percent), offset(offset), type(TYPE::PERCENT) {}

	friend class AttachLayout;

private:
	//Combined state variables
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

	struct State {
		bool enabled;
		bool visible;
	} state;

	struct Children {
		Sizable *sizable;
		Attachment left;
		Attachment top;
		Attachment right;
		Attachment bottom;
	};
	nall::linear_vector<Children> children;
};
