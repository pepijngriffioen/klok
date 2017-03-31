#include <iostream>
#include <stdlib.h>
#include <X11/cursorfont.h>
#include "Xsowl.h"
#include "Xsowldef.h"
using namespace std;

int argc;
char **argv;

unsigned long black, white, blue, red, green, cyan, magenta, orange, pink,
              brown, violet, greenyellow, gray, yellow, darkgray, lightgray;

Font widgetfont, inputfont;
TWindowListElement *windowlist = NULL;

long mask ( int type ) {
  for (int i = 0; i<typenumber; i++)
    if ( masktabel[i][0] == type )
      return masktabel[i][1];

  return 0;
}

int strlen ( char* string ) {
  int teller = 0;

  while ( *string ) {
    teller++;
    string++;
  }

  return teller;
}

void strcpy ( char *source, char *dest ) {
  while ( *source ) {
    *dest = *source;
    dest++; source++;
  }
  *dest = *source;
}

bool strcmp ( char *s1, char *s2 ) {

  while ( *s1 == *s2 && *s1!='\0') {
    s1++;
    s2++;
  }

  return ( *s1 == *s2 );
}

TButton::TButton ( int _x, int _y, int _width, int _height, char *_text, int _ID ) {
  x = _x;
  y = _y;
  width = _width;
  height = _height;
  ID = _ID;
  text = new char[strlen ( _text ) + 1];
  strcpy ( _text, text );
}

TButton::TButton ( int _x, int _y, int _width, int _height, char *_text, int _ID, TWindow *parent ) {
  x = _x;
  y = _y;
  width = _width;
  height = _height;
  ID = _ID;
  text = new char[strlen ( _text ) + 1];
  strcpy ( _text, text );
  parent->AddWidget ( this );
}

TButton::~TButton () {
 delete[] text;
}

void TButton::Init ( TWindow *parent ) {
  int direction, ascent, descent;
  XCharStruct overall;

  window = XCreateSimpleWindow ( display, parent->GetWindow (), x, y, width, height, 0, black, lightgray );

  drawcontext = XCreateGC ( display, window, 0L, 0 );
  XSetLineAttributes ( display, drawcontext, 2, LineSolid, CapRound, JoinRound );
  XSetFont ( display, drawcontext, widgetfont );
  XQueryTextExtents ( display, widgetfont, text, strlen ( text ), &direction, &ascent, &descent, &overall );
  tx = ( width - overall.rbearing + overall.lbearing ) / 2 + overall.lbearing;
  ty = ( height - ascent - descent ) / 2 + ascent;

  XMapRaised ( display, window );

  Draw ();

  XSelectInput ( display, window, ExposureMask | ButtonPressMask | KeyPressMask | EnterWindowMask | ButtonReleaseMask | KeyReleaseMask | LeaveWindowMask );
}

void TButton::Remove () {
  XDestroyWindow ( display, window );
  XFreeGC ( display, drawcontext );
}

int TButton::Check ( XEvent* ev ) {
  XEvent ev2;

  switch ( ev->type ) {
    case Expose: Draw(); break;
    case ButtonPress:
    case KeyPress:    XClearWindow ( display, window );
                      XSetForeground ( display, drawcontext, blue );
                      XDrawString ( display, window, drawcontext, tx + 2, ty + 2, text, strlen ( text ) );
                      XSetForeground ( display, drawcontext, gray );
                      XDrawRectangle ( display, window, drawcontext, 0, 0, width - 1, height - 1 );
                      XSelectInput ( display, window, ButtonReleaseMask | KeyReleaseMask | ButtonPressMask | KeyPressMask | LeaveWindowMask );
                      while ( true ) {
                        XNextEvent ( display, &ev2 );
                        if ( ev2.type == ButtonRelease || ev2.type == KeyRelease || ev2.type == LeaveNotify ) {
                          XSetForeground ( display, drawcontext, lightgray );
                          XDrawString ( display, window, drawcontext, tx + 2, ty + 2, text, strlen ( text ) );
                          Draw ();
                          if ( ev2.type == LeaveNotify ) {
                            XSelectInput ( display, window, ExposureMask | ButtonPressMask | KeyPressMask | EnterWindowMask | KeyReleaseMask | ButtonReleaseMask | LeaveWindowMask );
                            return 0;
                          }
                          else {
                            XSetForeground ( display, drawcontext, blue );
                            XDrawString ( display, window, drawcontext, tx, ty, text, strlen ( text ) );
                            return ID;
                          }
                        }
                      }
    case EnterNotify: XSetForeground ( display, drawcontext, blue );
                      XDrawString ( display, window, drawcontext, tx, ty, text, strlen ( text ) );
                      break;
    case LeaveNotify: XSetForeground ( display, drawcontext, black );
                      XDrawString ( display, window, drawcontext, tx, ty, text, strlen ( text ) );
                      break;
  }
  return 0;
}

void TButton::Draw () {
  XSetForeground ( display, drawcontext, black );
  XDrawString ( display, window, drawcontext, tx, ty, text, strlen ( text ) );
  XSetForeground ( display, drawcontext, white );
  XDrawLine ( display, window, drawcontext, 0, 0, width, 0 );
  XDrawLine ( display, window, drawcontext, 0, 0, 0, height );
  XSetForeground ( display, drawcontext, darkgray );
  XDrawLine ( display, window, drawcontext, 0, height - 1, width, height - 1 );
  XDrawLine ( display, window, drawcontext, width - 1, 0, width - 1, height );
}

TCheckBox::TCheckBox ( int _x, int _y, bool _checked, int _ID ) {
  x = _x;
  y = _y;
  ID = _ID;
  checked = _checked;
}


