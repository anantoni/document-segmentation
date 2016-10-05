object Form2: TForm2
  Left = 192
  Top = 125
  Width = 696
  Height = 480
  Caption = 'Form2'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 120
  TextHeight = 16
  object Chart1: TChart
    Left = 0
    Top = 0
    Width = 678
    Height = 433
    BackWall.Brush.Color = clWhite
    BackWall.Brush.Style = bsClear
    Title.Text.Strings = (
      'TChart')
    Title.Visible = False
    View3D = False
    Align = alClient
    TabOrder = 0
    object Series1: TBarSeries
      Marks.ArrowLength = 20
      Marks.Visible = False
      SeriesColor = clRed
      ShowInLegend = False
      Dark3D = False
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Bar'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
  end
end
