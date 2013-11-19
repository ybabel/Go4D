#define PAL1
#define PAGE1


/*
  Thanks goes to Federico 'pix' Feroldi for the original
  water algorithm idea...

  Some optimizations added by Jason Hood, and he has made an
  Allegro-able version, too.  (I may soon be using Allegro, so
  more things will be Allegro-able)

  I hope no one minds looking at my messy code...  It's really messy.
  (in my opinion, anyway)  It hasn't been properly commented/documented
  yet, either...  When it's properly documented, it'll be on my web site:
    http://www.VIS.colostate.edu/~scriven/

  Use options: -O3 -m486 -fforce-addr -ffast-math -fomit-frame-pointer
  (-O3 is the only one that makes much difference)
*/


#include <conio.h>
#include <string.h>
#include "..\myh\FixSin.h"
#include "..\myh\mode13c.h"
#include "..\myh\palette.h"
#include "..\myh\pcx.h"
#include "..\myh\fps.h"
#include "..\myh\mouse.h"


// Do you want to see the height field directly?
// Nevermind...  This isn't supported any more..
//#define HEIGHTONLY


#define WATERWID 320
#define WATERHGT 200


// The Height field...  Two pages, so that the filter will work correctly
int Height[2][WATERWID * WATERHGT];

// Yes, I've got three copies of the background, all next to each other.
// Press 's' a bunch of times to see why...
static byte BkGdImagePre[WATERWID * WATERHGT];
static byte BkGdImage[WATERWID * WATERHGT];
static byte BkGdImagePost[WATERWID * WATERHGT];

// temp is the buffer we store the image in before putting it onscreen.
// I don't know why I called it "temp"
byte temp[WATERWID * WATERHGT];

MOUSE mouse;


void water();

void help();

void DrawWaterNoLight(int page);
void DrawWaterWithLight(int page, int LightModifier);
void CalcWater(int npage, int density);
void SmoothWater(int npage);

void HeightBlob(int x, int y, int radius, int height, int page);
void HeightBox (int x, int y, int radius, int height, int page);

void WarpBlob(int x, int y, int radius, int height, int page);
void SineBlob(int x, int y, int radius, int height, int page);


void main(int argc, char **argv)
{
    int temp;

    temp = M_reset(&mouse);
    if(!temp)
    {
      printf("Sorry, you need to load your mouse driver to use this program.\n");
      return;
    }

    if(SetUpVirtual()==0)
    {
      PCXheader head;

      randomize();
      FCreateSines();

      if(argv[1])
        LoadPCX(argv[1], BkGdImage, &head, Pal1, 0);
      else
#if WATERWID == 320
        LoadPCX("water320.pcx", BkGdImage, &head, Pal1, 0);
#else
        LoadPCX("water160.pcx", BkGdImage, &head, Pal1, 0);
#endif


      printf("Controls are:      (you may use any background: \"water file.pcx\")\n");
      printf("\t?\tHelp...\n");
      printf("\t`\tPause\n");
      printf("Automatic effects:\n");
      printf("\t1\tToggle Surfer mode\n");
      printf("\t2\tToggle Rain mode\n");
      printf("\t3\tToggle Blob mode...\n");
      printf("\t4\tToggle \"swirly\" mode...\n");
      printf("\tb/B\tTurn on \"bump\" mode...\n");
      printf("\t<space>\tTurn off effects 1-4 and b\n");
      printf("Manual effects:\n");
      printf("\tMouse\tMake blobs (button 1 and button 2 are different)\n");
      printf("\t6\tMake a large waterdrop\n");
      printf("\t7\tMake a large waterdrop in the center\n");
      printf("\tz\tDistort / exaggerate the water\n");
      printf("Physics:\n");
      printf("\td/D\tDecrease / Increase water density\n");
      printf("\th/H\tDecrease / Increase splash height\n");
      printf("\tr/R\tDecrease / Increase waterdrop radius\n");
      printf("\tm\tToggle the water \"movement\"\n");
      printf("\tl/L\tChange the light level (Off, 100%%, 50%%, 25%%, ...)\n");
//      printf("\tw\tSet physics for water-like effect...\n");
      printf("\tw/j/s/S\tSet physics for water/jelly/sludge/SuperSludge material...\n");
      fflush(stdout);
      getch();

      VGA320x200();

      SlamPal(Pal1);

      water();

      Mode3h();

      ShutDownVirtual();

      ShowFps();
    }
    else{
      printf("You need more memory to run this program.\n");
    }
}


