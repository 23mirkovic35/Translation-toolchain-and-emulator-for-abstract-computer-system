#include "../inc/Sekcija.hpp"

int Sekcija::broj = 0;

Sekcija::Sekcija(string ime){
  this->ime = ime;
  this->sadrzaj = "";
  this->tabelaRelokacionihZapisa = new TabelaRelokacionihZapisa(ime);
  this->tabelaReferisanjaUnapred = new TabelaReferisanjaUnapred();
}

int Sekcija::dohvatiRedniBroj(){ return this->redniBroj; }

string Sekcija::dohvatiImeSekcije(){ return this->ime; }

void Sekcija::postaviImeSekcije(string ime) { this->ime = ime; }

string Sekcija::dohvatiPomeraj(){ return this->pomeraj; }

void Sekcija::postaviPomeraj(string pomeraj) { this->pomeraj = pomeraj; }

string Sekcija::dohvatiSadrzajSekcije() { return this->sadrzaj; }

void Sekcija::postaviSadrzajSekcije(string sadrzaj) { this->sadrzaj = sadrzaj; }

void Sekcija::ispisiSekciju(){
  cout<<endl<<"#"+dohvatiImeSekcije()+"."<<dohvatiVelicinuSekcije()<<endl;
  int blankoZnak = 0;
  int noviRed = 0;
  for(int i = 0; i<dohvatiSadrzajSekcije().size(); i++){
    if(noviRed%8==0 && i!=0){
      cout<<endl;
      blankoZnak = 0;
      noviRed=0;
    }
    else if(blankoZnak%2==0 && i!=0){
      cout<<" ";
    }
    cout<<dohvatiSadrzajSekcije().at(i);
    blankoZnak++;
    noviRed++;
  }
  cout<<endl;
}

void Sekcija::dodajSadrzajUSekciju(string noviSadrzaj) { this->sadrzaj = this->sadrzaj + noviSadrzaj; }

int Sekcija::dohvatiVelicinuSekcije(){ return this->velicina; }

void Sekcija::postaviVelicinuSekcije(int velicina){ this->velicina = velicina; }

TabelaRelokacionihZapisa* Sekcija::dohvatiTabeluRelokacionihZapisa() {return this->tabelaRelokacionihZapisa;}

TabelaReferisanjaUnapred* Sekcija::dohvatiTabeluReferisanjaUnapred() { return this->tabelaReferisanjaUnapred; }

void Sekcija::ispisiTabeluReferisanjaUnapred(){ this->tabelaReferisanjaUnapred->ispisiTabeluReferisanjaUnapred(); }

Sekcija* Sekcija::dohvatiSekciju(){ return this; }