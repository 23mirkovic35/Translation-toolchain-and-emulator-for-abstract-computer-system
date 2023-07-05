#include "../inc/linker.hpp"

Linker::Linker(string izlaz, list<string> ulazi){
  izlazniFajl.open(izlaz);
  if(!izlazniFajl.is_open()){
    cout<<"Greska: Nije moguce otvoriti izlazni fajl."<<endl;
    exit(-1);
  }
  listaUlaznihFajlova = ulazi;
}

map<int, char> Linker::dec_value_of_hex(){
      map<int, char> m{ { 0, '0' }, { 1, '1' },
                      { 2, '2' }, { 3, '3' },
                      { 4, '4' }, { 5, '5' },
                      { 6, '6' }, { 7, '7' },
                      { 8, '8' }, { 9, '9' },
                      { 10, 'A' }, { 11, 'B' },
                      { 12, 'C' }, { 13, 'D' },
                      { 14, 'E' }, { 15, 'F' } };
 
    return m;
}

map<char, int> Linker::hex_value_of_dec()
{
    // Map the values to decimal values
    map<char, int> m{ { '0', 0 }, { '1', 1 },
                      { '2', 2 }, { '3', 3 },
                      { '4', 4 }, { '5', 5 },
                      { '6', 6 }, { '7', 7 },
                      { '8', 8 }, { '9', 9 },
                      { 'A', 10 }, { 'B', 11 },
                      { 'C', 12 }, { 'D', 13 },
                      { 'E', 14 }, { 'F', 15 } };
 
    return m;
}

string Linker::Add_Hex(string a, string b)
{
    map<char, int> m = hex_value_of_dec();
    map<int, char> k = dec_value_of_hex();
 
    // Check if length of string first is
    // greater than or equal to string second
    if (a.length() < b.length())
        swap(a, b);
 
    // Store length of both strings
    int l1 = a.length(), l2 = b.length();
 
    string ans = "";
 
    // Initialize carry as zero
    int carry = 0, i, j;
 
    // Traverse till second string
    // get traversal completely
    for (i = l1 - 1, j = l2 - 1;
         j >= 0; i--, j--) {
 
        // Decimal value of element at a[i]
        // Decimal value of element at b[i]
        int sum = m[a[i]] + m[b[j]] + carry;
 
        // Hexadecimal value of sum%16
        // to get addition bit
        int addition_bit = k[sum % 16];
 
        // Add addition_bit to answer
        ans.push_back(addition_bit);
 
        // Update carry
        carry = sum / 16;
    }
 
    // Traverse remaining element
    // of string a
    while (i >= 0) {
 
        // Decimal value of element
        // at a[i]
        int sum = m[a[i]] + carry;
 
        // Hexadecimal value of sum%16
        // to get addition bit
        int addition_bit = k[sum % 16];
 
        // Add addition_bit to answer
        ans.push_back(addition_bit);
 
        // Update carry
        carry = sum / 16;
        i--;
    }
 
    // Check if still carry remains
    if (carry) {
        ans.push_back(k[carry]);
    }
 
    // Reverse the final string
    // for desired output
    reverse(ans.begin(), ans.end());
 
    if(ans.size()>4){
      ans = ans.substr(1);
    }

    // Return the answer
    return ans;
}

