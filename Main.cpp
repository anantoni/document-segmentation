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
    //unsigned char *IMAGE = &this->IMAGE[offs];

    /** Initialize all starting values to 0 **/
    int *values = new int[Ix*Iy];
    for(int i = 0; i < Ix*Iy; ++i) {
        values[i] = 0;
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

    vector<int> horizontal_cuts, horizontal_cut_kinds;

    int y = 0;
    while (y < Iy) {
        int black_valley;
        while (y < Iy && horizontal_histogram[y] == 0) {
            y++;
        }
        black_valley = y;

        //int midean = black_valley + (white_valley - black_valley)/2;
        if (black_valley >= y_valley_threshold) {
            horizontal_cuts.push_back(black_valley);
            horizontal_cut_kinds.push_back(1);
        }

        int white_valley;
        while (y < Iy && horizontal_histogram[y] != 0) {
            y++;
        }
        white_valley = y;

        if (white_valley >= y_valley_threshold) {
            horizontal_cuts.push_back(white_valley);
            horizontal_cut_kinds.push_back(0);
        }
    }

    delete[] horizontal_histogram;
    if(horizontal_cuts.size() < 2) {
        return;
    }

    int colour = 190;
    for(int i = 0; i < horizontal_cuts.size() - 1; ++i) {
        for(int y = horizontal_cuts[i]; y < horizontal_cuts[i+1]; ++y) {
            if (horizontal_cut_kinds[i] == 1) {
                for(int x = 0; x < Ix; ++x) {
                    if(IMAGE[y*Ix+x] != 0) {
                        IMAGE[y*Ix+x] = (values[y*Ix+x] = colour);
                    }
                }
            }
            else {
                for(int x = 0; x < Ix; ++x) {
                    if(IMAGE[y*Ix+x] != 0) {
                        IMAGE[y*Ix+x] = (values[y*Ix+x] = 255);
                    }
                }
            }
        }
        //if (horizontal_cut_kinds[i] == 1)
        //   colour -= 5;
    }

    FILE *fp = fopen(output.c_str(), "wb+");
    for(int y = 0; y < Iy; ++y) {
        for(int x = 0; x < Ix; ++x) {
            fwrite(&values[y*Ix+x], 1, sizeof(int), fp);
        }
    }
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

    /** Initialize all starting values to write to data file to 0 **/
    int *values_to_write = new int[Ix*Iy];
    for(int i = 0; i < Ix*Iy; ++i) {
        values_to_write[i] = 0;
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

    vector<int> horizontal_cuts, horizontal_cut_kinds;

    int y = 0;
    while (y < Iy) {
        int black_valley;
        while (y < Iy && horizontal_histogram[y] == 0) {
            y++;
        }
        black_valley = y;

        //int midean = black_valley + (white_valley - black_valley)/2;
        if (black_valley >= y_valley_threshold) {
            horizontal_cuts.push_back(black_valley);
            horizontal_cut_kinds.push_back(1);
        }

        int white_valley;
        while (y < Iy && horizontal_histogram[y] != 0) {
            y++;
        }
        white_valley = y;

        if (white_valley >= y_valley_threshold) {
            horizontal_cuts.push_back(white_valley);
            horizontal_cut_kinds.push_back(0);
        }
    }

    delete[] horizontal_histogram;
    if(horizontal_cuts.size() < 2) {
        return;
    }

    int colour = 190;
    for(int i = 0; i < horizontal_cuts.size() - 1; ++i) {

        if(horizontal_cut_kinds[i] == 1) {
            MainForm->words(horizontal_cuts[i], horizontal_cuts[i+1], colour, values_to_write);
        }
        else {
            for(int y = horizontal_cuts[i]; y < horizontal_cuts[i+1]; ++y) {
                if (horizontal_cut_kinds[i] == 1) {
                    for(int x = 0; x < Ix; ++x) {
                        if(IMAGE[y*Ix+x] != 0) {
                            IMAGE[y*Ix+x] = values_to_write[y*Ix+x] = colour;
                        }
                    }
                }

                else {
                    for(int x = 0; x < Ix; ++x) {
                        if(IMAGE[y*Ix+x] != 0) {
                            IMAGE[y*Ix+x] = (values_to_write[y*Ix+x] = 255);
                        }
                    }
                }
            }
        }
    }

    FILE *fp = fopen(output.c_str(), "wb+");
    for(int y = 0; y < Iy; ++y)
    for(int x = 0; x < Ix; ++x)
    fwrite(&values_to_write[y*Ix+x], 1, sizeof(int), fp);
    fclose(fp);

    delete[] values_to_write;
    ImagXpress7_1->DIBUpdate();
    ImagXpress7_1->LoadBuffer((long) this->IMAGE);
}

void TMainForm::words(int ys, int ye, int& colour, int* values_to_write) {

    int *vertical_histogram = new int[Ix];
    long sum = 0, count = 0;
    int x_axis_threshold;

    for(int x = 0; x < Ix; ++x) {
        vertical_histogram[x] = 0;

        for(int y = ys; y < ye; ++y) {
            if(IMAGE[offs+y*Ix+x] == 0)
            vertical_histogram[x]++;
        }

        sum += vertical_histogram[x];
        if(vertical_histogram[x])
        ++count;
    }

    x_axis_threshold = CheckBox2->Checked ? sum/count : StrToInt(xAxisLineHeightThreshold->Text);
    for(int x = 0; x < Ix; ++x) {
        vertical_histogram[x] = vertical_histogram[x] >= x_axis_threshold;
    }
    //x-axis cuts
    int x_valley_threshold = StrToInt(xAxisValleyWidthThreshold->Text);
    std::vector<int> x_cuts;

    vector<int> vertical_cuts, vertical_cut_kinds;

    int x = 0;
    while (x < Ix) {
        int black_valley;
        while (x < Ix && vertical_histogram[x] == 0) {
            x++;
        }
        black_valley = x;

        //int midean = black_valley + (white_valley - black_valley)/2;
        if (black_valley >= x_valley_threshold) {
            vertical_cuts.push_back(black_valley);
            vertical_cut_kinds.push_back(1);
        }

        int white_valley;
        while (x < Ix && vertical_histogram[x] != 0) {
            x++;
        }
        white_valley = x;

        if (white_valley >= x_valley_threshold) {
            vertical_cuts.push_back(white_valley);
            vertical_cut_kinds.push_back(0);
        }
    }

    if(vertical_cuts.size() < 2)
    return;

    for(int i = 0; i < vertical_cuts.size()-1; ++i) {
        if (vertical_cut_kinds[i] == 1) {
            for(int x = vertical_cuts[i]; x < vertical_cuts[i+1]; ++x) {
                for(int y = ys; y < ye; ++y) {
                    if(IMAGE[offs+y*Ix+x] != 0) {
                        IMAGE[offs+y*Ix+x] = (values_to_write[y*Ix+x] = colour);
                    }
                }
            }
        }
        else {
            for(int x = vertical_cuts[i]; x < vertical_cuts[i+1]; ++x) {
                for(int y = ys; y < ye; ++y) {
                    if(IMAGE[offs+y*Ix+x] != 0) {
                        IMAGE[offs+y*Ix+x] = (values_to_write[y*Ix+x] = 255);
                    }
                }
            }
        }
    }
    delete[] vertical_histogram;
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


void __fastcall TMainForm::EvaluateLinesClick(TObject *Sender)
{
for(int i=301; i<401; ++i){
ImagXpress7_1->FileName = "C:\\Users\\anantoni\\Documents\\eval\\IMAGEs\\" + IntToStr(i) + ".tif";
OpenDialog->FileName = ImagXpress7_1->FileName;
output = "C:\\Users\\anantoni\\Documents\\eval\\results\\lines\\" + IntToStr(i) + ".tif.dat";
Lines1Click(Sender);
}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EvaluateWordsClick(TObject *Sender)
{
for(int i=301; i<401; ++i){
ImagXpress7_1->FileName = "C:\\Users\\nymeria\\Documents\\eval\\IMAGEs\\" + IntToStr(i) + ".tif";
OpenDialog->FileName = ImagXpress7_1->FileName;
output = "C:\\Users\\nymeria\\Documents\\eval\\results\\words\\" + IntToStr(i) + ".tif.dat";
Words1Click(Sender);
}
}
//---------------------------------------------------------------------------












