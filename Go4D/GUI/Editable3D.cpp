#include <math.h>
#include "string.h"
#include "Editable3D.h"

namespace Go4D
{


  /******************************************************************************\
  * Editable3D
  \******************************************************************************/

  RTTI_IMPL_1(Editable3D, Editable);

  Editor3D * Editable3D::GetEditor3D()
  {
    return _dynamic_cast_<Editor3D *>(fEditor);
  }

  APlot Editable3D::GetDefaultPlot()
  {
    if (GetEditor3D()->fDefault == NULL)
      return APlot(0,0,0);
    else if (_dynamic_cast_<EditablePlot *>(GetEditor3D()->fDefault)==NULL)
      return APlot(0,0,0);
    else
      return *_dynamic_cast_<EditablePlot *>(GetEditor3D()->fDefault);
  }

  /******************************************************************************\
  * EditablePlot
  \******************************************************************************/

  RTTI_IMPL_4 (EditablePlot, Editable, Editable3D, APlot, Vector3);

  EditableID EditablePlot::_TypeName()
  {
    return "Plot";
  }

  EditableID EditablePlot::TypeName() const
  {
    return _TypeName();
  }

  EditablePlot::EditablePlot()
  {
    fProperties.Add("X");
    fProperties.Add("Y");
    fProperties.Add("Z");
    fProperties.Add("Color");
    fProperties.Add("Mass");

    fActions.Add("TranslateX");
    fActions.Add("TranslateY");
    fActions.Add("TranslateZ");

    fActions.Add("RotateX");
    fActions.Add("RotateY");
    fActions.Add("RotateZ");

    fActions.Add("Homothety");
    fMass = 1.0;
  }