void water()
{
  int Hpage = 0;
  int xang, yang;
  int swirlangle;
  int x, y, ox = 80, oy = 60;
  int key = 0;
  int mode=0x4000;
  int density = 4, pheight = 400, radius = 30;

  int movement=1;
  int light=1;

  int offset;


  M_reset(&mouse);

  xang = rand()%2048;
  yang = rand()%2048;
  swirlangle = rand()%2048;

  memset(Height[0], 0, sizeof(int)*WATERWID*WATERHGT);
  memset(Height[1], 0, sizeof(int)*WATERWID*WATERHGT);


  memcpy(BkGdImagePre,  BkGdImage, WATERWID*WATERHGT);
  memcpy(BkGdImagePost, BkGdImage, WATERWID*WATERHGT);

  memcpy(temp, BkGdImage, WATERWID*WATERHGT);


  M_set_window((160-(WATERWID/2))<<1, 100-(WATERHGT/2),
               (160+(WATERWID/2))<<1, 100+(WATERHGT/2));

  M_show();
  FpsStart();

  while(key != 27)
  {
    M_read_butt_pos(&mouse);

    if(mouse.buttons)
      switch(mouse.buttons)
      {
        case 1:
             if(mode & 0x4000)
               HeightBlob((mouse.x>>1)-((320-WATERWID)/2), mouse.y-((200-WATERHGT)/2), 2, pheight, Hpage);
             else if(movement)
               SineBlob((mouse.x>>1)-((320-WATERWID)/2), mouse.y-((200-WATERHGT)/2), radius, -pheight, Hpage);
             else
               SineBlob((mouse.x>>1)-((320-WATERWID)/2), mouse.y-((200-WATERHGT)/2), radius, -pheight, Hpage);
             break;
        case 2:
             if(mode & 0x4000)
               HeightBlob((mouse.x>>1)-((320-WATERWID)/2), mouse.y-((200-WATERHGT)/2), radius/2, pheight, Hpage);
             else if(movement)
               SineBlob((mouse.x>>1)-((320-WATERWID)/2), mouse.y-((200-WATERHGT)/2), radius, pheight, Hpage);
             else
               SineBlob((mouse.x>>1)-((320-WATERWID)/2), mouse.y-((200-WATERHGT)/2), radius, pheight, Hpage);
             break;
      }

    // The main "interface"
    if(kbhit())
    {
      key = getch();
      switch(key)
      {
        case 'd': density--; break;
        case 'D': density++; break;

        case 'h': pheight-=40; break;
        case 'H': pheight+=40; break;

        case 'r': radius++; break;
        case 'R': radius--; break;

        case 'l': light++; break;
        case 'L': if(light>0) light--; break;

        case 'c': memset(Height[0], 0, sizeof(int)*WATERWID*WATERHGT);
                  memset(Height[1], 0, sizeof(int)*WATERWID*WATERHGT);
                  break;
        case 'z': if(movement)
                    memset(Height[Hpage], 0, sizeof(int)*WATERWID*WATERHGT);
                  else
                    SmoothWater(Hpage);
                  break;

        case 'w':
        case 'W': mode=mode | 0x4000;
                  density=4; light=1; pheight=600;
                  break;
        case 'j':
        case 'J': mode=mode & 0xbfff;
                  density=3; pheight=400; // light=0;
                  break;
        case 's': density=6; pheight=400; // light=0; 
                  mode=mode & 0xbfff;
                  break;
        case 'S': density=8; pheight=400; // light=0; 
                  mode=mode & 0xbfff;
                  break;
        case 'b': mode = mode & (0xbfff - 4);  mode ^= 4;
                  density=4; pheight=1400; radius=80; // light=1; 
                  break;
        case 'B': mode = mode & (0xbfff - 4);  mode ^= 4;
                  density=4; pheight = -1400; radius=80; // light=1; 
                  break;


        case 'm':
        case 'M': movement ^= 1;
          if(movement)
          {
            pheight=400;
          }
          else
          {
            pheight=256;
          }
          break;

// Each bit of the mode variable is used to represent the state of a
// different option that can be turned on or off...
        case '1': mode ^= 1; break;
        case '2': mode ^= 2; break;
        case '3': mode ^= 4; break;
        case '4': mode ^= 8; if(mode&8) {xang=0; yang=0;} break;

        case ' ': mode &= 0x4000; break;

        case '6':
          HeightBlob(-1, -1, rand()%(radius/2)+2, pheight, Hpage);
          break;

        case '7':
          HeightBlob(WATERWID/2, WATERHGT/2, radius/2, pheight, Hpage);
          break;

        case '8':
          HeightBox(WATERWID/2, WATERHGT/2, radius/2, pheight, Hpage);
          break;

        case '`': getch(); break;
        case '?': help(); break;
      }
    }

  //  The surfer...
    if(mode&1)
    {
        x = (WATERWID/2)
          + ((
             (
              (FSin( (xang* 65) >>8) >>8) *
              (FSin( (xang*349) >>8) >>8)
             ) * ((WATERWID-8)/2)
            ) >> 16);
        y = (WATERHGT/2)
          + ((
             (
              (FSin( (yang*377) >>8) >>8) *
              (FSin( (yang* 84) >>8) >>8)
             ) * ((WATERHGT-8)/2)
            ) >> 16);
        xang += 13;
        yang += 12;

       if(mode & 0x4000)
       {
        offset = (oy+y)/2*WATERWID + (ox+x)/2;
        Height[Hpage][offset] = pheight;
        Height[Hpage][offset + 1] =
        Height[Hpage][offset - 1] =
        Height[Hpage][offset + WATERWID] =
        Height[Hpage][offset - WATERWID] = pheight >> 1;

        offset = y*WATERWID + x;
        Height[Hpage][offset] = pheight<<1;
        Height[Hpage][offset + 1] =
        Height[Hpage][offset - 1] =
        Height[Hpage][offset + WATERWID] =
        Height[Hpage][offset - WATERWID] = pheight;
       }
       else
       {
        SineBlob((ox+x)/2, (oy+y)/2, 3, -1200, Hpage);
        SineBlob(x, y, 4, -2000, Hpage);
       }

        ox = x;
        oy = y;
    }
  // The raindrops...
    if(mode&2)
    {
        x = rand()%(WATERWID-2) + 1;
        y = rand()%(WATERHGT-2) + 1;
        Height[Hpage][y*WATERWID + x] = rand()%(pheight<<2);
    }
  // The big splashes...
    if(mode&4)
    {
      if(rand()%20 == 0)
      {
        if(mode & 0x4000)
          HeightBlob(-1, -1, radius/2, pheight, Hpage);
        else
          SineBlob(-1, -1, radius, -pheight*6, Hpage);
      }
    }
  //  The surfer (2) ... Swirling effect
    if(mode&8)
    {
        x = (WATERWID/2)
          + ((
             (FCos(swirlangle)) * (25)
            ) >> 16);
        y = (WATERHGT/2)
          + ((
             (FSin(swirlangle)) * (25)
            ) >> 16);
        swirlangle += 50;
        if(mode & 0x4000)
          HeightBlob(x,y, radius/3, pheight, Hpage);
        else
          WarpBlob(x, y, radius, pheight, Hpage);
    }

    if(light)
      DrawWaterWithLight(Hpage, light-1);
    else
      DrawWaterNoLight(Hpage);

    if(movement)
      CalcWater(Hpage^1, density);
    else
      memcpy(&Height[Hpage^1][0], &Height[Hpage][0], sizeof(int)*WATERWID*WATERHGT);


    Hpage ^= 1;

    frames++;

//    if(frames>200) key=27;

  }

  FpsEnd();
  M_hide();
}