TCheckBox::TCheckBox ( int _x, int _y, bool _checked, int _ID, TWindow *parent ) {
  x = _x;
  y = _y;
  ID = _ID;
  checked = _checked;
  parent->AddWidget ( this );
}

void TCheckBox::Init ( TWindow *parent ) {
  window = XCreateSimpleWindow ( display, parent->GetWindow (), x, y, checksymbol_width + 4, checksymbol_height + 4, 0, black, lightgray );

  drawcontext = XCreateGC ( display, window, 0L, 0 );
  checkboxsymbol = XCreatePixmapFromBitmapData ( display, window, checksymbol_bits, checksymbol_width, checksymbol_height, red, lightgray, DefaultDepth ( display, screen ) );
  entercheckboxsymbol = XCreatePixmapFromBitmapData ( display, window, checksymbol_bits, checksymbol_width, checksymbol_height, blue, lightgray, DefaultDepth ( display, screen ) );
  XSetLineAttributes ( display, drawcontext, 2, LineSolid, CapRound, JoinRound );

  XMapRaised ( display, window );

  Draw ();

  XSelectInput ( display, window, ExposureMask | ButtonPressMask | KeyPressMask | EnterWindowMask | LeaveWindowMask );
}

void TCheckBox::Remove () {
  XDestroyWindow ( display, window );
  XFreeGC ( display, drawcontext );
}

int TCheckBox::Check ( XEvent *ev ) {
  switch ( ev->type ) {
    case Expose: Draw (); break;
    case EnterNotify: if ( checked )
                        XCopyArea ( display, entercheckboxsymbol, window, drawcontext, 0, 0, checksymbol_width, checksymbol_height, 2, 2 );
                      else {
                        XSetForeground ( display, drawcontext, blue );
                        XDrawLine ( display, window, drawcontext, 5, checksymbol_height - 2, checksymbol_width - 3, checksymbol_height - 2 );
                      }
                      break;
    case LeaveNotify: Draw (); break;
    case KeyPress:
    case ButtonPress: checked = !checked;
                      Draw ();
                      if ( checked )
                        XCopyArea ( display, entercheckboxsymbol, window, drawcontext, 0, 0, checksymbol_width, checksymbol_height, 2, 2 );
                      else {
                        XSetForeground ( display, drawcontext, blue );
                        XDrawLine ( display, window, drawcontext, 5, checksymbol_height - 2, checksymbol_width - 3, checksymbol_height - 2 );
                      }
                      return ID;
  }
  return 0;
}

void TCheckBox::Toggle () {
  checked = !checked;
  Draw ();
}

void TCheckBox::SetCheckState ( bool state ) {
  checked = state;
  Draw ();
}


void TCheckBox::Draw () {
  XClearWindow ( display, window );
  XSetForeground ( display, drawcontext, white );
  XDrawLine ( display, window, drawcontext, 0, 0, checksymbol_width + 4, 0 );
  XDrawLine ( display, window, drawcontext, 0, 0, 0, checksymbol_height + 4);
  XSetForeground ( display, drawcontext, darkgray );
  XDrawLine ( display, window, drawcontext, 0, checksymbol_height + 3, checksymbol_width + 3, checksymbol_height + 3 );
  XDrawLine ( display, window, drawcontext, checksymbol_width + 3, 0, checksymbol_width + 3, checksymbol_height + 3 );
  if ( checked )
    XCopyArea ( display, checkboxsymbol, window, drawcontext, 0, 0, checksymbol_width, checksymbol_height, 2, 2 );
}

TInputBox::TInputBox ( int _x, int _y, int _width, char *_text, int _n, int _ID ) {
  x = _x;
  y = _y;
  width = _width;
  n = _n;
  ID = _ID;
  cursorpos = showpos = 0;
  text = new char[n + 1];
  strcpy ( _text, text );
}

TInputBox::TInputBox ( int _x, int _y, int _width, char *_text, int _n, int _ID, TWindow *parent ) {
  x = _x;
  y = _y;
  width = _width;
  n = _n;
  ID = _ID;
  cursorpos = showpos = 0;
  text = new char[n + 1];
  strcpy ( _text, text );
  parent->AddWidget ( this );
}

TInputBox::~TInputBox () {
  delete[] text;
}

void TInputBox::Init ( TWindow *parent ) {
  int direction, ascent, _descent;
  XCharStruct overall;

  XQueryTextExtents ( display, inputfont, "Mj", 2, &direction, &ascent, &_descent, &overall );
  height = ascent + _descent + 4;
  descent = _descent;
  XQueryTextExtents ( display, inputfont, "M", 2, &direction, &ascent, &_descent, &overall );
  letterwidth = overall.rbearing;

  window = XCreateSimpleWindow ( display, parent->GetWindow (), x, y, width, height, 0, black, white );

  drawcontext = XCreateGC ( display, window, 0L, 0 );
  XSetLineAttributes ( display, drawcontext, 2, LineSolid, CapNotLast, JoinRound );
  XSetFont ( display, drawcontext, inputfont );
  XSetBackground ( display, drawcontext, white );
  XSetForeground ( display, drawcontext, black );

  XMapRaised ( display, window );

  Draw ();

  XSelectInput ( display, window, ExposureMask | ButtonPressMask | KeyPressMask | EnterWindowMask | LeaveWindowMask );
  XDefineCursor ( display, window, XCreateFontCursor ( display, XC_xterm ) );
}

void TInputBox::SetText ( char *_text ) {
  cursorpos = showpos = 0;
  strcpy ( _text, text );
  Draw ();
}

