//---------------------------------------------------------------------------

#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ImagXpr7_OCX.h"
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <OleCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
    __published:	// IDE-managed Components
    TImagXpress7_ *ImagXpress7_1;
    TMainMenu *MainMenu;
    TOpenDialog *OpenDialog;
    TMenuItem *File1;
    TMenuItem *Open1;
    TMenuItem *N1;
    TMenuItem *Exit1;
    TStatusBar *StatusBar;
    TMenuItem *N2;
    TMenuItem *Lines1;
    TMenuItem *Words1;
    TGroupBox *Settings;
    TEdit *yAxisLineWidthThreshold;
    TLabel *Label1;
    TLabel *Label2;
    TEdit *yAxisValleyHeightThreshold;
    TEdit *xAxisLineHeightThreshold;
    TEdit *xAxisValleyWidthThreshold;
    TLabel *Label3;
    TLabel *Label4;
    TButton *LinesButton;
    TButton *WordsButton;
    TCheckBox *CheckBox1;
    TCheckBox *CheckBox2;
    void __fastcall Open1Click(TObject *Sender);
    void __fastcall Exit1Click(TObject *Sender);
    void __fastcall Lines1Click(TObject *Sender);
    void __fastcall ImagXpress7_1MouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall Words1Click(TObject *Sender);
    void __fastcall LinesButtonClick(TObject *Sender);
    void __fastcall WordsButtonClick(TObject *Sender);
    void __fastcall EvaluateLinesClick(TObject *Sender);
    void __fastcall EvaluateWordsClick(TObject *Sender);

    private:	// User declarations
    int Ix, Iy;
    unsigned char *IMAGE;
    long offs;
    AnsiString text;
    AnsiString output;
    void words(int ys,int ye,int& tag,int *values);
    public:		// User declarations
    __fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
