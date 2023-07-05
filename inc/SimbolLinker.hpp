#ifndef SIMBOL_LINKER_HPP_
#define SIMBOL_LINKER_HPP_

#include <string>
#include <list>
#include <iostream>
typedef enum {LOC = 0, GLOB, EXT} Tip;
using namespace std;

class SimbolLinker {
private:
  int rbFajla;
  string ime;
  string sekcija;
  int vrednost; /* offset */
  Tip tip;
  int redniBroj = -1;
  int velicina; /* za sekcije */
public:
  SimbolLinker(int rbFajla, string ime, int vrednost, Tip tip, string sekcija);
  void postaviRedniBrojFajla(int rbFajla);
  int dohvatiRedniBrojFajla();
  void postaviIme(string ime);
  string dohvatiIme();
  void postaviSekciju(string sekcija);
  string dohvatiSekciju();
  void postaviVrednost(int vrednost);
  int dohvatiVrednost();
  void postaviTip(Tip tip);
  Tip dohvatiTip();
  void postaviRedniBroj(int);
  int dohvatiRedniBroj();
  SimbolLinker* dohvatiSimbol();
  void postaviVelicinu(int);
  int dohvatiVelicinu();
};


#endif