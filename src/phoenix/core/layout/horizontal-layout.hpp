struct HorizontalLayout : public Layout {
  void append(Sizable &sizable, unsigned width, unsigned height, unsigned spacing = 0);
  void append(Sizable &sizable);
  bool enabled();
  Geometry minimumGeometry();
  void remove(Sizable &sizable);
  void reset();
  void setAlignment(double alignment);
  void setEnabled(bool enabled = true);
  void setGeometry(const Geometry &geometry);
  void setMargin(unsigned margin);
  void setVisible(bool visible = true);
  void synchronize();
  bool visible();
  HorizontalLayout();
  ~HorizontalLayout();

//private:
  struct State {
    double alignment;
    bool enabled;
    unsigned margin;
    bool visible;
  } state;

  struct Children {
    Sizable *sizable;
    unsigned width, height, spacing;
  };
  nall::linear_vector<Children> children;
};