int Linker::hex2dec(string hex){
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

string Linker::dec2hex(string dec){
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

bool Linker::sekcijaVecUTabeliSekcija(string imeSekcije, int rbFajla){
  list<SekcijaLinker*>::iterator it;
  for(it = tabelaSekcija.begin(); it != tabelaSekcija.end(); ++it){
    SekcijaLinker* sekcija = (*it);
    if(sekcija->dohvatiImeSekcije() == imeSekcije && sekcija->dohvatiRedniBrojFajla()==rbFajla) return true;
  }
  return false;
}

void Linker::dodajSekcijeSaIstimImenom(string imeSekcije, string imeFajla){
  list<string>::iterator it;
  int rbFajl = 0;
  for(it = listaUlaznihFajlova.begin(); it != listaUlaznihFajlova.end(); ++it){
    if((*it) == imeFajla) {
      rbFajl++;
      continue; 
    }
    ifstream fajl;
    fajl.open((*it));
    if(!fajl.is_open()){
      cout<<"Greska: Ulazni fajl " + (*it) + " nije moguce otvoriti."<<endl;
      exit(-1);
    }
    string red;
    getline(fajl, red); 
    red = red.erase(0,9); // obrisi '#.symtab.'
    red = "0x"+red;
    int brojSimbolaUTabeli = hex2dec(red);
    bool kraj = false;
    int brojac = 0;
    while(!kraj){
      while(brojac!=brojSimbolaUTabeli+1){
        getline(fajl, red);
        brojac++;
      }
      while(getline(fajl, red)){
        if(red=="") break;
        if(red.find('#')!=string::npos){
          red = red.erase(0,1);
          int tacka = red.find('.');
          string ime = red.substr(0,tacka);
          if(ime == imeSekcije) {
            string velicinaHex = "0x"+red.substr(tacka+1);
            int velicina = hex2dec(velicinaHex);
            getline(fajl, red);
            if(!sekcijaVecUTabeliSekcija(ime, rbFajl)){
              SekcijaLinker* novaSekcija = new SekcijaLinker(rbFajl,ime,pozicioniBrojac,velicina);
              tabelaSekcija.push_back(novaSekcija);
              pozicioniBrojac+=velicina;
              this->sadrzaj+=red;
            }
          }
        }
      }
      kraj = true;
    }
    rbFajl++;
  }
}

void Linker::dodajSimbolUTabelu(SimbolLinker* simbol){
  if(simbol->dohvatiTip() == Tip::GLOB){
    if(simbolVecUTabeli(simbol->dohvatiIme())){
      cout<<"Greska: Visestruka definicija simbola "<<simbol->dohvatiIme()<<endl;
      exit(-1);
    }
    tabelaSimbola.push_back(simbol);
    simbol->postaviRedniBroj(tabelaSimbola.size()-1);
  }
  else if(simbol->dohvatiTip() == Tip::EXT){
    eksterniSimboli.push_back(simbol->dohvatiIme());
  }
}

bool Linker::simbolVecUTabeli(string imeSimbola){
  list<SimbolLinker*>::iterator it;
  for(it = tabelaSimbola.begin(); it != tabelaSimbola.end(); ++it){
    if((*it)->dohvatiIme() == imeSimbola) return true;
  }
  return false;
}

SimbolLinker* Linker::dohvatiSimbolIzTabeleSimbola(string imeSimbola){
  list<SimbolLinker*>::iterator it;
  for(it = tabelaSimbola.begin(); it != tabelaSimbola.end(); ++it){
    if((*it)->dohvatiIme() == imeSimbola) return (*it);
  }
  return nullptr;
}

bool Linker::sviEksterniSimboliDefinisani(){
  list<string>::iterator it;
  for(it=eksterniSimboli.begin(); it!=eksterniSimboli.end(); ++it){
    string ime = (*it);
    if(!simbolVecUTabeli(ime)){
      cout<<"Greska: Simbol " +ime+" nije definisan"<<endl; 
      return false;
    }
  }
  return true;
}

void Linker::raspodeliSekcije(){
  list<string>::iterator it;
  int rbFajla = 0;
  for(it = listaUlaznihFajlova.begin(); it != listaUlaznihFajlova.end(); ++it){
    ifstream ulazniFajl;
    ulazniFajl.open((*it));
    if(!ulazniFajl.is_open()){
      cout<<"Greska: Ulazni fajl " + (*it) + " nije moguce otvoriti."<<endl;
      exit(-1);
    }
    string red;
    getline(ulazniFajl, red); 
    red = red.erase(0,9); // obrisi '#.symtab.'
    red = "0x"+red;
    int brojSimbolaUTabeli = hex2dec(red);
    bool kraj = false;
    int brojac = 0;
    while(!kraj){
      while(brojac!=brojSimbolaUTabeli+1){
        getline(ulazniFajl, red);
        brojac++;
      }
      /* raspodela sekcija */
      while(getline(ulazniFajl, red)){
        if(red=="") break;
        if(red.find('#')!=string::npos){
          red = red.erase(0,1);
          int tacka = red.find('.');
          string imeSekcije = red.substr(0,tacka);
          string velicinaHex = "0x"+red.substr(tacka+1);
          int velicina = hex2dec(velicinaHex);
          getline(ulazniFajl, red);
          if(!sekcijaVecUTabeliSekcija(imeSekcije, rbFajla)){
            SekcijaLinker* novaSekcija = new SekcijaLinker(rbFajla,imeSekcije,pozicioniBrojac,velicina);
            tabelaSekcija.push_back(novaSekcija);
            pozicioniBrojac+=velicina;
            this->sadrzaj+=red;
            /* proveri da li se u ostalim fajlovima nalazi sekcija sa istim imenom, ako ima dodaj je odma ispod */
            dodajSekcijeSaIstimImenom(imeSekcije, (*it));
          }
        }
      }
      kraj = true;
    }
    rbFajla++;
  }
}

void Linker::popuniTabeluSimbola(){
  /* dodaj sekcije iz tabele sekcija */
  list<SekcijaLinker*>::iterator it_sekcija;
  int rbSimbola = 0;
  for(it_sekcija = tabelaSekcija.begin(); it_sekcija != tabelaSekcija.end(); ++it_sekcija){
    SekcijaLinker* sekcija = (*it_sekcija);
    if(simbolVecUTabeli(sekcija->dohvatiImeSekcije())){
      SimbolLinker* simbol = dohvatiSimbolIzTabeleSimbola(sekcija->dohvatiImeSekcije());
      int velicina = simbol->dohvatiVelicinu() + sekcija->dohvatiVelicinu();
      simbol->postaviVelicinu(velicina);
    }
    else {
      SimbolLinker* noviSimbol = new SimbolLinker(-1, sekcija->dohvatiImeSekcije(), sekcija->dohvatiAdresuPocetka(), Tip::LOC, sekcija->dohvatiImeSekcije());
      tabelaSimbola.push_back(noviSimbol);
      noviSimbol->postaviRedniBroj(rbSimbola++);
      noviSimbol->postaviVelicinu(sekcija->dohvatiVelicinu());
    }
  }
  /* dodaj globalne simbole iz ulaznih fajlova */
  list<string>::iterator it;
  int rbFajl = 0;
  for(it = listaUlaznihFajlova.begin(); it != listaUlaznihFajlova.end(); ++it){
    ifstream ulazniFajl;
    ulazniFajl.open((*it));
    if(!ulazniFajl.is_open()){
      cout<<"Greska: Ulazni fajl " + (*it) + " nije moguce otvoriti."<<endl;
      exit(-1);
    }
    string red;
    for(int i = 0; i<2; i++){
      getline(ulazniFajl, red); 
    }
    while(getline(ulazniFajl, red)){
      if(red=="") break;
      int blankoZnak = red.find(' ');
      red = red.substr(blankoZnak+1);
      string vrednostHex = red.substr(0, red.find(' '));
      int vrednost = hex2dec(vrednostHex);
      red = red.substr(red.find(' ')+1);
      string tipStr = red.substr(0, red.find(' '));
      Tip tip;
      if(tipStr == "LOC") tip = Tip::LOC;
      else if(tipStr == "GLOB") tip = Tip::GLOB;
      else tip = Tip::EXT;
      red = red.substr(red.find(' ')+1);
      string sekcija = red.substr(0, red.find(' ')); 
      red = red.substr(red.find(' ')+1);
      string ime = red;
      SimbolLinker* noviSimbol = new SimbolLinker(rbFajl, ime, vrednost, tip, sekcija);
      dodajSimbolUTabelu(noviSimbol);
    }
    rbFajl++;
  }
  if(!sviEksterniSimboliDefinisani()) exit(-1);
}

SekcijaLinker* Linker::dovhvatiSekciju(string imeSekcije, int rbFajla){
  list<SekcijaLinker*>::iterator it;
  for(it=tabelaSekcija.begin(); it!=tabelaSekcija.end(); ++it){
    SekcijaLinker* sekcija = (*it);
    if(sekcija->dohvatiImeSekcije() == imeSekcije && sekcija->dohvatiRedniBrojFajla() == rbFajla) return sekcija;
  }
  return nullptr;
}

void Linker::azurirajTabeluSimbola(){
  list<SimbolLinker*>::iterator it;
  for(it=tabelaSimbola.begin(); it!=tabelaSimbola.end(); ++it){
    SimbolLinker* simbol = (*it);
    if(simbol->dohvatiTip() == Tip::LOC) continue;
    SekcijaLinker* sekcija = dovhvatiSekciju(simbol->dohvatiSekciju(), simbol->dohvatiRedniBrojFajla());
    int pocetakSekcije = sekcija->dohvatiAdresuPocetka();
    int staraVrednost = simbol->dohvatiVrednost();
    int novaVrednost = staraVrednost + pocetakSekcije;
    simbol->postaviVrednost(novaVrednost);
  }
}

void Linker::azurirajSadzraj(){
  int rbFajla = 0;
  list<string>::iterator it_ulazniFajlovi;
  for(it_ulazniFajlovi = listaUlaznihFajlova.begin(); it_ulazniFajlovi!=listaUlaznihFajlova.end(); ++it_ulazniFajlovi){
    int prazneLinije = 0;
    string red;
    ifstream ulazniFajl;
    ulazniFajl.open((*it_ulazniFajlovi));
    while(getline(ulazniFajl,red)){
      if(red=="") prazneLinije++;
      if(prazneLinije==2) break;
    }
    /* doslo se do dela sa relokacionim zapisima */
    bool kraj = false;
    while(!kraj){
      getline(ulazniFajl,red);
      if(red=="") kraj = true;
      if(red.find('#')!=string::npos){
        red = red.erase(0,7); // obrisi  '#.rela.'      
        string imeSekcije = red.substr(0,red.find('.'));
        string brojZapisaHex = "0x"+red.substr(red.find('.')+1);
        int brojZapisa = hex2dec(brojZapisaHex);
        if(brojZapisa == 0) continue;
        int brojac = 0;
        int idZapisa = 0;
        while(brojac!=brojZapisa){
          getline(ulazniFajl,red);
          int pomeraj = hex2dec(red.substr(0,red.find(' ')));
          red = red.substr(red.find(' ')+1);
          string tipStr = red.substr(0,red.find(' '));
          TipZapisa tip;
          if(tipStr == "R_VN16_16") tip = TipZapisa::R_VN16_16;
          else tip = TipZapisa::R_VN16_PC16;
          red = red.substr(red.find(' ')+1);
          int simbolID = stoi(red.substr(0,red.find(' ')));
          red = red.substr(red.find(' ')+1);
          string imeSimbola = red.substr(0,red.find(' '));
          imeSimbola = imeSimbola.erase(0,1);
          imeSimbola = imeSimbola.erase(imeSimbola.size()-1,1);
          string littleEndianStr = red.substr(red.find(' ')+1);
          bool littleEndian;
          if(littleEndianStr == "true") littleEndian = true;
          else littleEndian = false;
          if(tip == TipZapisa::R_VN16_PC16){
            SimbolLinker* simbol = dohvatiSimbolIzTabeleSimbola(imeSimbola);
            if(simbol->dohvatiSekciju() == imeSekcije){
              /* ostavi za kasnije */
              SekcijaLinker* sekcija = dovhvatiSekciju(imeSekcije, rbFajla);
              int pocetakSekcije = sekcija->dohvatiAdresuPocetka();
              RelokacioniZapisLinker* relZapis = new RelokacioniZapisLinker(simbolID, pocetakSekcije + pomeraj, tip, littleEndian);
              /* prepravi sadrzaj */
              /* procitaj 2B koja trebaju da se promene */
              string prviBajt, drugiBajt;
              int pocetak = relZapis->dohvatiOfset()*2;
              string rec = sadrzaj.substr(pocetak,4);
              int s;
              if(simbol->dohvatiTip()==Tip::LOC){
                s = sekcija->dohvatiAdresuPocetka();

              }
              else {
                s = simbol->dohvatiVrednost();
              }
              int p = relZapis->dohvatiOfset();
              int razlika = s - p;
              string razlikaHex = dec2hex(to_string(razlika));
              string vrednostHex = Add_Hex(razlikaHex,rec);
              sadrzaj = sadrzaj.erase(pocetak,4);
              sadrzaj.insert(pocetak, vrednostHex);
            }
            else {
              SekcijaLinker* sekcija = dovhvatiSekciju(imeSekcije, rbFajla);
              int pocetakSekcije = sekcija->dohvatiAdresuPocetka();
              RelokacioniZapisLinker* relZapis = new RelokacioniZapisLinker(simbolID, pocetakSekcije + pomeraj, tip, littleEndian);
              relZapis->postaviRedniBrojRelokacionogZapisa(idZapisa++);
              sekcija->dodajRelokacioniZapis(relZapis);
              /* prepravi sadrzaj */
              /* procitaj 2B koja trebaju da se promene */
              string prviBajt, drugiBajt;
              int pocetak = relZapis->dohvatiOfset()*2;
              string rec = sadrzaj.substr(pocetak,4);
              int s;
              if(simbol->dohvatiTip()==Tip::LOC){
                s = sekcija->dohvatiAdresuPocetka();

              }
              else {
                s = simbol->dohvatiVrednost();
              }
              int p = relZapis->dohvatiOfset();
              int razlika = s - p;
              string razlikaHex = dec2hex(to_string(razlika));
              string vrednostHex = Add_Hex(razlikaHex,rec);
              sadrzaj = sadrzaj.erase(pocetak,4);
              sadrzaj.insert(pocetak, vrednostHex);
            }
            brojac++;
            continue;
          }
          SekcijaLinker* sekcija = dovhvatiSekciju(imeSekcije, rbFajla);
          int pocetakSekcije = sekcija->dohvatiAdresuPocetka();
          RelokacioniZapisLinker* relZapis = new RelokacioniZapisLinker(simbolID, pocetakSekcije + pomeraj, tip, littleEndian);
          relZapis->postaviRedniBrojRelokacionogZapisa(idZapisa++);
          sekcija->dodajRelokacioniZapis(relZapis);
          /* prepravi sadrzaj */
          /* procitaj 2B koja trebaju da se promene */
          string prviBajt, drugiBajt;
          int pocetak = relZapis->dohvatiOfset()*2;
          string rec = sadrzaj.substr(pocetak,4);
          if(tip == TipZapisa::R_VN16_16){
            if(!littleEndian){
              string staraVrednost = rec;
              SimbolLinker* simbol = dohvatiSimbolIzTabeleSimbola(imeSimbola);
              int vr = simbol->dohvatiVrednost()+hex2dec("0x"+staraVrednost);
              string vrHex = dec2hex(to_string(vr));
              int brojNula = 4 - vrHex.size();
              for(int i = 0; i<brojNula;i++){
                vrHex = "0"+vrHex;
              }
              sadrzaj = sadrzaj.erase(pocetak,4);
              sadrzaj.insert(pocetak, vrHex);
            }
            else {
              /* word direktiva */
              drugiBajt = rec.substr(0,2);
              prviBajt = rec.substr(2);
              string staraVrednost = prviBajt+drugiBajt;
              SimbolLinker* simbol = dohvatiSimbolIzTabeleSimbola(imeSimbola);
              int vr = simbol->dohvatiVrednost()+hex2dec("0x"+staraVrednost);
              string vrHex = dec2hex(to_string(vr));
              int brojNula = 4 - vrHex.size();
              for(int i = 0; i<brojNula;i++){
                vrHex = "0"+vrHex;
              }
              drugiBajt = vrHex.substr(0,2);
              prviBajt = vrHex.substr(2);
              vrHex = prviBajt + drugiBajt;
              sadrzaj = sadrzaj.erase(pocetak,4);
              sadrzaj.insert(pocetak, vrHex);
            }
          }
          brojac++;
        }
      }
    }
    rbFajla++;
  }
}

void Linker::upisiUIzlazniFajl(){
  int pocenaAdresa = 0;
  int blankoZnak = 0;
  int noviRed = 0;
  for(int i = 0; i<sadrzaj.size(); i++){
    if(i==0){
      string adresa = dec2hex(to_string(pocenaAdresa));
      for(int j = 0; j<adresa.size()-4;j++){
        adresa = "0"+adresa;
      }
      pocenaAdresa+=8;
      izlazniFajl<<adresa<<": ";
    }
    if(noviRed%16==0 && i!=0){
      izlazniFajl<<endl;
      string adresa = dec2hex(to_string(pocenaAdresa));
      for(int j = 0; j<adresa.size()-4;j++){
        adresa = "0"+adresa;
      }
      izlazniFajl<<adresa<<": ";
      pocenaAdresa+=8;
      blankoZnak = 0;
      noviRed=0;
    }
    else if(blankoZnak%2==0 && i!=0){
      izlazniFajl<<" ";
    }
    izlazniFajl<<sadrzaj.at(i);
    blankoZnak++;
    noviRed++;
  }
  izlazniFajl.close();
}

void Linker::pokreni(){
  raspodeliSekcije();  
  popuniTabeluSimbola();
  azurirajTabeluSimbola();
  azurirajSadzraj();
  upisiUIzlazniFajl();
  list<SimbolLinker*>::iterator it;
  for(it=tabelaSimbola.begin(); it!=tabelaSimbola.end(); ++it){
    cout<<(*it)->dohvatiIme()<<" "<<(*it)->dohvatiVrednost()<<endl;
  }
}