void TInputBox::DrawText () {
  int length;

  if ( strlen ( text ) < ( width - 6 ) / letterwidth )
    length = strlen ( text );
  else
    length = ( width - 6 ) / letterwidth;

  XSetForeground ( display, drawcontext, black );
  XDrawImageString ( display, window, drawcontext, 3, height - 2 - descent, & ( text[showpos] ), length );
}

void TInputBox::Draw () {
  XClearWindow ( display, window );
  XSetForeground ( display, drawcontext, darkgray );
  XDrawLine ( display, window, drawcontext, 1, 1, width, 1 );
  XDrawLine ( display, window, drawcontext, 1, 1, 1, height );
  DrawText ();
}

void TInputBox::MoveCursor ( int _cursorpos ) {
  int cursorline;

  XSetForeground ( display, drawcontext, white );
  cursorline = 3 + ( cursorpos - showpos ) * letterwidth;
  XDrawLine ( display, window, drawcontext, cursorline, 3, cursorline, height - 3 );
  cursorpos = _cursorpos;
  if ( cursorpos < showpos ) {
    showpos = cursorpos;
    Draw ();
  }
  if ( cursorpos > showpos + ( width - 6 ) / letterwidth ) {
    showpos = cursorpos - ( width - 6 ) / letterwidth;
    Draw ();
  }
  cursorline = 3 + ( cursorpos - showpos ) * letterwidth;
  XSetForeground ( display, drawcontext, black );
  XDrawLine ( display, window, drawcontext, cursorline, 3, cursorline, height - 3 );
}

void TInputBox::InsertKey ( char key ) {
  char *textwalk = & ( text[cursorpos] );
  char movechar = key, helpchar;

  while ( *textwalk ) {
    helpchar = *textwalk;
    *textwalk = movechar;
    movechar = helpchar;
    textwalk++;
  }
  *textwalk = movechar;
  textwalk++;
  *textwalk = '\0';

  Draw ();
  MoveCursor ( ++cursorpos );
}

void TInputBox::DeleteChar () {
  int cursorline;
  char *textwalk = & ( text[cursorpos] ), *forwardwalk = textwalk;

  forwardwalk++;
  while ( *textwalk ) {
    *textwalk = *forwardwalk;
    textwalk++;
    forwardwalk++;
  }

  Draw ();
  cursorline = 3 + ( cursorpos - showpos ) * letterwidth;
  XSetForeground ( display, drawcontext, black );
  XDrawLine ( display, window, drawcontext, cursorline, 3, cursorline, height - 3 );
}

int TInputBox::Check ( XEvent *ev ) {
  int cursorline, newpos, key, IDreturn = 0;

  switch ( ev->type ) {
    case Expose: Draw (); break;
    case EnterNotify: XSetForeground ( display, drawcontext, black );
                      cursorline = 3 + ( cursorpos - showpos ) * letterwidth;
                      XDrawLine ( display, window, drawcontext, cursorline, 3, cursorline, height - 3 );
                      break;
    case LeaveNotify: XSetForeground ( display, drawcontext, white );
                      cursorline = 3 + ( cursorpos - showpos ) * letterwidth;
                      XDrawLine ( display, window, drawcontext, cursorline, 3, cursorline, height - 3 );
                      break;
    case ButtonPress: newpos = ( ev->xkey.x - 3 ) / letterwidth + showpos;
                      if ( newpos > strlen ( text ) ) newpos = strlen ( text );
                      MoveCursor ( newpos );
                      break;
    case KeyPress:    key = XLookupKeysym ( & ( ev->xkey ), 0 );
                      switch ( key ) {
                        case XK_Shift_L:
                        case XK_Shift_R: break;
                        case XK_Left: if ( cursorpos > 0 )
                                        MoveCursor ( cursorpos - 1 );
                                      break;
                        case XK_Right: if ( cursorpos < strlen ( text ) )
                                         MoveCursor ( cursorpos + 1 );
                                       break;
                        case XK_BackSpace: if ( cursorpos > 0 ) {
                                             MoveCursor ( cursorpos - 1 );
                                             DeleteChar ();
                                             IDreturn = ID;
                                           }
                                           break;
                        case XK_Delete: DeleteChar ();
                                        IDreturn = ID;
                                        break;
                        case XK_Home: MoveCursor ( 0 );
                                      break;
                        case XK_End: MoveCursor ( strlen ( text ) ); break;
                        default: if ( strlen ( text ) < n ) {
                                   if ( ev->xkey.state & ShiftMask )
                                     InsertKey ( XLookupKeysym ( & (ev->xkey ), 1 ) );
                                   else
                                     InsertKey ( key );
                                   IDreturn = ID;
                                   break;
                                 }
                      }
  }
  IDreturn++; IDreturn--;
  return 0;
}

void TInputBox::Remove () {
  XDestroyWindow ( display, window );
  XFreeGC ( display, drawcontext );
}

TMenuItemListElement::TMenuItemListElement ( char *_text, int _ID ) {
  next = NULL;
  ID = _ID;
  text = new char[strlen ( _text ) + 1];
  strcpy ( _text, text );
}

TMenuListElement::TMenuListElement ( char *_text ) {
  next = NULL;
  menu = NULL;
  text = new char[strlen ( _text ) + 1];
  strcpy ( _text, text );
  width = 0;
  n = 0;
}

TMenu::~TMenu () {
  if ( begin )
    delete begin;
}