void DrawWaterNoLight(int page)
{

//  int ox, oy;
  int dx, dy;
  int x, y;
  int c;

  int offset=WATERWID + 1;

  int *ptr = &Height[page][0];

  for (y = (WATERHGT-1)*WATERWID; offset < y; offset += 2)
  {
    for (x = offset+WATERWID-2; offset < x; offset++)
    {
      dx = ptr[offset] - ptr[offset+1];
      dy = ptr[offset] - ptr[offset+WATERWID];
      c = BkGdImage[offset + WATERWID*(dy>>3) + (dx>>3)];

     // If anyone knows a better/faster way to do this, please tell me...
      temp[offset] = (c < 0) ? 0 : (c > 255) ? 255 : c;
     // This is the previous way I had been doing it...
//      if(c < 0) c=0;
//      else if (c > 255) c=255;
//      temp[offset] = c;

      offset++;
      dx = ptr[offset] - ptr[offset+1];
      dy = ptr[offset] - ptr[offset+WATERWID];
      c = BkGdImage[offset + WATERWID*(dy>>3) + (dx>>3)];
      temp[offset] = (c < 0) ? 0 : (c > 255) ? 255 : c;
 
    }
  }
  M_hide();

#if WATERWID == 320
  toVGA(temp);          // use a rep movsl
#else
  // Draw the image like a sprite...
  PutImageVGA((320-WATERWID)/2, (200-WATERHGT)/2, WATERWID, WATERHGT, temp);
#endif
  M_show();
}

