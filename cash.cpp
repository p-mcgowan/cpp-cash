/**
 * c++ cashout program
 * p-mcgowan
 */
#include "cash.h"

int main (int argc, char **argv) {
  std::cout.precision(2);
  cashSet *cBalance = (cashSet*)malloc(sizeof(cashSet));
  cashSet *cDeposit = (cashSet*)malloc(sizeof(cashSet));
  cashSet *cFloat = (cashSet*)malloc(sizeof(cashSet));

  std::vector<std::string> token = tokenize(argv, argc);
  std::map<std::string, std::string> opts;
  std::vector<int> params;

  std::string invalid = parseArgs(token, &opts, &params);
  if (invalid != "") {
    if (invalid != "help") {
      std::cout << invalid << std::endl;
    }
    usage();
    return 1;
  }

  if (opts["coin"] != "") {
    getCoinbox(cBalance, params);

    if (opts["config"] != "") {
      logdn("writing config to " << opts["config"]);
      fPrintCoinConfig(cBalance, opts["config"]);
    }
    printCoinBox(cBalance);

  } else {
    getBalance(cBalance, params);
    getDeposit(cBalance, cDeposit);
    getFloat(cBalance, cDeposit, cFloat);

    if (opts["config"] != "") {
      logdn("writing config to " << opts["config"]);
      fPrintConfig(cBalance, cDeposit, cFloat, opts["config"]);
    }
    printCash(cBalance, cDeposit, cFloat);
  }

  if (opts["silent"] == "false") {
    std::string path = getPath(opts["output"], opts["till"]);
    logdn("writing to " << path);
    fPrintCash(cBalance, cDeposit, cFloat, path);
  }

  free(cBalance);
  free(cDeposit);
  free(cFloat);
  return 0;
}


// TODO
//
std::string getPath(std::string o, std::string t) {
  std::string path;
  if (o != "") {
    path = o;
    if (fExists(path)) {
      std::cout << "File " << path << " exists, overwrite [y/N]? ";
      std::string input;
      std::cin >> input;
      if (!input.compare("y") || !input.compare("Y")) {
        return path;
      } else {
        return "";
      }
    }
  } else {
    path = getDate("ym") + '/' + getDate("ymd.HM") + t;
    if (fExists(path)) {
      path = getDate("ym") + '/' + getDate("ymd.HMS") + t;
    } else {
      mkdir(getDate("ym"));
    }
  }
  return path;
}


//TODO
//
std::string parseArgs(std::vector<std::string> token,
  std::map<std::string, std::string> *opts, std::vector<int> *params) {

  for (std::vector<std::string>::iterator it = token.begin();
       it < token.end(); it++) {
    if (*it == "--config" || *it == "-c") {
      if (it + 1 == token.end() || (*(it + 1))[0] == '-') {
        (*opts)["config"] = "cashConfig.txt";
      } else {
        (*opts)["config"] = *(++it);
      }
    } else if (*it == "--silent" || *it == "-s") {
      (*opts)["silent"] = "false";
    } else if (*it == "--coin" || *it == "-b") {
      (*opts)["coin"] = "true";
    } else if (*it == "--till" || *it == "-t") {
      if (it + 1 == token.end() || (*(it + 1))[0] == '-') {
        return *it + " requires an argument";
      } else {
        (*opts)["till"] = *(++it);
      }
    } else if (*it == "--help" || *it == "-h") {
      return "help";
    } else if (*it == "--output" || *it == "-o") {
      if (it + 1 == token.end() || (*(it + 1))[0] == '-') {
        //opts["output"] = "";
      } else {
        (*opts)["output"] = *(++it);
      }
    } else {
      if ((*it)[0] == '-') {
        return "invalid-- " + *it;
      } else {
        params->insert(params->end(), atoi((*it).c_str()));
      }
    }
  }
  return "";
}

void getCoinbox(cashSet *bal, std::vector<int> params) {
  if (params.size() == 11) {
    for (int i = 0; i < 11; i++) {
      bal->coin[i] = params[i];
    }
  } else {
    std::cout << "Enter number of rolls or bills in the coinbox, "
      "press return to go to the next denomination:" << std::endl;
    for (int i = 0; i < 11; i++) {
      std::cout << coinNames[i] << ": ";
      std::cin >> bal->coin[i];
    }
  }

  bal->total = getCoinboxTotal(bal);
  return;
}

int getCoinboxTotal(cashSet *sum) {
  int total = 0;
  for (int i = 0; i < 11; i++) {
    total += iRollVal[i] * sum->coin[i];
  }
  return total;
}

