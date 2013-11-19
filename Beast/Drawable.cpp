#include "Drawable.h"
#include "Face.h"

Renderer::Renderer(Image * aImage)
: fDrawables(false),
faceDrawer(*aImage, 4),
mappedFaceDrawer(*aImage, 4)
//mappedFaceDrawer(*aImage, NULL)
{
}

Renderer::~Renderer()
{
}

void Renderer::Clear()
{
  fDrawables.Clear();
}

void Renderer::AddDrawable(IDrawable * drawable)
{
  fDrawables.Add(drawable);
  fDrawables.Complete();
}

void Renderer::DelDrawable(IDrawable * drawable)
{
  fDrawables.Del(drawable);
  fDrawables.Complete();
}

void Renderer::ConvertInPlot(IDrawable * drawable, APlot &p1, APlot &p2, APlot &p3, APlot &p4)
{
  Vector3 origin = *drawable->GetPosition();
  Vector3 direction = *drawable->GetDirection();
  Vector3 normal=direction;
  real size = drawable->GetSize();
  Swap(normal[0], normal[1]);
  normal[1] *= -1;

  p1 = origin+direction*(-0.5*size)+normal*(0.5*size);
  p2 = origin+direction*(-0.5*size)+normal*(-0.5*size);
  p3 = origin+direction*(0.5*size);
  p4 = origin+direction*(0.5*size);
  switch (drawable->GetShape())
  {
  case dsTriangle : p3 += normal*(-0.05*size); p4 += normal*(0.05*size); break;
  case dsSquareMapped :
  case dsSquare : p3 += normal*(-0.5*size); p4 += normal*(0.5*size); break;
  }

  p1.GetColor() = drawable->GetColor();
  p2.GetColor() = drawable->GetColor();
  p3.GetColor() = drawable->GetColor();
  p4.GetColor() = drawable->GetColor();
}

void Renderer::Draw(Camera * camera)
{
  for (int i=0; i<fDrawables.Count(); i++)
  {
    APlot p1, p2, p3, p4;
    ConvertInPlot(fDrawables[i], p1, p2, p3, p4);

    APlot tmp0 = camera->ChangeToBase(p1);
    APlot tmp1 = camera->ChangeToBase(p2);
    APlot tmp2 = camera->ChangeToBase(p3);
    APlot tmp3 = camera->ChangeToBase(p4);

    switch (fDrawables[i]->GetShape())
    {
    case dsTriangle :
    case dsSquare :
      {
        FlatFace flatface(&faceDrawer,
          tmp0, tmp1, tmp2, tmp3,
          fDrawables[i]->GetColor(), 255-fDrawables[i]->GetColor(), true);
        flatface.SimpleDraw(*camera);
      }
      break;
    case dsSquareMapped :
      {
        FlatFace flatface(&mappedFaceDrawer,
          tmp0, tmp1, tmp2, tmp3,
          fDrawables[i]->GetColor(), 255-fDrawables[i]->GetColor(), true);
        const Image * image = dynamic_cast<IDrawableMapped *>(fDrawables[i])->GetMappedImage();
#define MARGIN 1
        DrawerImageData data(image);
        mappedFaceDrawer.SetData(&data);
        //mappedFaceDrawer.SetMapImage(image);
        mappedFaceDrawer.SetMapPoint(1, MARGIN, image->Height()-MARGIN-1);
        mappedFaceDrawer.SetMapPoint(2, image->Width()-MARGIN-1, image->Height()-MARGIN-1);
        mappedFaceDrawer.SetMapPoint(3, image->Width()-MARGIN-1, MARGIN);
        mappedFaceDrawer.SetMapPoint(0, 1, MARGIN);
        flatface.SimpleDraw(*camera);
      }
      break;
    }
  }
}


