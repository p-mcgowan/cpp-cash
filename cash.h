#include <iostream>
#include <string>
#include <cmath>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include <map>
#include "tokenize.h"
#include <iomanip>  // right justify (setw, std::right)
#include <ctime>

#if defined __ANDROID__ || defined __lonux__ || defined __APPLE__&&__MACH__
  int mkdir(std::string d) {
    system(("mkdir -p " + d).c_str());
    return 1;
  }
#elif defined __WINDOWS__
  int mkdir(std::string d) {
//    system(("mkdir -p " + d).c_str());
    return 1;
  }
#else
  int mkdir(std::string d) {
    std::cout << "OS unsupported - directories unavaliable." << std::endl;
    return 1;
  }
#endif

// TODO: remove - debug
#define logd(msg) std::cout << msg;
#define logds(msg) std::cout << msg << ' ';
#define logdn(msg) std::cout << msg << std::endl;
#define errm(msg) logdn(msg); exit(-1);

typedef struct cashset {
  int coin[11];
  int total;
  char type;
} cashSet;

std::string coinNames[11] = {
  "Penny",
  "Nickel",
  "Dime",
  "Quarter",
  "Loonie",
  "Twonie",
  "Five",
  "Ten",
  "Twenty",
  "Fifty",
  "Hundred"
};

float fCoinVal[11] = {
  0.01,
  0.05,
  0.10,
  0.25,
  1.00,
  2.00,
  5.00,
  10.00,
  20.00,
  50.00,
  100.00
};
int iCoinVal[11] = {
  1,
  5,
  10,
  25,
  100,
  200,
  500,
  1000,
  2000,
  5000,
  10000
};
int iRollVal[11] = {
  50,
  200,
  500,
  1000,
  100,
  200,
  500,
  1000,
  2000,
  5000,
  10000
};
float fRollVal[11] = {
  0.50,
  2.00,
  5.00,
  10.00,
  25.00,
  50.00,
  5.00,
  10.00,
  20.00,
  50.00,
  100.00
};

#define FLOAT 35000
#define BUFFER_SIZE 128

#define PENNY 0
#define NICKEL 1
#define DIME 2
#define QUARTER 3
#define LOONIE 4
#define TWONIE 5
#define FIVE 6
#define TEN 7
#define TWENTY 8
#define FIFTY 9
#define HUNDRED 10

inline bool fExists(std::string f) {
  return (access(f.c_str(), F_OK) != -1);
}
int min(int a, int b) {return (a < b)? a : b;}
void getCoinbox(cashSet *bal, std::vector<int> params);
std::string parseArgs(std::vector<std::string> token,
  std::map<std::string, std::string> *opts, std::vector<int> *params);
int getCoinboxTotal(cashSet *bal);
void getBalance(cashSet *bal, std::vector<int> params);
void getDeposit(cashSet *bal, cashSet *dep);
void getFloat(cashSet *bal, cashSet *dep, cashSet *flt);
int getTotal(cashSet *sum);
void printCoinBox(cashSet *bal);
void printTot(cashSet *bal);
void printBal(cashSet *bal);
void printCash(cashSet *bal, cashSet *dep, cashSet *flt);
void fPrintConfig(cashSet *bal, cashSet *dep, cashSet *flt, std::string path);
void fPrintConf(cashSet *bal, std::string set, std::ofstream &conf);
void fPrintCoinConfig(cashSet *bal, std::string path);
void fPrintCash(cashSet *bal, cashSet *dep, cashSet *flt, std::string path);
void fPrintBal(cashSet *bal, std::string setName, std::ofstream &output);
std::string toCurrency(int b);
std::string getDate(const char *format);
void usage();
std::string getPath(std::string o, std::string t);

#define A_VALID -1
#define A_USAGE -2
