//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "Unit2.h"
#include "Unit3.h"
#include "Unit4.h"
#include <time.h>


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ImagXpr7_OCX"

#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

 /*
void __fastcall TForm1::N4Click(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::N2Click(TObject *Sender)
{
  OpenDialog1->Execute();
  ImagXpress7_1->FileName = OpenDialog1->FileName;
  ImagXpress7_1->ColorDepth(1,0,0);
  ImagXpress7_1->ColorDepth(8,2,0);
  ImagXpress7_1->ScrollBars=3;
  Ix = ImagXpress7_1->IWidth;
  Iy = ImagXpress7_1->IHeight;
  ImagXpress7_1->ZoomToFit(2);

  ImagXpress7_1->SaveToBuffer = true;
  ImagXpress7_1->SaveFileType =  FT_TIFF;
  ImagXpress7_1->SaveFile ();


  HANDLE hIM = (HANDLE)ImagXpress7_1->SaveBufferHandle;
  if (IMAGE!=NULL) {GlobalFree(IMAGE); IMAGE=NULL;}
  IMAGE = (unsigned char *)GlobalLock(hIM);
  long ln = GlobalSize(hIM);
  offs=ln-(long)Ix*Iy;
  GlobalUnlock(hIM);


}
//---------------------------------------------------------------------------
void __fastcall TForm1::N5Click(TObject *Sender)
{
  for (int x=0;x<300;x++)
  for (int y=0;y<300;y++)
  {
    int p = ImagXpress7_1->DIBGetPixel(x,y);
    ImagXpress7_1->DIBSetPixel(x,y,1-p);
  }

  ImagXpress7_1->DIBUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ImagXpress7_1MouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
  int x = ImagXpress7_1->DIBXPos;
  int y = ImagXpress7_1->DIBYPos;
  //int p = ImagXpress7_1->DIBGetPixel(x,y);
  if ((x>=0) && (x<Ix) && (y>=0) && (y<Iy))
  {
    int p = *(IMAGE+y*Ix+x+offs);
    Caption = p;
  }
  if (StartSelection == true)
  {
     StopSelectionX = X;StopSelectionY = Y;
     if (X > ImagXpress7_1-> Width)  StopSelectionX = ImagXpress7_1-> Width - 1;
     if (X < 0) StopSelectionX = 0;
     if (Y > ImagXpress7_1-> Height)  StopSelectionY = ImagXpress7_1-> Height - 1;
     if (Y < 0)  StopSelectionY = 0;
     ImagXpress7_1-> RubberbandUpdate (StopSelectionX, StopSelectionY);
  }


}
//---------------------------------------------------------------------------

void __fastcall TForm1::N7Click(TObject *Sender)
{
  time_t starttime,endtime;
  int totaltime;
  starttime=time(NULL);

  Screen->Cursor =  crHourGlass;

  int H[256];
  for (int i=0;i<256;i++) H[i]=0;

  Form3->ProgressBar1->Min=0;
  Form3->ProgressBar1->Max=Ix;
  Form3->ProgressBar1->Position = 0;

  Form3->Show();

  for (int x=0;x<Ix;x++)
  {
    Form3->ProgressBar1->Position = x;
    for (int y=0;y<Iy;y++)
    {
//      int p = ImagXpress7_1->DIBGetPixel(x,y);
      int p = *(IMAGE+y*Ix+x+offs);
      H[p]++;
    }
  }

  Form3->Close();

  Form2->Series1->Clear();
  for (int i=0;i<256;i++) Form2->Series1->AddY(H[i]);

  endtime=time(NULL);
  totaltime=difftime(endtime,starttime);

  AnsiString M = "Time:";M=M+totaltime;
  Form2->Caption = M;

  Form2->Show();
  Screen->Cursor =  crDefault;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::N8Click(TObject *Sender)
{
  time_t starttime,endtime;
  int totaltime;
  starttime=time(NULL);

  Screen->Cursor =  crHourGlass;

  Form3->ProgressBar1->Min=0;
  Form3->ProgressBar1->Max=Ix;
  Form3->ProgressBar1->Position = 0;

  Form3->Show();

  for (int x=0;x<Ix;x++)
  {
    Form3->ProgressBar1->Position = x;
    for (int y=0;y<Iy;y++)
    {
//      int p = ImagXpress7_1->DIBGetPixel(x,y);
      int p = *(IMAGE+y*Ix+x+offs);

      if (p<140) *(IMAGE+y*Ix+x+offs)=0;
      else *(IMAGE+y*Ix+x+offs)=255;
    }
  }

  Form3->Close();
//  ImagXpress7_1->DIBUpdate();
  ImagXpress7_1->LoadBuffer((long)IMAGE);

  Screen->Cursor =  crDefault;

  endtime=time(NULL);
  totaltime=difftime(endtime,starttime);

  AnsiString M = "Time:";M=M+totaltime;
  ShowMessage(M);


}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
  if (IMAGE!=NULL) {GlobalFree(IMAGE); IMAGE=NULL;}        
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImagXpress7_1MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
   SelectionX = X;
   SelectionY = Y;
   StopSelectionX = X;
   StopSelectionY = Y;
   StartSelection = true;
   ImagXpress7_1->RubberBand (true, X, Y, false);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImagXpress7_1MouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
if (StartSelection)
  {
    StartSelection = false;
    int x1=SelectionX+ImagXpress7_1->ScrollX;
    int x2= StopSelectionX+ImagXpress7_1->ScrollX;
    int y1= SelectionY+ImagXpress7_1->ScrollY;
    int y2= StopSelectionY+ImagXpress7_1->ScrollY;
    float Current1=ImagXpress7_1->IPZoomF;
    int dx = (X+ImagXpress7_1->ScrollX)/Current1-ImagXpress7_1->DIBXPos;
    int dy = (Y+ImagXpress7_1->ScrollY)/Current1-ImagXpress7_1->DIBYPos;

    x1=x1/Current1-dx;
    x2=x2/Current1-dx;
    y1=y1/Current1-dy;
    y2=y2/Current1-dy;

    ImagXpress7_1->RubberBand (false, X, Y, false);

    for (int x=x1;x<=x2;x++)
    {
      for (int y=y1;y<=y2;y++)
      {
        int p = *(IMAGE+y*Ix+x+offs);

        if (p<140) *(IMAGE+y*Ix+x+offs)=0;
        else *(IMAGE+y*Ix+x+offs)=255;
      }
    }

    int sx = ImagXpress7_1->ScrollX;
    int sy = ImagXpress7_1->ScrollY;

    ImagXpress7_1->LoadBuffer((long)IMAGE);

    ImagXpress7_1->ScrollX = sx;
    ImagXpress7_1->ScrollY = sy;

    }

}
//---------------------------------------------------------------------------

void __fastcall TForm1::N9Click(TObject *Sender)
{
  for (long i=0;i<100000;i++)
  {
    int x = ((long)rand()*(Ix-2))/32767+1;
    int y = ((long)rand()*(Iy-2))/32767+1;
    *(IMAGE+y*Ix+x+offs)=0;
  }

  ImagXpress7_1->LoadBuffer((long)IMAGE);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::N10Click(TObject *Sender)
{
  int n=2, th=10;

  for (int x=n;x<Ix-n;x++)
  for (int y=n;y<Iy-n;y++)
  if (*(IMAGE+y*Ix+x+offs)<=1)
  {
    int count=0;
    for (int nx=-n; nx<=n; nx++)
    for (int ny=-n; ny<=n; ny++)
      if(*(IMAGE+(y+ny)*Ix+(x+nx)+offs)==255)
        count++;

      if (count>=th)
        *(IMAGE+y*Ix+x+offs)=1;
  }

  for (int x=n;x<Ix-n;x++)
  for (int y=n;y<Iy-n;y++)
   if (*(IMAGE+y*Ix+x+offs)==1)
    *(IMAGE+y*Ix+x+offs)=255;

  ImagXpress7_2->ScrollBars=3;
  ImagXpress7_2->LoadBuffer((long)IMAGE);

  float z = ImagXpress7_1->IPZoomF;
  ImagXpress7_2->Zoom(z);
  ImagXpress7_2->ScrollX = ImagXpress7_1->ScrollX;;
  ImagXpress7_2->ScrollY = ImagXpress7_1->ScrollY;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImagXpress7_1Scroll(TObject *Sender, short Bar,
      short Action)
{
  ImagXpress7_2->ScrollX = ImagXpress7_1->ScrollX;;
  ImagXpress7_2->ScrollY = ImagXpress7_1->ScrollY;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ImagXpress7_2Scroll(TObject *Sender, short Bar,
      short Action)
{
 ImagXpress7_1->ScrollX = ImagXpress7_2->ScrollX;;
 ImagXpress7_1->ScrollY = ImagXpress7_2->ScrollY;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N11Click(TObject *Sender)
{
   ImagXpress7_1->Rotate(3.5);
}
//---------------------------------------------------------------------------


void __fastcall TForm1::N13Click(TObject *Sender)
{
  Form4->ImagXpress7_1->hDIB = ImagXpress7_1->CopyDIB();
  Form4->ImagXpress7_1->ZoomToFit(2);
  Form4->ImagXpress7_1->ScrollBars=3;
  Form4->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormShow(TObject *Sender)
{
  Form4->Show();Form4->Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N14Click(TObject *Sender)
{
  ImagXpress7_1->ColorDepth(24,1,0);
  //Οριζόντιο RLSA
  for (int y=0;y<Iy;y++)
  {
     int x=0;
     while (x<Ix)
     {
       while (*(IMAGE+y*Ix+x+offs)==0) x++;
       int X=x;
       while (*(IMAGE+y*Ix+x+offs)!=0) x++;
       int L = x-X;

       if (L<50)
       {
          for (int xi=X;xi<x;xi++)
          *(IMAGE+y*Ix+xi+offs)=0;
          //ImagXpress7_1->DIBSetPixel(xi,y,clRed);
       }
     }
  }

  //Κατακόρυφο RLSA
  for (int x=0;x<Ix;x++)
  {
     int y=0;
     while (y<Iy)
     {
       while (*(IMAGE+y*Ix+x+offs)==0) y++;
       int Y=y;
       while (*(IMAGE+y*Ix+x+offs)!=0) y++;
       int L = y-Y;

       if (L<15)
       {
          for (int yi=Y;yi<y;yi++)
          *(IMAGE+yi*Ix+x+offs)=0;
          //ImagXpress7_1->DIBSetPixel(x,yi,clRed);
       }
     }
  }


  ImagXpress7_2->hDIB = ImagXpress7_1->CopyDIB();
  ImagXpress7_2->ScrollBars=3;
  ImagXpress7_2->ZoomToFit(2);

  ImagXpress7_2->ColorDepth(8,1,0);

  ImagXpress7_2->SaveToBuffer = true;
  ImagXpress7_2->SaveFileType =  FT_TIFF;
  ImagXpress7_2->SaveFile ();


  unsigned char *IMAGE2;
  HANDLE hIM = (HANDLE)ImagXpress7_2->SaveBufferHandle;
  IMAGE2 = (unsigned char *)GlobalLock(hIM);
  long ln = GlobalSize(hIM);
  long offs2=ln-(long)Ix*Iy;
  GlobalUnlock(hIM);

  for (int x=0;x<Ix;x++)
  for (int y=0;y<Iy;y++)
  if (*(IMAGE+y*Ix+x+offs)==0)
  {
     *(IMAGE+y*Ix+x+offs)=1;
     int xmin=x,xmax=x,ymin=y,ymax=y;
     Flood(x,y,xmin,xmax,ymin,ymax);

     for (int ix=xmin;ix<=xmax;ix++)
     for (int iy=ymin;iy<=ymax;iy++)
     if (*(IMAGE2+iy*Ix+ix+offs2)==255) *(IMAGE2+iy*Ix+ix+offs2)=130;
  }


  ImagXpress7_2->LoadBuffer((long)IMAGE2);


}
//---------------------------------------------------------------------------
void TForm1::Flood(int x, int y,int &xmin,int &xmax,int &ymin,int &ymax)
{
   if (x<xmin) xmin=x;
   if (x>xmax) xmax=x;
   if (y<ymin) ymin=y;
   if (y>ymax) ymax=y;

   for (int ix=x-1;ix<=x+1;ix++)
   for (int iy=y-1;iy<=y+1;iy++)
   {
     if ((ix>=0) && (ix<Ix) && (iy>=0) && (iy<Iy))
     {
       if (*(IMAGE+iy*Ix+ix+offs)==0)
       {
          *(IMAGE+iy*Ix+ix+offs)=1;
          Flood(ix,iy,xmin,xmax,ymin,ymax);
       }
     }
   }

}
*/
