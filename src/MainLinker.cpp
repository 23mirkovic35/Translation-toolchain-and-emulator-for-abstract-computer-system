#include "../inc/linker.hpp"
using namespace std;

bool proveriFormatUlaznihFajlova(list<string> ulazniFajlovi){
  for(list<string>::iterator it = ulazniFajlovi.begin(); it != ulazniFajlovi.end(); ++it){
    string fajl = (*it);
    string tip = fajl.substr(fajl.size()-2);
    if(tip != ".o") return false;
  }
  return true;
}

bool proveriFormatIzlaznogFajla(string izlazniFajl){
  string tip = izlazniFajl.substr(izlazniFajl.size()-4);
  if(tip!=".hex") return false;
  return true;
}

int main(int argc, char* argv[]) {
  list<string> listaUlaznihFajlova;
  string izlazniFajl = argv[3];
  if(!proveriFormatIzlaznogFajla(izlazniFajl)){
    cout<<"Greska: Izlazni fajl mora biti .hex!"<<endl;
    return -1;
  }
  for(int i = 4; i<argc; i++){
    listaUlaznihFajlova.push_back(argv[i]);
  }
  if(!proveriFormatUlaznihFajlova(listaUlaznihFajlova)){
    cout<<"Greska: Ulazni fajlovi linkera moraju biti objektni."<<endl;
    return -1;
  }
  Linker linker(izlazniFajl, listaUlaznihFajlova);
  linker.pokreni();
  return 0;
}