void TMenu::AddDropDownMenu ( char *name ) {
  TMenuListElement *walk = begin;
  int x1 = 10, direction, ascent, descent;
  XCharStruct overall;

  if ( begin ) {
    x1 = walk->x2 + 20;
    while ( walk->next ) {
      walk = walk->next;
      x1 = walk->x2 + 20;
    }
    walk->next = new TMenuListElement ( name );
    walk = walk->next;
  }
  else
    walk = begin = new TMenuListElement ( name );

  walk->x1 = x1;
  walk->width = 0;
  XQueryTextExtents ( display, widgetfont, name, strlen ( name ), &direction, &ascent, &descent, &overall );
  walk->x2 = x1 + overall.rbearing;
}

void TMenu::AddMenuItem ( char *name, int ID, char *menuname ) {
  TMenuListElement *menuwalk = begin;
  TMenuItemListElement *itemwalk;
  int ascent, descent, direction;
  XCharStruct overall;

  while ( menuwalk && !strcmp ( menuname, menuwalk->text ) )
    menuwalk = menuwalk->next;

  if ( menuwalk ) {
    if ( menuwalk->menu ) {
      itemwalk = menuwalk->menu;
      while ( itemwalk->next )
        itemwalk = itemwalk->next;
      itemwalk->next = new TMenuItemListElement ( name, ID );
    }
    else
      menuwalk->menu = new TMenuItemListElement ( name, ID );
    XQueryTextExtents ( display, widgetfont, name, strlen ( name ), &direction, &ascent, &descent, &overall );
    if ( overall.rbearing > menuwalk->width )
      menuwalk->width = overall.rbearing;
    ( menuwalk->n )++;
  }
}

void TMenu::Init ( TWindow *parent ) {
  int direction;
  XCharStruct overall;

  actual = NULL;
  XQueryTextExtents ( display, widgetfont, "Mj", 2, &direction, &ascent, &descent, &overall );

  width = parent->GetWidth ();
  window = XCreateSimpleWindow ( display, parent->GetWindow (), 0, 0, width, ascent + descent + 4, 0, black, lightgray );

  drawcontext = XCreateGC ( display, window, 0L, 0 );
  XSetLineAttributes ( display, drawcontext, 2, LineSolid, CapRound, JoinRound );
  XSetFont ( display, drawcontext, widgetfont );

  XMapRaised ( display, window );

  Draw ();

  XSelectInput ( display, window, ExposureMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask );
}

void TMenu::Draw () {
  TMenuListElement *walk = begin;

  XSetForeground ( display, drawcontext, black );
  while ( walk ) {
    XDrawString ( display, window, drawcontext, walk->x1, ascent + 1, walk->text, strlen ( walk->text ) );
    walk = walk->next;
  }

  XSetForeground ( display, drawcontext, darkgray );
  XDrawLine ( display, window, drawcontext, 0, ascent + descent + 3, width, ascent + descent + 3 );
  XSetForeground ( display, drawcontext, white );
  XDrawLine ( display, window, drawcontext, 0, ascent + descent + 4, width, ascent + descent + 4 );
}

void TMenu::MakeActive ( int x ) {
  TMenuListElement *walk = begin;

  actual = NULL;
  while ( walk && ( walk->x1 <= x ) && !actual) {
    if ( walk->x2 >= x )
      actual = walk;
    walk = walk->next;
  }

  if ( actual ) {
    XSetForeground ( display, drawcontext, blue );
    XDrawString ( display, window, drawcontext, actual->x1, ascent + 1, actual->text, strlen ( actual->text ) );
  }
}

TMenuItemListElement* TMenu::MakeItemActive ( int y, int& n, Window popup ) {
  TMenuItemListElement *walk = actual->menu;
  int yw = 5 + ascent + descent;
  n = 0;

  while ( walk && ( y > yw ) ) {
    walk = walk->next;
    yw += ascent + descent + 4;
    n++;
  }

  if ( walk ) {
    XSetForeground ( display, drawcontext, blue );
    XDrawString ( display, popup, drawcontext, 10, yw - descent, walk->text, strlen ( walk-> text ) );
  }

  return walk;
}

void TMenu::DeactivateItem ( TMenuItemListElement* &item, int n, Window popup ) {
  if ( item ) {
    XSetForeground ( display, drawcontext, black );
    XDrawString ( display, popup, drawcontext, 10, n * ( ascent + descent + 4 ) + 5 + ascent, item->text, strlen ( item->text ) );
    item = NULL;
  }
}

void TMenu::Deactivate () {
  if ( actual ) {
    XSetForeground ( display, drawcontext, black );
    XDrawString ( display, window, drawcontext, actual->x1, ascent + 1, actual->text, strlen ( actual->text ) );
    actual = NULL;
  }
}

void TMenu::DrawPopup ( Window popup, int width, int height ) {
  TMenuItemListElement* walk = actual->menu;
  int y = 5 + ascent;

  XSetForeground ( display, drawcontext, white );
  XDrawLine ( display, popup, drawcontext, 0, 0, width, 0 );
  XDrawLine ( display, popup, drawcontext, 0, 0, 0, height );
  XSetForeground ( display, drawcontext, darkgray );
  XDrawLine ( display, popup, drawcontext, 0, height - 1, width, height - 1 );
  XDrawLine ( display, popup, drawcontext, width - 1, 0, width - 1, height );
  XSetForeground ( display, drawcontext, black );

  while ( walk ) {
    XDrawString ( display, popup, drawcontext, 10, y, walk->text, strlen ( walk->text ) );
    y += ascent + descent + 4;
    walk = walk->next;
  }
}