void DrawWaterWithLight(int page, int LightModifier)
{

//  int ox, oy;
  int dx, dy;
  int x, y;
  int c;

  int offset=WATERWID + 1;

  int *ptr = &Height[page][0];


  for (y = (WATERHGT-1)*WATERWID; offset < y; offset += 2)
  {
    for (x = offset+WATERWID-2; offset < x; offset++)
    {
      dx = ptr[offset] - ptr[offset+1];
      dy = ptr[offset] - ptr[offset+WATERWID];
      c = BkGdImage[offset + WATERWID*(dy>>3) + (dx>>3)] - (dx>>LightModifier);

     // If anyone knows a better/faster way to do this, please tell me...
      temp[offset] = (c < 0) ? 0 : (c > 255) ? 255 : c;
     // This is the previous way I had been doing it...
//      if(c < 0) c=0;
//      else if (c > 255) c=255;
//      temp[offset] = c;

      offset++;
      dx = ptr[offset] - ptr[offset+1];
      dy = ptr[offset] - ptr[offset+WATERWID];
      c = BkGdImage[offset + WATERWID*(dy>>3) + (dx>>3)] - (dx>>LightModifier);
      temp[offset] = (c < 0) ? 0 : (c > 255) ? 255 : c;
 
    }
  }
  M_hide();

#if WATERWID == 320
  toVGA(temp);          // use a rep movsl
#else
  // Draw the image like a sprite...
  PutImageVGA((320-WATERWID)/2, (200-WATERHGT)/2, WATERWID, WATERHGT, temp);
#endif
  M_show();
}

