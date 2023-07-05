#ifndef TABELA_SIMBOLA_HPP_
#define TABELA_SIMBOLA_HPP_

#include <list>
#include <string>
#include "Simbol.hpp"
#include <string>
#include <stdlib.h>
#include <fstream>
#include <iostream>

using namespace std;

class TabelaSimbola {

private:
  list<Simbol*> tabelaSimbola;
public:
  TabelaSimbola();
  void dodajSimbolUTabelu(Simbol* simbol);
  bool simbolVecUTabeli(string ime);
  void ispisiTabeluSimbola();
  Simbol* dohvatiSimbol(string ime);
  Simbol* dohvatiSimbol(int);
  bool proveriDefinisanostSimbola();
  list<Simbol*> dohvatiTabeluSimbola();
};

#endif