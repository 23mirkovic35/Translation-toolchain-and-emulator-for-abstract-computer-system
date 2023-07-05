#ifndef SIMBOL_HPP_
#define SIMBOL_HPP_

#include <string>
#include <list>
#include <iostream>
typedef enum {LOC = 0, GLOB, EXT} Tip;
typedef enum {KOMANDA_WORD = 0, KOMANDA_PC_REL, OSTALE} TipKomande;
using namespace std;

typedef struct MestoKoriscenja
{
  int ofset;
  string sekcija;
  TipKomande tip;
  int velicina;
  int pocetak;
  int rbZapisa; /* redni broj relokacionog zapisa, -1 ako ga ne pravi */
  MestoKoriscenja(int ofset, string sekcija, TipKomande tip, int velicina, int pocetak, int rbZapisa){
    this->ofset = ofset;
    this->sekcija = sekcija;
    this->tip = tip;
    this->velicina = velicina;
    this->pocetak = pocetak;
    this->rbZapisa = rbZapisa;
  }
} MestoKoriscenja;


class Simbol {
private:
  string ime;
  string sekcija;
  bool definisan;
  int vrednost; /* offset */
  Tip tip;
  list<MestoKoriscenja> listaMestaKoriscenja;
  static int broj;
  int redniBroj = broj++; /* dodato zbog lepseg ispisa */
public:
  Simbol(string ime, int vrednost, Tip tip, string sekcija);
  void postaviIme(string ime);
  string dohvatiIme();
  void postaviSekciju(string sekcija);
  string dohvatiSekciju();
  void postaviDefinisan(bool definisan);
  bool dohvatiDefinisan();
  void postaviVrednost(int vrednost);
  int dohvatiVrednost();
  void postaviTip(Tip tip);
  Tip dohvatiTip();
  list<MestoKoriscenja> dohvatiListuMestaKoriscenja();
  int dohvatiVelicinuListeMestaKoriscenja();
  void dodajUListuMestaKoriscenja(MestoKoriscenja);
  void ispisiListuMestaKoriscenja();
  Simbol* dohvatiSimbol();
  int dohvatiRedniBroj();
};

#endif