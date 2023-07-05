#ifndef NEDEFINISAN_SIMBOL_HPP
#define NEDEFINISAN_SIMBOL_HPP

#include <string>
#include <stdlib.h>
#include <iostream>
using namespace std;

typedef enum {
  WORD = 0, PC_REL, OSTALO
}TipReferisanja;

class NedefinisanSimbol{

private:
  int simbolID;
  int adresa;
  string sekcija;
  int duzina = 2; /* uvek je 2B */
  TipReferisanja tip;
public:
  NedefinisanSimbol(int simbolID, int adresa, string sekcija, TipReferisanja tip){
    this->simbolID = simbolID;
    this->adresa = adresa;
    this->sekcija = sekcija;
    this->tip = tip;
  }

  NedefinisanSimbol(int simbolID, int adresa, string sekcija) {
    this->simbolID = simbolID;
    this->adresa = adresa;
    this->sekcija = sekcija;
    this->tip = TipReferisanja::OSTALO;
  }

  void ispisiNedefinisanSimbol(){
    cout<<simbolID<<" "<<adresa<<" "<<sekcija<<endl;
  }
  TipReferisanja dohvatiTipReferisanja() {return this->tip;}
  int dohvatiID(){ return this->simbolID; }
  int dohvatiAdresu(){ return this->adresa; }
};

#endif