void CalcWater(int npage, int density)
{
  int newh;
  int count = WATERWID + 1;

  int *newptr = &Height[npage][0];
  int *oldptr = &Height[npage^1][0];

  int x, y;

  // Sorry, this function might not be as readable as I'd like, because
  // I optimized it somewhat.  (enough to make me feel satisfied with it)

//  for(y=1; y<WATERHGT-1; y++)
//  {
//    for(x=1; x<WATERWID-1; x++)
//    {
  for (y = (WATERHGT-1)*WATERWID; count < y; count += 2)
  {
    for (x = count+WATERWID-2; count < x; count++)
    {
/*********  This does the four-pixel method.  It looks bad.
      newh          = ((oldptr[count + WATERWID]
                      + oldptr[count - WATERWID]
                      + oldptr[count + 1]
                      + oldptr[count - 1]
                       ) >> 1 )
                      - newptr[count];
**********/
// This does the eight-pixel method.  It looks much better.

      newh          = ((oldptr[count + WATERWID]
                      + oldptr[count - WATERWID]
                      + oldptr[count + 1]
                      + oldptr[count - 1]
                      + oldptr[count - WATERWID - 1]
                      + oldptr[count - WATERWID + 1]
                      + oldptr[count + WATERWID - 1]
                      + oldptr[count + WATERWID + 1]
                       ) >> 2 )
                      - newptr[count];


      newptr[count] =  newh - (newh >> density);
/*
      newh =  oldptr[count]
           +  oldptr[count-1-WATERWID]
           +  oldptr[count+1-WATERWID]
           +  oldptr[count-1+WATERWID]
           +  oldptr[count+1+WATERWID]
           + ((oldptr[count-1]
           +   oldptr[count+1]
           +   oldptr[count-WATERWID]
           +   oldptr[count+WATERWID])<<1);
*/
/*
// This is the "sludge" method...
      newh = (oldptr[count]<<2)
           +  oldptr[count-1-WATERWID]
           +  oldptr[count+1-WATERWID]
           +  oldptr[count-1+WATERWID]
           +  oldptr[count+1+WATERWID]
           + ((oldptr[count-1]
           +   oldptr[count+1]
           +   oldptr[count-WATERWID]
           +   oldptr[count+WATERWID])<<1);

      newptr[count] = (newh-(newh>>6)) >> density;
*/
//      count++;
    }
//    count += 2;
  }
/*
  for(count=1; count<WATERWID-1; count++)
  {
      newh          = ((oldptr[count + WATERWID]
                      + oldptr[count + 1]
                      + oldptr[count - 1]
//                      + oldptr[count - WATERWID + 1]
                      + oldptr[count + WATERWID + 1]
                       ) >> 1 )
                      - (newptr[count] >> 1);
      newptr[count] =  newh - (newh >> density);
  }
*/

/*
  memcpy(newptr, newptr + WATERWID, WATERWID*sizeof(int));
  memcpy(newptr+(WATERWID*WATERHGT)-WATERWID,
         newptr+(WATERWID*WATERHGT)-(2*WATERWID), WATERWID*sizeof(int));
  newptr += WATERWID;
  for(y=1; y<WATERHGT-1; y++)
  {
    *newptr = *(newptr+1);
    newptr += WATERWID-1;
    *newptr = *(newptr-1);
    newptr ++;
  }
*/
}
void SmoothWater(int npage)
{
  int newh;
  int count = WATERWID + 1;

  int *newptr = &Height[npage][0];
  int *oldptr = &Height[npage^1][0];

  int x, y;

  // Sorry, this function might not be as readable as I'd like, because
  // I optimized it somewhat.  (enough to make me feel satisfied with it)

  for(y=1; y<WATERHGT-1; y++)
  {
    for(x=1; x<WATERWID-1; x++)
    {
// This does the eight-pixel method.  It looks much better.

      newh          = ((oldptr[count + WATERWID]
                      + oldptr[count - WATERWID]
                      + oldptr[count + 1]
                      + oldptr[count - 1]
                      + oldptr[count - WATERWID - 1]
                      + oldptr[count - WATERWID + 1]
                      + oldptr[count + WATERWID - 1]
                      + oldptr[count + WATERWID + 1]
                       ) >> 3 )
                      + newptr[count];


      newptr[count] =  newh>>1;
      count++;
    }
    count += 2;
  }
}

