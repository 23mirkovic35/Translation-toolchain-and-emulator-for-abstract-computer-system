#include "../inc/SekcijaLinker.hpp"

SekcijaLinker::SekcijaLinker(int rbFajla, string ime, int adresaPocetka, int velicina) {
  this->rbFajla = rbFajla;
  this->ime = ime;
  this->adresaPocetka = adresaPocetka;
  this->velicina = velicina;
}

int SekcijaLinker::dohvatiRedniBrojFajla(){ return this->rbFajla; }
void SekcijaLinker::postaviRedniBrojFajla(int rbFajla){ this->rbFajla = rbFajla; }
string SekcijaLinker::dohvatiImeSekcije(){ return this->ime; }
void SekcijaLinker::postaviImeSekcije(string ime){ this->ime = ime; }
int SekcijaLinker::dohvatiAdresuPocetka(){ return this->adresaPocetka; }
void SekcijaLinker::postaviAdresuPocetka(int adresaPocetka){ this->adresaPocetka = adresaPocetka; }
int SekcijaLinker::dohvatiVelicinu(){ return this->velicina; }
void SekcijaLinker::postaviVelicinu(int velicina){ this->velicina = velicina; }
int SekcijaLinker::dohvatiVelicinuRelokacioneTable(){ return this->relokacioniZapisi.size(); }
list<RelokacioniZapisLinker*> SekcijaLinker::dohvatiListuRelokacionihZapisa(){ return this->relokacioniZapisi; }
void SekcijaLinker::dodajRelokacioniZapis(RelokacioniZapisLinker* relZapis){ this->relokacioniZapisi.push_back(relZapis); }
RelokacioniZapisLinker* SekcijaLinker::dohvatiRelokacioniZapis(int rb){
  list<RelokacioniZapisLinker*>::iterator it;
  for(it=relokacioniZapisi.begin(); it!=relokacioniZapisi.end(); ++it){
    RelokacioniZapisLinker* relZapis = (*it);
    if(rb == relZapis->dohvatiRedniBrojRelokacionigZapisa()) return relZapis;
  }
  return nullptr;
}
void SekcijaLinker::ispisiRelokacioneZapise(){
  list<RelokacioniZapisLinker*>::iterator it;
  for(it=relokacioniZapisi.begin(); it!=relokacioniZapisi.end(); ++it){
    RelokacioniZapisLinker* relZapis = (*it);
    cout<<relZapis->dohvatiRedniBrojRelokacionigZapisa()<<endl;
  }
}