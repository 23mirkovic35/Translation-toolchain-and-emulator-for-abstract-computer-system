#ifndef SEKCIJA_HPP_
#define SEKCIJA_HPP_

#include "TabelaReferisanjaUnapred.hpp"
#include "TabelaRelokacionihZapisa.hpp"
#include <string>
#include <stdlib.h>
#include <iostream>
using namespace std;

class Sekcija {

private:
  static int broj;
  int redniBroj = broj++;  /* redni broj u tabeli sekcija */
  string ime;
  string pomeraj;
  string sadrzaj;
  int velicina;
  TabelaRelokacionihZapisa* tabelaRelokacionihZapisa;
  TabelaReferisanjaUnapred* tabelaReferisanjaUnapred;
public:
  Sekcija(string);
  int dohvatiRedniBroj();
  string dohvatiImeSekcije();
  void postaviImeSekcije(string);
  string dohvatiPomeraj();
  void postaviPomeraj(string);
  string dohvatiSadrzajSekcije();
  void postaviSadrzajSekcije(string);
  void dodajSadrzajUSekciju(string);
  void ispisiSekciju();
  int dohvatiVelicinuSekcije();
  void postaviVelicinuSekcije(int);
  TabelaRelokacionihZapisa* dohvatiTabeluRelokacionihZapisa();
  TabelaReferisanjaUnapred* dohvatiTabeluReferisanjaUnapred();
  void ispisiTabeluReferisanjaUnapred();
  Sekcija* dohvatiSekciju();
};

#endif