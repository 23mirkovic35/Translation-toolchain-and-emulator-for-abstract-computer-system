#include "../inc/RelokacioniZapis.hpp"

int RelokacioniZapis::broj = 0;

RelokacioniZapis::RelokacioniZapis(int i , int o, TipZapisa t, bool littleEndian) {
  id = i;
  ofset = o;
  tip = t;
  this->littleEndian = littleEndian;
}

int RelokacioniZapis::dohvatiRedniBrojRelokacionigZapisa(){return this->redniBroj;}

int RelokacioniZapis::dohvatiOfset(){return this->ofset;}

TipZapisa RelokacioniZapis::dohvatiTipZapisa(){return this->tip;}

int RelokacioniZapis::dohvatiID(){return this->id;}

bool RelokacioniZapis::proveriEndiad(){return this->littleEndian;}

void RelokacioniZapis::postaviOfset(int ofset){this->ofset = ofset;}

void RelokacioniZapis::postaviTipZapisa(TipZapisa tip){this->tip = tip;}

void RelokacioniZapis::postaviID(int id){this->id = id;}

void RelokacioniZapis::postaviEndian(bool littleEndian){this->littleEndian = littleEndian;}