#ifndef _3DVIEW_H_
#define _3DVIEW_H_

#include "GUI/Gui.h"
#include "GUI/EditablePhysic.h"
#include "Graph3D/TemplateFaceDrawer.h"
#include "Graph3D/Pixeler.h"
#include "Graph3D/Shader.h"
#include "Graph3D/Mapper.h"
#include "Graph3D/Plotter.h"
#include "Graph2D/WriteDrawer.h"
#include "Platform/Device.h"

namespace Go4D
{
  namespace GUI
  {

    class Modeler3DView : public ANode
    {
      typedef ANode inherited;
    public:
      Modeler3DView(System * gui, const char * name, Device * dev);
      ~Modeler3DView();
      ANode * MouseEvent(MouseEventKind kind, int mousecode, real x, real y);
      void DrawPass(Image *, real x, real y, real width, real height);

      /// return if display must be refreshed !
      bool Cycle();
      void StartSimul();
      void StopSimul();

      EditorPhysic & GetEditor() { return fEditor; }
      bool & GetDrawPlotCoord() { return fDrawPlotCoord; }

      /// Drawing flags
      bool fDrawPlotCoord;
      bool fDrawPlotNumbers;
      bool fDrawPlots;
      bool fDrawSegments;
      bool fDrawFaces;
    protected :
      void ComputeBaseChange(Base, Editable *);
      void DrawEditable(Editable *, bool);
      void DrawSystem();
      APlot ChangeBase(Base &, const APlot &);
      PointZ GetInBase(Base &, const APlot &);
      void Draw(int xmin, int ymin, int xmax, int ymax);
      PointZ GetInCameraSystem(real x, real y, real z, Color c) const;
      PointZ GetInCameraSystem(const APlot & plot) const;
      PointZ DrawPlot(real x, real y, real z, Color c);
      void DrawLine(PointZ &, PointZ &);
      void DrawBase();

      EditorPhysic fEditor;
      ZImage * fImage;
      Device * fDevice; // for chrono
      Camera fCamera;
      Font6x10 font;
      Drawer<EnhancedZPixeler> enhancedDrawer;
      Drawer<ZPixeler> drawer;
      WriteDrawer<ZPixeler> writer;
      FaceDrawer<ZImagePlotter, FlatShader, NullMapper> faceDrawer;
      Base fDefaultBase;
      bool fEnhanceSelection;
      bool fIsSimulating;
    };

  }


}

#endif
