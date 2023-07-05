#include "../inc/SimbolLinker.hpp"

SimbolLinker::SimbolLinker(int rbFajla, string ime, int vrednost, Tip tip, string sekcija){
  this->rbFajla = rbFajla;
  this->ime = ime;
  this->vrednost = vrednost;
  this->tip = tip;
  this->sekcija = sekcija;
}
void SimbolLinker::postaviRedniBrojFajla(int rbFajla){ this->rbFajla = rbFajla;}
int SimbolLinker::dohvatiRedniBrojFajla(){ return this->rbFajla; }
void SimbolLinker::postaviIme(string ime){ this->ime = ime;}
string SimbolLinker::dohvatiIme(){ return this->ime; }
void SimbolLinker::postaviSekciju(string sekcija){ this->sekcija = sekcija; }
string SimbolLinker::dohvatiSekciju(){ return this->sekcija; }
void SimbolLinker::postaviVrednost(int vrednost){ this->vrednost = vrednost; }
int SimbolLinker::dohvatiVrednost(){ return this->vrednost; }
void SimbolLinker::postaviTip(Tip tip){ this->tip = tip; }
Tip SimbolLinker::dohvatiTip(){ return this->tip; }
SimbolLinker* SimbolLinker::dohvatiSimbol(){ return this; }
void SimbolLinker::postaviRedniBroj(int redniBroj){ this->redniBroj = redniBroj; }
int SimbolLinker::dohvatiRedniBroj(){ return this->redniBroj; }
void SimbolLinker::postaviVelicinu(int velicina){ this->velicina = velicina; }
int SimbolLinker::dohvatiVelicinu(){ return this->velicina; }