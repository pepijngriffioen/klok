Display *display;           // verbinding met de server
Window root_window;         // wortelvenster (onzichtbaar), toegewezen door server
int screen;                 // beeldscherm

const int typenumber = 11;

long masktabel[typenumber][2] = {
  { MotionNotify,    ButtonMotionMask | PointerMotionMask },  // Bewogen
  { ButtonPress,     ButtonPressMask     },  // muistoets omlaag
  { ButtonRelease,   ButtonReleaseMask   },  // muistoets omhoog
  { EnterNotify,     EnterWindowMask     },  // venster in gegaan
  { LeaveNotify,     LeaveWindowMask     },  // venster uit gegaan
  { Expose,          ExposureMask        },  // hertekenen vensterinhoud verzocht
  { FocusIn,         FocusChangeMask     },  // venster krijgt de focus
  { FocusOut,        FocusChangeMask     },  // venster verliest de focus
  { KeyPress,        KeyPressMask        },  // er wordt een toets ingedrukt
  { KeyRelease,      KeyReleaseMask      },  // er wordt een toets losgelaten
  { ResizeRequest,   ResizeRedirectMask  }   // venster verandert van grootte
};

#define checksymbol_width 16
#define checksymbol_height 16
static char checksymbol_bits[] = {
   0x00, 0x70, 0x00, 0x70, 0x00, 0x38, 0x00, 0x3c, 0x00, 0x1c, 0x06, 0x1e,
   0x0e, 0x0e, 0x1e, 0x0f, 0xb8, 0x0f, 0xf0, 0x07, 0xe0, 0x07, 0xc0, 0x07,
   0xc0, 0x03, 0x80, 0x03, 0x80, 0x03, 0x00, 0x00 };

#define XK_Left			0xFF51	/* Move left, left arrow */
#define XK_Right		0xFF53	/* Move right, right arrow */
#define XK_Shift_L		0xFFE1	/* Left shift */
#define XK_Shift_R		0xFFE2	/* Right shift */
#define XK_BackSpace		0xFF08	/* back space, back char */
#define XK_Delete		0xFFFF	/* Delete, rubout */
#define XK_Home			0xFF50
#define XK_End			0xFF57	/* EOL */

struct TWindowEventListItem {
  int event;
  int ID;
  TWindowEventListItem *next;
};

class TWindowEventWidget:public TWidget {
  public:
    TWindowEventWidget () {
      root = NULL;
      listmask = 0;
    }
    int Check ( XEvent *ev );
    void AddEvent ( int type, int ID );
    void RemoveEvent ( int type );
    void Init ( TWindow *parent ) {
      window = parent->GetWindow ();
    }
    long ExtractMask () { return listmask; }
  private:
    TWindowEventListItem *root;
    long listmask;
};


// intern
class TWidgetListElement {
  public:
    TWidget *widget;
    TWidgetListElement *next;
};

// intern
class TWidgetList {
  public:
    TWindowEventWidget *begin;

    TWidgetList ();
    ~TWidgetList ();
    void AddWidget ( TWidget *element );
    void RemoveWidget ( TWidget *element );
    int ExtractID ( XEvent *ev );
  private:
    TWidgetListElement *root;
};


class TMenuItemListElement {
  public:
    TMenuItemListElement ( char *_text, int _ID );
    ~TMenuItemListElement () {
      delete text;
      if ( next ) delete next;
    }
    TMenuItemListElement *next;
    char *text;
    int ID;
};

class TMenuListElement {
  public:
    TMenuListElement ( char *_text );
    ~TMenuListElement () {
      delete text;
      if ( next ) delete next;
      delete menu;
    }
    TMenuListElement *next;
    TMenuItemListElement *menu;
    char *text;
    int x1, x2, width, n;
};

class TWindowListElement {
  public:
    TWindowListElement ( TWindow *_window, TWindowListElement *_next ) {
      window = _window;
      next = _next;
    }
    TWindow *window;
    TWindowListElement *next;
};
