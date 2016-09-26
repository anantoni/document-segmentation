//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ImagXpr7_OCX"

#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
#include<vector>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
using namespace std;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner) : TForm(Owner) {
    ImagXpress7_1->ScrollBars = 3;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Open1Click(TObject *Sender) {

    if (OpenDialog->Execute() && FileExists(OpenDialog->FileName)) {
        ImagXpress7_1->FileName = OpenDialog->FileName;
        Ix = ImagXpress7_1->IWidth;
        Iy = ImagXpress7_1->IHeight;

        output = ImagXpress7_1->FileName + ".dat";
        ImagXpress7_1->ColorDepth(8, IPAL_Fixed, 0);
        //ImagXpress7_1->ZoomToFit(2);
        ImagXpress7_1->SaveToBuffer = true;
        ImagXpress7_1->SaveFileType = FT_TIFF;
        ImagXpress7_1->SaveFile();

        HANDLE hIM = (HANDLE)ImagXpress7_1->SaveBufferHandle;
        if (IMAGE != NULL) {
           GlobalFree(IMAGE);
           IMAGE=NULL;
        }
        
        IMAGE = (unsigned char *)GlobalLock(hIM);
        long ln = GlobalSize(hIM);
        offs = ln-(long)Ix*Iy;
        GlobalUnlock(hIM);
        text = "[" + ExtractFileName(ImagXpress7_1->FileName) + ", " + Ix + "x" + Iy + "@" + ImagXpress7_1->IBPP + "bpp]";
        StatusBar->SimpleText = text;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Exit1Click(TObject *Sender) {
    if (IMAGE != NULL) {
        GlobalFree(IMAGE);
    }
    MainForm->Close();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Lines1Click(TObject *Sender) {

    unsigned char *IMAGE = &this->IMAGE[offs];

    /** Initialize all starting values to 0 **/
    int *values = new int[Ix*Iy];
    for(int i = 0; i < Ix*Iy; ++i) {
        values[i] = 0;
    }

    /** Flip IMAGE colours **/
    for(int y = 0; y < Iy; y++) {
        for(int x = 0; x < Ix; x++) {
            //IMAGE[y*Ix + x] = IMAGE[y*Ix +x ] ? 0 : 255;
        }
    }

    /**
     *
     */
    int *horizontal_histogram;
    int y_valley_threshold = StrToInt(yAxisValleyHeightThreshold->Text);

    if ((horizontal_histogram = new int[Iy]) == NULL ) {
       cerr << "Error in y histogram allocation" << endl;
       exit(-1);
    }

    long sum = 0, count = 0;

    for(int y = 0; y < Iy; ++y) {
        horizontal_histogram[y] = 0;
        
        for(int x = 0; x < Ix; ++x) {
            if(IMAGE[y*Ix+x] == 0)
                ++horizontal_histogram[y];
        }

        sum += horizontal_histogram[y];
        if (horizontal_histogram[y])
           ++count;
    }

    int line_width_threshold = CheckBox1->Checked ? sum/count : StrToInt(yAxisLineWidthThreshold->Text);  //minimum line height in pixels

    for(int y = 0; y < Iy; ++y) {
        horizontal_histogram[y] = horizontal_histogram[y] >= line_width_threshold;
    }

    vector<int> horizontal_cuts;

    int y = 0;
    while (y < Iy) {
          int black_valley;
          while (y < Iy && horizontal_histogram[y] == 0) {
                y++;
          }
          black_valley = y;

          int white_valley;
          while (y < Iy && horizontal_histogram[y] != 0) {
                y++;
          }
          white_valley = y;

          int midean = white_valley + (black_valley - white_valley)/2;
          if (black_valley - white_valley >= y_valley_threshold) {
             horizontal_cuts.push_back(midean);
          }
    }

    delete[] horizontal_histogram;
    if(horizontal_cuts.size() < 2) {
        return;
    }
    //assign tags to each region
    for(int i = 0, tag = 1; i < horizontal_cuts.size() - 1; ++i, ++tag) {
        for(int y = horizontal_cuts[i]; y < horizontal_cuts[i+1]; ++y) {
            for(int x = 0; x < Ix; ++x) {
                if(IMAGE[y*Ix+x] != 0) {
                    IMAGE[y*Ix+x] = (values[y*Ix+x] = tag);
                }
            }
        }
    }
    
    FILE *fp = fopen(output.c_str(), "wb+");
    for(int y = 0; y < Iy; ++y)
            for(int x = 0; x < Ix; ++x)
                    fwrite(&values[y*Ix+x], 1, sizeof(int), fp);
    fclose(fp);

    delete[] values;

    ImagXpress7_1->DIBUpdate();
    ImagXpress7_1->LoadBuffer((long) this->IMAGE);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ImagXpress7_1MouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    StatusBar->SimpleText = text + " x: " + IntToStr(ImagXpress7_1->DIBXPos) + ", y: " + IntToStr(ImagXpress7_1->DIBYPos) + " value " + IntToStr(ImagXpress7_1->DIBGetPixel(ImagXpress7_1->DIBXPos,ImagXpress7_1->DIBYPos));
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Words1Click(TObject *Sender)
{

    unsigned char *IMAGE = &this->IMAGE[offs];

    int *values = new int[Ix*Iy];
    for(int i = 0; i < Ix*Iy; ++i) {
        values[i] = 0;
    }

    //flip colors
    for(int y = 0; y < Iy; ++y) {
        for(int x = 0; x < Ix; ++x) {
            //IMAGE[y*Ix+x] = IMAGE[y*Ix+x] ? 0 : 255;
        }
    }

    //y-axis histogram
    int *y_axis_hist = new int[Iy];
    int y_axis_threshold;   //minimum line width in pixels
    long sum = 0, count = 0;

    for(int y = 0; y < Iy; ++y) {
        y_axis_hist[y] = 0;

        for(int x = 0; x < Ix; ++x)
        if(IMAGE[y*Ix+x] == 0)
        ++y_axis_hist[y];

        sum += y_axis_hist[y];

        if(y_axis_hist[y] > 0)
        ++count;
    }
    y_axis_threshold = CheckBox1->Checked ? sum/(count ? count:1) : StrToInt(yAxisLineWidthThreshold->Text);
    for(int y = 0; y < Iy; ++y)
    y_axis_hist[y] = y_axis_hist[y] >= y_axis_threshold;


    //y-axis cuts
    int y_valley_threshold = StrToInt(yAxisValleyHeightThreshold->Text);
    vector<int> horizontal_cuts;

    for(int y = 0; y < Iy;) {
        for(; y < Iy && y_axis_hist[y] != 0; ++y);
        int vs = y;
        for(; y < Iy && y_axis_hist[y] == 0; ++y);
        int ve = y;
        int mid = vs + (ve-vs)/2;
        if(ve-vs >= y_valley_threshold)
        horizontal_cuts.push_back(mid);
    }
    delete[] y_axis_hist;

    if(horizontal_cuts.size() < 2)
    return;
    int tag = 1;
    for(int i = 0; i < horizontal_cuts.size() - 1; ++i)
    MainForm->words(horizontal_cuts[i], horizontal_cuts[i+1], tag, values);


    FILE *fp = fopen(output.c_str(), "wb+");
    for(int y = 0; y < Iy; ++y)
    for(int x = 0; x < Ix; ++x)
    fwrite(&values[y*Ix+x], 1, sizeof(int), fp);
    fclose(fp);

    delete[] values;
    ImagXpress7_1->DIBUpdate();
    ImagXpress7_1->LoadBuffer((long) this->IMAGE);
}
//---------------------------------------------------------------------------
void TMainForm::words(int ys, int ye, int& tag, int* values){
    int *x_axis_hist = new int[Ix];
    long sum = 0, count = 0;
    int x_axis_threshold;

    for(int x = 0; x < Ix; ++x){
        x_axis_hist[x] = 0;
        for(int y = ys; y < ye; ++y)
        if(IMAGE[offs+y*Ix+x] == 0)
        ++x_axis_hist[x];

        sum += x_axis_hist[x];
        if(x_axis_hist[x])
        ++count;
    }

    x_axis_threshold = CheckBox2->Checked ? sum/count : StrToInt(xAxisLineHeightThreshold->Text);
    for(int x = 0; x < Ix; ++x)
    x_axis_hist[x] = x_axis_hist[x] >= x_axis_threshold;

    //x-axis cuts
    int x_valley_threshold = StrToInt(xAxisValleyWidthThreshold->Text);
    std::vector<int> x_cuts;

    for(int x = 0; x < Ix;) {
        for(; x < Ix && x_axis_hist[x] != 0; ++x);
        int vs = x;
        for(; x < Ix && x_axis_hist[x] == 0; ++x);
        int ve = x;

        int mid = vs + (ve-vs)/2;
        if(ve-vs >= x_valley_threshold)
        x_cuts.push_back(mid);
    }

    if(x_cuts.size() < 2)
    return;

    for(int i = 0; i < x_cuts.size()-1; ++i) {
        for(int x = x_cuts[i]; x < x_cuts[i+1]; ++x) {
            for(int y = ys; y < ye; ++y) {
                if(IMAGE[offs+y*Ix+x] != 0) {
                    IMAGE[offs+y*Ix+x] = (values[y*Ix+x] = tag);
                }
            }
        }
        ++tag;
    }
    delete[] x_axis_hist;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LinesButtonClick(TObject *Sender)
{
    Lines1Click(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WordsButtonClick(TObject *Sender)
{
    Words1Click(Sender);
}
//---------------------------------------------------------------------------

/*
void __fastcall TMainForm::EvaluateLinesClick(TObject *Sender)
{
for(int i=301; i<401; ++i){
ImagXpress7_1->FileName = "C:\\Users\\ek\\Desktop\\eval\\IMAGEs\\" + IntToStr(i) + ".tif";
OpenDialog->FileName = ImagXpress7_1->FileName;
output = "C:\\Users\\ek\\Desktop\\eval\\results\\lines\\" + IntToStr(i) + ".tif.dat";
Lines1Click(Sender);
}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EvaluateWordsClick(TObject *Sender)
{
for(int i=301; i<401; ++i){
ImagXpress7_1->FileName = "C:\\Users\\ek\\Desktop\\eval\\IMAGEs\\" + IntToStr(i) + ".tif";
OpenDialog->FileName = ImagXpress7_1->FileName;
output = "C:\\Users\\ek\\Desktop\\eval\\results\\words\\" + IntToStr(i) + ".tif.dat";
Words1Click(Sender);
}
}
//---------------------------------------------------------------------------
*/
