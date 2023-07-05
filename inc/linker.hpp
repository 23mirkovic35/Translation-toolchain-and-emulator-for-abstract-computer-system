#ifndef LINKER_HPP
#define LINKER_HPP

#include "SekcijaLinker.hpp"
#include "SimbolLinker.hpp"
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iostream>
#include <list>
#include <regex>
#include <math.h>
#include <bits/stdc++.h>

using namespace std;

typedef struct Izlaz
{
  int adresa;
  string sadrzaj;
  Izlaz(int adresa, string sadrzaj){
    this->adresa = adresa;
    this->sadrzaj = sadrzaj;
  }
} Izlaz;

class Linker{

private:
  list<string> listaUlaznihFajlova;
  ofstream izlazniFajl;
  list<SekcijaLinker*> tabelaSekcija;
  list<SimbolLinker*> tabelaSimbola;
  list<string> eksterniSimboli;
  string sadrzaj = "";
  list<Izlaz> listaAdresa;
  int pozicioniBrojac = 0;
  int hex2dec(string);
  string dec2hex(string);
  bool sekcijaVecUTabeliSekcija(string,int);
  void dodajSekcijeSaIstimImenom(string,string);
  void dodajSimbolUTabelu(SimbolLinker*);
  bool simbolVecUTabeli(string);
  SimbolLinker* dohvatiSimbolIzTabeleSimbola(string);
  bool sviEksterniSimboliDefinisani();
  SekcijaLinker* dovhvatiSekciju(string,int);
  map<char, int> hex_value_of_dec(void);
  map<int, char> dec_value_of_hex(void);
  string Add_Hex(string,string);
public:
  Linker(string, list<string>);
  void pokreni();
  void raspodeliSekcije();
  void popuniTabeluSimbola();
  void azurirajTabeluSimbola();
  void azurirajSadzraj();
  void upisiUIzlazniFajl();
};

#endif