int TMenu::SelectFromMenu () {
  Window popupmenu, rw, cw;
  XSetWindowAttributes attr;
  XEvent ev;
  int rx, ry, wx, wy, itemn = 0;  // 14.4.2008 added: initialization
  unsigned int d;
  TMenuItemListElement *item = NULL;
  bool inside = false;

  attr.background_pixel = lightgray;
  attr.save_under = false;
  attr.override_redirect = true;
  attr.event_mask = ButtonPressMask | ButtonReleaseMask | KeyPressMask | ButtonMotionMask | PointerMotionMask;

  XSetForeground ( display, drawcontext, lightgray );
  XSetBackground ( display, drawcontext, blue );
  XDrawImageString ( display, window, drawcontext, actual->x1, ascent + 1, actual->text, strlen ( actual->text ) );

  XQueryPointer ( display, window, &rw, &cw, &rx, &ry, &wx, &wy, &d );
  popupmenu = XCreateWindow ( display, root_window, rx - wx + actual->x1, ry - wy + ascent + descent + 4, actual->width + 20, actual->n * (ascent + descent + 4 ) + 10, 0, CopyFromParent, InputOutput, CopyFromParent, CWBackPixel | CWSaveUnder | CWOverrideRedirect | CWEventMask, &attr );

  XMapRaised ( display, popupmenu );
  XDefineCursor ( display, popupmenu, XCreateFontCursor ( display, XC_arrow ) );

  DrawPopup ( popupmenu, actual->width + 20, actual->n * (ascent + descent + 4 ) + 10 );
  XGrabPointer ( display, popupmenu, true, ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime );

  do {
    XNextEvent ( display, &ev );
    if ( ev.xany.window == popupmenu )
      switch ( ev.type ) {
        case EnterNotify: item = MakeItemActive ( ev.xmotion.y, itemn, popupmenu ); inside = true; break;
        case LeaveNotify: DeactivateItem ( item, itemn, popupmenu ); inside = false; break;
        case MotionNotify: if ( item ) {
                             if ( ( ev.xmotion.y - 5 ) / ( ascent + descent + 4 ) != itemn ) {
                               DeactivateItem ( item, itemn, popupmenu );
                               item = MakeItemActive ( ev.xmotion.y, itemn, popupmenu );
                             }
                           }
                           else
                             if ( inside )
                               item = MakeItemActive ( ev.xmotion.y, itemn, popupmenu );
                           break;
      }
  } while ( ! ( ( ev.type == ButtonPress && ev.xany.window !=popupmenu ) || ( ev.type == ButtonRelease && ev.xany.window == popupmenu ) ) );

  XUngrabPointer ( display, CurrentTime );

  XDestroyWindow ( display, popupmenu );

  XSetForeground ( display, drawcontext, black );
  XSetBackground ( display, drawcontext, lightgray );
  XDrawImageString ( display, window, drawcontext, actual->x1, ascent + 1, actual->text, strlen ( actual->text ) );

  if ( item )
    return item->ID;
  else
    return 0;
}

int TMenu::Check ( XEvent *ev ) {
  int IDreturn = 0;

  switch ( ev->type ) {
    case Expose: Draw (); break;
    case EnterNotify: MakeActive ( ev->xcrossing.x ); break;
    case LeaveNotify: Deactivate (); break;
    case MotionNotify: if ( actual ) {
                         if ( actual->x1 >= ev->xmotion.x || ev->xmotion.x >= actual->x2 ) {
                           Deactivate ();
                           MakeActive ( ev->xmotion.x );
                         }
                       }
                       else
                         MakeActive ( ev->xmotion.x );
                       break;
    case ButtonRelease:
    case ButtonPress:  if ( actual )
                         IDreturn = SelectFromMenu ();
  }
  return IDreturn;
}

void TMenu::Remove () {
  XDestroyWindow ( display, window );
  XFreeGC ( display, drawcontext );
}

TLabel::TLabel ( int _x, int _y, char *_text, unsigned long _background ) {
  x = _x;
  y = _y;
  text = new char[strlen ( _text ) + 1];
  background = _background;
  strcpy ( _text, text );
}

TLabel::TLabel ( int _x, int _y, char *_text, unsigned long _background, TWindow *parent ) {
  x = _x;
  y = _y;
  text = new char[strlen ( _text ) + 1];
  strcpy ( _text, text );
  background = _background;
  parent->AddWidget ( this );
}

void TLabel::Init ( TWindow *parent ) {
  int direction, descent;
  XCharStruct overall;
  XQueryTextExtents ( display, widgetfont, text, strlen ( text ), &direction, &ascent, &descent, &overall );

  window = XCreateSimpleWindow ( display, parent->GetWindow (), x, y, overall.rbearing, ascent + descent, 0, black, background );

  drawcontext = XCreateGC ( display, window, 0L, 0 );
  XSetFont ( display, drawcontext, widgetfont );
  XSetForeground ( display, drawcontext, black );

  XMapRaised ( display, window );

  Draw ();

  XSelectInput ( display, window, ExposureMask );
}

void TLabel::Draw () {
  XDrawString ( display, window, drawcontext, 0, ascent, text, strlen ( text) );
}

int TLabel::Check ( XEvent *ev ) {
  Draw ();

  return 0;
}

void TLabel::Remove () {
  XDestroyWindow ( display, window );
  XFreeGC ( display, drawcontext );
}

void TLabel::SetTextColor ( long unsigned _foreground ) {
  XSetForeground ( display, drawcontext, _foreground );
  Draw ();
}

void TLabel::SetText ( char *_text ) {
  int direction, descent;
  XCharStruct overall;

  delete text;
  text = new char[strlen ( _text ) + 1];
  strcpy ( _text, text );

  XQueryTextExtents ( display, widgetfont, text, strlen ( text ), &direction, &ascent, &descent, &overall );
  XResizeWindow ( display, window, overall.rbearing, ascent + descent );
}

