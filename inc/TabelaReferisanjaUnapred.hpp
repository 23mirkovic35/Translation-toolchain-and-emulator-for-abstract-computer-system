#ifndef TABELA_REFERISANJA_UNAPRED_HPP
#define TABELA_REFERISANJA_UNAPRED_HPP

#include "NedefinishanSimbol.hpp"
#include <list>

class TabelaReferisanjaUnapred{

private:
  list<NedefinisanSimbol*> tabelaReferisanjaUnapred;
public:
  list<NedefinisanSimbol*> dohvatiTabelu() {return this->tabelaReferisanjaUnapred;}
  void dodajUTabeluReferisanjaUnapred(NedefinisanSimbol* simbol){ this->tabelaReferisanjaUnapred.push_back(simbol); }
  int velicinaTabeleReferisanjaUnapred() {return this->tabelaReferisanjaUnapred.size();}
  NedefinisanSimbol* dohvatiElementTabele(int index) {
    list<NedefinisanSimbol*>::iterator it;
    int i = 0;
    for(it = tabelaReferisanjaUnapred.begin(); it != tabelaReferisanjaUnapred.end(); ++it){
      if(i == index) return (*it);
      i++;
    }
    return nullptr;
  }
  void ispisiTabeluReferisanjaUnapred() {
    cout<<"------------"<<endl;
    list<NedefinisanSimbol*>::iterator it;
    for(it = tabelaReferisanjaUnapred.begin(); it != tabelaReferisanjaUnapred.end(); ++it){
      (*it)->ispisiNedefinisanSimbol();
    }
  }
  bool simbolUTabeliReferisanjaUnapred(int id){
    list<NedefinisanSimbol*>::iterator it;
    for(it = tabelaReferisanjaUnapred.begin(); it != tabelaReferisanjaUnapred.end(); ++it){
      if((*it)->dohvatiID() == id) return true;
    }
    return false;
  }
};

#endif
