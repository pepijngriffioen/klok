/*
------------------------------------------------------------------------
                           Xowl. version 1.0

              Copyright (c) 1997 Siegfried Nijssen
                            (snijssen@wi.leidenuniv.nl)
                  All Rights Reserved

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

-------------------------------------------------------------------------------
*/
#include <X11/Xlib.h>

 /* argumenten van main () */
extern int argc;
extern char **argv;

 /* kleuren voor het tekenen in een venster */
extern unsigned long black, white, blue, red, green, cyan, magenta, orange, pink,
                brown, violet, greenyellow, gray, yellow, darkgray, lightgray;

 /* voorgedefinieerde lettertypen */
extern Font widgetfont, inputfont;

 /* Hoe wordt een lijn getekend? */
enum LineStyle {LineStyleSolid, LineStyleOnOffDash, LineStyleDoubleDash};

 /* Wordt een rechthoek of een circel gevuld? */
enum FillStyle {FillStyleNone, FillStyleSolid};

 /* Hoe wordt een circel gevuld? */
enum ArcStyle {ArcStyleChord, ArcStylePieSlice};

 /* Waarop kan een venster reageren? */
#define NoButtonAction 0
#define Button1Action 1
#define Button2Action 2
#define Button3Action 4

#define NoSpecialKeyPressed 0
#define ShiftKeyPressed 1
#define ControlKeyPressed 2

#define EventMotionNotify   6
#define EventButtonPress    4
#define EventButtonRelease  5
#define EventEnterNotify    7
#define EventLeaveNotify    8
#define EventExpose         12
#define EventFocusIn        9
#define EventFocutOut       10
#define EventKeyPress       2
#define EventKeyRelease     3
#define EventResizeRequest  25

class TWindow;  // toegevoegd 14.4.2008

 /* Een element dat aan een venster gekoppeld kan worden */
class TWidget {
  friend class TWidgetList;
  friend class TWindow;
  public:
    TWidget () { ; }
    virtual ~TWidget () { ; }
  protected:
    virtual int Check ( XEvent *ev ) { return 0; }
    virtual void Init ( TWindow *parent ) { ; }
    virtual void Remove () { ; }
    Window window;
};

 /* Een knop */
class TButton:public TWidget {
  public:
    TButton ( int _x, int _y, int _width, int _height, char *_text, int _ID );
    TButton ( int _x, int _y, int _width, int _height, char *_text, int _ID, TWindow *parent );
    ~TButton ();
  protected:
    int x, y, width, height, ID, tx, ty;  // tx en ty om herberekenen te voorkomen
    char *text;
    GC drawcontext;
    int Check ( XEvent *ev );
    void Init ( TWindow *parent );
    void Remove ();
    void Draw ();
};

 /* Een aankruisvak */
class TCheckBox:public TWidget {
  public:
    TCheckBox ( int _x, int _y, bool _checked, int _ID );
    TCheckBox ( int _x, int _y, bool _checked, int _ID, TWindow *parent );
    void Toggle ();
    bool GetCheckState () { return checked; }
    void SetCheckState ( bool state );
  protected:
    int x, y, ID;
    bool checked;
    GC drawcontext;
    Pixmap checkboxsymbol, entercheckboxsymbol;
    int Check ( XEvent *ev );
    void Init ( TWindow *parent );
    void Remove ();
    void Draw ();
};

 /* Een intypevak */
class TInputBox:public TWidget {
  public:
    TInputBox ( int _x, int _y, int _width, char *_text, int _n, int _ID );
    TInputBox ( int _x, int _y, int _width, char *_text, int _n, int _ID, TWindow *parent );
    ~TInputBox ();
    void SetText ( char *_text );
    char* GetInputText () { return text; }
  protected:
    int x, y, width, height, ID, n, letterwidth, cursorpos, showpos, descent;
    char *text;
    GC drawcontext;
    int Check ( XEvent *ev );
    void Init ( TWindow *parent );
    void Remove ();
    void Draw ();
    void DrawText ();
    void MoveCursor ( int _cursorpos );
    void InsertKey ( char key );
    void DeleteChar ();
};

 /* Een element in een menubalk */
class TMenuListElement;
 /* Een element in een menu */
class TMenuItemListElement;

 /* Een menubalk */
