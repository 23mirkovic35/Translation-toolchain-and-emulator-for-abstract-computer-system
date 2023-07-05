#include "../inc/asembler.hpp"


bool proveriTipFajla(std::string fajl, bool tip){
  /*
    tip == false -> ulazni fajl
    tip == true -> izlazni fajl
  */
  if(tip){
    /* provera da li je ekstenzija fajla .o */
    if((fajl.substr( fajl.length() - 2 )) == ".o"){
      return true;
    }
    return false;
  }
  /* provera da li je ekstenzija fajla .s */
  if((fajl.substr( fajl.length() - 2 )) == ".s"){
    return true;
  }
  return false;
}

int main(int argc, char* argv[]) {

  if(argc < 4) {
    std::cout << "Nedovoljno argumenata za pokretanje asemblera!" << std::endl;
    std::cout<< "Primer komande: ./assembler -o izlazni_fajl.o ulazni_fajl.s"<<std::endl; 
    return -1;
  }
  
  std::string izlazniFajl = argv[2];
  std::string ulazniFajl = argv[3];
  
  if(!proveriTipFajla(izlazniFajl, true)) {
    std::cout << "Pokretanje asemblera nije uspelo zbog loseg formata izlaznog fajla!" << std::endl;
    return -1;
  }

  if(!proveriTipFajla(ulazniFajl, false)) {
    std::cout << "Pokretanje asemblera nije uspelo zbog loseg formata ulaznog fajla!" << std::endl;
    return -1;
  }
  Asembler asembler(ulazniFajl, izlazniFajl);
  asembler.pokreni();
  
  return 0;
}