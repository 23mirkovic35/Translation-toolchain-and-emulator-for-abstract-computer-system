#ifndef ASSEMBLER_HPP_
#define ASSEMBLER_HPP_

#include "TabelaSimbola.hpp"
#include "TabelaSekcija.hpp"
#include "RelokacioniZapis.hpp"
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iostream>
#include <list>
#include <regex>
#include <math.h>
#include <bits/stdc++.h>

using namespace std;

class Asembler {

private:
  ifstream ulazniFajl;
  ofstream izlazniFajl;
  list<string> kodIzFajla;
  string tekstGreske;
  int linijaKoda = 0;
  TabelaSimbola tabelaSimbola;
  TabelaSekcija tabelaSekcija;
  string trenutnaSekcija;
  int pozicioniBrojac;
  int hex2dec(string);
  string dec2hex(string);
public:
  Asembler(string ulaz, string izlaz);
  void zatvoriFajlove();
  void pokreni();
  void ucitajKod(); /* ucitaj kod iz fajla u listu i formatiraj ga */
  bool analizirajKod();
  void upisiUIzlazniFajl();
  void ispisTabeleSimbola();
  void ispisTabeleSekcija();
  void ispisTabeleRelokacionihZapisa();
  bool backpatching();
  
  string literalDec = "-?[0-9]+";
  string literalHex = "0x[0-9A-Fa-f]+";
  string simbol = "[a-zA-Z][a-zA-Z0-9_]*";
  string litIliSimb = literalDec + "|" + literalHex + "|" + simbol;
  string literal = literalDec + "|" + literalHex;
  string registar = "r[0-7]|psw";
  regex literalR{ "^" + literal + "$" };
  regex prazanRed{"^\\s*$"};
  regex literalDecR{ "^" + literalDec + "$" };
  regex literalHexR{ "^" + literalHex + "$" };
  regex simbolR{ "^" + simbol + "$" };  
  regex registarR{ "^" + registar + "$" };  
  regex direktivaExtern {"^\\.extern (" + simbol + "(, " + simbol + ")*)$"};
  regex direktivaGlobal {"^\\.global (" + simbol + "(, " + simbol + ")*)$"};
  regex direktivaWord{ "^\\.word (" + litIliSimb + ")(, (" + litIliSimb + "))*$" };
  regex direktivaSection{ "^\\.section " + simbol + "$" };
  regex direktivaSkip{ "^\\.skip (" + literal + ")$" };
  regex direktivaEnd{ "^\\.end$" };

  regex labela{ "^" + simbol + ":$" };
  
  /* asemblerske naredbe - bez operanada */
  regex haltInstrukcija{"^halt$"};
  regex retInstrukcija{"^ret$"};
  regex iretInstrukcija{"^iret$"};

  /* asemblerske naredbe - jedan registar */
  regex intInstrukcija{ "^int (r[0-7]|psw)$" }; 
  regex pushInstrukcija{ "^push (r[0-7]|psw)$" };
  regex popInstrukcija{ "^pop (r[0-7]|psw)$" };
  regex notInstrukcija{ "^not (r[0-7]|psw)$" };

  /* asemblerske naredbe - dva registara */
  regex xchgInstrukcija{ "^xchg (r[0-7]|psw), (r[0-7]|psw)$" };
  regex addInstrukcija{ "^add (r[0-7]|psw), (r[0-7]|psw)$" };
  regex subInstrukcija{ "^sub (r[0-7]|psw), (r[0-7]|psw)$" };
  regex mulInstrukcija{ "^mul (r[0-7]|psw), (r[0-7]|psw)$" };
  regex divInstrukcija{ "^div (r[0-7]|psw), (r[0-7]|psw)$" };
  regex cmpInstrukcija{ "^cmp (r[0-7]|psw), (r[0-7]|psw)$" };
  regex andInstrukcija{ "^and (r[0-7]|psw), (r[0-7]|psw)$" };
  regex orInstrukcija{ "^or (r[0-7]|psw), (r[0-7]|psw)$" };
  regex xorInstrukcija{ "^xor (r[0-7]|psw), (r[0-7]|psw)$" };
  regex testInstrukcija{ "^test (r[0-7]|psw), (r[0-7]|psw)$" };
  regex shlInstrukcija{ "^shl (r[0-7]|psw), (r[0-7]|psw)$" };
  regex shrInstrukcija{ "^shr (r[0-7]|psw), (r[0-7]|psw)$" };

  regex ldrInstrukcija{ "^ldr ("+registar+"), (.*)$"};
  regex strInstrukcija{ "^str ("+registar+"), (.*)$"};

  /* instrukcije skoka */
  regex instrukcijeSkoka{ "^(jmp|jeq|jne|jgt|call) (.*)$" };
  regex jmpInstrukcija{ "^jmp (.*)$" };
  regex jeqInstrukcija{ "^jeq (.*)$" };
  regex jneInstrukcija{ "^jne (.*)$" };
  regex jgtInstrukcija{ "^jgt (.*)$" };
  regex callInstrukcija{ "^call (.*)$" };

  /* nacini adresiranja za ldr i str */
  regex podaciApsolutnoAdresiranje{ "^\\$(" + litIliSimb + ")$" };
  regex podaciMemorijskoDirektno{ "^(" + litIliSimb + ")$"};
  regex podaciPCRelativno{ "^%(" + simbol + ")$"};
  regex podaciRegistarskoDirekstno{ "^(" + registar + ")$" };
  regex podaciRegistarskoIndirektno{"^\\[(" + registar + ")\\]$"};
  regex podaciRegistarskoIndirektnoSaPomerajem{ "^\\[(" + registar + ")\\+(" + litIliSimb + ")\\]$" };

  /* nacini adresiranja za instrukcije skokova */
  regex skokApsolutnoAdresiranje{ "^(" + litIliSimb + ")$" };
  regex skokPCRelativno{ "^%(" + simbol + ")$" };
  regex skokMemorijskoDirektno{ "^\\*(" + litIliSimb + ")$" };
  regex skokRegistarskoDirekstno{ "^\\*(" + registar + ")$" };
  regex skokRegistarskoIndirektno{ "^\\*\\[(" + registar + ")\\]$" };
  regex skokRegIndDisp{ "^\\*\\[(" + registar + ")\\+("+ litIliSimb +")\\]$" };

};

#endif