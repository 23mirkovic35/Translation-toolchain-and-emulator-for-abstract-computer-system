#ifndef TABELA_SEKCIJA_HPP_
#define TABELA_SEKCIJA_HPP_
#include "Sekcija.hpp"
#include <list>
#include <string>
#include <stdlib.h>
#include <iostream>
using namespace std;

class TabelaSekcija{
private:
  list<Sekcija*> tabelaSekcija;
public:
  void dodajSekcijuUTabelu(Sekcija*);
  list<Sekcija*> dohvatiTabeluSekcija();
  int dohvatiVelicinuTabele();
  Sekcija* dohvatiSekciju(string);
  Sekcija* dohvatiSekciju(int);
  void ispisiTabeluSekcija();
  void ispisiRelokacioneZapiseSekcija();
};

#endif