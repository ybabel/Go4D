#include <math.h>
#include "string.h"
#include "EditablePhysic.h"

namespace Go4D
{


  EditorPhysic * EditablePhysic::GetEditorPhysic()
  {
    return _dynamic_cast_<EditorPhysic *>(GetEditor3D());
  }


  /******************************************************************************\
  * EditableGravity
  \******************************************************************************/

  RTTI_IMPL_3(EditableGravity, Editable, Editable3D, EditablePhysic);

  EditableGravity::EditableGravity()
  {
    fProperties.Add("G");
    fG = -0.5;
  }

  EditableID EditableGravity::TypeName() const
  {
    return _TypeName();
  }

  EditableID EditableGravity::_TypeName()
  {
    return "Gravity";
  }

  void EditableGravity::SetValue(EditableID propName, Variant value)
  {
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : fG = value.ToReal(); break;
    }
  }

  Variant EditableGravity::GetValue(EditableID propName) const
  {
    Variant result(0.0f);
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : result.fValue.fReal = fG; break;
    }
    return result;
  }

  void EditableGravity::ApplyAction(EditableID actionName, Variant value)
  {
  }

  void EditableGravity::ConvertInAction(ParticleArray * particles, ActionArray * actions)
  {
    Gravity * gravity = new Gravity(particles, fG);
    actions->Add(gravity);
  }

  /******************************************************************************\
  * EditableViscosity
  \******************************************************************************/

  RTTI_IMPL_3(EditableViscosity, Editable, Editable3D, EditablePhysic);

  EditableViscosity::EditableViscosity()
  {
    fProperties.Add("Coef");
    fCoef = 0.1;
  }

  EditableID EditableViscosity::TypeName() const
  {
    return _TypeName();
  }

  EditableID EditableViscosity::_TypeName()
  {
    return "Viscosity";
  }

  void EditableViscosity::SetValue(EditableID propName, Variant value)
  {
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : fCoef = value.ToReal(); break;
    }
  }

  Variant EditableViscosity::GetValue(EditableID propName) const
  {
    Variant result(0.0f);
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : result.fValue.fReal = fCoef; break;
    }
    return result;
  }

  void EditableViscosity::ApplyAction(EditableID actionName, Variant value)
  {
  }

  void EditableViscosity::ConvertInAction(ParticleArray * particles, ActionArray * actions)
  {
    Viscosity * viscosity = new Viscosity(particles, fCoef);
    actions->Add(viscosity);
  }

  /******************************************************************************\
  * EditableZMassModifier
  \******************************************************************************/

  RTTI_IMPL_3(EditableZMassModifier, Editable, Editable3D, EditablePhysic);

  EditableZMassModifier::EditableZMassModifier()
  {
    fProperties.Add("Coef");
    fProperties.Add("Z");
    fProperties.Add("Above");
    fCoef = 0.01;
    fZ = 1;
    fAbove = true;
  }

  EditableID EditableZMassModifier::TypeName() const
  {
    return _TypeName();
  }

  EditableID EditableZMassModifier::_TypeName()
  {
    return "ZMassModifier";
  }

  void EditableZMassModifier::SetValue(EditableID propName, Variant value)
  {
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : fCoef = value.ToReal(); break;
    case 1 : fZ = value.ToReal(); break;
    case 2 : fAbove = value.ToInt()==1; break; 
    }
  }

  Variant EditableZMassModifier::GetValue(EditableID propName) const
  {
    Variant result(0.0f);
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : result.fValue.fReal = fCoef; break;
    case 1 : result.fValue.fReal = fZ; break;
    case 2 : return Variant(int(fAbove)); break;
    }
    return result;
  }

  void EditableZMassModifier::ApplyAction(EditableID actionName, Variant value)
  {
  }

  void EditableZMassModifier::ConvertInAction(ParticleArray * particles, ActionArray * actions)
  {
    ZMassModifier * zmassmodifier = new ZMassModifier(particles, fCoef, fZ, fAbove);
    actions->Add(zmassmodifier);
  }


  /******************************************************************************\
  * EditableGround
  \******************************************************************************/

  RTTI_IMPL_3(EditableGround, Editable, Editable3D, EditablePhysic);

  EditableGround::EditableGround()
  {
    fProperties.Add("Z");
    fProperties.Add("Restitution");
    fProperties.Add("Frot");
    fZ = -2.0;
    fRestitution = 0.1;
    fFrot = 0.1;
  }

  EditableID EditableGround::TypeName() const
  {
    return _TypeName();
  }

  EditableID EditableGround::_TypeName()
  {
    return "Ground";
  }

  void EditableGround::SetValue(EditableID propName, Variant value)
  {
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : fZ = value.ToReal(); break;
    case 1 : fRestitution = value.ToReal(); break;
    case 2 : fFrot = value.ToReal(); break;
    }
  }

  Variant EditableGround::GetValue(EditableID propName) const
  {
    Variant result(0.0f);
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : result.fValue.fReal = fZ; break;
    case 1 : result.fValue.fReal = fRestitution; break;
    case 2 : result.fValue.fReal = fFrot; break;
    }
    return result;
  }

  void EditableGround::ApplyAction(EditableID actionName, Variant value)
  {
  }

  void EditableGround::ConvertInAction(ParticleArray * particles, ActionArray * actions)
  {
    Ground * ground = new Ground(particles, fZ, fRestitution, fFrot);
    actions->Add(ground);
  }

  /******************************************************************************\
  * EditableFixPosition
  \******************************************************************************/

  RTTI_IMPL_3(EditableFixPosition, Editable, Editable3D, EditablePhysic);

  EditableFixPosition::EditableFixPosition()
  {
    fProperties.Add("fixX");
    fProperties.Add("fixY");
    fProperties.Add("fixZ");
    fFixX = true;
    fFixY = true;
    fFixZ = true;
  }

  EditableID EditableFixPosition::TypeName() const
  {
    return _TypeName();
  }

  EditableID EditableFixPosition::_TypeName()
  {
    return "FixPosition";
  }

  void EditableFixPosition::SetValue(EditableID propName, Variant value)
  {
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : fFixX = value.ToInt()==1; break;
    case 1 : fFixY = value.ToInt()==1; break;
    case 2 : fFixZ = value.ToInt()==1; break;
    }
  }

  Variant EditableFixPosition::GetValue(EditableID propName) const
  {
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : return Variant(int(fFixX)); break;
    case 1 : return Variant(int(fFixY)); break;
    case 2 : return Variant(int(fFixZ)); break;
    }
    return Variant(0.0f);
  }

  void EditableFixPosition::ApplyAction(EditableID actionName, Variant value)
  {
  }

  void EditableFixPosition::AddLink(EditableID linkName, Editable *aEditable)
  {
    if (!_dynamic_cast_<EditablePlot *>(aEditable)) return;
    fLinks.Add(aEditable);
  }

  void EditableFixPosition::DelLink(EditableID linkName, Editable * aEditable)
  {
    fLinks.Del(aEditable);
  }

  void EditableFixPosition::ConvertInAction(ParticleArray * particles, ActionArray * actions)
  {
    for (int i=0; i<GetLinkCount(""); i++)
    {
      int pindex = GetEditorPhysic()->ParticleIndex(GetLink("", i));
      assert(pindex != NOTFOUND);
      FixPosition * fp = new FixPosition(particles, pindex);
      fp->fixX = fFixX;
      fp->fixY = fFixY;
      fp->fixZ = fFixZ;
      actions->Add(fp);
    }
  }

  /******************************************************************************\
  * EditableSpring
  \******************************************************************************/

  RTTI_IMPL_3(EditableSpring, Editable, Editable3D, EditablePhysic);

  EditableSpring::EditableSpring()
  {
    fProperties.Add("Spring");
    fProperties.Add("Damping");
    fProperties.Add("Len");

    fActions.Add("GetMinLen");
    fActions.Add("GetMaxLen");

    fSpring = 10.0;
    fDamping = 1.0;
    fLen = 1.0;

    fData = NULL;
  }

  EditableID EditableSpring::TypeName() const
  {
    return _TypeName();
  }

  EditableID EditableSpring::_TypeName()
  {
    return "Spring";
  }

  void EditableSpring::SetValue(EditableID propName, Variant value)
  {
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : fSpring = value.ToReal(); break;
    case 1 : fDamping = value.ToReal(); break;
    case 2 : fLen = value.ToReal(); break;
    }
  }

  Variant EditableSpring::GetValue(EditableID propName) const
  {
    Variant result(0.0f);
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : result.fValue.fReal = fSpring; break;
    case 1 : result.fValue.fReal = fDamping; break;
    case 2 : result.fValue.fReal = fLen; break;
    }
    return result;
  }

  void EditableSpring::ApplyAction(EditableID actionName, Variant value)
  {
    switch( fActions.IndexOf(actionName) )
    {
    case 0 : fLen = GetMinLen(); break;
    case 1 : fLen = GetMaxLen(); break;
    }
  }

  void EditableSpring::AddLink(EditableID linkName, Editable * aEditable)
  {
    if (!_dynamic_cast_<EditableSegment *>(aEditable)) return;
    fLinks.Add(aEditable);
  }

  void EditableSpring::DelLink(EditableID linkName, Editable * aEditable)
  {
    fLinks.Del(aEditable);
  }

  real EditableSpring::GetMinLen()
  {
    real result = FLT_MAX;
    for (int i=0; i<fLinks.Count(); i++)
    {
      APlot *alpha = _dynamic_cast_<EditableSegment*>(fLinks[i])->Alpha();
      APlot *omega = _dynamic_cast_<EditableSegment*>(fLinks[i])->Omega();
      real norme = Vector3(*alpha- *omega).Norme();
      if (norme<result) result=norme;
    }
    return result;
  }

  real EditableSpring::GetMaxLen()
  {
    real result = FLT_MIN;
    for (int i=0; i<fLinks.Count(); i++)
    {
      APlot *alpha = _dynamic_cast_<EditableSegment*>(fLinks[i])->Alpha();
      APlot *omega = _dynamic_cast_<EditableSegment*>(fLinks[i])->Omega();
      real norme = Vector3(*alpha- *omega).Norme();
      if (norme>result) result=norme;
    }
    return result;
  }

  void EditableSpring::ConvertInAction(ParticleArray * particles, ActionArray * actions)
  {
    for (int i=0; i<GetLinkCount(""); i++)
    {
      EditableSegment * es= _dynamic_cast_<EditableSegment*>(GetLink("", i));
      int alphaindex = GetEditorPhysic()->ParticleIndex(es->EditableAlpha());
      int omegaindex = GetEditorPhysic()->ParticleIndex(es->EditableOmega());
      Vector3 alphav = _dynamic_cast_<EditablePlot *>(es->EditableAlpha())->fBaseChangedPlot;
      Vector3 omegav = _dynamic_cast_<EditablePlot *>(es->EditableOmega())->fBaseChangedPlot;
      real norme = Vector3(alphav-omegav).Norme();
      assert(alphaindex != NOTFOUND);
      assert(omegaindex != NOTFOUND);
      Spring * spring = new Spring(particles, alphaindex, omegaindex, fSpring, fDamping, fLen*norme);
      actions->Add(spring);
      fData = (void *) spring;
    }
  }

  /******************************************************************************\
  * EditableMuscle
  \******************************************************************************/

  RTTI_IMPL_3(EditableMuscle, Editable, Editable3D, EditablePhysic);

  EditableMuscle::EditableMuscle()
  {
    fProperties.Add("ContracCoef");
    fProperties.Add("RelaxCoef");
    fProperties.Add("ContractTime");
    fProperties.Add("RelaxTime");
    fProperties.Add("StartTime");

    fContractCoef = 1.5;
    fRelaxCoef = 0.5;
    fContractTime = 256;
    fRelaxTime = 256;
    fStartTime = 0;
  }

  EditableID EditableMuscle::TypeName() const
  {
    return _TypeName();
  }

  EditableID EditableMuscle::_TypeName()
  {
    return "Muscle";
  }

  void EditableMuscle::SetValue(EditableID propName, Variant value)
  {
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : fContractCoef = value.ToReal(); break;
    case 1 : fRelaxCoef = value.ToReal(); break;
    case 2 : fContractTime = value.ToInt(); break;
    case 3 : fRelaxTime = value.ToInt(); break;
    case 4 : fStartTime = value.ToInt(); break;
    }
  }

  Variant EditableMuscle::GetValue(EditableID propName) const
  {
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : return Variant(fContractCoef);
    case 1 : return Variant(fRelaxCoef);
    case 2 : return Variant(fContractTime);
    case 3 : return Variant(fRelaxTime);
    case 4 : return Variant(fStartTime);
    default : return Variant(0.0f);
    }
  }

  void EditableMuscle::ApplyAction(EditableID actionName, Variant value)
  {
  }

  void EditableMuscle::DelLink(EditableID linkName, Editable * aEditable)
  {
    fLinks.Del(aEditable);
  }

  void EditableMuscle::AddLink(EditableID linkName, Editable * aEditable)
  {
    if (!_dynamic_cast_<EditableSpring *>(aEditable)) return;
    fLinks.Add(aEditable);
  }

  void EditableMuscle::ConvertInAction(ParticleArray * particles, ActionArray * actions)
  {
    for (int i=0; i<GetLinkCount(""); i++)
    {
      EditableSpring * s = _dynamic_cast_<EditableSpring *>(GetLink("", i));
      assert(s->fData != NULL); // Muscle must be created after (in the list) of the springs
      Spring * spring = (Spring *)s->fData;
      Muscle * muscle = new Muscle(particles, spring, fContractCoef, fRelaxCoef, fContractTime, fRelaxTime, fStartTime);
      actions->Add(muscle);
    }
  }

  /******************************************************************************\
  * EditablePhysic
  \******************************************************************************/

  RTTI_IMPL_2(EditorPhysic, Editor, Editor3D);

  EditorPhysic::EditorPhysic()
  {
    fTypes.Add(EditableGravity::_TypeName());
    fTypes.Add(EditableGround::_TypeName());
    fTypes.Add(EditableViscosity::_TypeName());
    fTypes.Add(EditableFixPosition::_TypeName());
    fTypes.Add(EditableSpring::_TypeName());
    fTypes.Add(EditableMuscle::_TypeName());
    fTypes.Add(EditableZMassModifier::_TypeName());
    fParticles = NULL;
    fActions = NULL;
    fSystem = NULL;
    fParticles = new ParticleArray();
    fActions = new ActionArray();
    fSystem = new ParticleSystem(fParticles, fActions, 0.02);
  }

  EditorPhysic::~EditorPhysic()
  {
    delete fParticles;
    delete fActions;
    delete fSystem;
  }

  void EditorPhysic::Init()
  {
    fParticles->Clear();
    fActions->Clear();
    LookAndConvertParticles(Base(), fRoot);
    fParticles->Complete();
    LookAndConvertActions(Base(), fRoot);
    fActions->Complete();
    fSystem->Init();
  }

  void EditorPhysic::LookAndConvertParticles(Base aBase, Editable * aEditable)
  {
    aEditable->Refresh();
    if (_dynamic_cast_<EditableObject *>(aEditable))
    {
      aBase.AddBase(* _dynamic_cast_<EditableObject *>(aEditable) );
    }
    if (_dynamic_cast_<EditGroup *>(aEditable))
    {
      EditGroup * curGroup = _dynamic_cast_<EditGroup *>(aEditable);
      curGroup->First();
      while (curGroup->Current() != NULL)
      {
        LookAndConvertParticles(aBase, curGroup->Current());
        curGroup->Next();
      }
    }
    else if (_dynamic_cast_<EditablePlot *>(aEditable))
    {
      _dynamic_cast_<EditablePlot *>(aEditable)->fBaseChangedPlot
        = aBase.ChangeCoord(*_dynamic_cast_<APlot *>(aEditable));
      Particle * particle = new Particle;
      particle->fPosition = aBase.ChangeCoord(*_dynamic_cast_<APlot *>(aEditable));
      particle->fPosition.GetColor() = _dynamic_cast_<APlot *>(aEditable)->GetColor();
      particle->fMass = _dynamic_cast_<EditablePlot *>(aEditable)->GetMass();
      particle->fData = (void *)_dynamic_cast_<Editable *>(aEditable);
      fParticles->Add(particle);
    }
  }

  void EditorPhysic::LookAndConvertActions(Base aBase, Editable * aEditable)
  {
    aEditable->Refresh();
    if (_dynamic_cast_<EditGroup *>(aEditable))
    {
      EditGroup * curGroup = _dynamic_cast_<EditGroup *>(aEditable);
      curGroup->First();
      while (curGroup->Current() != NULL)
      {
        LookAndConvertActions(aBase, curGroup->Current());
        curGroup->Next();
      }
    }
    else if (_dynamic_cast_<EditablePhysic *>(aEditable))
    {
      _dynamic_cast_<EditablePhysic *>(aEditable)->ConvertInAction(fParticles, fActions);
    }
  }

  int EditorPhysic::ParticleIndex(Editable * aEditable)
  {
    for (int i=0; i<fParticles->Count(); i++)
      if ( (*fParticles)[i]->fData == (void *)_dynamic_cast_<Editable *>(aEditable))
        return i;
    return NOTFOUND;
  }

  Editable * EditorPhysic::InternalNewEditable(EditableID name)
  {
    Editable * result=NULL;
    result = inherited::InternalNewEditable(name);
    if (result != NULL) return result;
    switch (fTypes.IndexOf(name))
    {
    case 7 : return new EditableGravity();
    case 8 : return new EditableGround();
    case 9 : return new EditableViscosity();
    case 10 : return new EditableFixPosition();
    case 11 : return new EditableSpring();
    case 12 : return new EditableMuscle();
    case 13 : return new EditableZMassModifier();
    default : return NULL;
    }
  }

}
