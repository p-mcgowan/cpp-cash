#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <unistd.h>  // isatty

std::vector<std::string> tokenize(char **argv, int argc) {
  std::vector<std::string> t( argv+1, argv+argc );
  if (!isatty(fileno(stdin))) {
    std::string s;
    std::vector<std::string>::iterator it = t.begin();
    while (std::cin >> s) {
      t.push_back(s);
    }
  }
  return t;
}
