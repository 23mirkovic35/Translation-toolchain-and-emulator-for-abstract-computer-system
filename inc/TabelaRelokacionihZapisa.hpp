#ifndef TABELA_RELOKACIONIH_ZAPISA_HPP_
#define TABELA_RELOKACIONIH_ZAPISA_HPP_

#include "RelokacioniZapis.hpp"
#include <list>
#include <list>
#include <string>
#include <stdlib.h>
#include <iostream>

using namespace std;

class TabelaRelokacionihZapisa{
private:
  string sekcija;
  list<RelokacioniZapis*> tabelaRelokacionihZapisa;  
public:
  TabelaRelokacionihZapisa(string);
  list<RelokacioniZapis*> dohvatiTabeluRelokacionihZapisa();
  void ispisiTabeluRelokacionihZapisa();
  void dodajUTabeluRelokacionihZapisa(RelokacioniZapis*);
  int dohvatiVeliciniTabeleRelokacionihZapisa();
  RelokacioniZapis* dohvatiRelokacioniZapis(int);
};

#endif