void CalcWaterBigFilter(int npage, int density)
{
  int newh;
  int count = (2*WATERWID) + 2;

  int *newptr = &Height[npage][0];
  int *oldptr = &Height[npage^1][0];

  int x, y;

  // Sorry, this function might not be as readable as I'd like, because
  // I optimized it somewhat.  (enough to make me feel satisfied with it)

  for(y=2; y<WATERHGT-2; y++)
  {
    for(x=2; x<WATERWID-2; x++)
    {
// This does the 25-pixel method.  It looks much okay.

      newh        = (
                     (
                      (
                       (oldptr[count + WATERWID]
                      + oldptr[count - WATERWID]
                      + oldptr[count + 1]
                      + oldptr[count - 1]
                       )<<1)
                      + ((oldptr[count - WATERWID - 1]
                      + oldptr[count - WATERWID + 1]
                      + oldptr[count + WATERWID - 1]
                      + oldptr[count + WATERWID + 1]))
                      + ( (
                          oldptr[count - (WATERWID*2)]
                        + oldptr[count + (WATERWID*2)]
                        + oldptr[count - 2]
                        + oldptr[count + 2]
                        ) >> 1 )
                      + ( (
                          oldptr[count - (WATERWID*2) - 1]
                        + oldptr[count - (WATERWID*2) + 1]
                        + oldptr[count + (WATERWID*2) - 1]
                        + oldptr[count + (WATERWID*2) + 1]
                        + oldptr[count - 2 - WATERWID]
                        + oldptr[count - 2 + WATERWID]
                        + oldptr[count + 2 - WATERWID]
                        + oldptr[count + 2 + WATERWID]
                        ) >> 2 )
                     )
                    >> 3)
                    - (newptr[count]);


      newptr[count] =  newh - (newh >> density);
      count++;
    }
    count += 4;
  }
}



void HeightBlob(int x, int y, int radius, int height, int page)
{
  int rquad;
  int cx, cy, cyq;
  int left, top, right, bottom;


  rquad = radius * radius;

  // Make a randomly-placed blob...
  if(x<0) x = 1+radius+ rand()%(WATERWID-2*radius-1);
  if(y<0) y = 1+radius+ rand()%(WATERHGT-2*radius-1);

  left=-radius; right = radius;
  top=-radius; bottom = radius;

  // Perform edge clipping...
  if(x - radius < 1) left -= (x-radius-1);
  if(y - radius < 1) top  -= (y-radius-1);
  if(x + radius > WATERWID-1) right -= (x+radius-WATERWID+1);
  if(y + radius > WATERHGT-1) bottom-= (y+radius-WATERHGT+1);


  for(cy = top; cy < bottom; cy++)
  {
    cyq = cy*cy;
    for(cx = left; cx < right; cx++)
    {
      if(cx*cx + cyq < rquad)
        Height[page][WATERWID*(cy+y) + (cx+x)] += height;
    }
  }

}


void HeightBox (int x, int y, int radius, int height, int page)
{
  int cx, cy;
  int left, top, right, bottom;


  if(x<0) x = 1+radius+ rand()%(WATERWID-2*radius-1);
  if(y<0) y = 1+radius+ rand()%(WATERHGT-2*radius-1);

  left=-radius; right = radius;
  top=-radius; bottom = radius;

  // Perform edge clipping...
  if(x - radius < 1) left -= (x-radius-1);
  if(y - radius < 1) top  -= (y-radius-1);
  if(x + radius > WATERWID-1) right -= (x+radius-WATERWID+1);
  if(y + radius > WATERHGT-1) bottom-= (y+radius-WATERHGT+1);

  for(cy = top; cy < bottom; cy++)
  {
    for(cx = left; cx < right; cx++)
    {
        Height[page][WATERWID*(cy+y) + (cx+x)] = height;
    }
  }

}


