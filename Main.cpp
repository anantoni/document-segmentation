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
#include<math.h>
#include<string.h>
#include<sstream>
#include <assert.h>
#include "Unit1.h"
#include "Unit2.h"
#include "Unit3.h"
#include "Unit4.h"
using namespace std;

//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner) : TForm(Owner) {
    ImagXpress7_1->ScrollBars = 3;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OpenClick(TObject *Sender) {

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
void __fastcall TMainForm::ExitClick(TObject *Sender) {
    if (IMAGE != NULL) {
        GlobalFree(IMAGE);
    }
    MainForm->Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LinesClick(TObject *Sender) {
    ImagXpress7_1->FileName = OpenDialog->FileName;
    Ix = ImagXpress7_1->IWidth;
    Iy = ImagXpress7_1->IHeight;

    //output = ImagXpress7_1->FileName + ".dat";
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
    unsigned char *IMAGE = &this->IMAGE[offs];

    /** Initialize all starting values to 0 **/
    int *values = new int[Ix*Iy];
    for(int i = 0; i < Ix*Iy; i++) {
        values[i] = 0;
    }

    int *horizontal_histogram;
    int y_valley_threshold = StrToFloat(yAxisValleyHeightThreshold->Text);

    if ((horizontal_histogram = new int[Iy]) == NULL ) {
        cerr << "Error in y histogram allocation" << endl;
        exit(-1);
    }

    for(int y = 0; y < Iy; y++) {
        horizontal_histogram[y] = 0;

        for(int x = 0; x < Ix; x++) {
            if(IMAGE[y*Ix+x] == 0) {
                horizontal_histogram[y] += 1;
            }
        }
    }

    int line_width_threshold = StrToFloat(yAxisLineWidthThreshold->Text);

    for(int y = 0; y < Iy; y++) {
        float line_pixel_threshold = (float)((line_width_threshold*Ix)/100.0);
        horizontal_histogram[y] = horizontal_histogram[y] >= line_pixel_threshold;
    }

    vector<int> horizontal_cuts, horizontal_cut_kinds;

    int y = 0;
    while (y < Iy) {
        while (y < Iy && horizontal_histogram[y] != 0) {
            y++;
        }
        int black_valley = y;

        while (y < Iy && horizontal_histogram[y] == 0) {
            y++;
        }
        int white_valley = y;

        int midean = black_valley + (white_valley - black_valley)/2;
        if (white_valley - black_valley >= (float)y_valley_threshold*Ix/100.0) {
            horizontal_cuts.push_back(midean);
        }
    }

    delete[] horizontal_histogram;
    if(horizontal_cuts.size() < 2) {
        return;
    }

    int colour = 1;
    for(int i = 0; i < horizontal_cuts.size() - 1; i++) {
        for(int y = horizontal_cuts[i]; y < horizontal_cuts[i+1]; y++) {
            for(int x = 0; x < Ix; x++) {
                if(IMAGE[y*Ix+x] == 0) {
                    IMAGE[y*Ix+x] = values[y*Ix+x] = colour;
                }
            }
        }
        colour++;
    }

    FILE *fp = fopen(output.c_str(), "wb+");
    for(int y = 0; y < Iy; y++) {
        for(int x = 0; x < Ix; x++) {
            fwrite(&values[y*Ix+x], 1, sizeof(int), fp);
        }
    }
    fclose(fp);

    delete[] values;

    ImagXpress7_1->DIBUpdate();
    ImagXpress7_1->LoadBuffer((long) this->IMAGE);
}

void __fastcall TMainForm::LinesDetailedClick(TObject *Sender) {
    ImagXpress7_1->FileName = OpenDialog->FileName;
    Ix = ImagXpress7_1->IWidth;
    Iy = ImagXpress7_1->IHeight;

    output = ImagXpress7_1->FileName + ".dat";
    ImagXpress7_1->ColorDepth(8, IPAL_Fixed, 0);
    ImagXpress7_1->SaveToBuffer = true;
    ImagXpress7_1->SaveFileType = FT_TIFF;
    ImagXpress7_1->SaveFile();

    Form1->ImagXpress7_1->FileName = OpenDialog->FileName;
    Form1->Ix1 = Form1->ImagXpress7_1->IWidth;
    Form1->Iy1 = Form1->ImagXpress7_1->IHeight;
    Form1->ImagXpress7_1->ColorDepth(8, IPAL_Fixed, 0);
    Form1->ImagXpress7_1->ZoomToFit(2);
    Form1->ImagXpress7_1->SaveToBuffer = true;
    Form1->ImagXpress7_1->SaveFileType = FT_TIFF;
    Form1->ImagXpress7_1->SaveFile();

    Form1->ImagXpress7_2->FileName = OpenDialog->FileName;
    Form1->Ix2 = Form1->ImagXpress7_2->IWidth;
    Form1->Iy2 = Form1->ImagXpress7_2->IHeight;
    Form1->ImagXpress7_2->ColorDepth(8, IPAL_Fixed, 0);
    Form1->ImagXpress7_2->ZoomToFit(2);
    Form1->ImagXpress7_2->SaveToBuffer = true;
    Form1->ImagXpress7_2->SaveFileType = FT_TIFF;
    Form1->ImagXpress7_2->SaveFile();

    HANDLE hIM = (HANDLE)ImagXpress7_1->SaveBufferHandle;
    if (IMAGE != NULL) {
        GlobalFree(IMAGE);
        IMAGE = NULL;
    }

    HANDLE hIM1 = (HANDLE)Form1->ImagXpress7_1->SaveBufferHandle;
    if (Form1->IMAGE1 != NULL) {
        GlobalFree(Form1->IMAGE1);
        Form1->IMAGE1 = NULL;
    }

    HANDLE hIM2 = (HANDLE)Form1->ImagXpress7_2->SaveBufferHandle;
    if (Form1->IMAGE2 != NULL) {
        GlobalFree(Form1->IMAGE2);
        Form1->IMAGE2 = NULL;
    }

    IMAGE = (unsigned char *)GlobalLock(hIM);
    long ln = GlobalSize(hIM);
    offs = ln - (long)Ix*Iy;
    GlobalUnlock(hIM);

    Form1->IMAGE1 = (unsigned char *)GlobalLock(hIM1);
    long ln1 = GlobalSize(hIM1);
    Form1->offs1 = ln1 - (long)Form1->Ix1*Form1->Iy1;
    GlobalUnlock(hIM1);

    Form1->IMAGE2 = (unsigned char *)GlobalLock(hIM2);
    long ln2 = GlobalSize(hIM2);
    Form1->offs2 = ln2 - (long)Form1->Ix2*Form1->Iy2;
    GlobalUnlock(hIM2);

    text = "[" + ExtractFileName(ImagXpress7_1->FileName) + ", " + Ix + "x" + Iy + "@" + ImagXpress7_1->IBPP + "bpp]";
    StatusBar->SimpleText = text;
    unsigned char *IMAGE = &this->IMAGE[offs];
    unsigned char *IMAGE1 = &Form1->IMAGE1[Form1->offs1];
    unsigned char *IMAGE2 = &Form1->IMAGE2[Form1->offs2];

    /** Initialize all starting values to 0 **/
    int *values = new int[Ix*Iy];
    for(int i = 0; i < Ix*Iy; i++) {
        values[i] = 0;
    }

    int *horizontal_histogram;
    int y_valley_threshold = StrToFloat(yAxisValleyHeightThreshold->Text);

    if ((horizontal_histogram = new int[Iy]) == NULL ) {
        cerr << "Error in y histogram allocation" << endl;
        exit(-1);
    }

    for(int y = 0; y < Iy; y++) {
        horizontal_histogram[y] = 0;

        for(int x = 0; x < Ix; x++) {
            if(IMAGE[y*Ix+x] == 0) {
                horizontal_histogram[y] += 1;
            }
        }
    }

    for (int i = 0; i < Form1->Iy1; i++) {
        for (int j = 0; j < horizontal_histogram[i]; j++)
        if (IMAGE1[i* Form1->Ix1 + j] != 0)
        IMAGE1[i* Form1->Ix1 + j] = 50;
    }

    int line_width_threshold = StrToFloat(yAxisLineWidthThreshold->Text);

    for(int y = 0; y < Iy; y++) {
        float line_pixel_threshold = (float)((line_width_threshold*Ix)/100.0);
        horizontal_histogram[y] = (horizontal_histogram[y] >= line_pixel_threshold);
    }

    for(int i = 0; i < Form1->Iy2; i++) {
        if (horizontal_histogram[i] > 0)
        for(int j = 0; j < 50; j++)
        IMAGE2[i* Form1->Ix2 + j] = 100;
    }

    vector<int> horizontal_cuts;

    int y = 0;
    while (y < Iy) {
        while (y < Iy && horizontal_histogram[y] != 0) {
            y++;
        }
        int black_valley = y;

        while (y < Iy && horizontal_histogram[y] == 0) {
            y++;
        }
        int white_valley = y;

        int midean = black_valley + (white_valley - black_valley)/2;
        if (white_valley - black_valley >= (float)y_valley_threshold*Ix/100.0) {
            horizontal_cuts.push_back(midean);
        }
    }

    delete[] horizontal_histogram;
    if(horizontal_cuts.size() < 2) {
        return;
    }

    int colour = 1;
    for(int i = 0; i < horizontal_cuts.size() - 1; i++) {
        for(int y = horizontal_cuts[i]; y < horizontal_cuts[i+1]; y++) {
            for(int x = 0; x < Ix; x++) {
                if(IMAGE[y*Ix+x] == 0) {
                    IMAGE[y*Ix+x] = (values[y*Ix+x] = colour);
                }
            }
        }
        colour++;
    }

    FILE *fp = fopen(output.c_str(), "wb+");
    for(int y = 0; y < Iy; y++) {
        for(int x = 0; x < Ix; x++) {
            fwrite(&values[y*Ix+x], 1, sizeof(int), fp);
        }
    }
    fclose(fp);

    delete[] values;

    ImagXpress7_1->DIBUpdate();
    ImagXpress7_1->LoadBuffer((long) this->IMAGE);

    Form1->Show();
    Form1->ImagXpress7_1->DIBUpdate();
    Form1->ImagXpress7_1->LoadBuffer((long) Form1->IMAGE1);

    Form1->ImagXpress7_2->DIBUpdate();
    Form1->ImagXpress7_2->LoadBuffer((long) Form1->IMAGE2);
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::ImagXpress7_1MouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    StatusBar->SimpleText = text + " x: " + IntToStr(ImagXpress7_1->DIBXPos) + ", y: " + IntToStr(ImagXpress7_1->DIBYPos) + " value " + IntToStr(ImagXpress7_1->DIBGetPixel(ImagXpress7_1->DIBXPos,ImagXpress7_1->DIBYPos));
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WordsClick(TObject *Sender)
{
    ImagXpress7_1->FileName = OpenDialog->FileName;
    Ix = ImagXpress7_1->IWidth;
    Iy = ImagXpress7_1->IHeight;

    //output = ImagXpress7_1->FileName + ".dat";
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
    unsigned char *IMAGE = &this->IMAGE[offs];

    /** Initialize all starting values to write to data file to 0 **/
    int *values_to_write = new int[Ix*Iy];
    for(int i = 0; i < Ix*Iy; i++) {
        values_to_write[i] = 0;
    }

    int *horizontal_histogram;
    float y_valley_threshold = StrToFloat(yAxisValleyHeightThreshold->Text);

    if ((horizontal_histogram = new int[Iy]) == NULL ) {
        cerr << "Error in y histogram allocation" << endl;
        exit(-1);
    }

    for(int y = 0; y < Iy; y++) {
        horizontal_histogram[y] = 0;

        for(int x = 0; x < Ix; x++) {
            if(IMAGE[y*Ix+x] == 0) {
                ++horizontal_histogram[y];
            }
        }
    }

    float line_width_threshold = StrToFloat(yAxisLineWidthThreshold->Text);
    float line_pixel_threshold = (line_width_threshold*Ix)/100.0;
    for(int y = 0; y < Iy; y++) {
        horizontal_histogram[y] = (horizontal_histogram[y] >= line_pixel_threshold);
    }

    vector<int> horizontal_cuts;

    int y = 0;
    while (y < Iy) {
        while (y < Iy && horizontal_histogram[y] != 0)
        y++;
        int black_valley = y;

        while (y < Iy && horizontal_histogram[y] == 0)
        y++;
        int white_valley = y;

        int midean = black_valley + (white_valley - black_valley)/2;

        if (white_valley - black_valley >= (float)(y_valley_threshold * Iy)/100.0) {
            horizontal_cuts.push_back(midean);
        }
    }

    delete[] horizontal_histogram;
    if(horizontal_cuts.size() < 2) {
        return;
    }

    int colour = 1;
    for(int i = 0; i < horizontal_cuts.size() - 1; i++) {
        MainForm->SplitLineToWords(horizontal_cuts[i], horizontal_cuts[i+1], colour, values_to_write);
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

void __fastcall TMainForm::WordsDetailedClick(TObject *Sender)
{

    ImagXpress7_1->FileName = OpenDialog->FileName;
    Ix = ImagXpress7_1->IWidth;
    Iy = ImagXpress7_1->IHeight;

    output = ImagXpress7_1->FileName + ".dat";
    ImagXpress7_1->ColorDepth(8, IPAL_Fixed, 0);
    ImagXpress7_1->SaveToBuffer = true;
    ImagXpress7_1->SaveFileType = FT_TIFF;
    ImagXpress7_1->SaveFile();

    Form1->ImagXpress7_1->FileName = OpenDialog->FileName;
    Form1->Ix1 = Form1->ImagXpress7_1->IWidth;
    Form1->Iy1 = Form1->ImagXpress7_1->IHeight;
    Form1->ImagXpress7_1->ColorDepth(8, IPAL_Fixed, 0);
    Form1->ImagXpress7_1->ZoomToFit(2);
    Form1->ImagXpress7_1->SaveToBuffer = true;
    Form1->ImagXpress7_1->SaveFileType = FT_TIFF;
    Form1->ImagXpress7_1->SaveFile();

    Form1->ImagXpress7_2->FileName = OpenDialog->FileName;
    Form1->Ix2 = Form1->ImagXpress7_2->IWidth;
    Form1->Iy2 = Form1->ImagXpress7_2->IHeight;
    Form1->ImagXpress7_2->ColorDepth(8, IPAL_Fixed, 0);
    Form1->ImagXpress7_2->ZoomToFit(2);
    Form1->ImagXpress7_2->SaveToBuffer = true;
    Form1->ImagXpress7_2->SaveFileType = FT_TIFF;
    Form1->ImagXpress7_2->SaveFile();

    HANDLE hIM = (HANDLE)ImagXpress7_1->SaveBufferHandle;
    if (IMAGE != NULL) {
        GlobalFree(IMAGE);
        IMAGE = NULL;
    }

    HANDLE hIM1 = (HANDLE)Form1->ImagXpress7_1->SaveBufferHandle;
    if (Form1->IMAGE1 != NULL) {
        GlobalFree(Form1->IMAGE1);
        Form1->IMAGE1 = NULL;
    }

    HANDLE hIM2 = (HANDLE)Form1->ImagXpress7_2->SaveBufferHandle;
    if (Form1->IMAGE2 != NULL) {
        GlobalFree(Form1->IMAGE2);
        Form1->IMAGE2 = NULL;
    }

    IMAGE = (unsigned char *)GlobalLock(hIM);
    long ln = GlobalSize(hIM);
    offs = ln - (long)Ix*Iy;
    GlobalUnlock(hIM);

    Form1->IMAGE1 = (unsigned char *)GlobalLock(hIM1);
    long ln1 = GlobalSize(hIM1);
    Form1->offs1 = ln1 - (long)Form1->Ix1*Form1->Iy1;
    GlobalUnlock(hIM1);

    Form1->IMAGE2 = (unsigned char *)GlobalLock(hIM2);
    long ln2 = GlobalSize(hIM2);
    Form1->offs2 = ln2 - (long)Form1->Ix2*Form1->Iy2;
    GlobalUnlock(hIM2);

    text = "[" + ExtractFileName(ImagXpress7_1->FileName) + ", " + Ix + "x" + Iy + "@" + ImagXpress7_1->IBPP + "bpp]";
    StatusBar->SimpleText = text;
    unsigned char *IMAGE = &this->IMAGE[offs];
    unsigned char *IMAGE1 = &Form1->IMAGE1[Form1->offs1];
    unsigned char *IMAGE2 = &Form1->IMAGE2[Form1->offs2];

    /** Initialize all starting values to 0 **/
    int *values = new int[Ix*Iy];
    for(int i = 0; i < Ix*Iy; i++) {
        values[i] = 0;
    }

    int *horizontal_histogram;
    int y_valley_threshold = StrToFloat(yAxisValleyHeightThreshold->Text);

    if ((horizontal_histogram = new int[Iy]) == NULL ) {
        cerr << "Error in y histogram allocation" << endl;
        exit(-1);
    }

    for(int y = 0; y < Iy; y++) {
        horizontal_histogram[y] = 0;

        for(int x = 0; x < Ix; x++) {
            if(IMAGE[y*Ix+x] == 0) {
                horizontal_histogram[y] += 1;
            }
        }
    }

    for (int i = 0; i < Form1->Iy1; i++) {
        for (int j = 0; j < horizontal_histogram[i]; j++)
        if (IMAGE1[i* Form1->Ix1 + j] != 0)
        IMAGE1[i* Form1->Ix1 + j] = 50;
    }

    int line_width_threshold = StrToFloat(yAxisLineWidthThreshold->Text);

    for(int y = 0; y < Iy; y++) {
        float line_pixel_threshold = (float)((line_width_threshold*Ix)/100.0);
        horizontal_histogram[y] = (horizontal_histogram[y] >= line_pixel_threshold);
    }

    for(int i = 0; i < Form1->Iy2; i++) {
        if (horizontal_histogram[i] > 0)
        for(int j = 0; j < 50; j++)
        IMAGE2[i* Form1->Ix2 + j] = 100;
    }

    vector<int> horizontal_cuts;

    int y = 0;
    while (y < Iy) {
        while (y < Iy && horizontal_histogram[y] != 0) {
            y++;
        }
        int black_valley = y;

        while (y < Iy && horizontal_histogram[y] == 0) {
            y++;
        }
        int white_valley = y;

        int midean = black_valley + (white_valley - black_valley)/2;
        if (white_valley - black_valley >= (float)y_valley_threshold*Iy/100.0) {
            horizontal_cuts.push_back(midean);
        }
    }

    delete[] horizontal_histogram;
    if(horizontal_cuts.size() < 2) {
        return;
    }

    int colour = 1;
    for(int i = 0; i < horizontal_cuts.size() - 1; i++) {
        MainForm->SplitLineToWordsDetailed(horizontal_cuts[i], horizontal_cuts[i+1], colour, values);
    }

    FILE *fp = fopen(output.c_str(), "wb+");
    for(int y = 0; y < Iy; y++) {
        for(int x = 0; x < Ix; x++) {
            fwrite(&values[y*Ix+x], 1, sizeof(int), fp);
        }
    }

    fclose(fp);

    delete[] values;

    ImagXpress7_1->DIBUpdate();
    ImagXpress7_1->LoadBuffer((long) this->IMAGE);

    Form1->Show();
    Form1->ImagXpress7_1->DIBUpdate();
    Form1->ImagXpress7_1->LoadBuffer((long) Form1->IMAGE1);

    Form1->ImagXpress7_2->DIBUpdate();
    Form1->ImagXpress7_2->LoadBuffer((long) Form1->IMAGE2);


}

void TMainForm::SplitLineToWords(int ys, int ye, int& colour, int* values_to_write) {

    int *vertical_histogram = new int[Ix];
    long sum = 0, count = 0;
    int x_axis_threshold;

    unsigned char *IMAGE = &this->IMAGE[offs];
    for(int x = 0; x < Ix; x++) {
        vertical_histogram[x] = 0;

        for(int y = ys; y < ye; y++) {
            if(IMAGE[y*Ix+x] == 0)
            vertical_histogram[x]++;
        }
    }

    x_axis_threshold = StrToFloat(xAxisLineHeightThreshold->Text);
    float column_pixel_threshold = (float)(x_axis_threshold * Iy)/100.0;
    for(int x = 0; x < Ix; x++) {
        vertical_histogram[x] = vertical_histogram[x] >= column_pixel_threshold;
    }
    //x-axis cuts
    int x_valley_threshold = StrToFloat(xAxisValleyWidthThreshold->Text);
    std::vector<int> x_cuts;

    vector<int> vertical_cuts, vertical_cut_kinds;

    int x = 0;
    while (x < Ix) {
        while (x < Ix && vertical_histogram[x] != 0)
        x++;
        int black_valley = x;

        while (x < Ix && vertical_histogram[x] == 0)
        x++;
        int white_valley = x;

        int midean = black_valley + (white_valley - black_valley)/2;
        if (white_valley - black_valley >= (float)(x_valley_threshold * Iy)/100.0) {
            vertical_cuts.push_back(midean);
        }
    }

    if(vertical_cuts.size() < 2) {
        return;
    }
    for(int i = 0; i < vertical_cuts.size()-1; i++) {
        for(int x = vertical_cuts[i]; x < vertical_cuts[i+1]; x++) {
            for(int y = ys; y < ye; y++) {
                if(IMAGE[y*Ix+x] == 0) {
                    IMAGE[y*Ix+x] = (values_to_write[y*Ix+x] = colour);
                }
            }
        }
        colour++;
    }
    delete[] vertical_histogram;
}
//---------------------------------------------------------------------------
void TMainForm::SplitLineToWordsDetailed(int ys, int ye, int& colour, int* values_to_write) {

    int *vertical_histogram = new int[Ix];
    long sum = 0, count = 0;
    int x_axis_threshold;

    for(int x = 0; x < Ix; x++) {
        vertical_histogram[x] = 0;

        for(int y = ys; y < ye; y++) {
            if(IMAGE[y*Ix+x] == 0)
            vertical_histogram[x]++;
        }
    }

    unsigned char *IMAGE = &this->IMAGE[offs];
    unsigned char *IMAGE1 = &Form1->IMAGE1[Form1->offs1];
    unsigned char *IMAGE2 = &Form1->IMAGE2[Form1->offs2];

    for (int i = 0; i < Form1->Ix1; i++) {
        for (int j = 0; j < vertical_histogram[i]; j++)
        if (IMAGE1[i+ j * Form1->Ix1] != 0)
        IMAGE1[i + j * Form1->Ix1] = 50;
    }


    x_axis_threshold = StrToFloat(xAxisLineHeightThreshold->Text);
    for(int x = 0; x < Ix; x++) {
        float column_pixel_threshold = (float)(x_axis_threshold * Iy)/100.0;
        vertical_histogram[x] = vertical_histogram[x] >= column_pixel_threshold;
    }

    for(int i = 0; i < Form1->Ix2; i++) {
        if (vertical_histogram[i] > 0)
        for(int j = 0; j < 50; j++)
        IMAGE2[i + j* Form1->Ix2] = 100;
    }

    //x-axis cuts
    int x_valley_threshold = StrToFloat(xAxisValleyWidthThreshold->Text);
    std::vector<int> x_cuts;

    vector<int> vertical_cuts, vertical_cut_kinds;

    int x = 0;
    while (x < Ix) {
        while (x < Ix && vertical_histogram[x] != 0)
        x++;
        int black_valley = x;

        while (x < Ix && vertical_histogram[x] == 0)
        x++;
        int white_valley = x;

        int midean = black_valley + (white_valley - black_valley)/2;
        if (white_valley - black_valley >= (float)(x_valley_threshold * (float)Ix)/100.0) {
            vertical_cuts.push_back(midean);
        }
    }

    if(vertical_cuts.size() < 2) {
        return;
    }
    for(int i = 0; i < vertical_cuts.size()-1; i++) {
        for(int x = vertical_cuts[i]; x < vertical_cuts[i+1]; x++) {
            for(int y = ys; y < ye; y++) {
                if(IMAGE2[y*Form1->Ix2+x] != 0 && IMAGE2[y*Form1->Ix2+x] != 100) {
                    IMAGE2[y*Form1->Ix2+x] = colour;
                }
                if(IMAGE[y*Ix+x] == 0) {
                    IMAGE[y*Ix+x] = values_to_write[y*Ix+x] = colour;
                }
            }
        }
        colour++;
    }
    delete[] vertical_histogram;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LinesButtonClick(TObject *Sender)
{
    LinesClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WordsButtonClick(TObject *Sender)
{
    WordsClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::EvaluateLinesClick(TObject *Sender)
{
    for(int i=101; i<301; ++i){
        ImagXpress7_1->FileName = "C:\\Users\\nymeria\\Documents\\evaluation\\images\\" + IntToStr(i) + ".tif";
        OpenDialog->FileName = ImagXpress7_1->FileName;
        output = "C:\\Users\\nymeria\\Documents\\evaluation\\results\\lines\\" + IntToStr(i) + ".tif.dat";
        LinesClick(Sender);

    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EvaluateNewLinesClick(TObject *Sender)
{
    for(int i=101; i<301; ++i){
        ImagXpress7_1->FileName = "C:\\Users\\nymeria\\Documents\\evaluation\\images\\" + IntToStr(i) + ".tif";
        OpenDialog->FileName = ImagXpress7_1->FileName;
        output = "C:\\Users\\nymeria\\Documents\\evaluation\\results\\lines\\" + IntToStr(i) + ".tif.dat";
        SplitLinesNewClick(Sender);
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EvaluateWordsClick(TObject *Sender)
{
    for(int i=101; i<301; ++i){
        ImagXpress7_1->FileName = "C:\\Users\\nymeria\\Documents\\evaluation\\images\\" + IntToStr(i) + ".tif";
        OpenDialog->FileName = ImagXpress7_1->FileName;
        output = "C:\\Users\\nymeria\\Documents\\evaluation\\results\\words\\" + IntToStr(i) + ".tif.dat";
        WordsClick(Sender);
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SplitWordsNewBatchClick(TObject *Sender)
{
    for(int i=101; i<301; ++i){
        ImagXpress7_1->FileName = "C:\\Users\\nymeria\\Documents\\evaluation\\images\\" + IntToStr(i) + ".tif";
        OpenDialog->FileName = ImagXpress7_1->FileName;
        output = "C:\\Users\\nymeria\\Documents\\evaluation\\results\\words\\" + IntToStr(i) + ".tif.dat";
        SplitWordsNewClick(Sender);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SplitLinesNewClick(TObject *Sender)
{
    ImagXpress7_1->FileName = OpenDialog->FileName;
    Ix = ImagXpress7_1->IWidth;
    Iy = ImagXpress7_1->IHeight;

    //output = "C:\\Users\\nymeria\\Documents\\evaluation\\results\\words\\" + ImagXpress7_1->FileName + ".dat";
    ImagXpress7_1->ColorDepth(8, IPAL_Fixed, 0);
    //ImagXpress7_1->ZoomToFit(2);
    ImagXpress7_1->SaveToBuffer = true;
    ImagXpress7_1->SaveFileType = FT_TIFF;
    ImagXpress7_1->SaveFile();

    Form1->ImagXpress7_1->FileName = OpenDialog->FileName;
    Form1->Ix1 = Form1->ImagXpress7_1->IWidth;
    Form1->Iy1 = Form1->ImagXpress7_1->IHeight;
    Form1->ImagXpress7_1->ColorDepth(8, IPAL_Fixed, 0);
    Form1->ImagXpress7_1->ZoomToFit(2);
    Form1->ImagXpress7_1->SaveToBuffer = true;
    Form1->ImagXpress7_1->SaveFileType = FT_TIFF;
    Form1->ImagXpress7_1->SaveFile();

    Form1->ImagXpress7_2->FileName = OpenDialog->FileName;
    Form1->Ix2 = Form1->ImagXpress7_2->IWidth;
    Form1->Iy2 = Form1->ImagXpress7_2->IHeight;
    Form1->ImagXpress7_2->ColorDepth(8, IPAL_Fixed, 0);
    Form1->ImagXpress7_2->ZoomToFit(2);
    Form1->ImagXpress7_2->SaveToBuffer = true;
    Form1->ImagXpress7_2->SaveFileType = FT_TIFF;
    Form1->ImagXpress7_2->SaveFile();

    HANDLE hIM = (HANDLE)ImagXpress7_1->SaveBufferHandle;
    if (IMAGE != NULL) {
        GlobalFree(IMAGE);
        IMAGE=NULL;
    }

    HANDLE hIM1 = (HANDLE)Form1->ImagXpress7_1->SaveBufferHandle;
    if (Form1->IMAGE1 != NULL) {
        GlobalFree(Form1->IMAGE1);
        Form1->IMAGE1 = NULL;
    }

    HANDLE hIM2 = (HANDLE)Form1->ImagXpress7_2->SaveBufferHandle;
    if (Form1->IMAGE2 != NULL) {
        GlobalFree(Form1->IMAGE2);
        Form1->IMAGE2 = NULL;
    }

    IMAGE = (unsigned char *)GlobalLock(hIM);
    long ln = GlobalSize(hIM);
    offs = ln - (long)Ix*Iy;
    GlobalUnlock(hIM);

    Form1->IMAGE1 = (unsigned char *)GlobalLock(hIM1);
    long ln1 = GlobalSize(hIM1);
    Form1->offs1 = ln1 - (long)Form1->Ix1*Form1->Iy1;
    GlobalUnlock(hIM1);

    Form1->IMAGE2 = (unsigned char *)GlobalLock(hIM2);
    long ln2 = GlobalSize(hIM2);
    Form1->offs2 = ln2 - (long)Form1->Ix2*Form1->Iy2;
    GlobalUnlock(hIM2);

    text = "[" + ExtractFileName(ImagXpress7_1->FileName) + ", " + Ix + "x" + Iy + "@" + ImagXpress7_1->IBPP + "bpp]";
    StatusBar->SimpleText = text;
    unsigned char *IMAGE = &this->IMAGE[offs];
    unsigned char *IMAGE1 = &Form1->IMAGE1[Form1->offs1];
    unsigned char *IMAGE2 = &Form1->IMAGE2[Form1->offs2];

    //for (int i = 0; i < Form1->Ix1; i++)
    //    for (int j = 0; j < Form1->Iy1; j++)
    //        IMAGE1[j*Form1->Ix1 + i] = 255;

    /** Initialize all starting values to write to data file to 0 **/
    int *values_to_write = new int[Ix*Iy];
    for(int i = 0; i < Ix*Iy; i++) {
        values_to_write[i] = 0;
    }

    int **horizontal_histogram;
    float y_valley_threshold = StrToFloat(yAxisValleyHeightThreshold->Text);

    if ((horizontal_histogram = new int*[20]) == NULL ) {
        cerr << "Error in y histogram allocation" << endl;
        exit(-1);
    }

    for (int i = 0; i < 20; i++)
    horizontal_histogram[i] = new int[Iy];

    int chunk_size = floor((float)Ix/20.0);
    int integer_width_covered = 20 * chunk_size;
    int final_chunk_size = Ix - integer_width_covered;

    /** Generate projection profiles for chunks **/
    for (int i = 0; i < Iy; i++) {
        for (int chunk_no = 0; chunk_no < 20; chunk_no++) {
            horizontal_histogram[chunk_no][i] = 0;

            int start = chunk_no * chunk_size, end;

            if (chunk_no < 19)
            end = start + chunk_size - 1;
            else
            end = Ix;

            for (int j = start; j < end; j++) {
                if (IMAGE[i * Ix + j] == 0)
                horizontal_histogram[chunk_no][i] += 1;
            }
        }
    }



    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < Form1->Iy1; j++) {
            for (int index = i*chunk_size; index < i*chunk_size+horizontal_histogram[i][j]; index++) {
                IMAGE1[j*Ix + index] = 50;
            }

        }
    }
    int* smoothed_projection_profiles;
    if ((smoothed_projection_profiles = new int[Iy]) == NULL ) {
        cerr << "Error in y histogram allocation" << endl;
        exit(-1);
    }

    for (int i = 0; i < Iy; i++)
    for (int j = 0; j < 20; j++)
    smoothed_projection_profiles[i] += horizontal_histogram[j][i];

    float line_width_threshold = StrToFloat(yAxisLineWidthThreshold->Text);
    float line_pixel_threshold = (line_width_threshold*Ix)/100.0;
    for(int y = 0; y < Iy; y++) {
        smoothed_projection_profiles[y] = (smoothed_projection_profiles[y] >= line_pixel_threshold);
    }


    vector<int> peak_vector, valley_vector;
    int i = 0;
    while (i < Iy) {
        while (i < Iy && smoothed_projection_profiles[i] == 1)
        i++;
        int peak = i-1;

        while (i < Iy && smoothed_projection_profiles[i] == 0)
        i++;
        int valley = i-1;
        int midean = peak + (valley - peak)/2;
        if (i + 1 < Iy) {
            if (valley - peak >= (float)y_valley_threshold*Iy/100.0) {
                valley_vector.push_back(midean);
            }
        }

    }

    if (valley_vector.size() <= 2)
    return;

    for (int x = 1; x < 20; x++) {
        for(int y = 0; y < Iy; ++y) {
            horizontal_histogram[x][y] = (horizontal_histogram[x][y] >= line_pixel_threshold/20.0);
        }
    }
    /**
    *   New way to use all_valleys_vector
    **/
    vector<int> *all_valleys_vector = new vector<int>[20];
    /*for (int i = 0; i < Iy; i++) {
    if (smoothed_projection_profiles[i] == 1)
    for (int j = 0; j < 50; j++)
    IMAGE[i* Ix + j] = 0;
}*/


for (int j = 0; j < valley_vector.size(); j++)
all_valleys_vector[0].push_back(valley_vector[j]);

for (int j = 1; j < 20; j++) {
    int i = 0;

    while (i < Iy) {
        while (i < Iy && horizontal_histogram[j][i] > 0)
        i++;
        int peak = i-1;

        while (i < Iy && horizontal_histogram[j][i] == 0)
        i++;
        int valley_end = i-1;

        int midean = peak + (valley_end - peak)/2;
        if (peak != 0)
        if (valley_end - peak >= (float)y_valley_threshold*Iy/100.0)
        all_valleys_vector[j].push_back(midean);

    }
}

int initial_sizes[20];
for (int i = 0; i < 20; i++)
initial_sizes[i] = all_valleys_vector[i].size();

for (int i = 0; i < 20; i++)
delete[] horizontal_histogram;
delete[] horizontal_histogram;
/**
*  For all chunks
**/
for (int chunk_no = 1; chunk_no < 20; chunk_no++) {
    vector<int> already_used_valleys, connected_to_used_valleys, mins_vector;

    int current_chunk_pos = 0;
    for(vector<int>::iterator i = all_valleys_vector[chunk_no].begin(); i != all_valleys_vector[chunk_no].end();) {
        int current_valley = *i;
        int min = Iy, min_pos = -1;

        int pos = 0;
        for (vector<int>::iterator j = all_valleys_vector[chunk_no-1].begin(); j != all_valleys_vector[chunk_no-1].end(); ++j) {
            if (*j < 0)
            continue;

            if (abs(*j - *i) < min) {
                min = abs(*j - *i);
                min_pos = pos;
            }
            pos++;
        }

        int previous_valley = all_valleys_vector[chunk_no-1][min_pos];

        // Check previous chunk closest valley is already connected
        int contains_index = -1;
        for (int counter = 0; counter < already_used_valleys.size(); counter++) {
            if (already_used_valleys[counter] == all_valleys_vector[chunk_no-1][min_pos]) {
                contains_index = counter;
                break;
            }
        }

        //  If previous valley chunk not already connected -> Add the previous chucnk valley and the current valley to the two vectors
        if (contains_index == -1) {
            already_used_valleys.push_back(all_valleys_vector[chunk_no-1][min_pos]);    // store min valley value
            connected_to_used_valleys.push_back(current_chunk_pos);                     // store current valley position
            mins_vector.push_back(min);                                                 // store min
            i++;
            current_chunk_pos++;
        }

        // if already connected -> Check if we need to update and reject the previous current chunk closest valley or this one
        else {
            if (mins_vector[contains_index] <= min) {
                i = all_valleys_vector[chunk_no].erase(i);
            }
            else {
                all_valleys_vector[chunk_no].erase(all_valleys_vector[chunk_no].begin() + connected_to_used_valleys[contains_index]);

                connected_to_used_valleys[contains_index] = current_chunk_pos - 1;
                mins_vector[contains_index] = min;
                i = all_valleys_vector[chunk_no].begin() + current_chunk_pos;

            }
        }
        int modified_size = all_valleys_vector[chunk_no].size();
        int already_used_size = already_used_valleys.size();
        int previous_valleys[20], current_valleys[20], used_valleys[20];
        for (int z = 0; z < 20; z++)
        previous_valleys[z] = -1;
        for (int z = 0; z < all_valleys_vector[chunk_no-1].size(); z++)
        previous_valleys[z] = all_valleys_vector[chunk_no-1][z];
        for (int z = 0; z < 20; z++)
        current_valleys[z] = -1;
        for (int z = 0; z < all_valleys_vector[chunk_no].size(); z++)
        current_valleys[z] = all_valleys_vector[chunk_no][z];
        for (int z = 0; z < 20; z++)
        used_valleys[z] = -1;
        for (int z = 0; z < already_used_valleys.size(); z++)
        used_valleys[z] = already_used_valleys[z];

    }

    int previous_valleys[20], current_valleys[20], used_valleys[20];
    // This part expands unconnected valleys from the previous chunk to the current one
    for (int counter = 0; counter < all_valleys_vector[chunk_no-1].size(); counter++) {
        if(all_valleys_vector[chunk_no-1][counter] < 0)
        continue;

        bool found = false;
        int search = all_valleys_vector[chunk_no-1][counter];

        for (int k = 0; k < already_used_valleys.size(); k++) {
            if (already_used_valleys[k] == search) {
                found = true;
                break;
            }
        }

        if (!found) {
            if (all_valleys_vector[chunk_no].empty()) {
                all_valleys_vector[chunk_no].push_back(all_valleys_vector[chunk_no-1][counter]);
                continue;
            }
            int move_counter = 0;
            while(move_counter < all_valleys_vector[chunk_no].size() && all_valleys_vector[chunk_no][move_counter] < search)
            move_counter++;

            if (move_counter < all_valleys_vector[chunk_no].size()) {
                if (move_counter == 0)
                all_valleys_vector[chunk_no].insert(all_valleys_vector[chunk_no].begin(), all_valleys_vector[chunk_no-1][counter]);
                else
                all_valleys_vector[chunk_no].insert(all_valleys_vector[chunk_no].begin() + move_counter, all_valleys_vector[chunk_no-1][counter]);
            }
            else {
                all_valleys_vector[chunk_no].push_back(all_valleys_vector[chunk_no-1][counter]);
            }
        }


    }

    for (int z = 0; z < 20; z++)
    current_valleys[z] = -1;
    for (int z = 0; z < all_valleys_vector[chunk_no].size(); z++)
    current_valleys[z] = all_valleys_vector[chunk_no][z];
    int previous_size = all_valleys_vector[chunk_no-1].size();
    int current_size =  all_valleys_vector[chunk_no].size();
    assert (all_valleys_vector[chunk_no-1].size() == all_valleys_vector[chunk_no].size());
}

int end_sizes[20];
for (int i = 0; i < 20; i++)
end_sizes[i] = all_valleys_vector[i].size();
for (int chunk_no = 0; chunk_no < 20; chunk_no++) {
    int colour = 1;
    int valley_colour = 1;
    for(int i = 0; i < all_valleys_vector[chunk_no].size() - 1; i++) {
        if(all_valleys_vector[chunk_no][i] < 0)
        continue;

        int next_offset = 1;
        while(all_valleys_vector[chunk_no][i+next_offset] < 0)
        next_offset++;

        for(int x = all_valleys_vector[chunk_no][i]; x < all_valleys_vector[chunk_no][i+next_offset]; x++) {
            assert(all_valleys_vector[chunk_no][i] < all_valleys_vector[chunk_no][i+next_offset]);
            int valley = all_valleys_vector[chunk_no][i];
            int start = chunk_no * chunk_size, end, image2_end;

            if (chunk_no < 19)
            end = start + chunk_size;
            else  {
                end = Ix;
                image2_end = Form1->Ix2;
            }
            for(int y = start; y < end; y++) {
                if (x < Iy && y < Ix)
                if(IMAGE[x*Ix+y] == 0)
                IMAGE[x*Ix+y] = values_to_write[x*Ix+y] = colour;

            }

            /*for(int y = start; y < end; y++) {
            IMAGE[valley*Form1->Ix2+y] = valley_colour;
        }*/

    }
    colour++;

}
if (all_valleys_vector[chunk_no][all_valleys_vector[chunk_no].size()-1] > 0) {
    for(int x = all_valleys_vector[chunk_no][all_valleys_vector[chunk_no].size()-1]; x < Iy; x++) {
        int valley = all_valleys_vector[chunk_no][all_valleys_vector[chunk_no].size()-1];
        int start = chunk_no * chunk_size, end, image2_end;

        if (chunk_no < 19)
        end = start + chunk_size;
        else  {
            end = Ix;
            image2_end = Form1->Ix2;
        }
        for(int y = start; y < end; y++) {
            if (x < Iy && y < Ix)
            if(IMAGE[x*Ix+y] == 0)
            IMAGE[x*Ix+y] = values_to_write[x*Ix+y] = colour;

        }

        /*for(int y = start; y < end; y++) {
        IMAGE[valley*Form1->Ix2+y] = valley_colour;
    }*/
}
}

}



FILE *fp = fopen(output.c_str(), "wb+");
for(int y = 0; y < Iy; y++) {
    for(int x = 0; x < Ix; x++) {
        fwrite(&values_to_write[y*Ix+x], 1, sizeof(int), fp);
    }
}
fclose(fp);



delete[] smoothed_projection_profiles;
delete[] values_to_write;
for (int i = 0; i < 20; i++)
all_valleys_vector[i].clear();
delete[] all_valleys_vector;
ImagXpress7_1->DIBUpdate();
ImagXpress7_1->LoadBuffer((long) this->IMAGE);


/*Form1->Show();
Form1->ImagXpress7_1->DIBUpdate();
Form1->ImagXpress7_1->LoadBuffer((long) Form1->IMAGE1);


Form1->ImagXpress7_2->DIBUpdate();
Form1->ImagXpress7_2->LoadBuffer((long) Form1->IMAGE2);  */
delete[] IMAGE;
delete[] IMAGE1;
delete[] IMAGE2;
}

void __fastcall TMainForm::SplitWordsNewClick(TObject *Sender)
{

    ImagXpress7_1->FileName = OpenDialog->FileName;
    Ix = ImagXpress7_1->IWidth;
    Iy = ImagXpress7_1->IHeight;

    //output = "C:\\Users\\nymeria\\Documents\\evaluation\\results\\words\\" + ImagXpress7_1->FileName + ".dat";
    ImagXpress7_1->ColorDepth(8, IPAL_Fixed, 0);
    //ImagXpress7_1->ZoomToFit(2);
    ImagXpress7_1->SaveToBuffer = true;
    ImagXpress7_1->SaveFileType = FT_TIFF;
    ImagXpress7_1->SaveFile();

    Form1->ImagXpress7_1->FileName = OpenDialog->FileName;
    Form1->Ix1 = Form1->ImagXpress7_1->IWidth;
    Form1->Iy1 = Form1->ImagXpress7_1->IHeight;
    Form1->ImagXpress7_1->ColorDepth(8, IPAL_Fixed, 0);
    Form1->ImagXpress7_1->ZoomToFit(2);
    Form1->ImagXpress7_1->SaveToBuffer = true;
    Form1->ImagXpress7_1->SaveFileType = FT_TIFF;
    Form1->ImagXpress7_1->SaveFile();

    Form1->ImagXpress7_2->FileName = OpenDialog->FileName;
    Form1->Ix2 = Form1->ImagXpress7_2->IWidth;
    Form1->Iy2 = Form1->ImagXpress7_2->IHeight;
    Form1->ImagXpress7_2->ColorDepth(8, IPAL_Fixed, 0);
    Form1->ImagXpress7_2->ZoomToFit(2);
    Form1->ImagXpress7_2->SaveToBuffer = true;
    Form1->ImagXpress7_2->SaveFileType = FT_TIFF;
    Form1->ImagXpress7_2->SaveFile();

    HANDLE hIM = (HANDLE)ImagXpress7_1->SaveBufferHandle;
    if (IMAGE != NULL) {
        GlobalFree(IMAGE);
        IMAGE=NULL;
    }

    HANDLE hIM1 = (HANDLE)Form1->ImagXpress7_1->SaveBufferHandle;
    if (Form1->IMAGE1 != NULL) {
        GlobalFree(Form1->IMAGE1);
        Form1->IMAGE1 = NULL;
    }

    HANDLE hIM2 = (HANDLE)Form1->ImagXpress7_2->SaveBufferHandle;
    if (Form1->IMAGE2 != NULL) {
        GlobalFree(Form1->IMAGE2);
        Form1->IMAGE2 = NULL;
    }

    IMAGE = (unsigned char *)GlobalLock(hIM);
    long ln = GlobalSize(hIM);
    offs = ln - (long)Ix*Iy;
    GlobalUnlock(hIM);

    Form1->IMAGE1 = (unsigned char *)GlobalLock(hIM1);
    long ln1 = GlobalSize(hIM1);
    Form1->offs1 = ln1 - (long)Form1->Ix1*Form1->Iy1;
    GlobalUnlock(hIM1);

    Form1->IMAGE2 = (unsigned char *)GlobalLock(hIM2);
    long ln2 = GlobalSize(hIM2);
    Form1->offs2 = ln2 - (long)Form1->Ix2*Form1->Iy2;
    GlobalUnlock(hIM2);

    text = "[" + ExtractFileName(ImagXpress7_1->FileName) + ", " + Ix + "x" + Iy + "@" + ImagXpress7_1->IBPP + "bpp]";
    StatusBar->SimpleText = text;
    unsigned char *IMAGE = &this->IMAGE[offs];
    unsigned char *IMAGE1 = &Form1->IMAGE1[Form1->offs1];
    unsigned char *IMAGE2 = &Form1->IMAGE2[Form1->offs2];

    //for (int i = 0; i < Form1->Ix1; i++)
    //    for (int j = 0; j < Form1->Iy1; j++)
    //        IMAGE1[j*Form1->Ix1 + i] = 255;

    /** Initialize all starting values to write to data file to 0 **/
    int *values_to_write = new int[Ix*Iy];
    for(int i = 0; i < Ix*Iy; i++) {
        values_to_write[i] = 0;
    }

    int **horizontal_histogram;
    float y_valley_threshold = StrToFloat(yAxisValleyHeightThreshold->Text);

    if ((horizontal_histogram = new int*[20]) == NULL ) {
        cerr << "Error in y histogram allocation" << endl;
        exit(-1);
    }

    for (int i = 0; i < 20; i++)
    horizontal_histogram[i] = new int[Iy];

    int chunk_size = floor((float)Ix/20.0);
    int integer_width_covered = 20 * chunk_size;
    int final_chunk_size = Ix - integer_width_covered;

    /** Generate projection profiles for chunks **/
    for (int i = 0; i < Iy; i++) {
        for (int chunk_no = 0; chunk_no < 20; chunk_no++) {
            horizontal_histogram[chunk_no][i] = 0;

            int start = chunk_no * chunk_size, end;

            if (chunk_no < 19)
            end = start + chunk_size - 1;
            else
            end = Ix;

            for (int j = start; j < end; j++) {
                if (IMAGE[i * Ix + j] == 0)
                horizontal_histogram[chunk_no][i] += 1;
            }
        }
    }



    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < Form1->Iy1; j++) {
            for (int index = i*chunk_size; index < i*chunk_size+horizontal_histogram[i][j]; index++) {
                IMAGE1[j*Ix + index] = 50;
            }

        }
    }
    int* smoothed_projection_profiles;
    if ((smoothed_projection_profiles = new int[Iy]) == NULL ) {
        cerr << "Error in y histogram allocation" << endl;
        exit(-1);
    }

    for (int i = 0; i < Iy; i++)
    for (int j = 0; j < 20; j++)
    smoothed_projection_profiles[i] += horizontal_histogram[j][i];

    float line_width_threshold = StrToFloat(yAxisLineWidthThreshold->Text);
    float line_pixel_threshold = (line_width_threshold*Ix)/100.0;
    for(int y = 0; y < Iy; y++) {
        smoothed_projection_profiles[y] = (smoothed_projection_profiles[y] >= line_pixel_threshold);
    }


    vector<int> peak_vector, valley_vector;
    int i = 0;
    while (i < Iy) {
        while (i < Iy && smoothed_projection_profiles[i] == 1)
        i++;
        int peak = i-1;

        while (i < Iy && smoothed_projection_profiles[i] == 0)
        i++;
        int valley = i-1;
        int midean = peak + (valley - peak)/2;

            if (valley - peak >= (float)y_valley_threshold*Iy/100.0) {
                valley_vector.push_back(midean);
            }
        

    }

    if (valley_vector.size() <= 2)
    return;

    for (int x = 1; x < 20; x++) {
        for(int y = 0; y < Iy; y++) {
            horizontal_histogram[x][y] = (horizontal_histogram[x][y] >= line_pixel_threshold/20.0);
        }
    }
    /**
    *   New way to use all_valleys_vector
    **/
    vector<int> *all_valleys_vector = new vector<int>[20];
    /*for (int i = 0; i < Iy; i++) {
    if (smoothed_projection_profiles[i] == 1)
    for (int j = 0; j < 50; j++)
    IMAGE[i* Ix + j] = 0;
}*/


for (int j = 0; j < valley_vector.size(); j++)
all_valleys_vector[0].push_back(valley_vector[j]);

for (int j = 1; j < 20; j++) {
    int i = 0;

    while (i < Iy) {
        while (i < Iy && horizontal_histogram[j][i] > 0)
        i++;
        int peak = i-1;

        while (i < Iy && horizontal_histogram[j][i] == 0)
        i++;
        int valley_end = i-1;

        int midean = peak + (valley_end - peak)/2;
        if (peak != 0)
           if (valley_end - peak >= (float)y_valley_threshold*Iy/100.0)
              all_valleys_vector[j].push_back(midean);

    }
}

int initial_sizes[20];
for (int i = 0; i < 20; i++)
initial_sizes[i] = all_valleys_vector[i].size();

for (int i = 0; i < 20; i++)
delete[] horizontal_histogram;
delete[] horizontal_histogram;
/**
*  For all chunks
**/
for (int chunk_no = 1; chunk_no < 20; chunk_no++) {
    vector<int> already_used_valleys, connected_to_used_valleys, mins_vector;

    int current_chunk_pos = 0;
    for(vector<int>::iterator i = all_valleys_vector[chunk_no].begin(); i != all_valleys_vector[chunk_no].end();) {
        int current_valley = *i;
        int min = Iy, min_pos = -1;

        int pos = 0;
        for (vector<int>::iterator j = all_valleys_vector[chunk_no-1].begin(); j != all_valleys_vector[chunk_no-1].end(); ++j) {
            if (*j < 0)
            continue;

            if (abs(*j - *i) < min) {
                min = abs(*j - *i);
                min_pos = pos;
            }
            pos++;
        }

        int previous_valley = all_valleys_vector[chunk_no-1][min_pos];

        // Check previous chunk closest valley is already connected
        int contains_index = -1;
        for (int counter = 0; counter < already_used_valleys.size(); counter++) {
            if (already_used_valleys[counter] == all_valleys_vector[chunk_no-1][min_pos]) {
                contains_index = counter;
                break;
            }
        }

        //  If previous valley chunk not already connected -> Add the previous chucnk valley and the current valley to the two vectors
        if (contains_index == -1) {
            already_used_valleys.push_back(all_valleys_vector[chunk_no-1][min_pos]);    // store min valley value
            connected_to_used_valleys.push_back(current_chunk_pos);                     // store current valley position
            mins_vector.push_back(min);                                                 // store min
            i++;
            current_chunk_pos++;
        }

        // if already connected -> Check if we need to update and reject the previous current chunk closest valley or this one
        else {
            if (mins_vector[contains_index] <= min) {
                i = all_valleys_vector[chunk_no].erase(i);
            }
            else {
                all_valleys_vector[chunk_no].erase(all_valleys_vector[chunk_no].begin() + connected_to_used_valleys[contains_index]);

                connected_to_used_valleys[contains_index] = current_chunk_pos - 1;
                mins_vector[contains_index] = min;
                i = all_valleys_vector[chunk_no].begin() + current_chunk_pos;

            }
        }
        int modified_size = all_valleys_vector[chunk_no].size();
        int already_used_size = already_used_valleys.size();
        int previous_valleys[20], current_valleys[20], used_valleys[20];
        for (int z = 0; z < 20; z++)
        previous_valleys[z] = -1;
        for (int z = 0; z < all_valleys_vector[chunk_no-1].size(); z++)
        previous_valleys[z] = all_valleys_vector[chunk_no-1][z];
        for (int z = 0; z < 20; z++)
        current_valleys[z] = -1;
        for (int z = 0; z < all_valleys_vector[chunk_no].size(); z++)
        current_valleys[z] = all_valleys_vector[chunk_no][z];
        for (int z = 0; z < 20; z++)
        used_valleys[z] = -1;
        for (int z = 0; z < already_used_valleys.size(); z++)
        used_valleys[z] = already_used_valleys[z];

    }

    int previous_valleys[20], current_valleys[20], used_valleys[20];
    // This part expands unconnected valleys from the previous chunk to the current one
    for (int counter = 0; counter < all_valleys_vector[chunk_no-1].size(); counter++) {
        if(all_valleys_vector[chunk_no-1][counter] < 0)
        continue;

        bool found = false;
        int search = all_valleys_vector[chunk_no-1][counter];

        for (int k = 0; k < already_used_valleys.size(); k++) {
            if (already_used_valleys[k] == search) {
                found = true;
                break;
            }
        }

        if (!found) {
            if (all_valleys_vector[chunk_no].empty()) {
                all_valleys_vector[chunk_no].push_back(all_valleys_vector[chunk_no-1][counter]);
                continue;
            }
            int move_counter = 0;
            while(move_counter < all_valleys_vector[chunk_no].size() && all_valleys_vector[chunk_no][move_counter] < search)
            move_counter++;

            if (move_counter < all_valleys_vector[chunk_no].size()) {
                if (move_counter == 0)
                all_valleys_vector[chunk_no].insert(all_valleys_vector[chunk_no].begin(), all_valleys_vector[chunk_no-1][counter]);
                else
                all_valleys_vector[chunk_no].insert(all_valleys_vector[chunk_no].begin() + move_counter, all_valleys_vector[chunk_no-1][counter]);
            }
            else {
                all_valleys_vector[chunk_no].push_back(all_valleys_vector[chunk_no-1][counter]);
            }
        }


    }

    for (int z = 0; z < 20; z++)
    current_valleys[z] = -1;
    for (int z = 0; z < all_valleys_vector[chunk_no].size(); z++)
    current_valleys[z] = all_valleys_vector[chunk_no][z];
    int previous_size = all_valleys_vector[chunk_no-1].size();
    int current_size =  all_valleys_vector[chunk_no].size();
    assert (all_valleys_vector[chunk_no-1].size() == all_valleys_vector[chunk_no].size());
}

int *vertical_histogram = new int[Ix];
int x_axis_threshold;

x_axis_threshold = StrToFloat(xAxisLineHeightThreshold->Text);
float column_pixel_threshold = (float)(x_axis_threshold * Iy)/100.0;
for(int x = 0; x < Ix; x++) {
    vertical_histogram[x] = vertical_histogram[x] >= column_pixel_threshold;
}
//x-axis cuts
int x_valley_threshold = StrToFloat(xAxisValleyWidthThreshold->Text);

vector<int> vertical_cuts, vertical_cut_kinds;




int colour = 1;
for(int i = 0; i < all_valleys_vector[0].size() - 1; i++) {
    int valley_colour = 1;

    int *vertical_histogram = new int[Ix];
    for (int chunk_no = 0; chunk_no < 20; chunk_no++) {
        int start = chunk_no * chunk_size, end, image2_end;

        if (chunk_no < 19)
            end = start + chunk_size;
        else  {
            end = Ix;
            //image2_end = Form1->Ix2;
        }
        int ys = all_valleys_vector[chunk_no][i];
        int ye = all_valleys_vector[chunk_no][i+1];



        for(int x = start; x < end; x++) {
            vertical_histogram[x] = 0;
            if (x >= 1926)
                bool nai = true;

            for(int y = ys; y < ye; y++) {
                    if (y >= 2281)
               bool nai = true;
                if(IMAGE[y*Ix+x] == 0)
                    vertical_histogram[x]++;
            }
        }
    }

    x_axis_threshold = StrToFloat(xAxisLineHeightThreshold->Text);
    float column_pixel_threshold = (float)(x_axis_threshold * Iy)/100.0;
    for(int x = 0; x < Ix; x++) {
        vertical_histogram[x] = vertical_histogram[x] >= column_pixel_threshold;
    }
    //x-axis cuts
    int x_valley_threshold = StrToFloat(xAxisValleyWidthThreshold->Text);
    std::vector<int> x_cuts;

    vector<int> vertical_cuts, vertical_cut_kinds;

    int x = 0;
    while (x < Ix) {
        while (x < Ix && vertical_histogram[x] != 0)
        x++;
        int black_valley = x;

        while (x < Ix && vertical_histogram[x] == 0)
        x++;
        int white_valley = x;

        int midean = black_valley + (white_valley - black_valley)/2;
        if (white_valley - black_valley >= (float)(x_valley_threshold * Iy)/100.0) {
            vertical_cuts.push_back(midean);
        }
    }

    int all_vertical_cuts[20];
    for(int m = 0; m < vertical_cuts.size(); m++)
    all_vertical_cuts[m] = vertical_cuts[m];

    if(vertical_cuts.size() < 2) {
        return;
    }

    delete[] vertical_histogram;

    int vertical_cut_counter = 0;
    for(int chunk_no = 0; chunk_no < 20; chunk_no++) {
        int start = chunk_no * chunk_size, end, image2_end;
        if (chunk_no < 19)
        end = start + chunk_size;
        else  {
            end = Ix;
            //image2_end = Form1->Ix2;
        }
        for(int x = start; x < end; x++) {
            for(int y = all_valleys_vector[chunk_no][i]; y < all_valleys_vector[chunk_no][i+1]; y++) {
                if(vertical_cut_counter < vertical_cuts.size()-1 && x > vertical_cuts[vertical_cut_counter] && x < vertical_cuts[vertical_cut_counter+1])
                if(IMAGE[y*Ix+x] == 0)
                IMAGE[y*Ix+x] = values_to_write[y*Ix+x] = colour;

                if(vertical_cut_counter < vertical_cuts.size()-1 && x == vertical_cuts[vertical_cut_counter+1]) {
                    vertical_cut_counter++;
                    colour++;
                }

            }
        }
    }
}


FILE *fp = fopen(output.c_str(), "wb+");
for(int y = 0; y < Iy; y++) {
    for(int x = 0; x < Ix; x++) {
        fwrite(&values_to_write[y*Ix+x], 1, sizeof(int), fp);
    }
}
fclose(fp);



delete[] smoothed_projection_profiles;
delete[] values_to_write;
for (int i = 0; i < 20; i++)
all_valleys_vector[i].clear();
delete[] all_valleys_vector;
ImagXpress7_1->DIBUpdate();
ImagXpress7_1->LoadBuffer((long) this->IMAGE);


/*Form1->Show();
Form1->ImagXpress7_1->DIBUpdate();
Form1->ImagXpress7_1->LoadBuffer((long) Form1->IMAGE1);


Form1->ImagXpress7_2->DIBUpdate();
Form1->ImagXpress7_2->LoadBuffer((long) Form1->IMAGE2);  */
delete[] IMAGE;
delete[] IMAGE1;
delete[] IMAGE2;
}
