#ifndef _EDITABLEPHYSIC_H_
#define _EDITABLEPHYSIC_H_

#include "Editable3D.h"
#include "Physic/Particle.h"
#include "Physic/Actions.h"

namespace Go4D
{


  class EditorPhysic;

  /// specific editables for physical objects
  class EditablePhysic : public Editable3D
  {
    friend class EditorPhysic;

  public :
    /// convert the editable is actions. Note that one EditablePhysic can be
    /// converted in many actions that are stored in the actions array.
    /// This is more powerfull than return an action because on editable can
    /// be converted in many actions. For example you can link a string to
    /// many segments, and assign the same properties to those springs.
    virtual void ConvertInAction(ParticleArray * particles, ActionArray * actions)=0;
    EditorPhysic * GetEditorPhysic();
  };

  /// an editable gravity object
  class EditableGravity : public EditablePhysic
  {
    typedef EditablePhysic inherited;
  public :
    RTTI_DECL;
    EditableGravity();

    virtual EditableID TypeName() const;
    static EditableID _TypeName();
    virtual void SetValue(EditableID propName, Variant );
    virtual Variant GetValue(EditableID propName) const;
    virtual void ApplyAction(EditableID actionName, Variant value);
    virtual void ConvertInAction(ParticleArray * particles, ActionArray * actions);

  public :
    real fG;
  };

  /// an editable ground
  class EditableGround : public EditablePhysic
  {
    typedef EditablePhysic inherited;
  public :
    RTTI_DECL;
    EditableGround();
    virtual EditableID TypeName() const;
    static EditableID _TypeName();
    virtual void SetValue(EditableID propName, Variant );
    virtual Variant GetValue(EditableID propName) const;
    virtual void ApplyAction(EditableID actionName, Variant value);
    virtual void ConvertInAction(ParticleArray * particles, ActionArray * actions);

  public :
    real fZ;
    real fRestitution;
    real fFrot;
  };

  /// an eidtable viscosity
  class EditableViscosity : public EditablePhysic
  {
    typedef EditablePhysic inherited;
  public :
    RTTI_DECL;
    EditableViscosity();
    virtual EditableID TypeName() const;
    static EditableID _TypeName();
    virtual void SetValue(EditableID propName, Variant );
    virtual Variant GetValue(EditableID propName) const;
    virtual void ApplyAction(EditableID actionName, Variant value);
    virtual void ConvertInAction(ParticleArray * particles, ActionArray * actions);

  public :
    real fCoef;
  };

  /// an editable ZMassModifier
  class EditableZMassModifier : public EditablePhysic
  {
    typedef EditablePhysic inerhited;
  public :
    RTTI_DECL;
    EditableZMassModifier();
    virtual EditableID TypeName() const;
    static EditableID _TypeName();
    virtual void SetValue(EditableID propName, Variant );
    virtual Variant GetValue(EditableID propName) const;
    virtual void ApplyAction(EditableID actionName, Variant value);
    virtual void ConvertInAction(ParticleArray * particles, ActionArray * actions);

  protected :
    real fCoef;
    real fZ;
    bool fAbove;
  };

  /// an editable fix position
  class EditableFixPosition : public EditablePhysic
  {
    typedef EditablePhysic inherited;
  public :
    RTTI_DECL;
    EditableFixPosition();
    virtual EditableID TypeName() const;
    static EditableID _TypeName();
    virtual void SetValue(EditableID propName, Variant );
    virtual Variant GetValue(EditableID propName) const;
    virtual void ApplyAction(EditableID actionName, Variant value);
    virtual void DelLink(EditableID linkName, Editable *);
    virtual void AddLink(EditableID linkName, Editable *);
    virtual void ConvertInAction(ParticleArray * particles, ActionArray * actions);

  protected :
    bool fFixX;
    bool fFixY;
    bool fFixZ;
  };

  /// an editable spring
  class EditableSpring : public EditablePhysic
  {
    typedef EditablePhysic inherited;
  public :
    RTTI_DECL;
    EditableSpring();
    virtual EditableID TypeName() const;
    static EditableID _TypeName();
    virtual void SetValue(EditableID propName, Variant );
    virtual Variant GetValue(EditableID propName) const;
    virtual void ApplyAction(EditableID actionName, Variant value);
    virtual void DelLink(EditableID linkName, Editable *);
    virtual void AddLink(EditableID linkName, Editable *);
    virtual void ConvertInAction(ParticleArray * particles, ActionArray * actions);

  public :
    real fDamping;
    real fSpring;
    real fLen;
    // work only if the plots of the sergments are defined in the same base !!!
    real GetMinLen();
    real GetMaxLen();
  };

  /// an eidtable muscle
  class EditableMuscle : public EditablePhysic
  {
    typedef EditablePhysic inherited;
  public :
    RTTI_DECL;
    EditableMuscle();
    virtual EditableID TypeName() const;
    static EditableID _TypeName();
    virtual void SetValue(EditableID propName, Variant );
    virtual Variant GetValue(EditableID propName) const;
    virtual void ApplyAction(EditableID actionName, Variant value);
    virtual void DelLink(EditableID linkName, Editable *);
    virtual void AddLink(EditableID linkName, Editable *);
    virtual void ConvertInAction(ParticleArray * particles, ActionArray * actions);

  public :
    real fContractCoef;
    real fRelaxCoef;
    int fContractTime;
    int fRelaxTime;
    int fStartTime;
  };

  /// specific code for physical editable objets (map strings to class)
  class EditorPhysic : public Editor3D
  {
    typedef Editor3D inherited;
  public :
    RTTI_DECL;
    EditorPhysic();
    ~EditorPhysic();

    /// must be calle at least once before using ProcessTimeStep
    /// this method copy the positions of all the plots in the particle array
    /// and init the forces tambien, by looking in the tree
    /// All plots are converted in particles
    void Init();

    ParticleArray * fParticles;
    ActionArray * fActions;
    ParticleSystem * fSystem;
    int ParticleIndex(Editable *);

  protected :
    // scan the list of plots in the editable objects hierarchy and convert them
    // into particles with mass is fixed to 4 for the moment and which position
    // is the position of the plot.
    void LookAndConvertParticles(Base, Editable *);
    // scan the list of editables and convert those which are editable physics
    // in actions.
    void LookAndConvertActions(Base, Editable *);
    virtual Editable * InternalNewEditable(EditableID name);
  };

}

#endif