class TMenu:public TWidget {
  public:
    TMenu () { begin = NULL; window = 0; }
    ~TMenu ();
    void AddDropDownMenu ( char *name );
    void AddMenuItem ( char *name, int ID, char *menuname );
    int GetHeight () { return ascent + descent + 4; }
  protected:
    GC drawcontext;
    TMenuListElement *begin, *actual;
    int ascent, descent, width;
    int Check ( XEvent *ev );
    void Init ( TWindow *parent );
    void Remove ();
    void Draw ();
    void MakeActive ( int x );
    TMenuItemListElement* MakeItemActive ( int y, int& n, Window popup );
    void Deactivate ();
    void DeactivateItem ( TMenuItemListElement* &item, int yw, Window popup );
    int SelectFromMenu ();
    void DrawPopup ( Window popup, int width, int height );
};

 /* Een label */
class TLabel:public TWidget {
  public:
    TLabel ( int _x, int _y, char *_text, unsigned long _background );
    TLabel ( int _x, int _y, char *_text, unsigned long _background, TWindow *parent );
    ~TLabel () { delete[] text; }
    void SetTextColor ( unsigned long _foreground );
    void SetText ( char *_text );
  protected:
    int Check ( XEvent *ev );
    void Init ( TWindow *parent );
    void Remove ();
    void Draw ();
    GC drawcontext;
    char *text;
    int x, y, ascent;
    unsigned long background;
};


class TWidgetList;

 /* Een venster */
class TWindow {
  public:
    TWindow ( unsigned int width, unsigned int height, unsigned long background, char *title, char *icontitle );
    TWindow ( unsigned int width, unsigned int height, unsigned long background, char *title );
    TWindow ( unsigned int width, unsigned int height );
    ~TWindow ();

  // funkties om in het venster te tekenen, met gebruikmaking van de interne drawcontext
    void DrawLine ( int x1, int y1, int x2, int y2 );
    void DrawArc ( int x, int y, unsigned int width, unsigned int height, int arc1, int arc2 );
    void DrawPoint ( int x, int y );
    void DrawRectangle ( int x, int y, unsigned int width, unsigned int height );
    void DrawString ( int x, int y, char* string );
    void SetDrawForeground ( unsigned long color );
    void SetDrawBackground ( unsigned long color );
    void SetDrawFont ( char* name );
    void SetDrawFont ( Font fontname );
    void SetDrawLineStyle ( LineStyle linestyle, unsigned int width);
    void SetDrawFillStyle ( FillStyle fillstyle );
    void SetDrawArcStyle ( ArcStyle arcstyle );
    void ClearWindow ();
    int GetStringWidth ( char *string );
    int GetStringAscent ( char *string );
    int GetStringDescent ( char *string );

  // funktie om widgets te koppelen die niet gekoppeld zijn
    void AddWidget ( TWidget *widget );
  // funktie om widgets te ontkoppelen
    void RemoveWidget ( TWidget *widget );

  // funkties om events te behandelen
    void AddEvent ( int eventtype, int ID );
    void RemoveEvent ( int eventtype );
    int WaitForEvent ();
    int CheckForEvent ();

    int GetButtonStatus () { return buttonstatus; }
    int GetSpecialKeyStatus () { return specialkeystatus; }
    int GetKeyPressedCode () { return keypressedcode; }
    int GetPointerX () { return pointerx; }
    int GetPointerY () { return pointery; }
    int GetWidth () { return width; }
    int GetHeight () { return height; }

  // Widgets moeten window kennen; deze funktie niet gebruiken!
    Window GetWindow () { return window; }
  // GetEvent ( globaal ) moet deze funktie kennen, niet gebruiken!
    int GetEvent ( XEvent &ev );
  private:
    Window window;
    int width, height;

  // te gebruiken bij tekenfunkties
    GC drawcontext;
    Font font, fontinuse;
    bool fill;

    TWidgetList* widgetlist;

    int buttonstatus, specialkeystatus, keypressedcode, pointerx, pointery;

    void Init ( unsigned int _width, unsigned int _height, unsigned long background, char *title, char *icontitle );
};

void SetWidgetFont ( char *name );
void SetInputFont ( char *name );
int WaitForGlobalEvent ( TWindow* &window );
int CheckForGlobalEvent ( TWindow* &window );


int Xmain ();
