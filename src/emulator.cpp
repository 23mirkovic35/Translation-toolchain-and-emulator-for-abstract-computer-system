#include "../inc/emulator.hpp"

Emulator::Emulator(string ulaz){
  reg.PC = 0;
  reg.PSW = hex2dec("0x6000");
  reg.SP = 0;
  ulazniFajl.open(ulaz);
  if(ulazniFajl.is_open() == false){
    cout<<"Greska: Nije moguce otvoriti ulazni fajl "<<ulaz<<"."<<endl;
    exit(-1);
  }
}

int Emulator::hex2dec(string hex){
  hex = hex.substr(2);
	int result = 0;
	for (int i = 0; i < hex.length(); i++) {
		if (hex[i] >= 48 && hex[i] <= 57)
		{
			result += (hex[i] - 48) * pow(16, hex.length() - i - 1);
		}
		else if (hex[i] >= 65 && hex[i] <= 70) {
			result += (hex[i] - 55) * pow(16, hex.length() - i - 1);
		}
		else if (hex[i] >= 97 && hex[i] <= 102) {
			result += (hex[i] - 87) * pow(16, hex.length() - i - 1);
		}
	}
	return result;
}

string Emulator::dec2hex(string dec){
    int num = stoi(dec);
    map<int, char> m;
    char digit = '0';
    char c = 'a';
    for (int i = 0; i <= 15; i++) {
        if (i < 10) {
            m[i] = digit++;
        }
        else {
            m[i] = c++;
        }
    }
    string res = "";
    if (!num) {
        return "0";
    }
    if (num > 0) {
        while (num) {
            res = m[num % 16] + res;
            num /= 16;
        }
    }
    else {
        u_int n = num;
        while (n) {
            res = m[n % 16] + res;
            n /= 16;
        }
        if(res.at(res.size()-5) == 'f'){
          res = res.substr(res.size()-4);
        }
    }
    for (auto & c: res) c = toupper(c);
    return res;
}
TipAdresiranja Emulator::odrediNacinAdresiranja(unsigned short addrMode){
  unsigned short tip = addrMode&0xF;
  return (TipAdresiranja)tip;
}

void Emulator::obradiUlaznuDatoteku(){
  string red;
  while(getline(ulazniFajl,red)){
    string adresaHex = red.substr(0,red.find(':'));
    int adresa = hex2dec("0x"+adresaHex);
    red = red.substr(red.find(':')+1);
    string sadrzaj="";
    bool poslednji = false;
    while(red.find(' ')!=string::npos || poslednji == false){
      if(red.find(' ')==string::npos){
        sadrzaj+=red;
        poslednji = true;
        continue;
      }
      string bajt = red.substr(0,red.find(' '));
      sadrzaj+=bajt;
      red=red.substr(red.find(' ')+1);
    }
    Ulaz ulaz(adresa, sadrzaj);
    podaciIzDatoteke.push_back(ulaz);
  }
}

void Emulator::popuniMemoriju(){
  unsigned short MAX_ADRESA;
  stringstream ss;
  ss<<hex<<"FF00";
  ss>>MAX_ADRESA;
  list<Ulaz>::iterator it;
  for(it=podaciIzDatoteke.begin(); it!=podaciIzDatoteke.end(); ++it){
    int adresa = (*it).adresa;
    string sadrzaj = (*it).sadrzaj;
    int pomeraj = 0;
    while(true){
      if(adresa+pomeraj>=MAX_ADRESA){
        cout<<"Greska: Doslo je do prekoracenja."<<endl;
        exit(-1);
      }
      if(sadrzaj.size()==0)break;
      string bajt = sadrzaj.substr(0,2);
      unsigned short vrednost;
      stringstream ss;
      ss<<hex<<bajt;
      ss>>vrednost;
      memorija[adresa+pomeraj]=vrednost;
      pomeraj++;
      sadrzaj = sadrzaj.erase(0,2);
    }
  }

}