void TWindowEventWidget::AddEvent ( int type, int ID ) {
  TWindowEventListItem *help = new TWindowEventListItem;

  help->event = type;
  listmask  |= mask ( type );
  help->ID = ID;
  help->next  = root;

  root = help;
}

void TWindowEventWidget::RemoveEvent ( int type ) {
  TWindowEventListItem *help = root, *follow = NULL;

  while ( help && help->event != type ) {
    follow = help;
    help = help->next;
  }

  if ( help ) {
    if ( follow )
      follow->next = help->next;
    else
      root = help->next;
    delete help;
    listmask &= mask ( type ) ^ -1;
  }
}

int TWindowEventWidget::Check ( XEvent *ev ) {
  TWindowEventListItem *help = root;

  while ( help ) {
    if ( help->event == ev->type )
      return help->ID;
    help = help->next;
  }

  return 0;
}


TWidgetList::TWidgetList () {
  root = NULL;
  begin = new TWindowEventWidget;
}


TWidgetList::~TWidgetList () {
  TWidgetListElement *help;

  while ( root ) {
    help = root;
    root = root->next;
    delete help;
  }

  delete begin;
}


void TWidgetList::AddWidget ( TWidget *element ) {
  TWidgetListElement *help = new TWidgetListElement;
  help->next = root;
  help->widget = element;
  root = help;
}

void TWidgetList::RemoveWidget ( TWidget *element ) {
  TWidgetListElement *help = root, *follow = NULL;

  while ( help  && help->widget != element ) {
    follow = help;
    help = help->next;
  }
  if ( help ) {
    if ( follow )
      follow->next = help->next;
    else
      root = help->next;
    delete help;
  }
}


int TWidgetList::ExtractID ( XEvent *ev ) {
  TWidgetListElement *walker = root;
  int returnID = 0;

  while ( walker && !returnID ) {
    if ( ev->xany.window == walker->widget->window )
      returnID = walker->widget->Check ( ev );

    walker = walker->next;
  }

  return returnID;
}

void TWindow::Init ( unsigned int _width, unsigned int _height, unsigned long background, char *title, char *icontitle ) {
  XEvent ev;
  if ( windowlist == NULL )
    windowlist = new TWindowListElement ( this, NULL );
  else
    windowlist = new TWindowListElement ( this, windowlist );

  window = XCreateSimpleWindow ( display, root_window, 0, 0, _width, _height, 2, black, background );
  XStoreName ( display, window, title );
  XSetIconName ( display, window, icontitle );

  width = _width;
  height = _height;

  drawcontext = XCreateGC ( display, window, 0L, 0 );
  XSetLineAttributes ( display, drawcontext, 1, LineSolid, CapRound, JoinRound );
  SetDrawBackground ( background );
  font = XLoadFont ( display, "9x15" );
  fontinuse = font;
  fill = false;

  widgetlist = new TWidgetList;
  AddWidget ( widgetlist->begin );

  XMapRaised ( display, window );

  XSelectInput ( display, window, ExposureMask );
  do
    XNextEvent ( display, &ev );
  while ( ev.type != Expose );
}

TWindow::TWindow ( unsigned int width, unsigned int height, unsigned long background, char *title, char *icontitle ) {
  Init ( width, height, background, title, icontitle );
}

TWindow::TWindow ( unsigned int width, unsigned int height, unsigned long background, char *title ) {
  Init ( width, height, background, title, title );
}

TWindow::TWindow ( unsigned int width, unsigned int height ) {
  Init ( width, height, white, argv[0], argv[0] );
}

TWindow::~TWindow () {
  TWindowListElement *walk = windowlist, *follow = NULL;
  TWindow *temp;

  delete widgetlist;

  XDestroyWindow ( display, window );

  while ( walk->window != this ) {
    follow = walk;
    walk = walk->next;
  }

  if ( follow )
    follow->next = walk->next;
  else
    windowlist = walk->next;

  delete walk;
  CheckForGlobalEvent ( temp );
}

void TWindow::DrawLine (int x1, int y1, int x2, int y2 ) {
  XDrawLine ( display, window, drawcontext, x1, y1, x2, y2 );
}

void TWindow::DrawPoint ( int x, int y ) {
  XDrawPoint ( display, window, drawcontext, x, y );
}


// teken een deel van een ellips die ligt in de rechthoek (x,y,x+width,y+height),
// dat deel wordt getekend lopend van arc1 tot arc1+arc2.
void TWindow::DrawArc (int x, int y, unsigned int width, unsigned int height, int arc1, int arc2 ) {
  if ( fill )
    XFillArc ( display, window, drawcontext, x, y, width, height, arc1*64, arc2*64 );
  else
    XDrawArc ( display, window, drawcontext, x, y, width, height, arc1*64, arc2*64 );
}

void TWindow::DrawRectangle ( int x, int y, unsigned int width, unsigned int height ) {
  if ( fill )
    XFillRectangle ( display, window, drawcontext, x, y, width, height );
  else
    XDrawRectangle ( display, window, drawcontext, x, y, width, height );
}

void TWindow::DrawString ( int x, int y, char* string ) {
  if ( fill )
    XDrawImageString ( display, window, drawcontext, x, y, string, strlen ( string ) );
  else
    XDrawString ( display, window, drawcontext, x, y, string, strlen ( string ) );
}

int TWindow::GetStringWidth ( char *string ) {
  XCharStruct overall;
  int ascent, descent, direction;

  XQueryTextExtents ( display, fontinuse, string, strlen ( string ), &direction, &ascent, &descent, &overall );

  return overall.rbearing;
}

