#ifndef _EDITABLE3D_H_
#define _EDITABLE3D_H_

#include "Editor.h"
#include "Graph3D/Plot.h"
#include "Graph3D/Camera.h"

namespace Go4D
{

  class Editor3D;

  /// an editable specific for 3D objects
  class Editable3D : public EditNode
  {
    typedef EditNode inherited;
  protected :
    RTTI_DECL; // hack, to remove very fastly
    typedef EditNode Base;
    Editor3D * GetEditor3D();
    APlot GetDefaultPlot();
  };

  /// an editable 3D plot
  class EditablePlot : public Editable3D, public APlot
  {
    typedef Editable3D inherited;
  public :
    typedef EditNode Base;
    RTTI_DECL;
    EditablePlot();
    virtual EditableID TypeName() const;
    static EditableID _TypeName();
    virtual void SetValue(EditableID propName, Variant );
    virtual Variant GetValue(EditableID propName) const;
    virtual void ApplyAction(EditableID actionName, Variant value);

    real GetMass() const { return fMass; }

  public :
    APlot fBaseChangedPlot;
    real fMass;
  };

  /// EditablePlot that position is always calculated in order to be the
  /// barycenter of all the linked plots
  class EditableBarycenter : public EditablePlot
  {
    typedef EditablePlot inherited;
  public :
    RTTI_DECL;
    virtual EditableID TypeName() const;
    static EditableID _TypeName();
    virtual void AddLink(EditableID linkName, Editable *);
    virtual void DelLink(EditableID linkName, Editable *);
    virtual void Refresh();

  protected :
    void CalculatePosition();
  };

  /// an editable camera for 3D viewing
  class EditableCamera : public Editable3D, public Camera
  {
    typedef Editable3D inherited;
  public :
    RTTI_DECL;
    EditableCamera(int, int);
    virtual EditableID TypeName() const;
    static EditableID _TypeName();
    virtual void SetValue(EditableID propName, Variant );
    virtual Variant GetValue(EditableID propName) const;
    virtual void ApplyAction(EditableID actionName, Variant value);
    virtual void Refresh();
  };

  /**
  *  An object is group where all the sub elems (plots) are defined in the
  * local base
  */
  class EditableObject : public Editable3D, public EditGroup, public Base
  {
    typedef Editable3D inherited;
  public :
    RTTI_DECL;
    EditableObject();
    virtual EditableID TypeName() const;
    static EditableID _TypeName();
    virtual void SetValue(EditableID propName, Variant );
    virtual Variant GetValue(EditableID propName) const;
    virtual void ApplyAction(EditableID actionName, Variant value);
    virtual void Refresh();
    virtual void SetSelected(bool);

  protected :
    fstring fName;
  };

  /// link between 2 plots
  class EditableSegment : public Editable3D
  {
    typedef Editable3D inherited;
  public :
    RTTI_DECL;
    EditableSegment();
    virtual EditableID TypeName() const;
    static EditableID _TypeName();
    virtual void SetValue(EditableID propName, Variant );
    virtual Variant GetValue(EditableID propName) const;
    virtual void ApplyAction(EditableID actionName, Variant value);
    virtual void AddLink(EditableID linkName, Editable *);
    virtual void DelLink(EditableID linkName, Editable *);
    virtual void Refresh();

    APlot * Alpha() const;
    APlot * Omega() const;

    Editable * EditableAlpha() const;
    Editable * EditableOmega() const;
  };

  /// an editable face, not implemented yet
  class EditableFace : public Editable3D
  {
    typedef Editable3D inherited;
  public :
    RTTI_DECL;
    EditableFace();
    virtual EditableID TypeName() const;
    static EditableID _TypeName();
    virtual void SetValue(EditableID propName, Variant );
    virtual Variant GetValue(EditableID propName) const;
    virtual void ApplyAction(EditableID actionName, Variant value);
    virtual void AddLink(EditableID linkName, Editable *);
    virtual void DelLink(EditableID linkName, Editable *);
    virtual void Refresh();

    int PlotCount() const;
    APlot * GetPlot(int i) const;
    EditablePlot * GetEditablePlot(int i) const;

  public :
    Color fColor;
  };

  /// add specific code for 3D objects (to map string names with classes)
  class Editor3D : public Editor
  {
    typedef Editor inherited;
  public :
    Editor3D();
    ~Editor3D();
    RTTI_DECL;
    virtual bool IsClearable();

    real fTranslationStep;
    real fRotationStep;
    real fHomothetyStep;
    Camera * fMainCamera; // points to the camera of the viewer
    // at least one camera must exist
    Editable * fDefaultCamera;
    // use to rotate, can be NULL
    Editable * fDefault;

  protected :
    virtual Editable * InternalNewEditable(EditableID name);
  };

}

#endif