void Emulator::izvrsiInstrukcije(){

  bool kraj = false;
  /* procitaj prva dva bajta (40 01) */
  /* rec je o word direktivi pa je little-endian format */
  reg.PC = memorija[1] * 1<<8; // 1h * 256dec = 256dec
  reg.PC+= memorija[0]; // 256dec + 40h = 256dec + 64dec = 320 <- adresa prve instrukcije
  int i =0;
  while(!kraj){
    unsigned short stariPC = reg.PC;
    unsigned short kodInstrukcije = memorija[reg.PC++];
    switch(kodInstrukcije){
      case HALT:{
        kraj = true;
        break;
      }
      case INT:{
        // push psw, pc <= mem16[(regD mod 8)*2];
        RegsDescr = memorija[reg.PC++];
        regS = RegsDescr&0xf; // nije bitan
        regD = RegsDescr>>4;
        /* push PC - jer se skace na prekidnu rutinu*/
        /* sp <= sp - 2; mem16[sp] <= regD */
        reg.SP=reg.SP-2;
        unsigned short prvi = reg.PC & 0xFF;
        unsigned short drugi = reg.PC>>8;
        memorija[reg.SP] = prvi; // manja adresa
        memorija[reg.SP+1] = drugi; // veca adresa
        /* push PSW */
        /* sp <= sp - 2; mem16[sp] <= regD */
        reg.SP = reg.SP - 2;
        prvi = reg.PSW & 0xFF;
        drugi = reg.PSW>>8;
        memorija[reg.SP] = prvi; // manja adresa
        memorija[reg.SP+1] = drugi; // veca adresa
        unsigned short r_num;
        if(regD == 6) r_num = reg.SP ;
        else if(regD == 7) r_num = reg.PC;
        else if(regD==8) r_num = reg.PSW;
        else r_num = reg.r[regD];
        /* broj ulaza u IVT za koji se generise zahtev za prekid nazali se u odredisnom registru */
        /* (r<num> % 8)*2 */
        prvi = memorija[(r_num %8 )*2];
        drugi = memorija[(r_num %8 )*2 + 1];
        reg.PC = drugi<<8;
        reg.PC += prvi;
        break;
      }
      case IRET:{
        /* pop psw; pop pc */
        /* pop psw */
        unsigned short prvi = memorija[reg.SP];
        unsigned short drugi = memorija[reg.SP+1];
        string prviHex = dec2hex(to_string(prvi));
        if(prviHex.size()<2){
          int brojNula = 2 - prviHex.size();
          for(int j = 0; j<brojNula; j++){
            prviHex = "0"+prviHex;
          }
        }
        string drugiHex = dec2hex(to_string(drugi));
        if(drugiHex.size()<2){
          int brojNula = 2 - drugiHex.size();
          for(int j = 0; j<brojNula; j++){
            drugiHex = "0"+drugiHex;
          }
        }
        string hex = drugiHex + prviHex;
        unsigned short dec = hex2dec("0x"+hex);
        short _psw = static_cast<short>(dec);
        reg.PSW = _psw; 
        reg.SP = reg.SP + 2;
        /* pop PC */
        prvi = memorija[reg.SP];
        drugi = memorija[reg.SP+1];
        prviHex = dec2hex(to_string(prvi));
        if(prviHex.size()<2){
          int brojNula = 2 - prviHex.size();
          for(int j = 0; j<brojNula; j++){
            prviHex = "0"+prviHex;
          }
        }
        drugiHex = dec2hex(to_string(drugi));
        if(drugiHex.size()<2){
          int brojNula = 2 - drugiHex.size();
          for(int j = 0; j<brojNula; j++){
            drugiHex = "0"+drugiHex;
          }
        }
        hex = drugiHex + prviHex;
        dec = hex2dec("0x"+hex);
        short _pc = static_cast<short>(dec);
        reg.PC = _pc; 
        reg.SP = reg.SP + 2;
        break;
      }
      case CALL:{
        /* push PC; PC <= operand */
        /* pre nego sto se PC stavi na stek oderedi adr skoka */
        RegsDescr = memorija[reg.PC++];
        AddrMode = memorija[reg.PC++];
        regD = RegsDescr>>4;
        regS = RegsDescr&0xf;
        if(regS > 8){
          // Greska
          // idi u prekidnu rutinu
          prekid = true;
        }
        TipAdresiranja tip = odrediNacinAdresiranja(AddrMode);
        switch(tip){
          case IMMED:{
            unsigned short prvi = memorija[reg.PC++];
            unsigned short drugi = memorija[reg.PC++];
            operand = prvi*1<<8;
            operand+= drugi;
            break;
          }
          case REG_DIR:{
            if(regS>=0 && regS<=5) operand = reg.r[regS];
            else if(regS == 6) operand = reg.SP;
            else if(regS == 7) operand = reg.PC;
            else operand = reg.PSW;
            break;
          }
          case REG_IND:{
            unsigned short r_num;
            if(regS>=0 && regS<=5) r_num = reg.r[regS];
            else if(regS == 6) r_num = reg.SP;
            else if(regS == 7) r_num = reg.PC;
            else regS = reg.PSW;
            unsigned short prvi = memorija[r_num];
            unsigned short drugi = memorija[r_num+1];
            string prviHex = dec2hex(to_string(prvi));
            if(prviHex.size()<2){
              int brojNula = 2 - prviHex.size();
              for(int j = 0; j<brojNula; j++){
                prviHex = "0"+prviHex;
              }
            }
            string drugiHex = dec2hex(to_string(drugi));
            if(drugiHex.size()<2){
              int brojNula = 2 - drugiHex.size();
              for(int j = 0; j<brojNula; j++){
                drugiHex = "0"+drugiHex;
              }
            }
            string hex = drugiHex + prviHex;
            unsigned short dec = hex2dec("0x"+hex);
            short _dec = static_cast<short>(dec);
            operand = _dec;
            break;
          }
          case REG_IND_POMERAJ:{
            unsigned short r_num;
            if(regS>=0 && regS<=5) r_num = reg.r[regS];
            else if(regS == 6) r_num = reg.SP;
            else if(regS == 7) r_num = reg.PC;
            else regS = reg.PSW;
            unsigned short prvi = memorija[reg.PC++];
            unsigned short drugi = memorija[reg.PC++];
            string prviHex = dec2hex(to_string(prvi));
            if(prviHex.size()<2){
              int brojNula = 2 - prviHex.size();
              for(int j = 0; j<brojNula; j++){
                prviHex = "0"+prviHex;
              }
            }
            string drugiHex = dec2hex(to_string(drugi));
            if(drugiHex.size()<2){
              int brojNula = 2 - drugiHex.size();
              for(int j = 0; j<brojNula; j++){
                drugiHex = "0"+drugiHex;
              }
            }
            string hex = prviHex + drugiHex;
            unsigned short dec = hex2dec("0x"+hex);
            short pomeraj = static_cast<short>(dec);
            unsigned short adresa = pomeraj + r_num;
            prvi = memorija[adresa];
            drugi = memorija[adresa+1];
            prviHex = dec2hex(to_string(prvi));
            if(prviHex.size()<2){
              int brojNula = 2 - prviHex.size();
              for(int j = 0; j<brojNula; j++){
                prviHex = "0"+prviHex;
              }
            }
            drugiHex = dec2hex(to_string(drugi));
            if(drugiHex.size()<2){
              int brojNula = 2 - drugiHex.size();
              for(int j = 0; j<brojNula; j++){
                drugiHex = "0"+drugiHex;
              }
            }
            hex = drugiHex + prviHex;
            dec = hex2dec("0x"+hex);
            operand = static_cast<short>(dec);
            break;
          }
          case MEM:{
            unsigned short prvi = memorija[reg.PC++];
            string prviHex = dec2hex(to_string(prvi));
            if(prviHex.size()<2){
              int brojNula = 2 - prviHex.size();
              for(int j = 0; j<brojNula; j++){
                prviHex = "0" + prviHex;
              }
            }
            unsigned short drugi = memorija[reg.PC++];
            string drugiHex = dec2hex(to_string(drugi));
            if(drugiHex.size()<2){
              int brojNula = 2 - drugiHex.size();
              for(int j = 0; j<brojNula; j++){
                drugiHex = "0" + drugiHex;
              }
            }
            string hex = prviHex + drugiHex;
            unsigned short adresa = hex2dec("0x"+hex);
            prvi = memorija[adresa];
            drugi = memorija[adresa+1];
            prviHex = dec2hex(to_string(prvi));
            if(prviHex.size()<2){
              int brojNula = 2 - prviHex.size();
              for(int j = 0; j<brojNula; j++){
                prviHex = "0"+prviHex;
              }
            }
            drugiHex = dec2hex(to_string(drugi));
            if(drugiHex.size()<2){
              int brojNula = 2 - drugiHex.size();
              for(int j = 0; j<brojNula; j++){
                drugiHex = "0"+drugiHex;
              }
            }
            hex = drugiHex + prviHex;
            unsigned short dec = hex2dec("0x"+hex);
            operand = static_cast<short>(dec);
            break;
          }
          case PC_REL: {
            /* procitaj data deo instrukcije */
            unsigned short prvi = memorija[reg.PC++];
            string prviHex = dec2hex(to_string(prvi));
            if(prviHex.size()<2){
              int brojNula = 2 - prviHex.size();
              for(int j = 0; j<brojNula; j++){
                prviHex = "0" + prviHex;
              }
            }
            unsigned short drugi = memorija[reg.PC++];
            string drugiHex = dec2hex(to_string(drugi));
            if(drugiHex.size()<2){
              int brojNula = 2 - drugiHex.size();
              for(int j = 0; j<brojNula; j++){
                drugiHex = "0" + drugiHex;
              }
            }
            string hex = prviHex+drugiHex;
            unsigned short dec = hex2dec("0x"+hex);
            short pomeraj = static_cast<short>(dec);
            operand = reg.PC + pomeraj;
            break;
          }
        }
        /* push pc */
        reg.SP = reg.SP - 2;
        memorija[reg.SP] = reg.PC&0xFF;
        memorija[reg.SP+1] = reg.PC>>8;
        reg.PC = operand;
        break;
      }
      case RET:{
        /* pop pc */
        unsigned short prvi = memorija[reg.SP];
        unsigned short drugi = memorija[reg.SP+1];
        string prviHex = dec2hex(to_string(prvi));
        if(prviHex.size()<2){
          int brojNula = 2 - prviHex.size();
          for(int j = 0; j<brojNula; j++){
            prviHex = "0"+prviHex;
          }
        }
        string drugiHex = dec2hex(to_string(drugi));
        if(drugiHex.size()<2){
          int brojNula = 2 - drugiHex.size();
          for(int j = 0; j<brojNula; j++){
            drugiHex = "0"+drugiHex;
          }
        }
        string hex = drugiHex + prviHex;
        unsigned short dec = hex2dec("0x"+hex);
        reg.PC = dec;
        reg.SP = reg.SP + 2;
        break;
      }
      case JMP:{
        RegsDescr = memorija[reg.PC++];
        regS = RegsDescr&0xf;
        regD = RegsDescr>>4;
        AddrMode = memorija[reg.PC++];
        TipAdresiranja tipAdresiranja = odrediNacinAdresiranja(AddrMode);
        switch(tipAdresiranja){
          case IMMED:{
            unsigned short prvi = memorija[reg.PC++];
            unsigned short drugi = memorija[reg.PC++];
            operand = prvi*1<<8;
            operand+= drugi;
            reg.PC = operand;
            break;
          }
        }
        break;
      }
      case POP_LDR:{
        /* u zavisnoti od bajta AddrMode videce se da li je POP ili LDR */
        /* kod LDR prvi heksa broj ce biti 0, dok kod POP nece */
        RegsDescr = memorija[reg.PC++];
        regS = RegsDescr&0xf;
        regD = RegsDescr>>4;
        AddrMode = memorija[reg.PC++];
        TipAdresiranja tipAdresiranja = odrediNacinAdresiranja(AddrMode);
        switch(tipAdresiranja){
          case IMMED:{
            unsigned short prvi = memorija[reg.PC++];
            string prviHex = dec2hex(to_string(prvi));
            unsigned short drugi = memorija[reg.PC++];
            string drugiHex = dec2hex(to_string(drugi));
            string hex = prviHex + drugiHex;
            unsigned short dec = hex2dec("0x"+hex);
            operand = static_cast<short>(dec);
            if(regD == 0 || regD == 1 ||regD == 2 ||regD == 3 ||regD == 4 || regD == 5){
              reg.r[regD] = operand;
            }
            else if(regD == 6) reg.SP = operand;
            else if(regD == 7) reg.PC = operand;
            else reg.PSW = operand;
            break;
          }
          case REG_DIR:{
            break;
          }
          case REG_IND:{
            unsigned short u = AddrMode>>4;
            if(u!=0 && u!=4){
              //greska
              continue;
            }
            else if(u==4){
              // POP
              // regD <= mem16[SP]; SP <= SP + 2;
              unsigned short prvi = memorija[reg.SP];
              unsigned short drugi = memorija[reg.SP+1];
              string prviHex = dec2hex(to_string(prvi));
              string drugiHex = dec2hex(to_string(drugi));
              string hex = drugiHex + prviHex;
              unsigned short dec = hex2dec("0x"+hex);
              short vrednost = static_cast<short>(dec);
              if(regD>=0 && regD<=5) reg.r[regD] = vrednost;
              else if(regD == 6) reg.SP = vrednost;
              else if(regD == 7) reg.PC = vrednost;
              else reg.PSW = vrednost;
              reg.SP = reg.SP + 2;
            }
            else {
              unsigned short indeks;
              if(regS>=0 && regS<=5) indeks = reg.r[regS];
              else if(regS == 6) indeks = reg.SP;
              else if(regS == 7) indeks = reg.PC;
              else indeks = reg.PSW;
              unsigned short prvi = memorija[indeks];
              unsigned short drugi = memorija[indeks+1];
              string prviHex = dec2hex(to_string(prvi));
              string drugiHex = dec2hex(to_string(drugi));
              prviHex = dec2hex(to_string(prvi));
              if(prviHex.size()<2){
                int brojNula = 2 - prviHex.size();
                for(int j = 0; j<brojNula; j++){
                  prviHex = "0"+prviHex;
                }
              }
              drugiHex = dec2hex(to_string(drugi));
              if(drugiHex.size()<2){
                int brojNula = 2 - drugiHex.size();
                for(int j = 0; j<brojNula; j++){
                  drugiHex = "0"+drugiHex;
                }
              }
              string hex = drugiHex+prviHex;
              unsigned short dec = hex2dec("0x"+hex);
              unsigned short r_num = dec;
              if(regD>=0 && regD<=5) reg.r[regD]=r_num;
              else if(regD==6) reg.SP = r_num;
              else if(regD == 7) reg.PC=r_num;
              else reg.PSW=r_num;
            }
            break;
          }
          case REG_IND_POMERAJ:{
            /* uzmi vrednost registra i saberi ga sa pomerajem koji se nalazi u data delu instrukcije */
            regS = RegsDescr&0xF;
            unsigned short r_num;
            if(regS>=0 && regS<=5) r_num = reg.r[regS];
            else if(regS == 6) r_num = reg.SP;
            else if(regS == 7) r_num = reg.PC;
            else regS = reg.PSW;
            /* dohvati pomeraj */
            unsigned short prvi = memorija[reg.PC++];
            unsigned short drugi = memorija[reg.PC++];
            string prviHex = dec2hex(to_string(prvi));
            string drugiHex = dec2hex(to_string(drugi));
            string hex = prviHex + drugiHex;
            unsigned short dec = hex2dec("0x"+hex);
            short pomeraj = static_cast<short>(dec);
            unsigned short adresa = r_num + pomeraj;
            prvi = memorija[adresa];
            drugi = memorija[adresa+1];
            prviHex = dec2hex(to_string(prvi));
            if(prviHex.size()<2){
              int brojNula = 2 - prviHex.size();
              for(int j = 0; j<brojNula; j++){
                prviHex = "0"+prviHex;
              }
            }
            drugiHex = dec2hex(to_string(drugi));
            if(drugiHex.size()<2){
              int brojNula = 2 - drugiHex.size();
              for(int j = 0; j<brojNula; j++){
                drugiHex = "0"+drugiHex;
              }
            }
            hex = drugiHex + prviHex;
            dec = hex2dec("0x"+hex);
            operand = static_cast<short>(dec);
            if(regD>=0 && regD<=5) reg.r[regD] = operand;
            else if(regD == 6) reg.SP= operand;
            else if(regD == 7) reg.PC= operand;
            else reg.PSW = operand;
            break;
          }
          case MEM:{
            unsigned short prvi = memorija[reg.PC++];
            unsigned short drugi = memorija[reg.PC++];
            string prviHex = dec2hex(to_string(prvi));
            string drugiHex = dec2hex(to_string(drugi));
            prviHex = dec2hex(to_string(prvi));
            if(prviHex.size()<2){
              int brojNula = 2 - prviHex.size();
              for(int j = 0; j<brojNula; j++){
                prviHex = "0"+prviHex;
              }
            }
            drugiHex = dec2hex(to_string(drugi));
            if(drugiHex.size()<2){
              int brojNula = 2 - drugiHex.size();
              for(int j = 0; j<brojNula; j++){
                drugiHex = "0"+drugiHex;
              }
            }
            string hex = prviHex + drugiHex;
            unsigned short dec = hex2dec("0x"+hex);
            prvi = memorija[dec];
            drugi = memorija[dec+1];
            prviHex = dec2hex(to_string(prvi));
            if(prviHex.size()<2){
              int brojNula = 2 - prviHex.size();
              for(int j = 0; j<brojNula; j++){
                prviHex = "0"+prviHex;
              }
            }
            drugiHex = dec2hex(to_string(drugi));
            if(drugiHex.size()<2){
              int brojNula = 2 - drugiHex.size();
              for(int j = 0; j<brojNula; j++){
                drugiHex = "0"+drugiHex;
              }
            }
            hex = drugiHex + prviHex;
            dec = hex2dec("0x"+hex);
            unsigned short r_num = dec;
            if(regD>=0 && regD<=5) reg.r[regD] = r_num;
            else if(regD == 6) reg.SP= r_num;
            else if(regD == 7) reg.PC= r_num;
            else reg.PSW = r_num;
            break;
          }
          case PC_REL:{
            break;
          }
        }
        break;
      }
      case PUSH_STR:{
        RegsDescr = memorija[reg.PC++];
        regS = RegsDescr&0xf;
        regD = RegsDescr>>4;
        AddrMode = memorija[reg.PC++];
        TipAdresiranja tip = odrediNacinAdresiranja(AddrMode);
        switch(tip){
          case IMMED:{
            // greska
            // skok na prekidnu rutinu
            // ne moze neposredno adresiranje i str instrukcija
            break;
          }
          case REG_DIR:{
            break;
          }
          case REG_IND:{
            unsigned short u = AddrMode>>4;
            if(u >= 2){ // u != 0 and u != 1
              // GRESKA
              continue;
            }
            else if(u == 1){
              // PUSH
              reg.SP = reg.SP - 2;
              unsigned short r_num;
              if(regD == 0 || regD == 1 ||regD == 2 ||regD == 3 ||regD == 4 ||regD == 5){
                r_num = reg.r[regD];
              }
              else if(regD == 6) r_num = reg.SP;
              else if(regD == 7) r_num = reg.PC;
              else r_num = reg.PSW;
              unsigned short indeks = reg.SP;
              memorija[indeks] = r_num&0xFF; // nizi bajt
              memorija[indeks + 1] = r_num>>8; // visi bajt
            }
            else {
              // STR
              unsigned short r_num;
              if(regD == 0 || regD == 1 ||regD == 2 ||regD == 3 ||regD == 4 ||regD == 5){
                r_num = reg.r[regD];
              }
              else if(regD == 6) r_num = reg.SP;
              else if(regD == 7) r_num = reg.PC;
              else r_num = reg.PSW;
              unsigned short indeks;
              if(regS == 0 || regS == 1 ||regS == 2 ||regS == 3 ||regS == 4 ||regS == 5){
                indeks = reg.r[regS];
              }
              else if(regS == 6) indeks = reg.SP;
              else if(regS == 7) indeks = reg.PC;
              else indeks = reg.PSW;
              memorija[indeks] = r_num&0xFF; // nizi bajt
              memorija[indeks + 1] = r_num>>8; // visi bajt
            }
            break;
          }
          case REG_IND_POMERAJ:{
            break;
          }
          case MEM:{
            unsigned short prvi = memorija[reg.PC++];
            unsigned short drugi = memorija[reg.PC++];
            string prviHex = dec2hex(to_string(prvi));
            string drugiHex = dec2hex(to_string(drugi));
            if(prviHex.size()<2){
              int brojNula = 2 - prviHex.size();
              for(int j = 0; j<brojNula; j++){
                prviHex = "0"+prviHex;
              }
            }
            drugiHex = dec2hex(to_string(drugi));
            if(drugiHex.size()<2){
              int brojNula = 2 - drugiHex.size();
              for(int j = 0; j<brojNula; j++){
                drugiHex = "0"+drugiHex;
              }
            }
            string hex = prviHex + drugiHex;
            unsigned short dec = hex2dec("0x"+hex);
            operand = static_cast<short>(dec);
            unsigned short r_num;
            if(regD>=0 && regD<=5){
              r_num = reg.r[regD];
            }
            else if(regD == 6) r_num = reg.SP;
            else if(regD == 7) r_num = reg.PC;
            else r_num = reg.PSW;
            memorija[operand] = r_num&0xFF; // nizi bajt
            memorija[operand + 1] = r_num>>8; // visi bajt
            break;
          }
          case PC_REL:{
            break;
          }
        }
        break;
      }
      case ADD:{
        RegsDescr = memorija[reg.PC++];
        regD = RegsDescr>>4;
        if(regD > 8){
          // Greska
          //skok na prekidnurutinu
        }
        regS = RegsDescr&0xF;
        if(regS>0){
          // Greska
          //skok na prekidnurutinu
        }
        reg.r[regD] = reg.r[regD] + reg.r[regS];
        break;
      }
      case SUB:{
        RegsDescr = memorija[reg.PC++];
        regD = RegsDescr>>4;
        if(regD > 8){
          // Greska
          //skok na prekidnurutinu
        }
        regS = RegsDescr&0xF;
        if(regS>0){
          // Greska
          //skok na prekidnurutinu
        }
        reg.r[regD] = reg.r[regD] - reg.r[regS];
        break;
      }
      case MUL:{
        RegsDescr = memorija[reg.PC++];
        regD = RegsDescr>>4;
        if(regD > 8){
          // Greska
          //skok na prekidnurutinu
        }
        regS = RegsDescr&0xF;
        if(regS>0){
          // Greska
          //skok na prekidnurutinu
        }
        reg.r[regD] = reg.r[regD] * reg.r[regS];
        break;
      }
      case DIV:{
        RegsDescr = memorija[reg.PC++];
        regD = RegsDescr>>4;
        if(regD > 8){
          // Greska
          //skok na prekidnurutinu
        }
        regS = RegsDescr&0xF;
        if(regS>0){
          // Greska
          //skok na prekidnurutinu
        }
        reg.r[regD] = reg.r[regD] / reg.r[regS];
        break;
      }
    }
    if(kraj){
      ispisiResenje();
    }
  }
}

