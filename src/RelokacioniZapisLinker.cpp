#include "../inc/RelokacioniZapisLinker.hpp"

RelokacioniZapisLinker::RelokacioniZapisLinker(int id , int pomeraj, TipZapisa tip, bool littleEndian){
  this->id = id;
  this->pomeraj = pomeraj;
  this->tip = tip;
  this->littleEndian = littleEndian;
}
int RelokacioniZapisLinker::dohvatiRedniBrojRelokacionigZapisa(){ return this->redniBroj; }
void RelokacioniZapisLinker::postaviRedniBrojRelokacionogZapisa(int id){ this->redniBroj = id; }
int RelokacioniZapisLinker::dohvatiOfset(){ return this->pomeraj; }
void RelokacioniZapisLinker::postaviOfset(int pomeraj){ this->pomeraj = pomeraj; }
TipZapisa RelokacioniZapisLinker::dohvatiTipZapisa(){ return this->tip; }
void RelokacioniZapisLinker::postaviTipZapisa(TipZapisa){ this->tip = tip; }
int RelokacioniZapisLinker::dohvatiID(){ return this->id; }
void RelokacioniZapisLinker::postaviID(int id){ this->id = id; }
bool RelokacioniZapisLinker::proveriEndiad(){ return this->littleEndian; }
void RelokacioniZapisLinker::postaviEndian(bool littleEndian){ this->littleEndian = littleEndian; }