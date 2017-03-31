//
// Zet een windowtje met een analoge klok op het scherm, in C++
//
// Siegfried Nijssen, Walter Kosters
// Informatica, Universiteit Leiden
// kosters@liacs.nl
// 30.1.2004
//
// Verander zelf de gedeeltes tussen TODObegin en TODOeind
//

#include <iostream>
#include <unistd.h>
// unistd zorgt ervoor dat later sleep ( ) gebruikt kan worden.
#include <ctime>
// voor de tijd
#include <cmath>
// voor pi: M_PI, en sinus en cosinus
#include "Xsowl.h"
using namespace std;

const int okclick = 1;

// teken wijzer van zekere lengte in zekere kleur
void tekenwijzer (TWindow & venster, int xmidden, int ymidden,
		  int lengte, double hoek, unsigned long kleur) {
  venster.SetDrawForeground (kleur);
  venster.DrawLine (
    xmidden, ymidden,
    xmidden + (int) ( lengte * cos (hoek * M_PI / 180 - M_PI / 2) ),
    ymidden + (int) ( lengte * sin (hoek * M_PI / 180 - M_PI / 2) ) );
}  // tekenwijzer

int Xmain ( ) {

  tm* pointer;             // pointer naar struct tm
  time_t t;                // aantal seconden sinds ...
  int nummer = 0;          // wat is er geklikt?
  int xcentrumklok = 200;  // coordinaten centrum klok
  int ycentrumklok = 200;
  int langewijzer = 93;    // lengte wijzers
  int kortewijzer = 50;
  int uur;                 // de tijd ...
  int min;
  int sec;

  // maak een venster van 400 bij 600 pixels en toon het
  TWindow venster (400,600);

  // knop met "boodschap" op (100,500); 200 breed, 50 hoog.
  // CheckForEvent geeft '1' (= okclick) terug als erop gedrukt wordt
  TButton ok (100,500,200,50,(char*)"Gezien hoe laat het is?",okclick,&venster);

  // lijnen worden getekend als ononderbroken lijn met dikte 3
  venster.SetDrawLineStyle (LineStyleSolid,3);

  // nummer is 0 zolang er nog niet op de knop geklikt is
  while ( nummer == 0 ) {

    t = time(NULL);  // seconden sinds ...
    pointer = localtime(&t);
    uur = pointer->tm_hour;
    min = pointer->tm_min;
    sec = pointer->tm_sec;

    tekenwijzer (venster,xcentrumklok,ycentrumklok,
		 kortewijzer,(uur+min/60.0) * 30,black);
    tekenwijzer (venster,xcentrumklok,ycentrumklok,
		 langewijzer,min * 6,black);

    //TODObegin//
    // secondewijzer
    //TODOeind//

    // cirkel:
    venster.SetDrawForeground (black);
    venster.DrawArc(xcentrumklok-100,ycentrumklok-100,200,200,0,360);

    // kijk of er op een knop geklikt is
    nummer = venster.CheckForEvent ( );
    if ( nummer == okclick )
      cout << "Dat was de klok ..." << endl;
    else {
      sleep (1);  // wacht ongeveer een seconde

      //TODObegin//
      // lopende secondewijzer?
      //TODOeind//

    }  // else

  }  // while

  return 0;

}  // Xmain