void Emulator::ispisiResenje(){
  cout<<"------------------------------------------------\nEmulated processor executed halt instruction\nEmulated processor state: psw=0b";
  bitset<16> biti(reg.PSW);
  cout<<biti<<endl;
  int brojNula;
  string r0 = dec2hex(to_string(reg.r[0]));
  brojNula = 4 - r0.size();
  for(int i=0; i<brojNula; i++){
    r0="0"+r0;
  }
  r0="r0=0x"+r0;
  string r1 = dec2hex(to_string(reg.r[1]));
  brojNula = 4 - r1.size();
  for(int i=0; i<brojNula; i++){
    r1="0"+r1;
  }
  r1="r1=0x"+r1;
  string r2 = dec2hex(to_string(reg.r[2]));
  brojNula = 4 - r2.size();
  for(int i=0; i<brojNula; i++){
    r2="0"+r2;
  }
  r2="r2=0x"+r2;
  string r3 = dec2hex(to_string(reg.r[3]));
  brojNula = 4 - r3.size();
  for(int i=0; i<brojNula; i++){
    r3="0"+r3;
  }
  r3="r3=0x"+r3;
  string r4 = dec2hex(to_string(reg.r[4]));
  brojNula = 4 - r4.size();
  for(int i=0; i<brojNula; i++){
    r4="0"+r4;
  }
  r4="r4=0x"+r4;
  string r5 = dec2hex(to_string(reg.r[5]));
  brojNula = 4 - r5.size();
  for(int i=0; i<brojNula; i++){
    r5="0"+r5;
  }
  r5="r5=0x"+r5;
  string r6 = dec2hex(to_string(reg.SP));
  brojNula = 4 - r6.size();
  for(int i=0; i<brojNula; i++){
    r6="0"+r6;
  }
  r6="r6=0x"+r6;
  string r7 = dec2hex(to_string(reg.PC));
  brojNula = 4 - r7.size();
  for(int i=0; i<brojNula; i++){
    r7="0"+r7;
  }
  r7="r7=0x"+r7;
  cout<<r0<<"   "<<r1<<"   "<<r2<<"   "<<r3<<endl;
  cout<<r4<<"   "<<r5<<"   "<<r6<<"   "<<r7<<endl;
}

void Emulator::pokreni(){
  obradiUlaznuDatoteku();
  popuniMemoriju();
  izvrsiInstrukcije();
}