void WarpBlob(int x, int y, int radius, int height, int page)
{
  int cx, cy;
  int left,top,right,bottom;
  int square;
  int radsquare = radius * radius;

//  radsquare = (radius*radius) << 8;
  radsquare = (radius*radius);

  height /= 64;

  left=-radius; right = radius;
  top=-radius; bottom = radius;

  // Perform edge clipping...
  if(x - radius < 1) left -= (x-radius-1);
  if(y - radius < 1) top  -= (y-radius-1);
  if(x + radius > WATERWID-1) right -= (x+radius-WATERWID+1);
  if(y + radius > WATERHGT-1) bottom-= (y+radius-WATERHGT+1);

  for(cy = top; cy < bottom; cy++)
  {
    for(cx = left; cx < right; cx++)
    {
      square = cy*cy + cx*cx;
//      square <<= 8;
      if(square < radsquare)
      {
//        Height[page][WATERWID*(cy+y) + cx+x]
//          += (sqrt(radsquare)-sqrt(square))*height;
        Height[page][WATERWID*(cy+y) + cx+x]
          += (radius-sqrt(square))*(float)(height);
      }
    }
  }
}

void SineBlob(int x, int y, int radius, int height, int page)
{
  int cx, cy;
  int left,top,right,bottom;
  int square, dist;
  int radsquare = radius * radius;
  float length = (1024.0/(float)radius)*(1024.0/(float)radius);

  if(x<0) x = 1+radius+ rand()%(WATERWID-2*radius-1);
  if(y<0) y = 1+radius+ rand()%(WATERHGT-2*radius-1);


//  radsquare = (radius*radius) << 8;
  radsquare = (radius*radius);

//  height /= 8;

  left=-radius; right = radius;
  top=-radius; bottom = radius;


  // Perform edge clipping...
  if(x - radius < 1) left -= (x-radius-1);
  if(y - radius < 1) top  -= (y-radius-1);
  if(x + radius > WATERWID-1) right -= (x+radius-WATERWID+1);
  if(y + radius > WATERHGT-1) bottom-= (y+radius-WATERHGT+1);

  for(cy = top; cy < bottom; cy++)
  {
    for(cx = left; cx < right; cx++)
    {
      square = cy*cy + cx*cx;
      if(square < radsquare)
      {
        dist = sqrt(square*length);
        Height[page][WATERWID*(cy+y) + cx+x]
          += (int)((FCos(dist)+0xffff)*(height)) >> 19;
      }
    }
  }
}


void help()
{
  COLOR temp[256];

  GrabPal(temp);

  M_hide();

  Mode3h();

      printf("Controls are:      (you may use any background: \"water file.pcx\")\n");
      printf("\t?\tHelp...\n");
      printf("\t`\tPause\n");
      printf("Automatic effects:\n");
      printf("\t1\tToggle Surfer mode\n");
      printf("\t2\tToggle Rain mode\n");
      printf("\t3\tToggle Blob mode...\n");
      printf("\t4\tToggle \"swirly\" mode...\n");
      printf("\tb/B\tTurn on \"bump\" mode...\n");
      printf("\t<space>\tTurn off effects 1-4 and b\n");
      printf("Manual effects:\n");
      printf("\tMouse\tMake blobs (button 1 and button 2 are different)\n");
      printf("\t6\tMake a large waterdrop\n");
      printf("\t7\tMake a large waterdrop in the center\n");
      printf("\tz\tDistort / exaggerate the water\n");
      printf("Physics:\n");
      printf("\td/D\tDecrease / Increase water density\n");
      printf("\th/H\tDecrease / Increase splash height\n");
      printf("\tr/R\tDecrease / Increase waterdrop radius\n");
      printf("\tm\tToggle the water \"movement\"\n");
      printf("\tl/L\tChange the light level (Off, 100%%, 50%%, 25%%, ...)\n");
//      printf("\tw\tSet physics for water-like effect...\n");
      printf("\tw/j/s/S\tSet physics for water/jelly/sludge/SuperSludge material...\n");
      fflush(stdout);
      fflush(stdout);
      getch();

  VGA320x200();

  SlamPal(temp);

  M_show();
}

