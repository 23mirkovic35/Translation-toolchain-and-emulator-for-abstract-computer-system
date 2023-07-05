#ifndef SEKCIJA_LINKER_HPP_
#define SEKCIJA_LINKER_HPP_

#include "RelokacioniZapisLinker.hpp"
#include <list>
#include <string>
#include <stdlib.h>
#include <iostream>
using namespace std;

class SekcijaLinker
{
private:
  int rbFajla;
  string ime;
  int adresaPocetka;
  int velicina;
  list<RelokacioniZapisLinker*> relokacioniZapisi;
public:
  SekcijaLinker(int, string, int, int);
  int dohvatiRedniBrojFajla();
  void postaviRedniBrojFajla(int);
  string dohvatiImeSekcije();
  void postaviImeSekcije(string);
  int dohvatiAdresuPocetka();
  void postaviAdresuPocetka(int);
  int dohvatiVelicinu();
  void postaviVelicinu(int);
  int dohvatiVelicinuRelokacioneTable();
  list<RelokacioniZapisLinker*> dohvatiListuRelokacionihZapisa();
  void dodajRelokacioniZapis(RelokacioniZapisLinker*);
  RelokacioniZapisLinker* dohvatiRelokacioniZapis(int);
  void ispisiRelokacioneZapise();
};

#endif