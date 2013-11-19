#include "3DView.h"

#include "Graph3D/Face.h"
#include "Platform/Device.h"

namespace Go4D
{

  namespace GUI
  {


#define SIZE 200

    Modeler3DView::Modeler3DView(System * gui, const char * name, Device * dev)
      : inherited(gui, name),
//#ifndef UNDER_CE
//      fImage(dynamic_cast<ZImage *>(FinalImage())),
//#else
      fImage(reinterpret_cast<ZImage *>(FinalImage())),
//#endif
      fCamera(fImage->Width(), fImage->Height()),
      drawer(*fImage, false),
      font("font6x10.fon"),
      writer(*fImage, 0, true, 255),
      enhancedDrawer(*fImage, 1),
      faceDrawer(*fImage, 4, NULL)
    {
      fIsSimulating = false;
      fEnhanceSelection = false;
      fDrawPlotCoord = false;
      fDrawPlotNumbers = true;
      fDrawPlots = true;
      fDrawSegments = true;

      fCamera.SetTranslation(0,0,-4);
      fCamera.SetZoom(2);
      fCamera.SetRotation(M_PI/2.0,0,0);
      fDevice = dev;
      fDevice->StartChrono();

      fEditor.fMainCamera = &fCamera;
      fEditor.fDefaultCamera = new EditableCamera(fCamera.ScreenWidth(), fCamera.ScreenHeight());
      fEditor.SetEditor(fEditor.fDefaultCamera);
      *_dynamic_cast_<Camera*>(fEditor.fDefaultCamera) = fCamera;
    }

    Modeler3DView::~Modeler3DView()
    {
    }

    ANode * Modeler3DView::MouseEvent(MouseEventKind kind, int mousecode, real dx, real dy)
    {
      if (kind==mekDragOver)
      {
        if ( (mousecode & kMRIGHT) != 0 )
        {
          if ( (mousecode & kShift) != 0 )
            fEditor.ApplyAction("RotateZ", Variant(dy));
          else
          {
            fEditor.ApplyAction("RotateX", Variant(dy));
            fEditor.ApplyAction("RotateY", Variant(-dx));
          }
        }
        else
        {
          if ( (mousecode & kShift) != 0 )
            fEditor.ApplyAction("TranslateZ", Variant(dy));
          else
          {
            fEditor.ApplyAction("TranslateX", Variant(dx));
            fEditor.ApplyAction("TranslateY", Variant(dy));
          }
        }
      }
      return this;
    }

    void Modeler3DView::DrawPass(Image *aImage, real x, real y, real width, real height)
    {
      drawer.Plug(*aImage);
      writer.Plug(*aImage);
      enhancedDrawer.Plug(*aImage);
      faceDrawer.Plug(*aImage);
      drawer.SetClipZone(Trunc(x), Trunc(y), Trunc(width), Trunc(height));
      writer.SetClipZone(Trunc(x), Trunc(y), Trunc(width), Trunc(height));
      // becareful the enhanced drawer add 1 pixel before and aster
      enhancedDrawer.SetClipZone(Trunc(x)+1, Trunc(y)+1, Trunc(width)-1, Trunc(height)-1);
      faceDrawer.SetClipZone(Trunc(x), Trunc(y), Trunc(width), Trunc(height));
      fCamera.SetScreen(Trunc(x), Trunc(y), Trunc(width), Trunc(height));
      fCamera.SetScreenCenter(Trunc(width/2),Trunc((height*3)/4));
      Draw(Trunc(x), Trunc(y), Trunc(x+width), Trunc(y+height));
    }

