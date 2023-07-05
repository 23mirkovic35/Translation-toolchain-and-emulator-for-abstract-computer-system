#include "../inc/TabelaSimbola.hpp"
#include "../inc/TabelaRelokacionihZapisa.hpp"
#include "../inc/TabelaSekcija.hpp"

TabelaSimbola::TabelaSimbola() {
  tabelaSimbola = list<Simbol*>();
}

void TabelaSimbola::dodajSimbolUTabelu(Simbol* simbol){
  tabelaSimbola.push_back(simbol);
}

bool TabelaSimbola::simbolVecUTabeli(string ime){
  list<Simbol*>::iterator it;
  for(it = tabelaSimbola.begin(); it != tabelaSimbola.end(); ++it){
    Simbol* simbol = (*it);
    if(simbol->dohvatiIme() == ime) return true;
  }
  return false;
}

void TabelaSimbola::ispisiTabeluSimbola(){
  cout<<"#.symtab."<<tabelaSimbola.size()<<endl;
  list<Simbol*>::iterator it;
  for(it = tabelaSimbola.begin(); it != tabelaSimbola.end(); ++it){
    Simbol* simbol = (*it);
    int redniBroj = simbol->dohvatiRedniBroj();
    int vrednost = simbol->dohvatiVrednost();
    Tip tip = simbol->dohvatiTip();
    string tipString = "";
    if(tip == LOC) tipString = "LOC";
    else if(tip == GLOB) tipString = "GLOB";
    else tipString = "EXT";
    string sekcija = simbol->dohvatiSekciju();
    string ime = simbol->dohvatiIme();
    cout<<redniBroj<<" "<<vrednost<<" "<<tipString<<" "<<sekcija<<" "<<ime<<endl;
  }
}

bool TabelaSimbola::proveriDefinisanostSimbola(){
  list<Simbol*>::iterator it;
  int i = 0;
  for(it = tabelaSimbola.begin(); it != tabelaSimbola.end(); ++it){
    if(i==0 || (*it)->dohvatiTip() == Tip::EXT) continue;
    if(!(*it)->dohvatiDefinisan()) return false;
    i++;
  }
  return true;
}

list<Simbol*> TabelaSimbola::dohvatiTabeluSimbola() {
  return this->tabelaSimbola;
}

Simbol* TabelaSimbola::dohvatiSimbol(string ime){
  list<Simbol*>::iterator it;
  for(it = tabelaSimbola.begin(); it != tabelaSimbola.end(); ++it){
    Simbol* simbol = (*it);
    if(simbol->dohvatiIme() == ime) return simbol->dohvatiSimbol();
  }
  return nullptr;
}

Simbol* TabelaSimbola::dohvatiSimbol(int indeks){
  list<Simbol*>::iterator it;
  int i = 0;
  for(it = tabelaSimbola.begin(); it != tabelaSimbola.end(); ++it){
    if(i == indeks) return (*it)->dohvatiSimbol();
    i++;
  }
  return nullptr;  
}

void TabelaSekcija::dodajSekcijuUTabelu(Sekcija* novaSekcija){
  tabelaSekcija.push_back(novaSekcija);
}

list<Sekcija*> TabelaSekcija::dohvatiTabeluSekcija(){
  return this->tabelaSekcija;
}

Sekcija* TabelaSekcija::dohvatiSekciju(string nazivSekcije){
  list<Sekcija*>::iterator it;
  for(it = tabelaSekcija.begin(); it != tabelaSekcija.end(); ++it){
    Sekcija* sekcija = (*it);
    if(sekcija->dohvatiImeSekcije() == nazivSekcije) return sekcija->dohvatiSekciju(); 
  }
  return nullptr;
}

void TabelaSekcija::ispisiTabeluSekcija(){
  list<Sekcija*>::iterator it;
  for(it = tabelaSekcija.begin(); it != tabelaSekcija.end(); ++it){
    (*it)->ispisiSekciju();
  }
}

void TabelaSekcija::ispisiRelokacioneZapiseSekcija(){
  list<Sekcija*>::iterator it;
  for(it = tabelaSekcija.begin(); it != tabelaSekcija.end(); ++it){
    cout<<endl;
    (*it)->dohvatiTabeluRelokacionihZapisa()->ispisiTabeluRelokacionihZapisa();
  }
}

int TabelaSekcija::dohvatiVelicinuTabele(){return this->tabelaSekcija.size();}

Sekcija* TabelaSekcija::dohvatiSekciju(int index){
  list<Sekcija*>::iterator it;
  int i = 0;
  for(it = tabelaSekcija.begin(); it != tabelaSekcija.end(); ++it){
    if(i == index) return (*it)->dohvatiSekciju();    
    i++;
  }
  return nullptr;
}

TabelaRelokacionihZapisa::TabelaRelokacionihZapisa(string sekcija){
  this->sekcija = sekcija;
}

list<RelokacioniZapis*> TabelaRelokacionihZapisa::dohvatiTabeluRelokacionihZapisa(){
  return tabelaRelokacionihZapisa;
}

void TabelaRelokacionihZapisa::ispisiTabeluRelokacionihZapisa() {
  cout<<"#.rela."<<sekcija<<endl;
  list<RelokacioniZapis*>::iterator it;
  for(it = tabelaRelokacionihZapisa.begin(); it != tabelaRelokacionihZapisa.end(); ++it){
    RelokacioniZapis* relZapis = (*it);
    string tip;
    string endian;
    if(relZapis->dohvatiTipZapisa() == 0) 
      tip = "R_VN16_16";
    else 
      tip = "R_VN16_PC16";
    if(relZapis->proveriEndiad())
      endian = "true";
    else 
      endian = "false";
    cout<<relZapis->dohvatiOfset()<<" "<<tip<<" "<< relZapis->dohvatiID()<<" "<< endian<<endl;
  }
}

void TabelaRelokacionihZapisa::dodajUTabeluRelokacionihZapisa(RelokacioniZapis* relokacioniZapis) {
  tabelaRelokacionihZapisa.push_back(relokacioniZapis);
}

int TabelaRelokacionihZapisa::dohvatiVeliciniTabeleRelokacionihZapisa(){
  return tabelaRelokacionihZapisa.size();
}

RelokacioniZapis* TabelaRelokacionihZapisa::dohvatiRelokacioniZapis(int indeks){
  int i = 0;
  list<RelokacioniZapis*>::iterator it;
  for(it = tabelaRelokacionihZapisa.begin(); it != tabelaRelokacionihZapisa.end(); ++it){
    if(i == indeks) return (*it);
    i++;
  }
  return nullptr;
}