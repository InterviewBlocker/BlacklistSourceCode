#pragma once
#include<string.h>
#include<iostream>
#include<vector>
#include<string>
#define WSIZE (32*1024)
#define MIN_MATCH 3
#define MAX_MATCH (MIN_MATCH+255)
#define MIN_DATA (MIN_MATCH+MAX_MATCH+1)
#define MAX_DIST (WSIZE-(MIN_DATA))
typedef unsigned char uch;
typedef unsigned int uit;
typedef unsigned short ush;
typedef unsigned long ulg;
using std::string;
using std::vector;
using std::cout;
using std::cin;
using std::endl;