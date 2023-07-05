#include "../inc/asembler.hpp"

Asembler::Asembler(string ulaz, string izlaz) {
  tekstGreske = "";
  linijaKoda = 1;
  pozicioniBrojac = 0;
  trenutnaSekcija = "UND";
  tabelaSekcija = TabelaSekcija();
  tabelaSimbola = TabelaSimbola();
  Simbol* undefined = new Simbol("-", 0, Tip::LOC, "UND");
  tabelaSimbola.dodajSimbolUTabelu(undefined);
	ulazniFajl.open(ulaz);
	if (ulazniFajl.is_open() == false) {
		cout << "Neuspesno otvoren ulazni fajl!" << endl;
		exit(1);
	}

	izlazniFajl.open(izlaz);
	if (izlazniFajl.is_open() == false) {
		cout << "Neuspesno otvoren izlazni fajl!" << endl;
		exit(1);
	}

}

int Asembler::hex2dec(string hex){
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

string Asembler::dec2hex(string dec){
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

void Asembler::ucitajKod() {
  string red;
  while(true) {
    if(!getline(ulazniFajl, red)) break; /* kraj fajla */
    if(regex_match(red,prazanRed)) { /* ignorisi prazan red */
      continue;
    }
    while(red.at(0) == ' ') red.erase(0,1); /* obrisi blanko znake sa pocetka */
    if(red.at(0) == '#') continue; /* ignorisi komentar koji predstavlja ceo red */
    if(red.find('#') != string::npos) {
      /* obrisi komentar koji je deo reda */
      int indeks = red.find('#');
      red = red.substr(0,indeks);
    }
    while(red.at(red.size()-1) == ' ') red = red.substr(0,red.size()-1); /* obrisi blanko znake sa kraja */
    if(!red.compare(".end")) {/* kraj asemblerskog koda */
        kodIzFajla.push_back(red);
        break;
    }
    if(red.find(':') != string::npos) {
      /* pronadjena je labela */
      int indeks = red.find(':');
      if(indeks != red.size()-1){
        /* nije samo labela u datom redu */
        string labela = red.substr(0, indeks+1);
        kodIzFajla.push_back(labela);
        string ostatak = red.substr(indeks+1, red.size()-1);
        while(ostatak.at(0) == ' ') ostatak.erase(0,1);
        kodIzFajla.push_back(ostatak);
        continue;
      }
    }
    kodIzFajla.push_back(red);
  }
}

bool Asembler :: analizirajKod() {
  list<string>::iterator it;
  for(it = kodIzFajla.begin(); it != kodIzFajla.end(); ++it) {
    string linija = (*it);
    /* Asemblerske diretive */
    /* .global <lista_simbola> */
    if(regex_match(linija, direktivaGlobal)){
      int blankoZnak = linija.find(' ');
      string simboli = linija.substr(blankoZnak+1, linija.size()-1);
      list<string> listaSimbola;
      string simbol = "";
      bool poslednji = false;
			while ((simboli.find(",") != string::npos) || poslednji == false) {
        int zarez = simboli.find(",");
				simbol = simboli.substr(0, zarez);
        while(simbol.at(0) == ' ') simbol.erase(0,1);
        /* proveri sa li se simbol nalazi u tabeli simbola */
        if(tabelaSimbola.simbolVecUTabeli(simbol)){ // vraca true ako se nalazi
          Simbol* simbol_pokazivac = tabelaSimbola.dohvatiSimbol(simbol); 
          Tip tip_simbola = simbol_pokazivac->dohvatiTip();
          if(tip_simbola == Tip::EXT) {
            /* simbol vec postoji u tabeli simbola i oznacen je kao extern-i */
            tekstGreske = "Greska: Simbol " + simbol + " vec postoji u tabeli simbola i oznacen je kao ekterni.";
            return false;
          }
          else if(tip_simbola == Tip::LOC){
            /* simbol postoji u tabeli simbola i oznacen je kao lokacni, sada samo promeniti sa LOC na GLOB */
            /* testirati kada se uradi deo vezan za labelu */
            simbol_pokazivac->postaviTip(Tip::GLOB);
            /* ukoliko je potrebno promeni relokacioni zapis */
            list<MestoKoriscenja> mestaKoriscenja = simbol_pokazivac->dohvatiListuMestaKoriscenja();
            list<MestoKoriscenja>::iterator it;
            for(it = mestaKoriscenja.begin(); it != mestaKoriscenja.end(); ++it){
              int rbRelZapisa = (*it).rbZapisa;
              if(rbRelZapisa == -1) continue;
              string imeSekcije = (*it).sekcija;
              RelokacioniZapis* relZapis = tabelaSekcija.dohvatiSekciju(imeSekcije)->dohvatiTabeluRelokacionihZapisa()->dohvatiRelokacioniZapis(rbRelZapisa);
              relZapis->postaviID(simbol_pokazivac->dohvatiRedniBroj());
            }
          }
          else if(tip_simbola == Tip::GLOB){
            tekstGreske = "Greska: Simbol " + simbol + " vec postoji u tabeli simbola.";
            return false;
          }

        }
        else {
          /* ubaci simbol u tabelu simbola */
          Simbol* noviSimbol = new Simbol(simbol, 0, Tip::GLOB, trenutnaSekcija);
          tabelaSimbola.dodajSimbolUTabelu(noviSimbol);
        }
        Simbol* simbol_pokazivac = tabelaSimbola.dohvatiSimbol(simbol);
        if(simbol_pokazivac->dohvatiVelicinuListeMestaKoriscenja()!=0){
          list<MestoKoriscenja> listaMesta = simbol_pokazivac->dohvatiListuMestaKoriscenja();
          list<MestoKoriscenja>::iterator it;
          for(it = listaMesta.begin(); it != listaMesta.end(); ++it){
            MestoKoriscenja mesto = (*it);

            string imeSekcije = mesto.sekcija;
            int ofset = mesto.ofset * 2; 
            int velicina = mesto.velicina;
            int pocetak = mesto.pocetak;
            TipKomande tip = mesto.tip;
            Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(imeSekcije);
            string sadrzajSekcije = sekcija->dohvatiSadrzajSekcije();
            switch (tip)
            {
            case KOMANDA_WORD:{
              sadrzajSekcije = sadrzajSekcije.erase(ofset+pocetak*2, 4);
              sadrzajSekcije.insert(ofset+pocetak*2, "0000");
              sekcija->postaviSadrzajSekcije(sadrzajSekcije);
              break;
            }
            case KOMANDA_PC_REL:{
              sadrzajSekcije = sadrzajSekcije.erase(ofset, 4);
              string hex = dec2hex("-2");
              sadrzajSekcije.insert(ofset, hex);
              sekcija->postaviSadrzajSekcije(sadrzajSekcije);
              break;
            }
            case OSTALE:{
              sadrzajSekcije = sadrzajSekcije.erase(ofset, 4);
              sadrzajSekcije.insert(ofset, "0000");
              sekcija->postaviSadrzajSekcije(sadrzajSekcije);
              break;
            }
            default:
              break;
            }
          }
        }
        if (simboli.find(",") == string::npos) {
					poslednji = true;
				}
				else {
					simboli = simboli.substr(zarez + 1);
				}
      }
    }
    /* .extern <lista_simbola> */
    else if(regex_match(linija, direktivaExtern)){
      int blankoZnak = linija.find(' ');
      string simboli = linija.substr(blankoZnak + 1, linija.size()-1);
      list<string> listaSimbola;
      string simbol = "";
			bool poslednji = false;
			while ((simboli.find(",") != string::npos) || poslednji == false) {
				int zarez = simboli.find(",");
				simbol = simboli.substr(0, zarez);
        while(simbol.at(0) == ' ') simbol.erase(0,1);
        /* proveri da li se simboln nalazi u tabeli simbola */
        if(tabelaSimbola.simbolVecUTabeli(simbol)){ // vraca true ako se nalazi
          /* ukoliko je tip simbola LOC, onda samo promeni tip na EXT */
          Simbol* simbol_pokazivac = tabelaSimbola.dohvatiSimbol(simbol);
          if(simbol_pokazivac->dohvatiTip() == Tip::LOC) {
            simbol_pokazivac->postaviTip(Tip::EXT);
          }
          else { // GLOB ili EXT
            tekstGreske = "Greska: Simbol " + simbol +" se vec nalazi u tabeli simbola!";
            return false;
          }
        }
        else { // ubaci simbol u tabelu simbola 
          Simbol* noviSimbol = new Simbol(simbol, 0, Tip::EXT, "UND");
          tabelaSimbola.dodajSimbolUTabelu(noviSimbol);
        }
				if (simboli.find(",") == string::npos) {
					poslednji = true;
				}
				else {
					simboli = simboli.substr(zarez + 1);
				}
      }
    }
    /* .section <ime_sekcije> */
    else if(regex_match(linija, direktivaSection)){
      /* azuriraj velicinu prethodne sekcije */
      if(trenutnaSekcija != "UND") { // treba azurirati velicinu sekcije samo ako nije u pitanju sekcija UND
        Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
        sekcija->postaviVelicinuSekcije(pozicioniBrojac);
      }
      int blankoZnak = linija.find(' ');
      string nazivSekcije = linija.substr(blankoZnak+1, linija.size()-1);
      /* proveri da li postoji simbol u tabeli simbola sa tim nazivom */
      if(tabelaSimbola.simbolVecUTabeli(nazivSekcije)) {
        tekstGreske = "Greska: U tabeli simbola vec postoji simbol/sekcija sa datim imenom";
        return false;
      }
      /* postavi novu trenutno sekciju */
      trenutnaSekcija = nazivSekcije;
      /* resetuj pozicioniBrojac jer je pocela nova sekcija */
      pozicioniBrojac = 0;
      Simbol* noviSimbol = new Simbol(nazivSekcije, 0, Tip::LOC, trenutnaSekcija);
      noviSimbol->postaviDefinisan(true);
      tabelaSimbola.dodajSimbolUTabelu(noviSimbol);
      Sekcija* novaSekcija = new Sekcija(trenutnaSekcija);
      tabelaSekcija.dodajSekcijuUTabelu(novaSekcija);

    }
    /* .word <lista_simbola> ili .word literal */
    else if(regex_match(linija,direktivaWord)){
      int blankoZnak = linija.find(' ');
      string argumenti = linija.substr(blankoZnak+1, linija.size()-1);
      bool poslednji = false;
      while(argumenti.find(',') != string::npos || poslednji == false) {
        int zarez = argumenti.find(",");
				string literalSimbol = argumenti.substr(0, zarez);
        while(literalSimbol.at(0) == ' ') literalSimbol.erase(0,1);
        /* provera da li je literal */
        if(regex_match(literalSimbol, literalR)){ // u pitanju je literal
          /* proveri da li je literal u decimalnom ili heksadecimalnom obliku */
          if(regex_match(literalSimbol, literalDecR)){ // u pitanju je decimalan broj
            string heksadecimalniZapis = dec2hex(literalSimbol);
            if(heksadecimalniZapis.size() <=4) {
              int brojNula = 4 - heksadecimalniZapis.size();
              for(int i = 0; i < brojNula; i++){
                heksadecimalniZapis = "0"+heksadecimalniZapis;
              }
            }
            else{
              tekstGreske = "Greska: Literal koji ste stavili je prevelik.";
              return false;
            }
            string prviBajt = heksadecimalniZapis.substr(0,2);
            string drugiBajt = heksadecimalniZapis.substr(2,3);
            Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
            sekcija->dodajSadrzajUSekciju(drugiBajt);
            sekcija->dodajSadrzajUSekciju(prviBajt);
          }
          else { // u pitnju je heksadecimalan broj
            string heksadecimalniZapis = literalSimbol.erase(0,2); // obrisi '0x' iz zapisa
            /* proveri da li heksadecimalni broj ima vise od 2B, odnosno 4 heksa cifre */
            if(heksadecimalniZapis.size() > 4){
              tekstGreske = "Greska: Literal koji ste stavili je prevelik.";
              return false;
            }
            else {
              int brojNula = 4 - heksadecimalniZapis.size();
              for(int i = 0; i < brojNula; i++){
                heksadecimalniZapis = "0"+heksadecimalniZapis;
              }
              string prviBajt = heksadecimalniZapis.substr(0,2);
              string drugiBajt = heksadecimalniZapis.substr(2,3);
              Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
              sekcija->dodajSadrzajUSekciju(drugiBajt);
              sekcija->dodajSadrzajUSekciju(prviBajt);
            }
          }
        }
        else if(regex_match(literalSimbol,simbolR)){ // u pitanju je simbol
          /* ako se simbol ne nalazi u tabeli simbola onda ga dodaj */
          if(!tabelaSimbola.simbolVecUTabeli(literalSimbol)){
            Simbol* noviSimbol = new Simbol(literalSimbol, 0, Tip::LOC, "UND");
            tabelaSimbola.dodajSimbolUTabelu(noviSimbol);
          }
          /* dohvati simbol sa zadatim imenom iz tabele simola */
          Simbol* simbol = tabelaSimbola.dohvatiSimbol(literalSimbol);
          /* dodaj novo mesto koriscenja simbola */
          Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
          MestoKoriscenja mesto = MestoKoriscenja(pozicioniBrojac, trenutnaSekcija, TipKomande::KOMANDA_WORD,2,0,-1);
          /* proveri da li je simbol definisan */
          if(simbol->dohvatiDefinisan()){ // simbol je definisan
            /* proveri tip simbola */
            if(simbol->dohvatiTip() == Tip::LOC){ // lokalni simbol
              /* raspored bajtova je little-endian -> 4567 se smesta kao 6745 */
              string hex = dec2hex(to_string(simbol->dohvatiVrednost()));
              if(hex.size()<4){
                int brojNula = 4 - hex.size();
                for(int i = 0; i<brojNula; i++){
                  hex = "0"+hex;
                }
              }
              string prviBajt = hex.substr(0,2);
              string drugiBajt = hex.substr(2,3);
              Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
              sekcija->dodajSadrzajUSekciju(drugiBajt);
              sekcija->dodajSadrzajUSekciju(prviBajt); 
              /* napravi relokacioni zapis */
              /* posto je rec o LOC simbolu, u tabeli relokacionih zapisa se upisuje id sekcije, a ne simbola */
              string imeSekcije = simbol->dohvatiSekciju();
              int sekcijaID = tabelaSimbola.dohvatiSimbol(imeSekcije)->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(sekcijaID, pozicioniBrojac, TipZapisa::R_VN16_16, true);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
            else { // eksterni ili globalni simbol
              Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
              sekcija->dodajSadrzajUSekciju("0000");
              /* napravi relokacioni zapis */
              int simbolID = simbol->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(simbolID, pozicioniBrojac, TipZapisa::R_VN16_16, true);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
          }
          else { // simbol nije definisan
            /* ukoliko simbol nije definisan, simbol dodati u tabelu referisanja unepred date sekcije */
            Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
            sekcija->dohvatiTabeluReferisanjaUnapred()->dodajUTabeluReferisanjaUnapred(new NedefinisanSimbol(simbol->dohvatiRedniBroj(), pozicioniBrojac, trenutnaSekcija, TipReferisanja::WORD));
            /* takodje u sadrzaj tekuce sekcije je potrebno dodati 0x0000 jer simbol nije definisan */
            sekcija->dodajSadrzajUSekciju("0000");
          }
          simbol->dodajUListuMestaKoriscenja(mesto);
        }
        if (argumenti.find(",") == string::npos) {
					poslednji = true;
				}
				else {
					argumenti = argumenti.substr(zarez + 1);
				}
      }
      pozicioniBrojac += 2;
    }
    /* .skip <literal> */
    else if(regex_match(linija, direktivaSkip)){
      int blankoZnak = linija.find(' ');
      string lit = linija.substr(blankoZnak + 1, linija.size() - 1);
      /* proveri da li je literal validan */
      if(regex_match(lit,literalR)){
        /* proveri da li je u pitanju decimalni broj ili heksadecimali */
        if(regex_match(lit,literalDecR)){ // u pitanju je decimalni broj
          int broj = stoi(lit);
          if(broj < 0){
            tekstGreske = "Greska: parametar direktive skip mora biti pozitivan broj!";
            return false;
          }
          for(int i = 0; i < broj; i++){
            Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
            sekcija->dodajSadrzajUSekciju("00");
          }
          pozicioniBrojac+=broj;
        }
        else { // u pitanju je heksadecimalan broj
          int broj = hex2dec(lit);
          for(int i = 0; i < broj; i++){
            Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
            sekcija->dodajSadrzajUSekciju("00");
          }
          pozicioniBrojac+=broj;
        }
      }
      else {
        tekstGreske = "Greska: Neispravan format direktive skip!";
        return false;
      }
    }
    /* .end */
    else if(regex_match(linija, direktivaEnd)){
      if(trenutnaSekcija != "UND") { // treba azurirati velicinu sekcije samo ako nije u pitanju sekcija UND
        Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
        sekcija->postaviVelicinuSekcije(pozicioniBrojac);
      }
      break;
    }
    /* labela */
    /* <simbol>: */
    else if(regex_match(linija, labela)){
      int dvotacka = linija.find(':');
      string simbol = linija.substr(0, dvotacka);
      if(tabelaSimbola.simbolVecUTabeli(simbol)){
        Simbol* simbol_pokazivac = tabelaSimbola.dohvatiSimbol(simbol);
        if(simbol_pokazivac->dohvatiTip() == Tip::GLOB){
          simbol_pokazivac->postaviDefinisan(true);
          simbol_pokazivac->postaviVrednost(pozicioniBrojac);
          simbol_pokazivac->postaviSekciju(trenutnaSekcija);
        }
        else if(simbol_pokazivac->dohvatiTip() == Tip::LOC ){
          if(simbol_pokazivac->dohvatiSekciju() != "UND"){
            tekstGreske = "Greska: Simbol " + simbol + " se vec nalazi u tabeli simbola i oznacen je kao lokalni.";
            return false;
          }
          else {
            simbol_pokazivac->postaviSekciju(trenutnaSekcija);
            simbol_pokazivac->postaviVrednost(pozicioniBrojac);
            simbol_pokazivac->postaviDefinisan(true);
          }
        }
        else if(simbol_pokazivac->dohvatiTip() == Tip::EXT){
          tekstGreske = "Greska: Simbol " + simbol + " se vec nalazi u tabeli simbola i oznacen je kao eksterni.";
          return false;
        }
      }
      else {
        /* dodaj simbol u tabelu simbola */
        Simbol* noviSimbol = new Simbol(simbol, pozicioniBrojac, Tip::LOC, trenutnaSekcija);
        noviSimbol->postaviDefinisan(true); // simbol je sada definisan pa je potrebno postaviti polje definisan na true;
        tabelaSimbola.dodajSimbolUTabelu(noviSimbol);
      }
      /* treba da se radi backpatching */
    }
    /* asemblerske naredbe */
    /* halt - instrukcija za zaustavljenje procesora */
    else if(regex_match(linija,haltInstrukcija)){
      /* velicina instrukcije je 1B */
      pozicioniBrojac += 1;
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("00");
    }
    /* int - instrukcija softverskog prekida */
    else if(regex_match(linija,intInstrukcija)){
      /* velicina instrukcije je 2B */
      pozicioniBrojac += 2;
      int blankoZnak = linija.find(' ');
      string regD = linija.substr(blankoZnak+1, linija.size()+1);
      string indeksRegistra = regD.substr(1);
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("10");
      sekcija->dodajSadrzajUSekciju(indeksRegistra+"F");
    }
    /* iret - instrukcija povratka iz prekidne rutine */
    else if(regex_match(linija,iretInstrukcija)){
      /* velicina instrukcije je 1B */
      pozicioniBrojac += 1;
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("20");
    }  
    /* ret - instrukcija povratka iz potprograma */
    else if(regex_match(linija,retInstrukcija)){
      /* velicina instrukcije je 1B */
      pozicioniBrojac += 1;
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("40");
    }
    /* push - instrukcija smestanja podataka */
    else if(regex_match(linija,pushInstrukcija)){
      /* velicina instrukcije je 3B */
      /*
        sp <= sp - 2; mem[sp] <= regD;
        registar r6 predstavlja sp;
        InstrDescr : 1010 0000 -> a0;
        RegsDescr : DDDD 0101 -> ?6 (DDDD predstavlja indeks registra regD);
        AddrMode : 0001 0010 -> sp se pre formiranja operanda umanjuje za 2, i u pisanju je registarsko indirektno adresiranje;
      */
      pozicioniBrojac += 3;
      int blankoZnak = linija.find(' ');
      string regD = linija.substr(blankoZnak+1, linija.size()+1);
      string indeksRegistra = regD.substr(1);
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("B0");
      sekcija->dodajSadrzajUSekciju(indeksRegistra+"6");
      sekcija->dodajSadrzajUSekciju("12");
    }
    /* pop - instrukcija ucitavanja podataka */
    else if(regex_match(linija,popInstrukcija)){
      /* velicina instrukcije je 3B */
      /*
        regD <= mem[sp]; sp <= sp + 2;
        registar r6 predstavlja sp;
        InstrDescr : 1010 0000 -> a0;
        RegsDescr : DDDD 0101 -> ?6 (DDDD predstavlja indeks registra regD);
        AddrMode : 0100 0010 -> sp se posle formiranja operanda uveca za 2, i u pisanju je registarsko indirektno adresiranje;
      */
      pozicioniBrojac += 3;
      int blankoZnak = linija.find(' ');
      string regD = linija.substr(blankoZnak+1, linija.size()+1);
      string indeksRegistra = regD.substr(1);
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("A0");
      sekcija->dodajSadrzajUSekciju(indeksRegistra+"6");
      sekcija->dodajSadrzajUSekciju("42");
    }
    /* xchg - instrukcija atomicne znamene vrednosti */
    else if(regex_match(linija, xchgInstrukcija)){
      /* velicina instrukcije je 2B */
      pozicioniBrojac += 2;
      int blankoZnak = linija.find(' ');
      string str = linija.substr(blankoZnak+1,linija.size()-1);
      int zarez = str.find(',');
      string regD = str.substr(0,zarez);
      string regS = str.substr(zarez+1, str.size()-1);
      while(regS.find(' ') != string::npos) regS.erase(0,1);
      string indeksRegD = regD.erase(0,1);
      string indeksRegS = regS.erase(0,1);
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("60");
      sekcija->dodajSadrzajUSekciju(indeksRegD + indeksRegS);
    }
    /* instrukcije aritmetickih operacija */
    /* add */
    else if(regex_match(linija, addInstrukcija)){
      /* velicina instrukcije je 2B */
      pozicioniBrojac += 2;
      int blankoZnak = linija.find(' ');
      string str = linija.substr(blankoZnak+1,linija.size()-1);
      int zarez = str.find(',');
      string regD = str.substr(0,zarez);
      string regS = str.substr(zarez+1, str.size()-1);
      while(regS.find(' ') != string::npos) regS.erase(0,1);
      string indeksRegD = regD.erase(0,1);
      string indeksRegS = regS.erase(0,1);
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("70");
      sekcija->dodajSadrzajUSekciju(indeksRegD + indeksRegS);
    }
    /* sub */
    else if(regex_match(linija, subInstrukcija)){
      /* velicina instrukcije je 2B */
      pozicioniBrojac += 2;
      int blankoZnak = linija.find(' ');
      string str = linija.substr(blankoZnak+1,linija.size()-1);
      int zarez = str.find(',');
      string regD = str.substr(0,zarez);
      string regS = str.substr(zarez+1, str.size()-1);
      while(regS.find(' ') != string::npos) regS.erase(0,1);
      string indeksRegD = regD.erase(0,1);
      string indeksRegS = regS.erase(0,1);
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("71");
      sekcija->dodajSadrzajUSekciju(indeksRegD + indeksRegS);
    }
    /* mul */
    else if(regex_match(linija, mulInstrukcija)){
      /* velicina instrukcije je 2B */
      pozicioniBrojac += 2;
      int blankoZnak = linija.find(' ');
      string str = linija.substr(blankoZnak+1,linija.size()-1);
      int zarez = str.find(',');
      string regD = str.substr(0,zarez);
      string regS = str.substr(zarez+1, str.size()-1);
      while(regS.find(' ') != string::npos) regS.erase(0,1);
      string indeksRegD = regD.erase(0,1);
      string indeksRegS = regS.erase(0,1);
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("72");
      sekcija->dodajSadrzajUSekciju(indeksRegD + indeksRegS);
    }
    /* div */
    else if(regex_match(linija, divInstrukcija)){
      /* velicina instrukcije je 2B */
      pozicioniBrojac += 2;
      int blankoZnak = linija.find(' ');
      string str = linija.substr(blankoZnak+1,linija.size()-1);
      int zarez = str.find(',');
      string regD = str.substr(0,zarez);
      string regS = str.substr(zarez+1, str.size()-1);
      while(regS.find(' ') != string::npos) regS.erase(0,1);
      string indeksRegD = regD.erase(0,1);
      string indeksRegS = regS.erase(0,1);
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("73");
      sekcija->dodajSadrzajUSekciju(indeksRegD + indeksRegS);
    }
    /* cmp */
    else if(regex_match(linija, cmpInstrukcija)){
      /* velicina instrukcije je 2B */
      pozicioniBrojac += 2;
      int blankoZnak = linija.find(' ');
      string str = linija.substr(blankoZnak+1,linija.size()-1);
      int zarez = str.find(',');
      string regD = str.substr(0,zarez);
      string regS = str.substr(zarez+1, str.size()-1);
      while(regS.find(' ') != string::npos) regS.erase(0,1);
      string indeksRegD = regD.erase(0,1);
      string indeksRegS = regS.erase(0,1);
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("74");
      sekcija->dodajSadrzajUSekciju(indeksRegD + indeksRegS);
    }
    /* instrukcije logicnih operacija */
    /* not */
    else if(regex_match(linija,notInstrukcija)){
      /* velicina instrukcije je 2B */
      pozicioniBrojac += 2;
      int blankoZnak = linija.find(' ');
      string regD = linija.substr(blankoZnak+1, linija.size()+1);
      string indeksRegistra = regD.substr(1);
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("80");
      sekcija->dodajSadrzajUSekciju(indeksRegistra+"0");
    }
    /* and */
    else if(regex_match(linija, andInstrukcija)){
      /* velicina instrukcije je 2B */
      pozicioniBrojac += 2;
      int blankoZnak = linija.find(' ');
      string str = linija.substr(blankoZnak+1,linija.size()-1);
      int zarez = str.find(',');
      string regD = str.substr(0,zarez);
      string regS = str.substr(zarez+1, str.size()-1);
      while(regS.find(' ') != string::npos) regS.erase(0,1);
      string indeksRegD = regD.erase(0,1);
      string indeksRegS = regS.erase(0,1);
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("81");
      sekcija->dodajSadrzajUSekciju(indeksRegD + indeksRegS);
    }
    /* or */
    else if(regex_match(linija, orInstrukcija)){
      /* velicina instrukcije je 2B */
      pozicioniBrojac += 2;
      int blankoZnak = linija.find(' ');
      string str = linija.substr(blankoZnak+1,linija.size()-1);
      int zarez = str.find(',');
      string regD = str.substr(0,zarez);
      string regS = str.substr(zarez+1, str.size()-1);
      while(regS.find(' ') != string::npos) regS.erase(0,1);
      string indeksRegD = regD.erase(0,1);
      string indeksRegS = regS.erase(0,1);
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("82");
      sekcija->dodajSadrzajUSekciju(indeksRegD + indeksRegS);
    }
    /* xor */
    else if(regex_match(linija, xorInstrukcija)){
      /* velicina instrukcije je 2B */
      pozicioniBrojac += 2;
      int blankoZnak = linija.find(' ');
      string str = linija.substr(blankoZnak+1,linija.size()-1);
      int zarez = str.find(',');
      string regD = str.substr(0,zarez);
      string regS = str.substr(zarez+1, str.size()-1);
      while(regS.find(' ') != string::npos) regS.erase(0,1);
      string indeksRegD = regD.erase(0,1);
      string indeksRegS = regS.erase(0,1);
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("83");
      sekcija->dodajSadrzajUSekciju(indeksRegD + indeksRegS);
    }
    /* test */
    else if(regex_match(linija, testInstrukcija)){
      /* velicina instrukcije je 2B */
      pozicioniBrojac += 2;
      int blankoZnak = linija.find(' ');
      string str = linija.substr(blankoZnak+1,linija.size()-1);
      int zarez = str.find(',');
      string regD = str.substr(0,zarez);
      string regS = str.substr(zarez+1, str.size()-1);
      while(regS.find(' ') != string::npos) regS.erase(0,1);
      string indeksRegD = regD.erase(0,1);
      string indeksRegS = regS.erase(0,1);
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("84");
      sekcija->dodajSadrzajUSekciju(indeksRegD + indeksRegS);
    }
    /* instrukcije pomerackih operacija */
    /* shl */
    else if(regex_match(linija, shlInstrukcija)){
      /* velicina instrukcije je 2B */
      pozicioniBrojac += 2;
      int blankoZnak = linija.find(' ');
      string str = linija.substr(blankoZnak+1,linija.size()-1);
      int zarez = str.find(',');
      string regD = str.substr(0,zarez);
      string regS = str.substr(zarez+1, str.size()-1);
      while(regS.find(' ') != string::npos) regS.erase(0,1);
      string indeksRegD = regD.erase(0,1);
      string indeksRegS = regS.erase(0,1);
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("90");
      sekcija->dodajSadrzajUSekciju(indeksRegD + indeksRegS);
    }
    /* shr */
    else if(regex_match(linija, shrInstrukcija)){
      /* velicina instrukcije je 2B */
      pozicioniBrojac += 2;
      int blankoZnak = linija.find(' ');
      string str = linija.substr(blankoZnak+1,linija.size()-1);
      int zarez = str.find(',');
      string regD = str.substr(0,zarez);
      string regS = str.substr(zarez+1, str.size()-1);
      while(regS.find(' ') != string::npos) regS.erase(0,1);
      string indeksRegD = regD.erase(0,1);
      string indeksRegS = regS.erase(0,1);
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      sekcija->dodajSadrzajUSekciju("91");
      sekcija->dodajSadrzajUSekciju(indeksRegD + indeksRegS);
    }
    /* ldr - instrukcija ucitavanja podataka */
    else if(regex_match(linija, ldrInstrukcija)){
      /* velicina instrukcije zavisi od nacina adresiranja*/
      int blankoZnak = linija.find(' ');
      string str = linija.substr(blankoZnak+1,linija.size()-1);
      int zarez = str.find(',');
      string regD = str.substr(0,zarez);
      string indeksRegD;
      if(regD == "psw") indeksRegD = "8";
      else indeksRegD = regD.erase(0,1); 
      string operand = str.substr(zarez+1);
      operand = operand.erase(0,1);
      /* dodaj InstrDescr i RegsDescr u sadrzaj sekcije jer je on uvek isti */
      /* dohvati trenutnu sekciju */
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      /* InstrDescr = 1010 0000 -> A0 */
      sekcija->dodajSadrzajUSekciju("A0");
      /* RegsDescr = DDDD SSSS, biti SSSS su nebitni i neka budu npr. 1111, kod PC rel su bitni i oni su 0111 sto predstavlja reg r7 (odnosno pc) */
      string heksadecimalanZapisRegD = dec2hex(indeksRegD);
      /* tipovi adresiranja */
      if(regex_match(operand, podaciRegistarskoDirekstno)){
        pozicioniBrojac+=3;
        while(operand.find(' ')!=string::npos) operand = operand.erase(0,1);
        if(operand == "psw"){
          sekcija->dodajSadrzajUSekciju(heksadecimalanZapisRegD+"8");
        }
        else {
          string regS = operand.erase(0,1);
          sekcija->dodajSadrzajUSekciju(heksadecimalanZapisRegD+regS);
        }
        sekcija->dodajSadrzajUSekciju("01");

      }
      else if(regex_match(operand, podaciApsolutnoAdresiranje)){
        sekcija->dodajSadrzajUSekciju(heksadecimalanZapisRegD+"F");
        /* posto je  u pitanju apsolutno(neposredno) adresiranje, velicina instrukcije je 5B*/
        pozicioniBrojac+=5;
        /* u sadrzaj sekcije dodaj 2B koji predstavljaju AddrMode */
        /* AddrMode = UUUU AAAA -> 0000 0000 -> 00 00  */
        sekcija->dodajSadrzajUSekciju("00");
        int znakAdresiranja = operand.find('$');
        string literalSimbol = operand.erase(znakAdresiranja,1);
        /* proveri da li je u pitanju labela ili simbol */
        if(regex_match(literalSimbol, literalR)){ // vraca true ako je u pitanju literal
          /* proveri da li je literal decimalan ili heksadecimalan broj */
          if(regex_match(literalSimbol, literalDecR)){ // u pitanju je decimalan broj
            /* pretvori decimalni u heksadecimalni broj */
            string heksadecimalniZapis = dec2hex(literalSimbol);
            if(heksadecimalniZapis.size() > 4){
              tekstGreske = "Greska: Heksadecimalni zapis broja " + literalSimbol + " je prevelik.";
              return false;
            }
            int brojNula = 4 - heksadecimalniZapis.size();
            for(int i = 0; i < brojNula; i++){
              heksadecimalniZapis = "0" + heksadecimalniZapis;
            }
            /* dodaj u sadrzaj sekcije */
            sekcija->dodajSadrzajUSekciju(heksadecimalniZapis);
          }
          else { // u pitanju je heksadecimalan broj
            string heksadecimalanBroj = literalSimbol.erase(0,2); // obrisi '0x'
            if(heksadecimalanBroj.size()>4){
              tekstGreske = "Greska: Heksadecimalan broj "+literalSimbol+" je preveli.";
              return false;
            }
            int brojNula = 4 - heksadecimalanBroj.size();
            for(int i = 0; i < brojNula; i++){
              heksadecimalanBroj = "0" + heksadecimalanBroj;
            }
            sekcija->dodajSadrzajUSekciju(heksadecimalanBroj);
          }
        }
        else { // u pitanju je simbol
          /* proveri da li se simbol nalazi u tabeli simbola */
          if(!tabelaSimbola.simbolVecUTabeli(literalSimbol)){
            /* ako simbol nije u tabeli dodaj ga */
            tabelaSimbola.dodajSimbolUTabelu(new Simbol(literalSimbol,0,Tip::LOC, "UND"));
          }
          Simbol* simbol = tabelaSimbola.dohvatiSimbol(literalSimbol);
          MestoKoriscenja mesto = MestoKoriscenja(pozicioniBrojac, trenutnaSekcija, TipKomande::OSTALE,5,3,-1);
          /* dodaj novo mesto koriscenja simbola */       
          /* proveri da li je simbol definisan */
          if(simbol->dohvatiDefinisan()){
            /* proveri tip simbola */
            if(simbol->dohvatiTip() == Tip::LOC){
              /* ako je simbol lokalan i defisan onda treba da se u sadrzaj sekcije unese njegova vrednost */
              int vrednost = simbol->dohvatiVrednost();
              string hexZapis = dec2hex(to_string(vrednost));
              if(hexZapis.size() < 4){
                int brojNula = 4 - hexZapis.size();
                for(int i = 0; i , brojNula; i++){
                  hexZapis = "0"+hexZapis;
                }
              }
              sekcija->dodajSadrzajUSekciju(hexZapis);
              /* napravi relokacioni zapis */
              /* posto je u pitanju LOC simbol, u tabeli relokacionih zapisa se stavlja ID sekcije gde je simbol definisan, a ne ID simbola */
              string imeSekcije = simbol->dohvatiSekciju();
              int sekcijaID = tabelaSimbola.dohvatiSimbol(imeSekcije)->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(sekcijaID,pozicioniBrojac-2, TipZapisa::R_VN16_16,false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
            else {
              sekcija->dodajSadrzajUSekciju("0000");
              int simbolID = simbol->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(simbolID,pozicioniBrojac-2, TipZapisa::R_VN16_16,false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
          }
          else { // simbol nije definisan
            sekcija->dodajSadrzajUSekciju("0000");
            /* dodaj u tabelu referisanja unapred*/
            int simbolID = simbol->dohvatiRedniBroj();
            sekcija->dohvatiTabeluReferisanjaUnapred()->dodajUTabeluReferisanjaUnapred(new NedefinisanSimbol(simbolID,pozicioniBrojac-2,trenutnaSekcija,TipReferisanja::OSTALO));
          }
          simbol->dodajUListuMestaKoriscenja(mesto);
        }
      }
      else if(regex_match(operand, podaciMemorijskoDirektno)){
        sekcija->dodajSadrzajUSekciju(heksadecimalanZapisRegD+"F");
        /* posto se radi o memorijskom direktnom adresiranju, velicina instrukcije je 5B */
        pozicioniBrojac+=5;
        while(operand.find(' ')!=string::npos) operand = operand.erase(0,1);
        /* u sadrzaj sekcije dodaj 2B koji predstavljaju AddrMode */
        /* AddrMode = UUUU AAAA -> 0000 0100 -> 00 04  */
        sekcija->dodajSadrzajUSekciju("04");
        /* proveri da li je u pitanju literal ili simbol */
        if(regex_match(operand, literalR)){ // u pitanju je literal
          /* proveri da li je u pitanju decimalan broj ili heksadecimalan broj */
          if(regex_match(operand,literalHexR)){ // u pitanju je heksadecimalan broj
            operand = operand.erase(0,2); // obrisi '0x'
            if(operand.size()>4){
              tekstGreske = "Greska: Heksadecimalan broj 0x"+operand+" je preveli.";
              return false;
            }
            int brojNula = 4-operand.size();
            for(int i = 0; i < brojNula; i++){
              operand = "0"+operand;
            }
            sekcija->dodajSadrzajUSekciju(operand);
          }
          else { // u pitanju je decimalan broj
            string hex = dec2hex(operand);
            if(hex.size()>4){
              tekstGreske = "Greska:  broj "+operand+" je preveli.";
              return false;
            }
            int brojNula = 4-hex.size();
            for(int i = 0; i < brojNula; i++){
              hex = "0"+hex;
            }
            sekcija->dodajSadrzajUSekciju(hex);
          }
        }
        else { // u pitanju je simbol
          if(!tabelaSimbola.simbolVecUTabeli(operand)){
            tabelaSimbola.dodajSimbolUTabelu(new Simbol(operand, 0, Tip::LOC, "UND"));
          }
          Simbol* simbol = tabelaSimbola.dohvatiSimbol(operand);
          MestoKoriscenja mesto = MestoKoriscenja(pozicioniBrojac, trenutnaSekcija, TipKomande::OSTALE,5,3,-1);
          if(simbol->dohvatiDefinisan()){
            if(simbol->dohvatiTip() == Tip::LOC){
              int vrednost = simbol->dohvatiVrednost();
              string hexZapis = dec2hex(to_string(vrednost));
              if(hexZapis.size() < 4){
                int brojNula = 4 - hexZapis.size();
                for(int i = 0; i , brojNula; i++){
                  hexZapis = "0"+hexZapis;
                }
              }
              sekcija->dodajSadrzajUSekciju(hexZapis);
              string imeSekcije = simbol->dohvatiSekciju();
              int sekcijaID = tabelaSimbola.dohvatiSimbol(imeSekcije)->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(sekcijaID,pozicioniBrojac-2, TipZapisa::R_VN16_16,false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
            else {
              sekcija->dodajSadrzajUSekciju("0000");
              int simbolID = simbol->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(simbolID,pozicioniBrojac-2, TipZapisa::R_VN16_16,false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
          }
          else{
            sekcija->dodajSadrzajUSekciju("0000");
            /* dodaj u tabelu referisanja unapred*/
            int simbolID = simbol->dohvatiRedniBroj();
            sekcija->dohvatiTabeluReferisanjaUnapred()->dodajUTabeluReferisanjaUnapred(new NedefinisanSimbol(simbolID,pozicioniBrojac-2,trenutnaSekcija,TipReferisanja::OSTALO));
          }
          simbol->dodajUListuMestaKoriscenja(mesto);
        }
      }
      else if(regex_match(operand, podaciPCRelativno)){
        sekcija->dodajSadrzajUSekciju(heksadecimalanZapisRegD+"7");
        pozicioniBrojac+=5;
        operand = operand.erase(0,1); // obrisi ' %'
        /* AddrMode = UUUU AAAA -> 0000 00101 -> 05  */
        sekcija->dodajSadrzajUSekciju("05");
        if(!tabelaSimbola.simbolVecUTabeli(operand)){
          tabelaSimbola.dodajSimbolUTabelu(new Simbol(operand, 0, Tip::LOC, "UND"));
        }
        Simbol* simbol = tabelaSimbola.dohvatiSimbol(operand);
        MestoKoriscenja mesto = MestoKoriscenja(pozicioniBrojac-2, trenutnaSekcija, TipKomande::KOMANDA_PC_REL,5,0,-1);
        if(simbol->dohvatiDefinisan()){
          /* proveri da li se simbol definisan u istoj sekciji gde je i koriscen */
          if(simbol->dohvatiSekciju() == trenutnaSekcija){ // definisan je u istoj sekciji gde je koriscen
            /* ukoliko je simbol definisan u istoj sekciji gde je i koriscen onda u Data deo instukcije treba upisati razliku izmedju adresa simbola i pozicionogBrojaca */
            int broj = simbol->dohvatiVrednost() - pozicioniBrojac;
            string hex = dec2hex(to_string(broj));
            sekcija->dodajSadrzajUSekciju(hex);
          }
          else { // nijee definisan u istoj sekciji gde je koriscen
            /* proveri tip simbola */
            if(simbol->dohvatiTip() == Tip::LOC){
              int addend = simbol->dohvatiVrednost() - 2;
              string hex = dec2hex(to_string(addend));
              if(hex.size()<4){
                int brojNula = 4-hex.size();
                for(int i = 0; i<brojNula; i++){
                  hex="0"+hex;
                }
              }
              sekcija->dodajSadrzajUSekciju(hex);
              string imeSekcije = simbol->dohvatiSekciju();
              int sekcijaID = tabelaSimbola.dohvatiSimbol(imeSekcije)->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(sekcijaID, pozicioniBrojac-2, TipZapisa::R_VN16_PC16, false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
            else {
              sekcija->dodajSadrzajUSekciju(dec2hex("-2"));
              RelokacioniZapis* relZapis = new RelokacioniZapis(simbol->dohvatiRedniBroj(), pozicioniBrojac-2, TipZapisa::R_VN16_PC16, false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
          }
          
        }
        else {
          sekcija->dohvatiTabeluReferisanjaUnapred()->dodajUTabeluReferisanjaUnapred(new NedefinisanSimbol(simbol->dohvatiRedniBroj(), pozicioniBrojac-2, trenutnaSekcija, TipReferisanja::PC_REL));
          sekcija->dodajSadrzajUSekciju("0000");
        }
        simbol->dodajUListuMestaKoriscenja(mesto);
      }
      else if(regex_match(operand, podaciRegistarskoIndirektno)){
        pozicioniBrojac+=3;
        operand = operand.erase(0,1); // obrisi '[' sa pocetka
        operand = operand.erase(operand.size()-1,1); // obrisi ']' sa kraja
        string regS;
        if(operand == "psw") regS = "8";
        else {
          regS = operand.erase(0,1);
        }
        sekcija->dodajSadrzajUSekciju(regD+regS);
        sekcija->dodajSadrzajUSekciju("02");
      }
      else{ // reg indr sa pomerajem
        pozicioniBrojac+=5;
        if(operand.find('[')==string::npos || operand.find(']')==string::npos){
          tekstGreske = "Greska: Pogresan izgled instrukcije ldr";
          return false;
        }
        operand = operand.erase(0,1); // obrisi '[' sa pocetka
        operand = operand.erase(operand.size()-1,1); // obrisi ']' sa kraja
        /* obrisi blanko znake */
        int blankoZnak;
        while((blankoZnak=operand.find(' '))!=string::npos) operand = operand.erase(blankoZnak,1);
        string str;
        if(operand.at(0) == 'r'){
          str = operand.substr(0,2);
        }
        else str = operand.substr(0,3);
        if(regex_match(str,registarR)){
          string regS;
          if(str == "psw"){
            regS = "8";
            operand = operand.erase(0,3);
          }
          else {
            regS = str.erase(0,1);
            operand = operand.erase(0,2);
          }
          sekcija->dodajSadrzajUSekciju(indeksRegD+regS);
          sekcija->dodajSadrzajUSekciju("03");
          if(operand.at(0)!='+'){
            tekstGreske = "Greska: Pogresan izgled instrukcije ldr";
            return false;
          }
          operand = operand.erase(0,1);
          /* proveri da li je u pitanju literal ili simbol */
          if(regex_match(operand, literalR)){
            if(regex_match(operand,literalDecR)){
              string hex = dec2hex(operand);
              int brojNula = 4-hex.size();
              for(int i=0;i<brojNula;i++){
                hex="0"+hex;
              }
              sekcija->dodajSadrzajUSekciju(hex);
            }
            else{
              string hex = operand.erase(0,2);
              if(hex.size()>4){
                tekstGreske="Greska: Heksadecimalan broj "+ hex + " je prevelik.";
                return false;
              }
              int brojNula = 4-hex.size();
              for(int i=0;i<brojNula;i++){
                hex="0"+hex;
              }
              sekcija->dodajSadrzajUSekciju(hex);
            }
          }
          else {
            if(!tabelaSimbola.simbolVecUTabeli(operand)){
              tabelaSimbola.dodajSimbolUTabelu(new Simbol(operand,0,Tip::LOC,"UND"));
            }
            Simbol* simbol = tabelaSimbola.dohvatiSimbol(operand);
            MestoKoriscenja mesto = MestoKoriscenja(pozicioniBrojac, trenutnaSekcija, TipKomande::OSTALE,2,0,-1);
            if(simbol->dohvatiDefinisan()){
              if(simbol->dohvatiTip() == Tip::LOC){
                int vrednost = simbol->dohvatiVrednost();
                string hexZapis = dec2hex(to_string(vrednost));
                if(hexZapis.size() < 4){
                  int brojNula = 4 - hexZapis.size();
                  for(int i = 0; i , brojNula; i++){
                    hexZapis = "0"+hexZapis;
                  }
                }
                sekcija->dodajSadrzajUSekciju(hexZapis);
                string imeSekcije = simbol->dohvatiSekciju();
                int sekcijaID = tabelaSimbola.dohvatiSimbol(imeSekcije)->dohvatiRedniBroj();
                RelokacioniZapis* relZapis = new RelokacioniZapis(sekcijaID,pozicioniBrojac-2, TipZapisa::R_VN16_16,false);
                sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
                mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
              }
              else {
                sekcija->dodajSadrzajUSekciju("0000");
                int simbolID = simbol->dohvatiRedniBroj();
                RelokacioniZapis* relZapis = new RelokacioniZapis(simbolID,pozicioniBrojac-2, TipZapisa::R_VN16_16,false);
                sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
                mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
              }
            }
            else{
              sekcija->dodajSadrzajUSekciju("0000");
              /* dodaj u tabelu referisanja unapred*/
              int simbolID = simbol->dohvatiRedniBroj();
              sekcija->dohvatiTabeluReferisanjaUnapred()->dodajUTabeluReferisanjaUnapred(new NedefinisanSimbol(simbolID,pozicioniBrojac-2,trenutnaSekcija,TipReferisanja::OSTALO));
            }
            simbol->dodajUListuMestaKoriscenja(mesto);
          }
        }
        else {
          tekstGreske = "Greska: Pogresan izgled instrukcije ldr";
          return false;
        }
      }
    }
    /* str - instrukcija smestanja podataka */
    else if(regex_match(linija, strInstrukcija)){
      /* velicina instrukcije zavisi od nacina adresiranja*/
      int blankoZnak = linija.find(' ');
      string str = linija.substr(blankoZnak+1,linija.size()-1);
      int zarez = str.find(',');
      string regD = str.substr(0,zarez);
      string indeksRegD;
      if(regD == "psw") indeksRegD = "8";
      else indeksRegD = regD.erase(0,1); 
      string operand = str.substr(zarez+1);
      operand = operand.erase(0,1);
      /* dodaj InstrDescr i RegsDescr u sadrzaj sekcije jer je on uvek isti */
      /* dohvati trenutnu sekciju */
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      /* InstrDescr = 1011 0000 -> B0 */
      sekcija->dodajSadrzajUSekciju("B0");
      /* RegsDescr = DDDD SSSS, biti SSSS su nebitni i neka budu npr. 1111, kod PC rel su bitni i oni su 0111 sto predstavlja reg r7 (odnosno pc) */
      string heksadecimalanZapisRegD = dec2hex(indeksRegD);
      /* tipovi adresiranja */
      if(regex_match(operand, podaciRegistarskoDirekstno)){
        pozicioniBrojac+=3;
        while(operand.find(' ')!=string::npos) operand = operand.erase(0,1);
        if(operand == "psw"){
          sekcija->dodajSadrzajUSekciju(heksadecimalanZapisRegD+"8");
        }
        else {
          string regS = operand.erase(0,1);
          sekcija->dodajSadrzajUSekciju(heksadecimalanZapisRegD+regS);
        }
        sekcija->dodajSadrzajUSekciju("01");

      }
      else if(regex_match(operand, podaciApsolutnoAdresiranje)){
        sekcija->dodajSadrzajUSekciju(heksadecimalanZapisRegD+"F");
        /* posto je  u pitanju apsolutno(neposredno) adresiranje, velicina instrukcije je 5B*/
        pozicioniBrojac+=5;
        /* u sadrzaj sekcije dodaj 2B koji predstavljaju AddrMode */
        /* AddrMode = UUUU AAAA -> 0000 0000 -> 00 00  */
        sekcija->dodajSadrzajUSekciju("00");
        int znakAdresiranja = operand.find('$');
        string literalSimbol = operand.erase(znakAdresiranja,1);
        /* proveri da li je u pitanju labela ili simbol */
        if(regex_match(literalSimbol, literalR)){ // vraca true ako je u pitanju literal
          /* proveri da li je literal decimalan ili heksadecimalan broj */
          if(regex_match(literalSimbol, literalDecR)){ // u pitanju je decimalan broj
            /* pretvori decimalni u heksadecimalni broj */
            string heksadecimalniZapis = dec2hex(literalSimbol);
            if(heksadecimalniZapis.size() > 4){
              tekstGreske = "Greska: Heksadecimalni zapis broja " + literalSimbol + " je prevelik.";
              return false;
            }
            int brojNula = 4 - heksadecimalniZapis.size();
            for(int i = 0; i < brojNula; i++){
              heksadecimalniZapis = "0" + heksadecimalniZapis;
            }
            /* dodaj u sadrzaj sekcije */
            sekcija->dodajSadrzajUSekciju(heksadecimalniZapis);
          }
          else { // u pitanju je heksadecimalan broj
            string heksadecimalanBroj = literalSimbol.erase(0,2); // obrisi '0x'
            if(heksadecimalanBroj.size()>4){
              tekstGreske = "Greska: Heksadecimalan broj "+literalSimbol+" je preveli.";
              return false;
            }
            int brojNula = 4 - heksadecimalanBroj.size();
            for(int i = 0; i < brojNula; i++){
              heksadecimalanBroj = "0" + heksadecimalanBroj;
            }
            sekcija->dodajSadrzajUSekciju(heksadecimalanBroj);
          }
        }
        else { // u pitanju je simbol
          /* proveri da li se simbol nalazi u tabeli simbola */
          if(!tabelaSimbola.simbolVecUTabeli(literalSimbol)){
            /* ako simbol nije u tabeli dodaj ga */
            tabelaSimbola.dodajSimbolUTabelu(new Simbol(literalSimbol,0,Tip::LOC, "UND"));
          }
          Simbol* simbol = tabelaSimbola.dohvatiSimbol(literalSimbol);
          MestoKoriscenja mesto = MestoKoriscenja(pozicioniBrojac, trenutnaSekcija, TipKomande::OSTALE,5,3,-1);
          /* dodaj novo mesto koriscenja simbola */       
          /* proveri da li je simbol definisan */
          if(simbol->dohvatiDefinisan()){
            /* proveri tip simbola */
            if(simbol->dohvatiTip() == Tip::LOC){
              /* ako je simbol lokalan i defisan onda treba da se u sadrzaj sekcije unese njegova vrednost */
              int vrednost = simbol->dohvatiVrednost();
              string hexZapis = dec2hex(to_string(vrednost));
              if(hexZapis.size() < 4){
                int brojNula = 4 - hexZapis.size();
                for(int i = 0; i , brojNula; i++){
                  hexZapis = "0"+hexZapis;
                }
              }
              sekcija->dodajSadrzajUSekciju(hexZapis);
              /* napravi relokacioni zapis */
              /* posto je u pitanju LOC simbol, u tabeli relokacionih zapisa se stavlja ID sekcije gde je simbol definisan, a ne ID simbola */
              string imeSekcije = simbol->dohvatiSekciju();
              int sekcijaID = tabelaSimbola.dohvatiSimbol(imeSekcije)->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(sekcijaID,pozicioniBrojac-2, TipZapisa::R_VN16_16,false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
            else {
              sekcija->dodajSadrzajUSekciju("0000");
              int simbolID = simbol->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(simbolID,pozicioniBrojac-2, TipZapisa::R_VN16_16,false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
          }
          else { // simbol nije definisan
            sekcija->dodajSadrzajUSekciju("0000");
            /* dodaj u tabelu referisanja unapred*/
            int simbolID = simbol->dohvatiRedniBroj();
            sekcija->dohvatiTabeluReferisanjaUnapred()->dodajUTabeluReferisanjaUnapred(new NedefinisanSimbol(simbolID,pozicioniBrojac-2,trenutnaSekcija,TipReferisanja::OSTALO));
          }
          simbol->dodajUListuMestaKoriscenja(mesto);
        }
      }
      else if(regex_match(operand, podaciMemorijskoDirektno)){
        sekcija->dodajSadrzajUSekciju(heksadecimalanZapisRegD+"F");
        /* posto se radi o memorijskom direktnom adresiranju, velicina instrukcije je 5B */
        pozicioniBrojac+=5;
        while(operand.find(' ')!=string::npos) operand = operand.erase(0,1);
        /* u sadrzaj sekcije dodaj 2B koji predstavljaju AddrMode */
        /* AddrMode = UUUU AAAA -> 0000 0100 -> 00 04  */
        sekcija->dodajSadrzajUSekciju("04");
        /* proveri da li je u pitanju literal ili simbol */
        if(regex_match(operand, literalR)){ // u pitanju je literal
          /* proveri da li je u pitanju decimalan broj ili heksadecimalan broj */
          if(regex_match(operand,literalHexR)){ // u pitanju je heksadecimalan broj
            operand = operand.erase(0,2); // obrisi '0x'
            if(operand.size()>4){
              tekstGreske = "Greska: Heksadecimalan broj 0x"+operand+" je preveli.";
              return false;
            }
            int brojNula = 4-operand.size();
            for(int i = 0; i < brojNula; i++){
              operand = "0"+operand;
            }
            sekcija->dodajSadrzajUSekciju(operand);
          }
          else { // u pitanju je decimalan broj
            string hex = dec2hex(operand);
            if(hex.size()>4){
              tekstGreske = "Greska:  broj "+operand+" je preveli.";
              return false;
            }
            int brojNula = 4-hex.size();
            for(int i = 0; i < brojNula; i++){
              hex = "0"+hex;
            }
            sekcija->dodajSadrzajUSekciju(hex);
          }
        }
        else { // u pitanju je simbol
          if(!tabelaSimbola.simbolVecUTabeli(operand)){
            tabelaSimbola.dodajSimbolUTabelu(new Simbol(operand, 0, Tip::LOC, "UND"));
          }
          Simbol* simbol = tabelaSimbola.dohvatiSimbol(operand);
          MestoKoriscenja mesto = MestoKoriscenja(pozicioniBrojac, trenutnaSekcija, TipKomande::OSTALE,5,3,-1);
          if(simbol->dohvatiDefinisan()){
            if(simbol->dohvatiTip() == Tip::LOC){
              int vrednost = simbol->dohvatiVrednost();
              string hexZapis = dec2hex(to_string(vrednost));
              if(hexZapis.size() < 4){
                int brojNula = 4 - hexZapis.size();
                for(int i = 0; i , brojNula; i++){
                  hexZapis = "0"+hexZapis;
                }
              }
              sekcija->dodajSadrzajUSekciju(hexZapis);
              string imeSekcije = simbol->dohvatiSekciju();
              int sekcijaID = tabelaSimbola.dohvatiSimbol(imeSekcije)->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(sekcijaID,pozicioniBrojac-2, TipZapisa::R_VN16_16,false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
            else {
              sekcija->dodajSadrzajUSekciju("0000");
              int simbolID = simbol->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(simbolID,pozicioniBrojac-2, TipZapisa::R_VN16_16,false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
          }
          else{
            sekcija->dodajSadrzajUSekciju("0000");
            /* dodaj u tabelu referisanja unapred*/
            int simbolID = simbol->dohvatiRedniBroj();
            sekcija->dohvatiTabeluReferisanjaUnapred()->dodajUTabeluReferisanjaUnapred(new NedefinisanSimbol(simbolID,pozicioniBrojac-2,trenutnaSekcija,TipReferisanja::OSTALO));
          }
          simbol->dodajUListuMestaKoriscenja(mesto);
        }
      }
      else if(regex_match(operand, podaciPCRelativno)){
        sekcija->dodajSadrzajUSekciju(heksadecimalanZapisRegD+"7");
        pozicioniBrojac+=5;
        operand = operand.erase(0,1); // obrisi ' %'
        /* AddrMode = UUUU AAAA -> 0000 0101 -> 00 05  */
        sekcija->dodajSadrzajUSekciju("05");
        if(!tabelaSimbola.simbolVecUTabeli(operand)){
          tabelaSimbola.dodajSimbolUTabelu(new Simbol(operand, 0, Tip::LOC, "UND"));
        }
        Simbol* simbol = tabelaSimbola.dohvatiSimbol(operand);
        MestoKoriscenja mesto = MestoKoriscenja(pozicioniBrojac-2, trenutnaSekcija, TipKomande::KOMANDA_PC_REL,5,0,-1);
        if(simbol->dohvatiDefinisan()){
          /* proveri da li se simbol definisan u istoj sekciji gde je i koriscen */
          if(simbol->dohvatiSekciju() == trenutnaSekcija){ // definisan je u istoj sekciji gde je koriscen
            /* ukoliko je simbol definisan u istoj sekciji gde je i koriscen onda u Data deo instukcije treba upisati razliku izmedju adresa simbola i pozicionogBrojaca */
            int broj = simbol->dohvatiVrednost() - pozicioniBrojac;
            string hex = dec2hex(to_string(broj));
            sekcija->dodajSadrzajUSekciju(hex);
          }
          else { // nijee definisan u istoj sekciji gde je koriscen
            /* proveri tip simbola */
            if(simbol->dohvatiTip() == Tip::LOC){
              int addend = simbol->dohvatiVrednost() - 2;
              string hex = dec2hex(to_string(addend));
              if(hex.size()<4){
                int brojNula = 4-hex.size();
                for(int i = 0; i<brojNula; i++){
                  hex="0"+hex;
                }
              }
              sekcija->dodajSadrzajUSekciju(hex);
              string imeSekcije = simbol->dohvatiSekciju();
              int sekcijaID = tabelaSimbola.dohvatiSimbol(imeSekcije)->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(sekcijaID, pozicioniBrojac-2, TipZapisa::R_VN16_PC16, false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
            else {
              sekcija->dodajSadrzajUSekciju(dec2hex("-2"));
              RelokacioniZapis* relZapis = new RelokacioniZapis(simbol->dohvatiRedniBroj(), pozicioniBrojac-2, TipZapisa::R_VN16_PC16, false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
          }
          
        }
        else {
          sekcija->dohvatiTabeluReferisanjaUnapred()->dodajUTabeluReferisanjaUnapred(new NedefinisanSimbol(simbol->dohvatiRedniBroj(), pozicioniBrojac-2, trenutnaSekcija, TipReferisanja::PC_REL));
          sekcija->dodajSadrzajUSekciju("0000");
        }
        simbol->dodajUListuMestaKoriscenja(mesto);
      }
      else if(regex_match(operand, podaciRegistarskoIndirektno)){
        pozicioniBrojac+=3;
        operand = operand.erase(0,1); // obrisi '[' sa pocetka
        operand = operand.erase(operand.size()-1,1); // obrisi ']' sa kraja
        string regS;
        if(operand == "psw") regS = "8";
        else {
          regS = operand.erase(0,1);
        }
        sekcija->dodajSadrzajUSekciju(regD+regS);
        sekcija->dodajSadrzajUSekciju("02");
      }
      else{ // reg indr sa pomerajem
        pozicioniBrojac+=5;
        if(operand.find('[')==string::npos || operand.find(']')==string::npos){
          tekstGreske = "Greska: Pogresan izgled instrukcije ldr";
          return false;
        }
        operand = operand.erase(0,1); // obrisi '[' sa pocetka
        operand = operand.erase(operand.size()-1,1); // obrisi ']' sa kraja
        /* obrisi blanko znake */
        int blankoZnak;
        while((blankoZnak=operand.find(' '))!=string::npos) operand = operand.erase(blankoZnak,1);
        string str;
        if(operand.at(0) == 'r'){
          str = operand.substr(0,2);
        }
        else str = operand.substr(0,3);
        if(regex_match(str,registarR)){
          string regS;
          if(str == "psw"){
            regS = "8";
            operand = operand.erase(0,3);
          }
          else {
            regS = str.erase(0,1);
            operand = operand.erase(0,2);
          }
          sekcija->dodajSadrzajUSekciju(indeksRegD+regS);
          sekcija->dodajSadrzajUSekciju("03");
          if(operand.at(0)!='+'){
            tekstGreske = "Greska: Pogresan izgled instrukcije ldr";
            return false;
          }
          operand = operand.erase(0,1);
          /* proveri da li je u pitanju literal ili simbol */
          if(regex_match(operand, literalR)){
            if(regex_match(operand,literalDecR)){
              string hex = dec2hex(operand);
              int brojNula = 4-hex.size();
              for(int i=0;i<brojNula;i++){
                hex="0"+hex;
              }
              sekcija->dodajSadrzajUSekciju(hex);
            }
            else{
              string hex = operand.erase(0,2);
              if(hex.size()>4){
                tekstGreske="Greska: Heksadecimalan broj "+ hex + " je prevelik.";
                return false;
              }
              int brojNula = 4-hex.size();
              for(int i=0;i<brojNula;i++){
                hex="0"+hex;
              }
              sekcija->dodajSadrzajUSekciju(hex);
            }
          }
          else {
            if(!tabelaSimbola.simbolVecUTabeli(operand)){
              tabelaSimbola.dodajSimbolUTabelu(new Simbol(operand,0,Tip::LOC,"UND"));
            }
            Simbol* simbol = tabelaSimbola.dohvatiSimbol(operand);
            MestoKoriscenja mesto = MestoKoriscenja(pozicioniBrojac, trenutnaSekcija, TipKomande::OSTALE,2,0,-1);
            if(simbol->dohvatiDefinisan()){
              if(simbol->dohvatiTip() == Tip::LOC){
                int vrednost = simbol->dohvatiVrednost();
                string hexZapis = dec2hex(to_string(vrednost));
                if(hexZapis.size() < 4){
                  int brojNula = 4 - hexZapis.size();
                  for(int i = 0; i , brojNula; i++){
                    hexZapis = "0"+hexZapis;
                  }
                }
                sekcija->dodajSadrzajUSekciju(hexZapis);
                string imeSekcije = simbol->dohvatiSekciju();
                int sekcijaID = tabelaSimbola.dohvatiSimbol(imeSekcije)->dohvatiRedniBroj();
                RelokacioniZapis* relZapis = new RelokacioniZapis(sekcijaID,pozicioniBrojac-2, TipZapisa::R_VN16_16,false);
                sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
                mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
              }
              else {
                sekcija->dodajSadrzajUSekciju("0000");
                int simbolID = simbol->dohvatiRedniBroj();
                RelokacioniZapis* relZapis = new RelokacioniZapis(simbolID,pozicioniBrojac-2, TipZapisa::R_VN16_16,false);
                sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
                mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
              }
            }
            else{
              sekcija->dodajSadrzajUSekciju("0000");
              /* dodaj u tabelu referisanja unapred*/
              int simbolID = simbol->dohvatiRedniBroj();
              sekcija->dohvatiTabeluReferisanjaUnapred()->dodajUTabeluReferisanjaUnapred(new NedefinisanSimbol(simbolID,pozicioniBrojac-2,trenutnaSekcija,TipReferisanja::OSTALO));
            }
            simbol->dodajUListuMestaKoriscenja(mesto);
          }
        }
        else {
          tekstGreske = "Greska: Pogresan izgled instrukcije ldr";
          return false;
        }
      }
    }
    /* instrukcije skoka */
    else if(regex_match(linija, instrukcijeSkoka)){
      int blankoZnak = linija.find(' ');
      string instrukcija = linija.substr(0,blankoZnak);
      string operand = linija.substr(blankoZnak+1,linija.size()-1);
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(trenutnaSekcija);
      if(instrukcija == "jmp") sekcija->dodajSadrzajUSekciju("50");
      else if(instrukcija == "jeq") sekcija->dodajSadrzajUSekciju("51");
      else if(instrukcija == "jne") sekcija->dodajSadrzajUSekciju("52");
      else if(instrukcija == "jgt") sekcija->dodajSadrzajUSekciju("53");
      else if(instrukcija == "call") sekcija->dodajSadrzajUSekciju("30");
      string heksadecimalanZapisRegD = "F";
      /* tipovi adresiranja */
      if(regex_match(operand, skokRegistarskoDirekstno)){
        pozicioniBrojac+=3;
        while(operand.find(' ')!=string::npos) operand = operand.erase(0,1);
        operand = operand.erase(0,1); // obrisi '*'
        if(operand == "psw"){
          sekcija->dodajSadrzajUSekciju(heksadecimalanZapisRegD+"8");
        }
        else {
          string regS = operand.erase(0,1);
          sekcija->dodajSadrzajUSekciju(heksadecimalanZapisRegD+regS);
        }
        sekcija->dodajSadrzajUSekciju("01");

      }
      else if(regex_match(operand, skokApsolutnoAdresiranje)){
        sekcija->dodajSadrzajUSekciju(heksadecimalanZapisRegD+"F");
        /* posto je  u pitanju apsolutno(neposredno) adresiranje, velicina instrukcije je 5B*/
        pozicioniBrojac+=5;
        /* u sadrzaj sekcije dodaj 2B koji predstavljaju AddrMode */
        /* AddrMode = UUUU AAAA -> 0000 0000 -> 00 00  */
        sekcija->dodajSadrzajUSekciju("00");
        string literalSimbol = operand;
        /* proveri da li je u pitanju labela ili simbol */
        if(regex_match(literalSimbol, literalR)){ // vraca true ako je u pitanju literal
          /* proveri da li je literal decimalan ili heksadecimalan broj */
          if(regex_match(literalSimbol, literalDecR)){ // u pitanju je decimalan broj
            /* pretvori decimalni u heksadecimalni broj */
            string heksadecimalniZapis = dec2hex(literalSimbol);
            if(heksadecimalniZapis.size() > 4){
              tekstGreske = "Greska: Heksadecimalni zapis broja " + literalSimbol + " je prevelik.";
              return false;
            }
            int brojNula = 4 - heksadecimalniZapis.size();
            for(int i = 0; i < brojNula; i++){
              heksadecimalniZapis = "0" + heksadecimalniZapis;
            }
            /* dodaj u sadrzaj sekcije */
            sekcija->dodajSadrzajUSekciju(heksadecimalniZapis);
          }
          else { // u pitanju je heksadecimalan broj
            string heksadecimalanBroj = literalSimbol.erase(0,2); // obrisi '0x'
            if(heksadecimalanBroj.size()>4){
              tekstGreske = "Greska: Heksadecimalan broj "+literalSimbol+" je preveli.";
              return false;
            }
            int brojNula = 4 - heksadecimalanBroj.size();
            for(int i = 0; i < brojNula; i++){
              heksadecimalanBroj = "0" + heksadecimalanBroj;
            }
            sekcija->dodajSadrzajUSekciju(heksadecimalanBroj);
          }
        }
        else { // u pitanju je simbol
          /* proveri da li se simbol nalazi u tabeli simbola */
          if(!tabelaSimbola.simbolVecUTabeli(literalSimbol)){
            /* ako simbol nije u tabeli dodaj ga */
            tabelaSimbola.dodajSimbolUTabelu(new Simbol(literalSimbol,0,Tip::LOC, "UND"));
          }
          Simbol* simbol = tabelaSimbola.dohvatiSimbol(literalSimbol);
          MestoKoriscenja mesto = MestoKoriscenja(pozicioniBrojac, trenutnaSekcija, TipKomande::OSTALE,5,3,-1);
          /* dodaj novo mesto koriscenja simbola */       
          /* proveri da li je simbol definisan */
          if(simbol->dohvatiDefinisan()){
            /* proveri tip simbola */
            if(simbol->dohvatiTip() == Tip::LOC){
              /* ako je simbol lokalan i defisan onda treba da se u sadrzaj sekcije unese njegova vrednost */
              int vrednost = simbol->dohvatiVrednost();
              string hexZapis = dec2hex(to_string(vrednost));
              if(hexZapis.size() < 4){
                int brojNula = 4 - hexZapis.size();
                for(int i = 0; i , brojNula; i++){
                  hexZapis = "0"+hexZapis;
                }
              }
              sekcija->dodajSadrzajUSekciju(hexZapis);
              /* napravi relokacioni zapis */
              /* posto je u pitanju LOC simbol, u tabeli relokacionih zapisa se stavlja ID sekcije gde je simbol definisan, a ne ID simbola */
              string imeSekcije = simbol->dohvatiSekciju();
              int sekcijaID = tabelaSimbola.dohvatiSimbol(imeSekcije)->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(sekcijaID,pozicioniBrojac-2, TipZapisa::R_VN16_16,false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
            else {
              sekcija->dodajSadrzajUSekciju("0000");
              int simbolID = simbol->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(simbolID,pozicioniBrojac-2, TipZapisa::R_VN16_16,false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
          }
          else { // simbol nije definisan
            sekcija->dodajSadrzajUSekciju("0000");
            /* dodaj u tabelu referisanja unapred*/
            int simbolID = simbol->dohvatiRedniBroj();
            sekcija->dohvatiTabeluReferisanjaUnapred()->dodajUTabeluReferisanjaUnapred(new NedefinisanSimbol(simbolID,pozicioniBrojac-2,trenutnaSekcija,TipReferisanja::OSTALO));
          }
          simbol->dodajUListuMestaKoriscenja(mesto);
        }
      }
      else if(regex_match(operand, skokMemorijskoDirektno)){
        sekcija->dodajSadrzajUSekciju(heksadecimalanZapisRegD+"F");
        /* posto se radi o memorijskom direktnom adresiranju, velicina instrukcije je 5B */
        pozicioniBrojac+=5;
        while(operand.find(' ')!=string::npos) operand = operand.erase(0,1);
        operand = operand.erase(0,1); // obrisi '*'
        /* u sadrzaj sekcije dodaj 2B koji predstavljaju AddrMode */
        /* AddrMode = UUUU AAAA -> 0000 0100 -> 04  */
        sekcija->dodajSadrzajUSekciju("04");
        /* proveri da li je u pitanju literal ili simbol */
        if(regex_match(operand, literalR)){ // u pitanju je literal
          /* proveri da li je u pitanju decimalan broj ili heksadecimalan broj */
          if(regex_match(operand,literalHexR)){ // u pitanju je heksadecimalan broj
            operand = operand.erase(0,2); // obrisi '0x'
            if(operand.size()>4){
              tekstGreske = "Greska: Heksadecimalan broj 0x"+operand+" je preveli.";
              return false;
            }
            int brojNula = 4-operand.size();
            for(int i = 0; i < brojNula; i++){
              operand = "0"+operand;
            }
            sekcija->dodajSadrzajUSekciju(operand);
          }
          else { // u pitanju je decimalan broj
            string hex = dec2hex(operand);
            if(hex.size()>4){
              tekstGreske = "Greska:  broj "+operand+" je preveli.";
              return false;
            }
            int brojNula = 4-hex.size();
            for(int i = 0; i < brojNula; i++){
              hex = "0"+hex;
            }
            sekcija->dodajSadrzajUSekciju(hex);
          }
        }
        else { // u pitanju je simbol
          if(!tabelaSimbola.simbolVecUTabeli(operand)){
            tabelaSimbola.dodajSimbolUTabelu(new Simbol(operand, 0, Tip::LOC, "UND"));
          }
          Simbol* simbol = tabelaSimbola.dohvatiSimbol(operand);
          MestoKoriscenja mesto = MestoKoriscenja(pozicioniBrojac, trenutnaSekcija, TipKomande::OSTALE,5,3,-1);
          if(simbol->dohvatiDefinisan()){
            if(simbol->dohvatiTip() == Tip::LOC){
              int vrednost = simbol->dohvatiVrednost();
              string hexZapis = dec2hex(to_string(vrednost));
              if(hexZapis.size() < 4){
                int brojNula = 4 - hexZapis.size();
                for(int i = 0; i , brojNula; i++){
                  hexZapis = "0"+hexZapis;
                }
              }
              sekcija->dodajSadrzajUSekciju(hexZapis);
              string imeSekcije = simbol->dohvatiSekciju();
              int sekcijaID = tabelaSimbola.dohvatiSimbol(imeSekcije)->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(sekcijaID,pozicioniBrojac-2, TipZapisa::R_VN16_16,false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
            else {
              sekcija->dodajSadrzajUSekciju("0000");
              int simbolID = simbol->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(simbolID,pozicioniBrojac-2, TipZapisa::R_VN16_16,false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
          }
          else{
            sekcija->dodajSadrzajUSekciju("0000");
            /* dodaj u tabelu referisanja unapred*/
            int simbolID = simbol->dohvatiRedniBroj();
            sekcija->dohvatiTabeluReferisanjaUnapred()->dodajUTabeluReferisanjaUnapred(new NedefinisanSimbol(simbolID,pozicioniBrojac-2,trenutnaSekcija,TipReferisanja::OSTALO));
          }
          simbol->dodajUListuMestaKoriscenja(mesto);
        }
      }
      else if(regex_match(operand, skokPCRelativno)){
        sekcija->dodajSadrzajUSekciju(heksadecimalanZapisRegD+"7");
        pozicioniBrojac+=5;
        operand = operand.erase(0,1); // obrisi ' %'
        /* AddrMode = UUUU AAAA -> 0000 0101 -> 05 */
        sekcija->dodajSadrzajUSekciju("05");
        if(!tabelaSimbola.simbolVecUTabeli(operand)){
          tabelaSimbola.dodajSimbolUTabelu(new Simbol(operand, 0, Tip::LOC, "UND"));
        }
        Simbol* simbol = tabelaSimbola.dohvatiSimbol(operand);
        MestoKoriscenja mesto = MestoKoriscenja(pozicioniBrojac-2, trenutnaSekcija, TipKomande::KOMANDA_PC_REL,5,0,-1);
        if(simbol->dohvatiDefinisan()){
          /* proveri da li se simbol definisan u istoj sekciji gde je i koriscen */
          if(simbol->dohvatiSekciju() == trenutnaSekcija){ // definisan je u istoj sekciji gde je koriscen
            /* ukoliko je simbol definisan u istoj sekciji gde je i koriscen onda u Data deo instukcije treba upisati razliku izmedju adresa simbola i pozicionogBrojaca */
            int broj = simbol->dohvatiVrednost() - pozicioniBrojac;
            string hex = dec2hex(to_string(broj));
            sekcija->dodajSadrzajUSekciju(hex);
          }
          else { // nijee definisan u istoj sekciji gde je koriscen
            /* proveri tip simbola */
            if(simbol->dohvatiTip() == Tip::LOC){
              int addend = simbol->dohvatiVrednost() - 2;
              string hex = dec2hex(to_string(addend));
              if(hex.size()<4){
                int brojNula = 4-hex.size();
                for(int i = 0; i<brojNula; i++){
                  hex="0"+hex;
                }
              }
              sekcija->dodajSadrzajUSekciju(hex);
              string imeSekcije = simbol->dohvatiSekciju();
              int sekcijaID = tabelaSimbola.dohvatiSimbol(imeSekcije)->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(sekcijaID, pozicioniBrojac-2, TipZapisa::R_VN16_PC16, false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
            else {
              sekcija->dodajSadrzajUSekciju(dec2hex("-2"));
              RelokacioniZapis* relZapis = new RelokacioniZapis(simbol->dohvatiRedniBroj(), pozicioniBrojac-2, TipZapisa::R_VN16_PC16, false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
              mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
            }
          }
          
        }
        else {
          sekcija->dohvatiTabeluReferisanjaUnapred()->dodajUTabeluReferisanjaUnapred(new NedefinisanSimbol(simbol->dohvatiRedniBroj(), pozicioniBrojac-2, trenutnaSekcija, TipReferisanja::PC_REL));
          sekcija->dodajSadrzajUSekciju("0000");
        }
        simbol->dodajUListuMestaKoriscenja(mesto);
      }
      else if(regex_match(operand, skokRegistarskoIndirektno)){
        pozicioniBrojac+=3;
        operand = operand.erase(0,2); // obrisi '*[' sa pocetka
        operand = operand.erase(operand.size()-1,1); // obrisi ']' sa kraja
        string regS;
        if(operand == "psw") regS = "8";
        else {
          regS = operand.erase(0,1);
        }
        sekcija->dodajSadrzajUSekciju(heksadecimalanZapisRegD+regS);
        sekcija->dodajSadrzajUSekciju("02");
      }
      else{ // reg indr sa pomerajem
        pozicioniBrojac+=5;
        if(operand.find('[')==string::npos || operand.find(']')==string::npos || operand.find('*')==string::npos){
          tekstGreske = "Greska: Pogresan izgled instrukcije " + instrukcija;
          return false;
        }
        operand = operand.erase(0,2); // obrisi '*[' sa pocetka
        operand = operand.erase(operand.size()-1,1); // obrisi ']' sa kraja
        /* obrisi blanko znake */
        int blankoZnak;
        while((blankoZnak=operand.find(' '))!=string::npos) operand = operand.erase(blankoZnak,1);
        string str;
        if(operand.at(0) == 'r'){
          str = operand.substr(0,2);
        }
        else str = operand.substr(0,3);
        if(regex_match(str,registarR)){
          string regS;
          if(str == "psw"){
            regS = "8";
            operand = operand.erase(0,3);
          }
          else {
            regS = str.erase(0,1);
            operand = operand.erase(0,2);
          }
          sekcija->dodajSadrzajUSekciju(heksadecimalanZapisRegD+regS);
          sekcija->dodajSadrzajUSekciju("03");
          if(operand.at(0)!='+'){
            tekstGreske = "Greska: Pogresan izgled instrukcije " + instrukcija;
            return false;
          }
          operand = operand.erase(0,1);
          /* proveri da li je u pitanju literal ili simbol */
          if(regex_match(operand, literalR)){
            if(regex_match(operand,literalDecR)){
              string hex = dec2hex(operand);
              int brojNula = 4-hex.size();
              for(int i=0;i<brojNula;i++){
                hex="0"+hex;
              }
              sekcija->dodajSadrzajUSekciju(hex);
            }
            else{
              string hex = operand.erase(0,2);
              if(hex.size()>4){
                tekstGreske="Greska: Heksadecimalan broj "+ hex + " je prevelik.";
                return false;
              }
              int brojNula = 4-hex.size();
              for(int i=0;i<brojNula;i++){
                hex="0"+hex;
              }
              sekcija->dodajSadrzajUSekciju(hex);
            }
          }
          else {
            if(!tabelaSimbola.simbolVecUTabeli(operand)){
              tabelaSimbola.dodajSimbolUTabelu(new Simbol(operand,0,Tip::LOC,"UND"));
            }
            Simbol* simbol = tabelaSimbola.dohvatiSimbol(operand);
            MestoKoriscenja mesto = MestoKoriscenja(pozicioniBrojac, trenutnaSekcija, TipKomande::OSTALE,2,0,-1);
            if(simbol->dohvatiDefinisan()){
              if(simbol->dohvatiTip() == Tip::LOC){
                int vrednost = simbol->dohvatiVrednost();
                string hexZapis = dec2hex(to_string(vrednost));
                if(hexZapis.size() < 4){
                  int brojNula = 4 - hexZapis.size();
                  for(int i = 0; i , brojNula; i++){
                    hexZapis = "0"+hexZapis;
                  }
                }
                sekcija->dodajSadrzajUSekciju(hexZapis);
                string imeSekcije = simbol->dohvatiSekciju();
                int sekcijaID = tabelaSimbola.dohvatiSimbol(imeSekcije)->dohvatiRedniBroj();
                RelokacioniZapis* relZapis = new RelokacioniZapis(sekcijaID,pozicioniBrojac-2, TipZapisa::R_VN16_16,false);
                sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
                mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
              }
              else {
                sekcija->dodajSadrzajUSekciju("0000");
                int simbolID = simbol->dohvatiRedniBroj();
                RelokacioniZapis* relZapis = new RelokacioniZapis(simbolID,pozicioniBrojac-2, TipZapisa::R_VN16_16,false);
                sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
                mesto.rbZapisa = relZapis->dohvatiRedniBrojRelokacionigZapisa();
              }
            }
            else{
              sekcija->dodajSadrzajUSekciju("0000");
              /* dodaj u tabelu referisanja unapred*/
              int simbolID = simbol->dohvatiRedniBroj();
              sekcija->dohvatiTabeluReferisanjaUnapred()->dodajUTabeluReferisanjaUnapred(new NedefinisanSimbol(simbolID,pozicioniBrojac-2,trenutnaSekcija,TipReferisanja::OSTALO));
            }
            simbol->dodajUListuMestaKoriscenja(mesto);
          }
        }
        else {
          tekstGreske = "Greska: Pogresan izgled instrukcije ldr";
          return false;
        }
      }
    }
    else {
      cout<<linija<<endl;
      tekstGreske = "Greska: Los format instrukcije!";
      return false;
    }
    linijaKoda++;
  }
  if(!backpatching()){
    tekstGreske = "Greska: Nisu svi simboli definisani!";
    return false;
  }
  tabelaSimbola.ispisiTabeluSimbola();
  tabelaSekcija.ispisiTabeluSekcija();
  tabelaSekcija.ispisiRelokacioneZapiseSekcija();
  return true;
}

bool Asembler::backpatching(){
  /* prolaz kroz tabelu simbola */
  for(int i = 0; i < tabelaSimbola.dohvatiTabeluSimbola().size(); i++){
    /* dohvati tekuci simbol */
    Simbol* simbol = tabelaSimbola.dohvatiSimbol(i);
    int simbolID = simbol->dohvatiRedniBroj();
    /* preskoci nulti simbol */
    if(i == 0) continue;
    /* ako je simbol extern ili nedefinisan, preskoci */
    if(!simbol->dohvatiDefinisan() && simbol->dohvatiTip()!=Tip::EXT) return false;
    /* prolaz kroz tabelu sekcija */
    for(int j = 0; j < tabelaSekcija.dohvatiTabeluSekcija().size(); j++){
      /* dohvati tekucu sekcija */
      Sekcija* sekcija = tabelaSekcija.dohvatiSekciju(j);
      /* uzmi tabelu referisanja unapred svake sekcije */
      TabelaReferisanjaUnapred* tabelaReferisanjaUnapred = sekcija->dohvatiTabeluReferisanjaUnapred();
      /* preskoci ako je tabela prazna */
      if(tabelaReferisanjaUnapred->velicinaTabeleReferisanjaUnapred() == 0) continue; 
      /* proveri da li se simbol sa zadatim ID-jem nalazi u tabeli referisanja unapred */
      if(!tabelaReferisanjaUnapred->simbolUTabeliReferisanjaUnapred(simbolID)) continue; // preskoci ako se ne nalazi u tabeli referisanja unapred
      /* simbol se nalazi u tabeli referisanja unapred */
      /* prolaz kroz tabelu referisanja unapred */
      for(int k = 0; k < tabelaReferisanjaUnapred->velicinaTabeleReferisanjaUnapred(); k++){
        /* u tabeli referisanja unapred pronadji simbol sa istim ID-jem */
        NedefinisanSimbol* nedefinisanSimbol = tabelaReferisanjaUnapred->dohvatiElementTabele(k);
        if(simbolID == nedefinisanSimbol->dohvatiID()){
          /* dohvati tip referisanja */
          TipReferisanja tip = nedefinisanSimbol->dohvatiTipReferisanja();
          /*
            u zavisnoti od tipa referisanja razlikuje se nacin obrade, WORD i OSTALO se razlikuju u tome sto se kod 
            WORD-a koristi little-endian, dok se kod ostalih nacina adresiranja(svi osim PC relativnog) koristi
            big-endian format, PC_REL se na poseban nacin obradjuje 
          */
          switch (tip)
          {
          case WORD:{
            string sadrzajSekcije = sekcija->dohvatiSadrzajSekcije();
            int pomeraj = nedefinisanSimbol->dohvatiAdresu();
            /* proveri tip simbola */
            if(simbol->dohvatiTip() == Tip::LOC){
              /* ukoliko je rec od lokalnom simbolu upisi trenutnu vrednost simbola u sadrzaj sekcije */
              int vrednost = simbol->dohvatiVrednost();
              string heksadecimalniZapis = dec2hex(to_string(vrednost));
              if(heksadecimalniZapis.size()<4){
                  int brojNula = 4 - heksadecimalniZapis.size();
                  for(int m = 0; m < brojNula; m++){
                    heksadecimalniZapis = "0" + heksadecimalniZapis;
                  }
              }
              string prvi = heksadecimalniZapis.substr(0,2);
              string drugi = heksadecimalniZapis.substr(2,3);
              heksadecimalniZapis = drugi+prvi;
              sadrzajSekcije = sadrzajSekcije.erase(pomeraj*2, 4);
              sadrzajSekcije.insert(pomeraj*2, heksadecimalniZapis);
              sekcija->postaviSadrzajSekcije(sadrzajSekcije);
              /* napravi relokacioni zapis */
              /* posto je rec o lokalnom simbolu u tabeli relokacionih zapisa ne treba da stoji ID simbola vec ID sekcije u kojoj je simbol definisan */
              string imeSekcijeUKojojJeSimbolDefinisan = tabelaSimbola.dohvatiSimbol(simbol->dohvatiIme())->dohvatiSekciju();
              int sekcijaID = tabelaSimbola.dohvatiSimbol(imeSekcijeUKojojJeSimbolDefinisan)->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(sekcijaID,pomeraj,TipZapisa::R_VN16_16,true);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
            }
            else{ // globalni ili lokalni
              sadrzajSekcije = sadrzajSekcije.erase(pomeraj*2, 4);
              /* posto je rec o globalnom/eksternom simbolu u sadrzaj sekcije se upisuje 0000, ostavlja se linkeru da promeni */
              sadrzajSekcije.insert(pomeraj*2, "0000");
              sekcija->postaviSadrzajSekcije(sadrzajSekcije);
              /* napravi relokacioni zapis */
              RelokacioniZapis* relZapis = new RelokacioniZapis(simbolID,pomeraj,TipZapisa::R_VN16_16,true);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
            }
            break;
          }
          case PC_REL:{
            string sadrzajSekcije = sekcija->dohvatiSadrzajSekcije();
            int pomeraj = nedefinisanSimbol->dohvatiAdresu();
            if(simbol->dohvatiSekciju() == sekcija->dohvatiImeSekcije()){
              int broj = simbol->dohvatiVrednost() - (pomeraj+2);
              string hex = dec2hex(to_string(broj));
              if(hex.size()<4){
                int brojNula = 4 -hex.size();
                for(int l = 0; l<brojNula; l++){
                  hex = "0"+hex;
                }
              }
              sadrzajSekcije = sadrzajSekcije.erase(pomeraj*2, 4);
              sadrzajSekcije.insert(pomeraj*2, hex);
              sekcija->postaviSadrzajSekcije(sadrzajSekcije);
            }
            else {
              if(simbol->dohvatiTip() == Tip::LOC){
                int addend = simbol->dohvatiVrednost() - 2;
                string hex = dec2hex(to_string(addend));
                if(hex.size()<4){
                  int brojNula = 4-hex.size();
                  for(int i = 0; i<brojNula; i++){
                    hex="0"+hex;
                  }
                }
                sadrzajSekcije = sadrzajSekcije.erase(pomeraj*2, 4);
                sadrzajSekcije.insert(pomeraj*2, hex);
                sekcija->postaviSadrzajSekcije(sadrzajSekcije);
                string imeSekcije = simbol->dohvatiSekciju();
                int sekcijaID = tabelaSimbola.dohvatiSimbol(imeSekcije)->dohvatiRedniBroj();
                sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(new RelokacioniZapis(sekcijaID, pomeraj, TipZapisa::R_VN16_PC16, false));
              }
              else {
                string hex = dec2hex("-2");
                sadrzajSekcije = sadrzajSekcije.erase(pomeraj*2, 4);
                sadrzajSekcije.insert(pomeraj*2, hex);
                sekcija->postaviSadrzajSekcije(sadrzajSekcije);
                sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(new RelokacioniZapis(simbol->dohvatiRedniBroj(), pomeraj, TipZapisa::R_VN16_PC16, false));
              }
            }
            break;
          }
          case OSTALO:{
            string sadrzajSekcije = sekcija->dohvatiSadrzajSekcije();
            int pomeraj = nedefinisanSimbol->dohvatiAdresu();
            /* proveri tip simbola */
            if(simbol->dohvatiTip() == Tip::LOC){
              /* ukoliko je rec od lokalnom simbolu upisi trenutnu vrednost simbola u sadrzaj sekcije */
              int vrednost = simbol->dohvatiVrednost();
              string heksadecimalniZapis = dec2hex(to_string(vrednost));
              if(heksadecimalniZapis.size()<4){
                  int brojNula = 4 - heksadecimalniZapis.size();
                  for(int m = 0; m < brojNula; m++){
                    heksadecimalniZapis = "0" + heksadecimalniZapis;
                  }
              }
              string prvi = heksadecimalniZapis.substr(0,2);
              string drugi = heksadecimalniZapis.substr(2,3);
              heksadecimalniZapis = prvi+drugi;
              sadrzajSekcije = sadrzajSekcije.erase(pomeraj*2, 4);//
              sadrzajSekcije.insert(pomeraj*2, heksadecimalniZapis);//
              sekcija->postaviSadrzajSekcije(sadrzajSekcije);
              /* napravi relokacioni zapis */
              /* posto je rec o lokalnom simbolu u tabeli relokacionih zapisa ne treba da stoji ID simbola vec ID sekcije u kojoj je simbol definisan */
              string imeSekcijeUKojojJeSimbolDefinisan = tabelaSimbola.dohvatiSimbol(simbol->dohvatiIme())->dohvatiSekciju();
              int sekcijaID = tabelaSimbola.dohvatiSimbol(imeSekcijeUKojojJeSimbolDefinisan)->dohvatiRedniBroj();
              RelokacioniZapis* relZapis = new RelokacioniZapis(sekcijaID,pomeraj,TipZapisa::R_VN16_16,false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
            }
            else{ // globalni ili lokalni
              sadrzajSekcije = sadrzajSekcije.erase(pomeraj*2, 4);//
              /* posto je rec o globalnom/eksternom simbolu u sadrzaj sekcije se upisuje 0000, ostavlja se linkeru da promeni */
              sadrzajSekcije.insert(pomeraj*2, "0000");//
              sekcija->postaviSadrzajSekcije(sadrzajSekcije);
              /* napravi relokacioni zapis */
              RelokacioniZapis* relZapis = new RelokacioniZapis(simbolID,pomeraj,TipZapisa::R_VN16_16,false);
              sekcija->dohvatiTabeluRelokacionihZapisa()->dodajUTabeluRelokacionihZapisa(relZapis);
            }
            break;
          }
          default:
            break;
          }
        }
      }
    }
  }
  return true;
}

void Asembler :: upisiUIzlazniFajl() {
  list<Simbol*> listaSimbola = tabelaSimbola.dohvatiTabeluSimbola();
  izlazniFajl<<"#.symtab."<<dec2hex(to_string(listaSimbola.size()))<<endl;
  list<Simbol*>::iterator it;
  for(it = listaSimbola.begin(); it != listaSimbola.end(); ++it){
    Simbol* simbol = (*it);
    int redniBroj = simbol->dohvatiRedniBroj();
    string vrednost = dec2hex(to_string(simbol->dohvatiVrednost()));
    if(vrednost.size()<4){
      int brojNula = 4 - vrednost.size();
      for(int i =0; i<brojNula; i++) vrednost = "0" + vrednost;
    }
    Tip tip = simbol->dohvatiTip();
    string tipString = "";
    if(tip == LOC) tipString = "LOC";
    else if(tip == GLOB) tipString = "GLOB";
    else tipString = "EXT";
    string sekcija = simbol->dohvatiSekciju();
    string ime = simbol->dohvatiIme();
    izlazniFajl<<redniBroj<<" "<<vrednost<<" "<<tipString<<" "<<sekcija<<" "<<ime<<endl;
  }
  list<Sekcija*> listaSekcija = tabelaSekcija.dohvatiTabeluSekcija();
  list<Sekcija*>::iterator it2;
  for(it2 = listaSekcija.begin(); it2 != listaSekcija.end(); ++it2){
    Sekcija* sekcija = (*it2);
    izlazniFajl<<endl<<"#"+sekcija->dohvatiImeSekcije()+"."<<dec2hex(to_string(sekcija->dohvatiVelicinuSekcije()))<<endl;
    izlazniFajl<<sekcija->dohvatiSadrzajSekcije();
  }
  izlazniFajl<<endl;
  izlazniFajl<<endl;
  for(it2 = listaSekcija.begin(); it2 != listaSekcija.end(); ++it2){
    Sekcija* sekcija = (*it2);
    list<RelokacioniZapis*> listaRelZapisa = (*it2)->dohvatiTabeluRelokacionihZapisa()->dohvatiTabeluRelokacionihZapisa();
    izlazniFajl<<"#.rela."<<sekcija->dohvatiImeSekcije()<<"."<<dec2hex(to_string(listaRelZapisa.size()))<<endl;
    list<RelokacioniZapis*>::iterator it3;
    for(it3 = listaRelZapisa.begin(); it3 != listaRelZapisa.end(); ++it3){
      RelokacioniZapis* relZapis = (*it3);
      string pomeraj = dec2hex(to_string(relZapis->dohvatiOfset()));
      if(pomeraj.size()<4){
        int brojNula = 4 - pomeraj.size();
        for(int i =0; i<brojNula; i++) pomeraj = "0" + pomeraj;
      }
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
      Simbol* simbol = tabelaSimbola.dohvatiSimbol(relZapis->dohvatiID());
      izlazniFajl<<pomeraj<<" "<<tip<<" "<< relZapis->dohvatiID()<<" ("<<simbol->dohvatiIme()<<") "<< endian<<endl;
    }
  }

}

void Asembler :: pokreni() {
  ucitajKod();
  if(!analizirajKod()){
    cout<<tekstGreske<<endl;
    exit(-1);
  }
  upisiUIzlazniFajl();
}