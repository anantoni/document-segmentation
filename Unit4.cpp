//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit4.h"
#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ImagXpr7_OCX"
#pragma resource "*.dfm"
TForm4 *Form4;
//---------------------------------------------------------------------------
__fastcall TForm4::TForm4(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm4::Button1Click(TObject *Sender)
{
  ImagXpress7_2->ScrollBars=3;

  double Emax=0;
  float sk;

  for (int i=-50;i<=50;i++)
  {
    ImagXpress7_1->hDIB = Form1->ImagXpress7_1->CopyDIB();
    float th = (float)i/10;
    Edit1->Text = th;
    ImagXpress7_1->Rotate(th);
    ImagXpress7_1->ZoomToFit(2);

    ImagXpress7_1->ColorDepth(8,2,0);
    ImagXpress7_1->SaveToBuffer = true;
    ImagXpress7_1->SaveFileType =  FT_TIFF;
    ImagXpress7_1->SaveFile ();
    unsigned char *IMAGE;
    long offs;
    int Ix = ImagXpress7_1->IWidth;
    int Iy = ImagXpress7_1->IHeight;

    HANDLE hIM = (HANDLE)ImagXpress7_1->SaveBufferHandle;
   // if (IMAGE!=NULL) {GlobalFree(IMAGE); IMAGE=NULL;}
    IMAGE = (unsigned char *)GlobalLock(hIM);
    long ln = GlobalSize(hIM);
    offs=ln-(long)Ix*Iy;
    GlobalUnlock(hIM);

    ImagXpress7_2->CreateDIB(Ix,Iy,24,clWhite);

    double E=0;
    for (int y=0;y<Iy;y++)
    {
      long c=0;
      for (int x=0;x<Ix;x++)
      if (*(IMAGE+y*Ix+x+offs)<128) c++;
      E=E+c*c;
      for (int x=Ix-c;x<Ix;x++)
        ImagXpress7_2->DIBSetPixel(x,y,clBlack);
    }

    if (E>Emax) {Emax=E;sk=th;}

    Edit2->Text = E/1000;
    ImagXpress7_2->DIBUpdate();
    ImagXpress7_2->ZoomToFit(2);
    Application->ProcessMessages();
   // ShowMessage("stop");
  }

  Edit1->Text = sk;
  Edit2->Text = Emax;
  Form1->ImagXpress7_1->Rotate(sk);
  Form1->ImagXpress7_1->DIBUpdate();

}
//---------------------------------------------------------------------------
