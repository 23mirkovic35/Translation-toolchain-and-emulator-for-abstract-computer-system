#ifndef EMULATOR_HPP_
#define EMULATOR_HPP_

#include <list>
#include <string>
#include <iostream>
#include <bits/stdc++.h>
#include <sstream>
#include <fstream>
#include <bitset>
using namespace std;

typedef enum{
  HALT = 0x0,
  INT = 0x10,
  IRET = 0x20,
  CALL = 0x30,
  RET = 0x40,
  JMP = 0x50,
  JEQ = 0x51,
  JNE = 0x52,
  JGT = 0x53,
  PUSH_STR = 0xB0,
  POP_LDR = 0xA0,
  XCHG = 0x60,
  ADD = 0x70,
  SUB = 0x71,
  MUL = 0x72,
  DIV = 0x73,
  CMP = 0x74,
  NOT = 0x80,
  AND = 0x81,
  OR = 0x82,
  XOR = 0x83,
  TEST = 0x84,
  SHL = 0x90,
  SHR = 0x91
}Instrukcija;

typedef enum{
  IMMED = 0,
  REG_DIR = 1,
  REG_IND = 2,
  REG_IND_POMERAJ = 3,
  MEM = 4,
  PC_REL = 5
}TipAdresiranja;

typedef struct Registri{
  unsigned short r[6] = { 0 };
	unsigned short PC, SP, PSW;
}Registri;

typedef struct Ulaz{
  int adresa;
  string sadrzaj;
  Ulaz(int adresa, string sadrzaj){
    this->adresa = adresa;
    this->sadrzaj = sadrzaj;
  }
}Ulaz;

class Emulator{

private:
  ifstream ulazniFajl;
  list<Ulaz> podaciIzDatoteke;
  Registri reg;
  Instrukcija instrukcija;
  unsigned short memorija[1<<16] ={0};
  short operand;
  unsigned short RegsDescr,AddrMode;
  unsigned short regD,regS;
  int hex2dec(string);
  string dec2hex(string);
  TipAdresiranja odrediNacinAdresiranja(unsigned short addrMode);
  bool prekid = false;
public:
  Emulator(string);
  void pokreni();
  void obradiUlaznuDatoteku();
  void popuniMemoriju();
  void izvrsiInstrukcije();
  void ispisiResenje();
};

#endif