  void EditablePlot::SetValue(EditableID propName, Variant value)
  {
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : fElems[0] = value.ToReal(); break;
    case 1 : fElems[1] = value.ToReal(); break;
    case 2 : fElems[2] = value.ToReal(); break;
    case 3 : fColor = value.ToInt(); break;
    case 4 : fMass = value.ToReal(); break;
    }
  }

  Variant EditablePlot::GetValue(EditableID propName) const
  {
    Variant result(0.0f);
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : result.fValue.fReal = fElems[0]; break;
    case 1 : result.fValue.fReal = fElems[1]; break;
    case 2 : result.fValue.fReal = fElems[2]; break;
    case 3 : result = Variant(fColor); break;
    case 4 : result.fValue.fReal = fMass; break;
    }
    return result;
  }

  void EditablePlot::ApplyAction(EditableID actionName, Variant value)
  {
    real r = value.ToReal();
    real ts = GetEditor3D()->fTranslationStep;
    real rs = GetEditor3D()->fRotationStep;
    real hs = GetEditor3D()->fHomothetyStep;
    Vector3 tmp=*this;
    Rotation rot;
    bool rotate = false;
    bool homothete = false;
    switch( fActions.IndexOf(actionName) )
    {
    case 0 : fElems[0] += r*ts; break;
    case 1 : fElems[1] += r*ts; break;
    case 2 : fElems[2] += r*ts; break;
    case 3 : rot.RotateX(r*rs); rotate = true; break;
    case 4 : rot.RotateY(r*rs); rotate = true; break;
    case 5 : rot.RotateZ(r*rs); rotate = true; break;
    case 6 : homothete = true; break;
    }
    if (rotate)
    {
      tmp = (rot*(tmp-GetDefaultPlot()))+GetDefaultPlot();
      *_dynamic_cast_<APlot * >(this) = tmp;
    }
    if (homothete)
    {
      tmp = (tmp-GetDefaultPlot())*(1.0f+hs*r) + GetDefaultPlot();
      *_dynamic_cast_<APlot * >(this) = tmp;
    }
  }

  /******************************************************************************\
  * EditableBarycenter
  \******************************************************************************/

  RTTI_IMPL_5(EditableBarycenter, Editable, Editable3D, EditablePlot, APlot, Vector3);

  EditableID EditableBarycenter::TypeName() const
  {
    return _TypeName();
  }

  EditableID EditableBarycenter::_TypeName()
  {
    return "Barycenter";
  }

  void EditableBarycenter::AddLink(EditableID linkName, Editable * aEditable)
  {
    if (!_dynamic_cast_<APlot *>(aEditable)) return;
    fLinks.Add(aEditable);
    CalculatePosition();
  }

  void EditableBarycenter::DelLink(EditableID linkName, Editable * aEditable)
  {
    if (!_dynamic_cast_<APlot *>(aEditable)) return;
    fLinks.Del(aEditable);
    CalculatePosition();
  }

  void EditableBarycenter::Refresh()
  {
    CalculatePosition();
  }

  void EditableBarycenter::CalculatePosition()
  {
    int count = 0;
    Vector3 position(0,0,0);
    fLinks.First();
    while (fLinks.Current() != NULL)
    {
      count++;
      position += *_dynamic_cast_<Vector3 *>(fLinks.Current());
      fLinks.Next();
    }
    if (count!=0)
      position = position/real(count);
    (*(APlot*)this) = position;
  }

  /******************************************************************************\
  * EditableCamera
  \******************************************************************************/

  RTTI_IMPL_3(EditableCamera, Editable, Editable3D, Camera);

  EditableCamera::EditableCamera(int width, int height)
    : Camera(width, height)
  {
    fProperties.Add("X");
    fProperties.Add("Y");
    fProperties.Add("Z");
    fProperties.Add("Theta");
    fProperties.Add("Phi");
    fProperties.Add("Psi");
    fProperties.Add("Zoom");
    fProperties.Add("Focal");
    fProperties.Add("Cutting Height");

    fActions.Add("TranslateX");
    fActions.Add("TranslateY");
    fActions.Add("TranslateZ");

    fActions.Add("RotateX");
    fActions.Add("RotateY");
    fActions.Add("RotateZ");
  }

  EditableID EditableCamera::TypeName() const
  {
    return _TypeName();
  }

  EditableID EditableCamera::_TypeName()
  {
    return "Camera";
  }

  void EditableCamera::SetValue(EditableID propName, Variant value)
  {
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : fTranslation[0] = value.ToReal(); break;
    case 1 : fTranslation[1] = value.ToReal(); break;
    case 2 : fTranslation[2] = value.ToReal(); break;
    case 3 : fTet = value.ToReal(); break;
    case 4 : fPhi = value.ToReal(); break;
    case 5 : fPsi = value.ToReal(); break;
    case 6 : fZoom = value.ToReal(); break;
    case 7 : fFocal = value.ToReal(); break;
    case 8 : fCutPlaneHeight = value.ToReal(); break;
    }
    *(GetEditor3D()->fMainCamera) = *this;
  }

  Variant EditableCamera::GetValue(EditableID propName) const
  {
    Variant result(0.0f);
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : result.fValue.fReal = fTranslation[0]; break;
    case 1 : result.fValue.fReal = fTranslation[1]; break;
    case 2 : result.fValue.fReal = fTranslation[2]; break;
    case 3 : result.fValue.fReal = fTet; break;
    case 4 : result.fValue.fReal = fPhi; break;
    case 5 : result.fValue.fReal = fPsi; break;
    case 6 : result.fValue.fReal = fZoom; break;
    case 7 : result.fValue.fReal = fFocal; break;
    case 8 : result.fValue.fReal = fCutPlaneHeight; break;
    }
    return result;
  }

  void EditableCamera::ApplyAction(EditableID actionName, Variant value)
  {
    real r = value.ToReal();
    real ts = GetEditor3D()->fTranslationStep;
    real rs = GetEditor3D()->fRotationStep;
    switch( fActions.IndexOf(actionName) )
    {
    case 0 : TranslateX(r*ts); break;
    case 1 : TranslateY(r*ts); break;
    case 2 : TranslateZ(r*ts); break;

    case 3 : RotateX(r*rs); break;
    case 4 : RotateY(r*rs); break;
    case 5 : RotateZ(r*rs); break;
    }
    *(GetEditor3D()->fMainCamera) = *this;
  }

  void EditableCamera::Refresh()
  {
    if (fSelected)
      *(GetEditor3D()->fMainCamera) = *this;
  }

  /******************************************************************************\
  * EditableObject
  \******************************************************************************/

  RTTI_IMPL_4 (EditableObject, Editable, Editable3D, EditGroup, EditNode);

  EditableObject::EditableObject()
  {
    fProperties.Add("X");
    fProperties.Add("Y");
    fProperties.Add("Z");
    fProperties.Add("Theta");
    fProperties.Add("Phi");
    fProperties.Add("Psi");
    fProperties.Add("SizeX");
    fProperties.Add("SizeY");
    fProperties.Add("SizeZ");
    fProperties.Add("Name");

    fActions.Add("TranslateX");
    fActions.Add("TranslateY");
    fActions.Add("TranslateZ");

    fActions.Add("RotateX");
    fActions.Add("RotateY");
    fActions.Add("RotateZ");

    fSizeX = 1.0f;
    fSizeY = 1.0f;
    fSizeZ = 1.0f;
    ClearStr(fName);
  }

  EditableID EditableObject::TypeName() const
  {
    return _TypeName();
  }

  EditableID EditableObject::_TypeName()
  {
    return "Object";
  }

  void EditableObject::SetValue(EditableID propName, Variant value)
  {
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : origin[0] = value.ToReal(); break;
    case 1 : origin[1] = value.ToReal(); break;
    case 2 : origin[2] = value.ToReal(); break;
    case 3 : fTet = value.ToReal(); break;
    case 4 : fPhi = value.ToReal(); break;
    case 5 : fPsi = value.ToReal(); break;
    case 6 : fSizeX = value.ToReal(); break;
    case 7 : fSizeY = value.ToReal(); break;
    case 8 : fSizeZ = value.ToReal(); break;
    case 9 : CopyStr(fName, value.ToString()); break;
    }
    matrx.RotateEuler(fTet, fPhi, fPsi);
  }

  Variant EditableObject::GetValue(EditableID propName) const
  {
    Variant result(0.0f);
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : result.fValue.fReal = origin[0]; break;
    case 1 : result.fValue.fReal = origin[1]; break;
    case 2 : result.fValue.fReal = origin[2]; break;
    case 3 : result.fValue.fReal = fTet; break;
    case 4 : result.fValue.fReal = fPhi; break;
    case 5 : result.fValue.fReal = fPsi; break;
    case 6 : result.fValue.fReal = fSizeX; break;
    case 7 : result.fValue.fReal = fSizeY; break;
    case 8 : result.fValue.fReal = fSizeZ; break;
    case 9 : return Variant(fName);
    }
    return result;
  }

  void EditableObject::ApplyAction(EditableID actionName, Variant value)
  {
    real r = value.ToReal();
    real ts = GetEditor3D()->fTranslationStep;
    real rs = GetEditor3D()->fRotationStep;
    switch( fActions.IndexOf(actionName) )
    {
    case 0 : origin[0] += r*ts; break;
    case 1 : origin[1] += r*ts; break;
    case 2 : origin[2] += r*ts; break;

    case 3 : fTet += r*rs; break;
    case 4 : fPhi += r*rs; break;
    case 5 : fPsi += r*rs; break;
    }
    matrx.RotateEuler(fTet, fPhi, fPsi);
  }

  void EditableObject::Refresh()
  {
  }

  void EditableObject::SetSelected(bool selected)
  {
    inherited::SetSelected(selected);
    EditGroup::SetSelected(selected);
  }

  /******************************************************************************\
  * EditableSegment
  \******************************************************************************/

  RTTI_IMPL_2(EditableSegment, Editable, Editable3D);

  EditableSegment::EditableSegment()
  {
  }

  EditableID EditableSegment::TypeName() const
  {
    return _TypeName();
  }

  EditableID EditableSegment::_TypeName()
  {
    return "Segment";
  }

  void EditableSegment::SetValue(EditableID propName, Variant )
  {
  }

  Variant EditableSegment::GetValue(EditableID propName) const
  {
    return Variant(0.0f);
  }

  void EditableSegment::ApplyAction(EditableID actionName, Variant value)
  {
  }

  void EditableSegment::AddLink(EditableID linkName, Editable * aEditable)
  {
    if (!_dynamic_cast_<APlot *>(aEditable)) return;
    if (fLinks.Count() == 2) return;
    fLinks.Add(aEditable);
  }

  void EditableSegment::DelLink(EditableID linkName, Editable * aEditable)
  {
    fLinks.Del(aEditable);
  }

  void EditableSegment::Refresh()
  {
  }

  APlot * EditableSegment::Alpha() const
  {
    return _dynamic_cast_<APlot *>(fLinks[0]);
  }

  APlot * EditableSegment::Omega() const
  {
    return _dynamic_cast_<APlot *>(fLinks[1]);
  }

  Editable * EditableSegment::EditableAlpha() const
  {
    return _dynamic_cast_<Editable *>(fLinks[0]);
  }

  Editable * EditableSegment::EditableOmega() const
  {
    return _dynamic_cast_<Editable *>(fLinks[1]);
  }

  /******************************************************************************\
  * EditableFace
  \******************************************************************************/

  RTTI_IMPL_2(EditableFace, Editable, Editable3D);

  EditableFace::EditableFace()
  {
    fProperties.Add("Color");
    fColor = 0;
  }

  EditableID EditableFace::TypeName() const
  {
    return _TypeName();
  }

  EditableID EditableFace::_TypeName()
  {
    return "Face";
  }

  void EditableFace::SetValue(EditableID propName, Variant value)
  {
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : fColor = value.ToInt(); break;
    }
  }

  Variant EditableFace::GetValue(EditableID propName) const
  {
    Variant result(0.0f);
    switch( fProperties.IndexOf(propName) )
    {
    case 0 : result = Variant(fColor); break;
    }
    return result;
  }

  void EditableFace::ApplyAction(EditableID actionName, Variant value)
  {
  }

  void EditableFace::AddLink(EditableID linkName, Editable *aEditable)
  {
    if (!_dynamic_cast_<APlot *>(aEditable)) return;
    if (fLinks.Count() == 4) return;
    fLinks.Add(aEditable);
  }

  void EditableFace::DelLink(EditableID linkName, Editable *aEditable)
  {
    fLinks.Del(aEditable);
  }

  void EditableFace::Refresh()
  {
  }

  int EditableFace::PlotCount() const
  {
    return fLinks.Count();
  }

  APlot * EditableFace::GetPlot(int i) const
  {
    return _dynamic_cast_<APlot *>(fLinks[i]);
  }

  EditablePlot * EditableFace::GetEditablePlot(int i) const
  {
    return _dynamic_cast_<EditablePlot *>(fLinks[i]);
  }

  /******************************************************************************\
  * Editor3D
  \******************************************************************************/

  RTTI_IMPL_1(Editor3D,Editor);

  Editor3D::Editor3D()
  {
    fTranslationStep = 0.1;
    fRotationStep = M_PI/8.0;
    fHomothetyStep = 0.1;
    fTypes.Add(EditablePlot::_TypeName());
    fTypes.Add(DefaultEditGroup::_TypeName());
    fTypes.Add(EditableBarycenter::_TypeName());
    fTypes.Add(EditableCamera::_TypeName());
    fTypes.Add(EditableObject::_TypeName());
    fTypes.Add(EditableSegment::_TypeName());
    fTypes.Add(EditableFace::_TypeName());
  }

  Editor3D::~Editor3D()
  {
  }

  Editable * Editor3D::InternalNewEditable(EditableID name)
  {
    Editable * result=NULL;
    switch (fTypes.IndexOf(name))
    {
    case 0 : return new EditablePlot();
    case 1 : return new DefaultEditGroup();
    case 2 : return new EditableBarycenter();
    case 3 :
      result = new EditableCamera(fMainCamera->ScreenWidth(), fMainCamera->ScreenHeight());
      *_dynamic_cast_<Camera*>(result) = *fMainCamera;
      return result;
    case 4 : return new EditableObject();
    case 5 : return new EditableSegment();
    case 6 : return new EditableFace();
    default : return NULL;
    }
  }

  bool Editor3D::IsClearable()
  {
    if (fCurrent == fDefaultCamera) return false;
    return inherited::IsClearable();
  }

}
