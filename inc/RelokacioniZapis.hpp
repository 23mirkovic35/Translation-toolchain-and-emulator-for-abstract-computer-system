#ifndef RELOKACIONI_ZAPIS_HPP_
#define RELOKACIONI_ZAPIS_HPP_

#include "Simbol.hpp"
 
using namespace std;

typedef enum {
  R_VN16_16 = 0, // ABS
  R_VN16_PC16 = 1 // PC relativno
} TipZapisa;

class RelokacioniZapis{
private:
  static int broj;
  int redniBroj = broj++;
  int ofset;
  TipZapisa tip;
  int id; /* id simbola ili sekcije */
  bool littleEndian; 
public:
  RelokacioniZapis(int i , int o, TipZapisa t, bool littleEndian);
  int dohvatiRedniBrojRelokacionigZapisa();
  int dohvatiOfset();
  void postaviOfset(int);
  TipZapisa dohvatiTipZapisa();
  void postaviTipZapisa(TipZapisa);
  int dohvatiID();
  void postaviID(int);
  bool proveriEndiad();
  void postaviEndian(bool);
};



#endif