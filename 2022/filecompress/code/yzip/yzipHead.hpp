#pragma once
#include<string.h>
#include<iostream>
#include<vector>
#include<string>
#include<map>
#define WSIZE (32*1024)
#define MIN_MATCH 3
#define MAX_MATCH (MIN_MATCH+255)
#define MIN_DATA (MIN_MATCH+MAX_MATCH+1)//原因
#define MAX_DIST 32506//原因
typedef unsigned char uch;
typedef unsigned int uit;
typedef unsigned short ush;
typedef unsigned long ulg;
using std::string;
using std::vector;
using std::cout;
using std::cin;
using std::endl;