int TWindow::GetStringAscent ( char *string ) {
  XCharStruct overall;
  int ascent, descent, direction;

  XQueryTextExtents ( display, fontinuse, string, strlen ( string ), &direction, &ascent, &descent, &overall );

  return ascent;
}

int TWindow::GetStringDescent ( char *string ) {
  XCharStruct overall;
  int ascent, descent, direction;

  XQueryTextExtents ( display, fontinuse, string, strlen ( string ), &direction, &ascent, &descent, &overall );

  return descent;
}

void TWindow::SetDrawForeground ( unsigned long color ) {
  XSetForeground ( display, drawcontext, color );
}

void TWindow::SetDrawBackground ( unsigned long color ) {
  XSetBackground ( display, drawcontext, color );
}

void TWindow::SetDrawFont ( char *name ) {
  XUnloadFont ( display, font );
  font = XLoadFont ( display, name );
  fontinuse = font;
  XSetFont ( display, drawcontext, font );
}

void TWindow::SetDrawFont ( Font fontname ) {
  fontinuse = fontname;
  XSetFont ( display, drawcontext, fontname );
}

void TWindow::SetDrawLineStyle ( LineStyle linestyle, unsigned int width) {
  switch ( linestyle ) {
    case LineStyleSolid: XSetLineAttributes ( display, drawcontext, width, LineSolid, CapRound, JoinRound ); break;
    case LineStyleOnOffDash: XSetLineAttributes ( display, drawcontext, width, LineOnOffDash, CapRound, JoinRound ); break;
    case LineStyleDoubleDash: XSetLineAttributes ( display, drawcontext, width, LineDoubleDash, CapRound, JoinRound );
  }
}

void TWindow::SetDrawFillStyle ( FillStyle fillstyle ) {
  switch ( fillstyle ) {
    case FillStyleNone: fill = false; break;
    case FillStyleSolid: fill = true;
  }
}

void TWindow::SetDrawArcStyle ( ArcStyle arcstyle ) {
  switch ( arcstyle ) {
    case ArcStyleChord: XSetArcMode ( display, drawcontext, ArcChord ); break;
    case ArcStylePieSlice: XSetArcMode ( display, drawcontext, ArcPieSlice );
  }
}

void TWindow::ClearWindow () {
  XClearWindow ( display, window );
}

void TWindow::AddEvent ( int eventtype, int ID ) {
  widgetlist->begin->AddEvent ( eventtype, ID );
  XSelectInput ( display, window, widgetlist->begin->ExtractMask () | ExposureMask | ResizeRedirectMask );
}

void TWindow::RemoveEvent ( int eventtype ) {
  widgetlist->begin->RemoveEvent ( eventtype );
  XSelectInput ( display, window, widgetlist->begin->ExtractMask () | ExposureMask | ResizeRedirectMask );
}

void TWindow::AddWidget ( TWidget *widget ) {
  widgetlist->AddWidget ( widget );
  widget->Init ( this );
}

void TWindow::RemoveWidget ( TWidget *widget ) {
  widget->Remove ();
  widgetlist->RemoveWidget ( widget );
}

int TWindow::CheckForEvent () {
  XEvent ev;

  if ( XEventsQueued ( display, QueuedAfterFlush ) ) {
    XNextEvent ( display, &ev );
    return GetEvent ( ev );
  }
  else
    return 0;
}

int TWindow::WaitForEvent () {
  int IDreturn;
  XEvent ev;

  do
    XNextEvent ( display, &ev );
  while ( ! ( IDreturn = GetEvent ( ev )  ) );

  return IDreturn;
}

int TWindow::GetEvent ( XEvent &ev ) {
  TWindowListElement *walk;
  int IDreturn = 0, ID;

  if ( ev.xany.window == window ) {
    switch ( ev.type ) {
      case MotionNotify:   pointerx = ev.xmotion.x;
                           pointery = ev.xmotion.y;
                           buttonstatus = NoButtonAction;
                           specialkeystatus = NoSpecialKeyPressed;
                           if ( ev.xmotion.state & Button1Mask )
                             buttonstatus |= Button1Action;
                           if ( ev.xmotion.state & Button2Mask )
                             buttonstatus |= Button2Action;
                           if ( ev.xmotion.state & Button3Mask )
                             buttonstatus |= Button3Action;
                           if ( ev.xmotion.state & ShiftMask )
                             specialkeystatus |= ShiftKeyPressed;
                           if ( ev.xmotion.state & ControlMask )
                             specialkeystatus |= ControlKeyPressed;
                           break;
      case ButtonPress:
      case ButtonRelease:  pointerx = ev.xbutton.x;
                           pointery = ev.xbutton.y;
                           specialkeystatus = NoSpecialKeyPressed;
                           switch ( ev.xbutton.button ) {
                             case Button1: buttonstatus = Button1Action; break;
                             case Button2: buttonstatus = Button2Action; break;
                             case Button3: buttonstatus = Button3Action; break;
                             default:      buttonstatus = NoButtonAction;
                           }
                           if ( ev.xbutton.state & ShiftMask )
                             specialkeystatus |= ShiftKeyPressed;
                           if ( ev.xbutton.state & ControlMask )
                             specialkeystatus |= ControlKeyPressed;
                           break;
      case EnterNotify:
      case LeaveNotify:    pointerx = ev.xcrossing.x;
                           pointery = ev.xcrossing.y;
                           break;
      case Expose:
      case FocusIn:
      case FocusOut:       break;
      case KeyPress:
      case KeyRelease:     pointerx = ev.xkey.x;
                           pointery = ev.xkey.y;
                           specialkeystatus = NoSpecialKeyPressed;
                           if ( ev.xmotion.state & ShiftMask ) {
                             specialkeystatus |= ShiftKeyPressed;
                             keypressedcode = XLookupKeysym ( &(ev.xkey), 1 );
                           }
                           else
                             keypressedcode = XLookupKeysym ( &(ev.xkey), 0 );
                           if ( ev.xmotion.state & ControlMask )
                             specialkeystatus |= ControlKeyPressed;
                           break;
      case ResizeRequest:  width = ev.xresizerequest.width;
                           height = ev.xresizerequest.height;
                           break;
    }
  }
  if ( ev.type == Expose ) {
    walk = windowlist;
    while ( walk && !IDreturn ) {
      ID = walk->window->widgetlist->ExtractID ( &ev );
      if ( ID && walk->window == this )
        IDreturn = ID;
      walk = walk->next;
    }
  }
  else
    IDreturn = widgetlist->ExtractID ( &ev );

  return IDreturn;
}