void getBalance(cashSet *bal, std::vector<int> params) {
  bal->type = 'b';
  if (params.size() == 11) {
    for (int i = 0; i < 11; i++) {
      bal->coin[i] = params[i];
    }
  } else {
    std::cout << "Enter denominations in the till, "
      "press return to go to the next denomination:" << std::endl;
    for (int i = 0; i < 11; i++) {
      std::cout << coinNames[i] << ": ";
      std::cin >> bal->coin[i];
    }
  }

  bal->total = getTotal(bal);
  return;
}

void getDeposit(cashSet *bal, cashSet *dep) {
  int amtDeposit = bal->total - FLOAT;
  int amtDepositC = amtDeposit;
  dep->type = 'd';

  for (int i = 10; i >= 0; i--) {
    dep->coin[i] = (bal->coin[i] == 0)? 0 :
    min(bal->coin[i], floor(amtDeposit / iCoinVal[i]));
    amtDeposit -= dep->coin[i] * iCoinVal[i];
  }

  dep->total = getTotal(dep);
  // while -- return
  // TODO: fix properly...
  switch (abs(dep->total - bal->total + FLOAT)) {
    case 5:
      if (bal->coin[DIME] >= 3 && dep->coin[QUARTER] >= 1) {
        dep->coin[DIME] += 3;
        dep->coin[QUARTER] -= 1;
        dep->total = getTotal(dep);
      }
      break;
    case 400:
      // this will only trigger if we have almost no lower coins
      break;
    case 0: break;
  }
  return;
}

void getFloat(cashSet *bal, cashSet *dep, cashSet *flt) {
  flt->type = 'f';
  for (int i = 0; i < 11; i++) {
    flt->coin[i]  = bal->coin[i]  - dep->coin[i];
  }
  flt->total = getTotal(flt);
  return;
}

int getTotal(cashSet *sum) {
  int total = 0;
  for (int i = 0; i < 11; i++) {
    total += iCoinVal[i] * sum->coin[i];
  }
  return total;
}

void printCoinBox(cashSet *bal) {
  std::cout << std::endl << "Coinbox balance:" << std::endl;
  std::cout << "--------------------------" << std::endl;
  for (int i = 0; i < 11; i++) {
    std::cout << std::right << std::fixed << bal->coin[i]  << "\tx "
    << std::setw(6) << fRollVal[i] <<  "\t"
    << std::setw(10)  << std::fixed <<
    (float)(bal->coin[i] * fRollVal[i]) << std::endl;
  }
  std::cout << "==========================" << std::endl
            << "Coinbox Total: "
            << toCurrency(bal->total) << std::endl << std::endl;
  return;
}

void printCash(cashSet *bal, cashSet *dep, cashSet *flt) {
  std::cout << std::endl << "Till balance:" << std::endl;
  printBal(bal);

  std::cout << "Deposit:" << std::endl;
  printBal(dep);

  std::cout << "Float:" << std::endl;
  printBal(flt);
}

void printBal(cashSet *bal) {
  std::cout << "--------------------------" << std::endl;
  for (int i = 0; i < 11; i++) {
    std::cout << std::right << bal->coin[i]  <<
    "\tx " << std::setw(6) << fCoinVal[i] <<
    "\t" << std::setw(10) << std::fixed <<
    (float)(bal->coin[i] * fCoinVal[i]) << std::endl;
  }
  std::cout << "==========================" << std::endl;
  switch (bal->type) {
    case 'b':
      std::cout << "Balance ";
      break;
    case 'd':
      std::cout << "Deposit ";
      break;
    case 'f':
      std::cout << "Float ";
      break;
  }
  std::cout << "Total: " << toCurrency(bal->total) << std::endl << std::endl;
}

void fPrintCoinConfig(cashSet *bal, std::string path) {
  std::ofstream conf(path.c_str());
  if (!conf.is_open()) {
    std::cerr << "Error opening " << path << std::endl;;
    return;
  }
  conf.precision(2);
  for (int i = 0; i < 11; i++) {
    conf << "coinbox" << coinNames[i] << "=" << bal->coin[i] << std::endl;
    conf << "coinbox" << coinNames[i] << "Val=" <<
    (bal->coin[i] * iRollVal[i]) << std::endl;
  }
  conf << "Total=" << toCurrency(bal->total) << std::endl;
  conf.close();
}

void fPrintConfig(cashSet *bal, cashSet *dep, cashSet *flt, std::string path) {
  std::ofstream conf(path.c_str());
  if (!conf.is_open()) {
    std::cerr << "Error opening " << path << std::endl;
    return;
  }
  fPrintConf(bal, "bal", conf);
  fPrintConf(dep, "dep", conf);
  fPrintConf(flt, "flt", conf);
  conf.close();
}


