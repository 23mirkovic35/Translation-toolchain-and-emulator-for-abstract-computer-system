#include "../inc/Simbol.hpp"

int Simbol::broj = 0;

Simbol::Simbol(string ime, int vrednost, Tip tip, string sekcija){
  this->ime = ime;
  this->vrednost = vrednost;
  this->tip = tip;
  this->sekcija = sekcija;
  this->definisan = false;
}
  
void Simbol :: postaviIme(string ime){
  this->ime = ime;
}

string Simbol :: dohvatiIme(){
  return this->ime;
}

void Simbol :: postaviSekciju(string sekcija) {
  this->sekcija = sekcija;
}

string Simbol :: dohvatiSekciju() {
  return this->sekcija;
}

void Simbol :: postaviDefinisan (bool definisan) {
  this->definisan = definisan;
}

bool Simbol :: dohvatiDefinisan() {
  return this->definisan;
}

void Simbol :: postaviVrednost(int vrednost){
  this->vrednost = vrednost;
}

int Simbol :: dohvatiVrednost() {
  return this->vrednost;
}

void Simbol :: postaviTip(Tip tip) {
  this->tip = tip;
}

Tip Simbol :: dohvatiTip() {
  return this->tip;
}

list<MestoKoriscenja> Simbol :: dohvatiListuMestaKoriscenja(){
  return this->listaMestaKoriscenja;
}

int Simbol :: dohvatiVelicinuListeMestaKoriscenja(){
  return this->listaMestaKoriscenja.size();
}

void Simbol :: dodajUListuMestaKoriscenja(MestoKoriscenja novoMesto){
  this->listaMestaKoriscenja.push_back(novoMesto);
}

void Simbol :: ispisiListuMestaKoriscenja(){
  list<MestoKoriscenja>::iterator it;
  int i = 1;
  for(it = listaMestaKoriscenja.begin(); it!=listaMestaKoriscenja.end(); ++it){
    cout<<i++<<". "<<(*it).sekcija<< " "<<(*it).ofset<<endl;
  }
}

Simbol* Simbol :: dohvatiSimbol() {
  return this;
}

int Simbol :: dohvatiRedniBroj(){
  return this->redniBroj;
}