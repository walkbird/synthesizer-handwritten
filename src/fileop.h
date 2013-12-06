#ifndef FILEOP_H
#define FILEOP_H

#include <string>
#include <vector>
using namespace std;

vector<string> loadWord(string path);
vector<string> loadText(string path);
bool writeWord(vector<string> &words,string path);

#endif