void fPrintConf(cashSet *bal, std::string set, std::ofstream &conf) {
  conf.precision(2);
  for (int i = 0; i < 11; i++) {
    conf << set << "n" << coinNames[i] << "=" << bal->coin[i] << std::endl;
    conf << set << "f" << coinNames[i] << "=" << std::fixed <<
    (float)(bal->coin[i] * fCoinVal[i]) << std::endl;
  }
  conf << set << "Total=" << bal->total / 100 << ".";
  if (bal->total % 100 < 10)
    conf << "0";
  conf << bal->total % 100 << std::endl;
}

void fPrintCash(cashSet *bal, cashSet *dep, cashSet *flt, std::string path) {
  std::ofstream output(path.c_str());
  if (!output.is_open()) {
    std::cerr << "Error opening " << path << std::endl;
    return;
  }
  output << "Till balance:" << std::endl;
  fPrintBal(bal, "Balance ", output);

  output << "Deposit:" << std::endl;
  fPrintBal(dep, "Deposit ", output);

  output << "Float:" << std::endl;
  fPrintBal(flt, "Float ", output);
  output.close();
}

void fPrintBal(cashSet *bal, std::string setName, std::ofstream &output) {
  output.precision(2);
  output << "--------------------------" << std::endl;
  for (int i = 0; i < 11; i++) {
    output << std::right << bal->coin[i]  <<
    "\tx " << std::setw(6) << fCoinVal[i] <<
    "\t" << std::setw(10) << std::fixed <<
    (float)(bal->coin[i] * fCoinVal[i]) << std::endl;
  }
  output << "==========================" << std::endl << setName
         << "Total: " << toCurrency(bal->total) << std::endl << std::endl;
}


//TODO Writup and maybe header-ize
//
std::string getDate(const char *format) {
  std::stringstream ss;

  time_t now = time(0);
  tm *t = localtime(&now);

  for (int i = 0; i < std::strlen(format); i++) {
    switch ((int)format[i]) {
      case 'd':
        ss << std::setfill('0') << std::setw(2) << t->tm_mday;
      break;
      case 'm':
        ss << std::setfill('0') << std::setw(2) << t->tm_mon + 1;
      break;
      case 'y':
        ss << t->tm_year - 100;
      break;
      case 'Y':
        ss << t->tm_year + 1900;
      break;
      case 'H':
        ss << std::setfill('0') << std::setw(2) << t->tm_hour;
      break;
      case 'M':
        ss << std::setfill('0') << std::setw(2) << t->tm_min;
      break;
      case 'S':
        ss << std::setfill('0') << std::setw(2) << t->tm_sec;
      break;
      case '.':
        ss << '.';
      break;
      default: //not sure... skip?
      break;
    }
  }
  /*std::stringstream ss;
  for (int i = 0; i < 5; i++) {

    if (format == 'f') {
      ss << t->tm_year - 100
      << std::setfill('0') << std::setw(2) << t->tm_mon + 1
      << std::setfill('0') << std::setw(2) << t->tm_mday;
  } else if (format == 'm') {
    ss << t->tm_year - 100
    << std::setfill('0') << std::setw(2) << t->tm_mon + 1;
  }
  return ss.str();*/
  return ss.str();
}


// TODO
//
std::string toCurrency(int b) {
  std::stringstream ss;
  ss << b / 100 << '.' << std::setfill('0') << std::setw(2) << b % 100;
  return ss.str();
}


// TODO
//
void usage() {
  std::cout <<
    "Usage: cash [OPTIONS]... [VALUES]" << std::endl <<
    "  Options:" << std::endl <<
    "  -c, --config [FILE]  Write vars to FILE or cashConfig.txt" << std::endl <<
    "  -s, --silent         Do not output to files" << std::endl <<
    "  -b, --coin           Calculate using coin roll values" << std::endl <<
    "  -t, --till TILL      Append -TILL to filename" << std::endl <<
    "  -o, --output FILE    Write to ./FILE instead of default" << std::endl <<
    "  -h, --help           Show this menu" << std::endl;
}
/*    if (!(*it).compare("--config") || !(*it).compare("-c")) {
      (*opts)["config"] = "true";
    } else if (!(*it).compare("--silent") || !(*it).compare("-s")) {
      (*opts)["silent"] = "false";
    } else if (!(*it).compare("--coin") || !(*it).compare("-b")) {
      (*opts)["coin"] = "true";
    } else if (!(*it).compare("--till") || !(*it).compare("-t")) {
      (*opts)["till"] = *(++it);
      i++;
    } else if (!(*it).compare("--help") || !(*it).compare("-h")) {
      return -2;
    } else if (!(*it).compare("--output") || !(*it).compare("-o")) {
      (*opts)["output"] = *(++it);
      i++;
      //TODO --absolute path
    } else {
      if ((*it)[0] == '-') {  // bad option
        return i;
      } else {  // params
 */
