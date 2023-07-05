#ifndef RELOKACIONI_ZAPIS_LINKER_HPP_
#define RELOKACIONI_ZAPIS_LINKER_HPP_

#include <string>

using namespace std;

typedef enum {
  R_VN16_16 = 0, // ABS
  R_VN16_PC16 = 1 // PC relativno
} TipZapisa;

class RelokacioniZapisLinker{
private:
  int redniBroj = 0;
  int pomeraj;
  TipZapisa tip;
  int id;
  bool littleEndian; 
public:
  RelokacioniZapisLinker(int id , int pomeraj, TipZapisa tip, bool littleEndian);
  int dohvatiRedniBrojRelokacionigZapisa();
  void postaviRedniBrojRelokacionogZapisa(int);
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