    void Modeler3DView::ComputeBaseChange(Base aBase, Editable *aEditable)
    {
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
          ComputeBaseChange(aBase, curGroup->Current());
          curGroup->Next();
        }
      }
      else if (_dynamic_cast_<EditablePlot *>(aEditable) && fDrawPlots)
      {
        _dynamic_cast_<EditablePlot *>(aEditable)->fBaseChangedPlot
          = ChangeBase(aBase, *_dynamic_cast_<APlot *>(aEditable));
      }
    }

    void Modeler3DView::DrawEditable(Editable * aEditable, bool enhanceSelection)
    {
      aEditable->Refresh();
      if (_dynamic_cast_<EditGroup *>(aEditable))
      {
        EditGroup * curGroup = _dynamic_cast_<EditGroup *>(aEditable);
        curGroup->First();
        while (curGroup->Current() != NULL)
        {
          DrawEditable(curGroup->Current(), enhanceSelection);
          curGroup->Next();
        }
      }
      else if (_dynamic_cast_<EditableSegment *>(aEditable) && fDrawSegments)
      {
        Editable * eAlpha = _dynamic_cast_<EditableSegment *>(aEditable)->EditableAlpha();
        Editable * eOmega = _dynamic_cast_<EditableSegment *>(aEditable)->EditableOmega();
        if ( (eAlpha==NULL) || (eOmega==NULL) ) return;

        APlot alpha = _dynamic_cast_<EditablePlot *>(eAlpha)->fBaseChangedPlot;
        APlot omega = _dynamic_cast_<EditablePlot *>(eOmega)->fBaseChangedPlot;
        PointZ alphaZ = GetInCameraSystem(alpha);
        PointZ omegaZ = GetInCameraSystem(omega);
        if ( fCamera.IsVisible(alphaZ) && fCamera.IsVisible(omegaZ) )
        {
          if (aEditable->Selected()&&enhanceSelection)
            enhancedDrawer.Line(alphaZ, omegaZ);
          else
            drawer.Line(alphaZ, omegaZ);
          if (fDrawPlotNumbers)
          {
            fstring number;
            PointZ curZ=alphaZ;
            curZ.x += omegaZ.x;
            curZ.y += omegaZ.y;
            curZ.z += omegaZ.z;
            curZ.x /= 2;
            curZ.y /= 2;
            curZ.z /= 2;
            curZ.color = byte(curZ.color+64);
            Int2Str(aEditable->ID(), number);
            writer.WriteString(curZ, font, number);
          }
        }
      }
      else if (_dynamic_cast_<EditablePlot *>(aEditable) && fDrawPlots)
      {
        PointZ curZ = GetInCameraSystem(_dynamic_cast_<EditablePlot *>(aEditable)->fBaseChangedPlot);
        if (aEditable->Selected())
          curZ.color = byte(curZ.color+64);
        if (fCamera.IsVisible(curZ))
        {

          int size = int(fCamera.ProjectSize(curZ, 0.05)*_dynamic_cast_<EditablePlot *>(aEditable)->GetMass() )+1;
          if (aEditable->Selected())
            size+=(enhanceSelection?2:0);
          drawer.FillCircle(curZ, size);
          if (aEditable->Selected()&&fDrawPlotCoord)
          {
            APlot editable = _dynamic_cast_<EditablePlot *>(aEditable)->fBaseChangedPlot;
            APlot tmpPlot;

            tmpPlot=editable;
            tmpPlot[2] = 0.0;
            PointZ curZonXY = GetInCameraSystem(tmpPlot);
            tmpPlot[1] = 0.0;
            PointZ curZonX = GetInCameraSystem(tmpPlot);

            tmpPlot=editable;
            tmpPlot[1] = 0.0;
            PointZ curZonXZ = GetInCameraSystem(tmpPlot);
            tmpPlot[0] = 0.0;
            PointZ curZonZ = GetInCameraSystem(tmpPlot);

            tmpPlot=editable;
            tmpPlot[0] = 0.0;
            PointZ curZonYZ = GetInCameraSystem(tmpPlot);
            tmpPlot[2] = 0.0;
            PointZ curZonY = GetInCameraSystem(tmpPlot);

            tmpPlot[1] = 0.0;
            PointZ origin = GetInCameraSystem(tmpPlot);

            DrawLine(curZ, curZonXY);
            DrawLine(curZ, curZonXZ);
            DrawLine(curZ, curZonYZ);

            DrawLine(curZonXY, curZonX);
            DrawLine(curZonXY, curZonY);
            DrawLine(curZonXZ, curZonX);
            DrawLine(curZonXZ, curZonZ);
            DrawLine(curZonYZ, curZonY);
            DrawLine(curZonYZ, curZonZ);

            DrawLine(origin, curZonX);
            DrawLine(origin, curZonY);
            DrawLine(origin, curZonZ);
          }
          if (fDrawPlotNumbers)
          {
            fstring number;
            curZ.color = byte(curZ.color+64);
            curZ.x += size;
            Int2Str(aEditable->ID(), number);
            writer.WriteString(curZ, font, number);
          }
        }
      }
      else if (_dynamic_cast_<EditableFace *>(aEditable)&&fDrawFaces)
      {
        EditableFace * aface = _dynamic_cast_<EditableFace *>(aEditable);
        if (aface->PlotCount() != 4) return;
        APlot tmp0 = fCamera.ChangeToBase(aface->GetEditablePlot(0)->fBaseChangedPlot);
        APlot tmp1 = fCamera.ChangeToBase(aface->GetEditablePlot(1)->fBaseChangedPlot);
        APlot tmp2 = fCamera.ChangeToBase(aface->GetEditablePlot(2)->fBaseChangedPlot);
        APlot tmp3 = fCamera.ChangeToBase(aface->GetEditablePlot(3)->fBaseChangedPlot);
        FlatFace flatface(&faceDrawer,
          tmp0, tmp1, tmp2, tmp3,
          aface->fColor, 255, true);
        flatface.SimpleDraw(fCamera);
      }
    }

    void Modeler3DView::DrawSystem()
    {
      int i;
      // draw springs (not optimzed for the moment)
      for (i=0; i<fEditor.fActions->Count(); i++)
      {
        PAction curAction = (*fEditor.fActions)[i];
        if (_dynamic_cast_<LinkForce *>(curAction))
        {
          LinkForce * curLink = _dynamic_cast_<LinkForce *>(curAction);
          PointZ curZA = GetInCameraSystem((*fEditor.fParticles)[curLink->ParticleAlpha()]->fPosition);
          PointZ curZB = GetInCameraSystem((*fEditor.fParticles)[curLink->ParticleOmega()]->fPosition);
          if ((curZA.z > 0) && (curZB.z>0))
            drawer.Line(curZA, curZB);
        }
      }

      // draw particles
      for (i=0; i<fEditor.fParticles->Count(); i++)
      {
        Particle * curParticle = (*fEditor.fParticles)[i];
        PointZ curZ = GetInCameraSystem(curParticle->fPosition);
        if (curZ.z > 0)
        {
          int size = int(fCamera.ProjectSize(curZ, 0.05))+1;
          drawer.FillCircle(curZ, size);
        }
      }
    }

    APlot Modeler3DView::ChangeBase(Base &aBase, const APlot &aPlot)
    {
      APlot tmp = aPlot;
      tmp = aBase.ChangeCoord(tmp);
      return tmp;
    }

    PointZ Modeler3DView::GetInBase(Base &aBase, const APlot &aPlot)
    {
      APlot tmp = ChangeBase(aBase, aPlot);
      return GetInCameraSystem(tmp);
    }

    bool Modeler3DView::Cycle()
    {
      // draw the screen every 0.25 second
      if (!fIsSimulating)
        if (fDevice->GetChrono() > 250)
        {
          fEnhanceSelection = !fEnhanceSelection;
          fDevice->StartChrono();
        }
        else
          return false;
      return true;
    }

    void Modeler3DView::Draw(int xmin, int ymin, int xmax, int ymax)
    {
      drawer.FastBox(xmin, ymin, xmax ,ymax, 255);
      if (!fIsSimulating)
      {
        fImage->ClearAllZ();
        // draw the base
        DrawBase();
        // draw all the Editable elements
        ComputeBaseChange(fDefaultBase, _dynamic_cast_<Editable *>(fEditor.Root()));
        DrawEditable(_dynamic_cast_<Editable *>(fEditor.Root()), fEnhanceSelection);
      }
      else
      {
        fImage->ClearAllZ();
        DrawSystem();
        fEditor.fSystem->ProcessTimeStep();
      }
    }

    void Modeler3DView::StartSimul()
    {
      fIsSimulating = true;
      fEditor.Init();
    }

    void Modeler3DView::StopSimul()
    {
      fIsSimulating = false;
    }

    PointZ Modeler3DView::GetInCameraSystem(real x, real y, real z, Color c) const
    {
      APlot aPlot(x, y, z);
      aPlot.GetColor()=c;
      APlot cur = fCamera.ChangeToBase(aPlot);
      fCamera.Project(cur);
      return cur.GetPointZ();
    }

    PointZ Modeler3DView::GetInCameraSystem(const APlot & plot) const
    {
      APlot cur = fCamera.ChangeToBase(plot);
      fCamera.Project(cur);
      return cur.GetPointZ();
    }

    PointZ Modeler3DView::DrawPlot(real x, real y, real z, Color c)
    {
      PointZ curZ = GetInCameraSystem(x, y, z, c);
      if (curZ.z > 0)
        drawer.FillCircle(curZ, 2);
      return curZ;
    }

    void Modeler3DView::DrawLine(PointZ & alpha, PointZ & omega)
    {
      if ( (alpha.z > 0) && (omega.z > 0) )
        drawer.Line(alpha, omega);
    }

    void Modeler3DView::DrawBase()
    {
      PointZ Origin=GetInCameraSystem(0,0,0, 64);
      PointZ XAxis=GetInCameraSystem(1,0,0, 64);
      PointZ YAxis=GetInCameraSystem(0,1,0, 64);
      PointZ ZAxis=GetInCameraSystem(0,0,1, 64+16); // Change color to indicate Z axis
      DrawLine(Origin, XAxis);
      DrawLine(Origin, YAxis);
      DrawLine(ZAxis, Origin); // ZAxis if first, because it determine the color of the line
    }


  }


}
