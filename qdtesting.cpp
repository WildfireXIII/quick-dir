//*************************************************************
//  File: qdtesting.cpp
//  Date created: 1/17/2016
//  Date edited: 2/19/2016
//  Author: Nathan Martindale
//  Copyright © 2016 Digital Warrior Labs
//  Description: 
//*************************************************************

#include <iostream>
#include <string>
#include <vector>

#include <FileLayer/FileLayer.cpp>
#include <ConsoleLayer/ConsoleLayer.cpp>

#include "KeyWordAnalyzer.h"

using namespace std;
using namespace dwl;

static FileLayer* fl;
static ConsoleLayer* cl;

KeyWordAnalyzer* kwAnalyzer;
static vector<KeyWord>* words;
static vector<KeyWord>* availableWords;
string fullPath = ".\\";
string keyed = "";
string keys = "";
int keyedNum = 0;

bool root = false;

int charsPrinted = 0;

void printDirsBelow();
void changeDirectory();
void showCurrentEditingPath();
void printKeys(vector<KeyWord>* keywords, int highlightKeyIndex);
void handleKeyPress();
void finalizePath();

static void printDirsBelow()
{
	vector<FileListing>* ls = fl->getDirectoryListing();
	vector<string>* wordList = new vector<string>();
	for (int i = 0; i < ls->size(); i++)
	{
		FileListing cur = ls->at(i);
		if (cur.FullName == "." || !cur.isDirectory) { continue; }
		wordList->push_back(cur.FullName);
	}

	kwAnalyzer = new KeyWordAnalyzer(wordList);
	words = kwAnalyzer->getKeyWords();

	printKeys(words, 0);

	cl->restoreCursorPos();
	cl->setFGColor(cl->LIGHTWHITE);
}

static void changeDirectory()
{
	if (!fl->isDirectory(fullPath + keyed)) { return; }
	
	fl->setCurrentPath(fullPath + keyed);
	keyedNum = 0;
	fullPath = fl->getCurrentPath(); 
	keyed = "";
	keys = "";
	if (fullPath.length() > 3) { fullPath += "\\"; } 
	printDirsBelow();
	showCurrentEditingPath();
}

static void printKeys(vector<KeyWord>* keywords, int highlightKeyIndex)
{
	// first clear below
	cl->storeCursorPos();
	cl->moveCursorDown(1);

	// move to beginning of line
	int x, y;
	cl->getCursorPos(&x, &y);
	cl->setCursorPos(0, y);
	
	// write two lines of spaces
	int width, height;
	cl->getConsoleSize(&width, &height);
	for (int i = 0; i < charsPrinted; i++) { cout << " "; } 
	
	charsPrinted = 0;

	// move to beginning of line
	cl->restoreCursorPos();
	cl->moveCursorDown(1);
	cl->getCursorPos(&x, &y);
	cl->setCursorPos(0, y);

	int keywordNum = keywords->size();
	int largestRowNum = 0;

	for (int i = 0; i < keywords->size(); i++)
	{
		KeyWord current = keywords->at(i);
		int keyIndex = highlightKeyIndex;
		for (int j = 0; j < current.FullText.length(); j++)
		{
			if (keyIndex < current.Keys->size() && j == current.Keys->at(keyIndex))
			{
				cl->setFGColor(cl->LIGHTWHITE);
				keyIndex++;
				cout << current.FullText[j];
			}
			else
			{
				//cl->setFGColor(cl->GREEN);
				cl->setFGColor(cl->WHITE);
				cout << current.FullText[j];
			}
			charsPrinted++;
		}

		cout << "  ";
		charsPrinted += 2;
	}

	cl->restoreCursorPos();
}

// NOTE: assumes cursor is already ON THE NECESSARY LINE.
// Also note that it will leave cursor at the end of the path for further typing
// (save cursor pos at that point?)
static void showCurrentEditingPath()
{
	// move cursor to home 
	int x, y;
	cl->getCursorPos(&x, &y);
	cl->setCursorPos(0, y);

	// clear line
	int width, height;
	cl->getConsoleSize(&width, &height);

	// insert spaces
	for (int i = 0; i < width - 1; i++) { cout << " ";	}
	cl->setCursorPos(0, y);

	cout << "> " << fullPath << keyed;
}

// assumes changeDirectory has already been called
static void finalizePath()
{
	string cmd = "cd " + fl->getCurrentPath();
	cout << "changing to '" << cmd << "'" << endl;
	string currently = fl->getCWD();
	cout << " currently in " << currently << endl;
	fl->outputCWD("C:\\dwl\\tmp");
	return;
}

static void handleKeyPress()
{
	int press = cl->getNextKeyPress();
	string OLD_KEYS = keys; // in case useless key pressed
	if (press == 13) // stop on enter //WIN
	{
		finalizePath();
		return; 
	} 
	else if (press == 8) 
	{ 
		if (strlen(keys.c_str()) > 0) 
		{ 
			keys = keys.substr(0, strlen(keys.c_str()) - 1); 
			keyedNum--;
		}
	} // backspace //WIN
	else if (press == 9) // TAB (complete based on shorter)
	{ 
		changeDirectory();
		handleKeyPress();
		return;
	} 
	else
	{
		char converted = (char)press;
		converted = tolower(converted);
		
		// special characters
		if (press == 190) // .
		{  
			converted = '.';
		} 

		keys += converted;
		keyedNum++;
	}
	words = kwAnalyzer->filterKeys(keys, false);

	if (words->size() == 0)
	{
		keys = OLD_KEYS; // undo last keypress (didn't find anything)
		handleKeyPress();
		return;
	}

	keyed = kwAnalyzer->getFilledWord();
	
	showCurrentEditingPath();
	if (kwAnalyzer->isSingleWord()) { changeDirectory(); handleKeyPress(); return; }
	
	printKeys(words, keyedNum);
	
	cl->setFGColor(cl->LIGHTWHITE);
	handleKeyPress();
}

int main()
{
	words = new vector<KeyWord>();
	availableWords = new vector<KeyWord>();
	
	cout << "Hello world!" << endl;

	fl = new FileLayer();
	cl = new ConsoleLayer();

	// get some space to work with (so doesn't screw with stored cursor pos)
	cout << "\n\n";
	cl->moveCursorUp(2);
	cl->storeCursorPos();

	
	cout << "> ";
	printDirsBelow();
	handleKeyPress();

	cl->setFGColor(cl->WHITE);

	cl->moveCursorDown(3);
	// move to beginning of line
	int x, y;
	cl->getCursorPos(&x, &y);
	cl->setCursorPos(0, y);
	
	return 0;
}