void SetWidgetFont ( char *name ) {
  XUnloadFont ( display, widgetfont );
  widgetfont = XLoadFont ( display, name );
}

void SetInputFont ( char *name ) {
  XUnloadFont ( display, inputfont );
  inputfont = XLoadFont ( display, name );
}

void initcolors () {
  XColor exact, Xblue, Xred, Xgreen, Xcyan, Xmagenta, Xorange, Xpink,
                Xbrown, Xviolet, Xgreenyellow, Xgray, Xyellow, Xdarkgray, Xlightgray;
  unsigned int depth;
  Colormap cmap;

  depth = DefaultDepth( display, screen );
  cmap = DefaultColormap( display, screen );
  black = BlackPixel( display, screen );
  white = WhitePixel( display, screen );
  if( depth == 1 )
    Xblue.pixel = Xred.pixel = Xgreen.pixel = Xcyan.pixel =
      Xmagenta.pixel = Xorange.pixel = Xpink.pixel = Xbrown.pixel =
      Xviolet.pixel = Xgreenyellow.pixel = Xgray.pixel =
      Xyellow.pixel = Xdarkgray.pixel = Xlightgray.pixel = white;
  else
    if ( !XAllocNamedColor ( display, cmap, "yellow", &exact, &Xyellow ) ||
         !XAllocNamedColor ( display, cmap, "blue", &exact, &Xblue ) ||
         !XAllocNamedColor ( display, cmap, "red", &exact, &Xred ) ||
         !XAllocNamedColor ( display, cmap, "green", &exact, &Xgreen ) ||
         !XAllocNamedColor ( display, cmap, "cyan", &exact, &Xcyan ) ||
         !XAllocNamedColor ( display, cmap, "magenta", &exact, &Xmagenta ) ||
         !XAllocNamedColor ( display, cmap, "coral", &exact, &Xorange ) ||
         !XAllocNamedColor ( display, cmap, "pink", &exact, &Xpink ) ||
         !XAllocNamedColor ( display, cmap, "brown", &exact, &Xbrown ) ||
         !XAllocNamedColor ( display, cmap, "violet", &exact, &Xviolet ) ||
         !XAllocNamedColor ( display, cmap, "greenyellow", &exact, &Xgreenyellow ) ||
         !XAllocNamedColor ( display, cmap, "gray", &exact, &Xgray ) ||
         !XAllocNamedColor ( display, cmap, "darkgray", &exact, &Xdarkgray ) ||
         !XAllocNamedColor ( display, cmap, "lightgray", &exact, &Xlightgray ) ) {
      cerr << "FOUT: kleurtoekenning onmogelijk." << endl;
      exit ( 1 );
    }
  yellow = Xyellow.pixel;
  blue = Xblue.pixel;
  red = Xred.pixel;
  green = Xgreen.pixel;
  cyan = Xcyan.pixel;
  magenta = Xmagenta.pixel;
  orange = Xorange.pixel;
  pink = Xpink.pixel;
  brown = Xbrown.pixel;
  violet = Xviolet.pixel;
  greenyellow = Xgreenyellow.pixel;
  gray = Xgray.pixel;
  darkgray = Xdarkgray.pixel;
  lightgray = Xlightgray.pixel;
}

int GetGlobalEvent ( TWindow* &window, XEvent &ev ) {
  TWindowListElement *walk = windowlist;
  int ID;

  while ( walk ) {
    if ( ( ID = walk->window->GetEvent ( ev ) ) ) {
      window = walk->window;
      return ID;
    }
    walk = walk->next;
  }

  return 0;
}

int WaitForGlobalEvent ( TWindow* &window ) {
  XEvent ev;
  int IDreturn = 0;

  do
    XNextEvent ( display, &ev );
  while ( ! ( IDreturn = GetGlobalEvent ( window, ev ) ) );

  return IDreturn;
}

int CheckForGlobalEvent ( TWindow* &window ) {
  XEvent ev;

  if ( XEventsQueued ( display, QueuedAfterFlush ) ) {
    XNextEvent ( display, &ev );
    return GetGlobalEvent ( window, ev );
  }
  else
    return 0;
}

int main ( int Argc, char *Argv[] ) {
  int returnval;

  argc = Argc;
  argv = new char*[Argc];
  for ( int i = 0; i<argc; i++ )
    argv[i] = Argv[i];

  display = XOpenDisplay ( NULL );
  screen = DefaultScreen ( display );
  root_window = DefaultRootWindow ( display );

  widgetfont = XLoadFont ( display, "-adobe-helvetica-bold-r-normal--14-*-*-*-*-*-*-*" );
  inputfont = XLoadFont ( display, "9x15" );

  initcolors ();

  returnval = Xmain ();

  XCloseDisplay ( display );

  delete[] argv;

  return returnval;
}
