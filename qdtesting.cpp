//*************************************************************
//  File: qdtesting.cpp
//  Date created: 1/17/2016
//  Date edited: 2/18/2016
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

void goUp();
void printDirsBelow();
void changeDirectory();
void showCurrentEditingPath();
void printKeys(vector<KeyWord>* keywords, int highlightKeyIndex);
void handleKeyPress();
void finalizePath();

static void printDirsBelow()
{
	/*cl->storeCursorPos();
	cl->moveCursorDown(1);

	// move to beginning of line
	int x, y;
	cl->getCursorPos(&x, &y);
	cl->setCursorPos(0, y);*/

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

	/*KeyWordAnalyzer* a = new KeyWordAnalyzer(wordList);
	words = a->getKeyWords();
	availableWords = words;*/

	/*for (int i = 0; i < words->size(); i++)
	{
		KeyWord current = words->at(i);
		int keyIndex = 0;
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
				cl->setFGColor(cl->GREEN);
				cout << current.FullText[j];
			}
		}

		cout << "  ";
	}*/

	printKeys(words, 0);

	cl->restoreCursorPos();
	cl->setFGColor(cl->LIGHTWHITE);
}

static void changeDirectory()
{
	//if (keyed.find("..") != string::npos) { goUp(); }
	if (!fl->isDirectory(fullPath + keyed)) { return; }
	
	fl->setCurrentPath(fullPath + keyed);
	keyedNum = 0;
	//fullPath += keyed;
	fullPath = fl->getCurrentPath() + "\\"; 
	keyed = "";
	keys = "";
	//if (!root) { fullPath += "/"; } // TODO: THIS WILL HAVE TO CHANGE IF MOVING UP A DIRECTORY
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
	//for (int i = 0; i < width*2; i++) { cout << " "; }
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
		//cout << "  ";
		//cout << "\n";
	}


	cl->restoreCursorPos();
	//cl->moveCursorDown(1);
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
	// fl->setCWD(); //doesn't officially have this yet!
	string cmd = "cd " + fl->getCurrentPath();
	cout << "changing to '" << cmd << "'" << endl;
	//system(cmd.c_str()); // TODO: obviously change this when available
	string currently = fl->getCWD();
	cout << " currently in " << currently << endl;
	fl->outputCWD();
	return;
}

static void goUp()
{
	root = false;
	//keyed = fl->getCurrentPath();
	string keyedPath = keyed.substr(0,keyed.length()-1);
	fullPath = fl->getCurrentPath();
	
	
	// get second to last slash
	int indexOfSlash = keyedPath.find_last_of("/\\");
	keyed = keyed.substr(0, indexOfSlash);
	if (keyed.length() == 2) { root = true; keyed += "\\"; }
	//changeDirectory();
	//handleKeyPress();
}

static void handleKeyPress()
{
	int press = cl->getNextKeyPress();
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

		vector<KeyWord>* tempPass = new vector<KeyWord>();

		char converted = (char)press;
		converted = tolower(converted);
		//cout << converted; // unnecessary, since we're printing out current editing path anyway
		
		
		// special characters
		if (press == 190) // .
		{  
			converted = '.';
		} 

		// loop through keys and add to temppass whatever matches
		/*bool foundMatch = false;
		for (int i = 0; i < availableWords->size(); i++)
		{
			// get the next key for current keyword
			KeyWord current = (*availableWords)[i];

			// TODO: not sure if I need this check or not, but leaving it in for now EDIT: actually, pretty sure this is important
			if (keyedNum >= current.Keys->size()) { continue; } // previously just >

			char keyOfCurrent = tolower(current.FullText[current.Keys->at(keyedNum)]);

			if (keyOfCurrent == converted) 
			{ 
				foundMatch = true;
				tempPass->push_back(current); 
			}
		}*/

		// analyze remaining from temppass and see if any text can be auto-added


		keys += converted;
		keyedNum++;
	}
	// cout << "keys now: " << keys << endl;
	words = kwAnalyzer->filterKeys(keys, false);

	/*if (!foundMatch) 
	{ 
		handleKeyPress();
		return;
	}*/
	if (words->size() == 0)
	{
		handleKeyPress();
		return;
	}


	// if only one, that's it, so add all of it
	/*if (tempPass->size() == 1)
	{
		//cout << (*tempPass)[0].FullText.substr((*tempPass)[0].Keys->at(keyedNum) + 1);
		keyed += (*tempPass)[0].FullText.substr((*tempPass)[0].Keys->at(keyedNum));
		showCurrentEditingPath();
		// change directories, we have our path so far
		changeDirectory();
		handleKeyPress();
		return;
	}
	else
	{
		keyedNum++;
		int nextClosest = 256; // start high, finding min
		for (int i = 0; i < tempPass->size(); i++)
		{
			// first check if it even has another key (if not, fill in, and wait for tab or other character)
			if ((*tempPass)[i].Keys->size() <= keyedNum) 
			{
				nextClosest = (*tempPass)[i].FullText.length();
				continue;
			}
			
			if ((*tempPass)[i].Keys->at(keyedNum) < nextClosest) { nextClosest = (*tempPass)[i].Keys->at(keyedNum); }
		}

		// autocomplete to the closest point
		//int lastIndex = (*tempPass)[0].Keys->at(keyedNum - 1) + 1;
		int lastIndex = (*tempPass)[0].Keys->at(keyedNum - 1);
		int dif = nextClosest - lastIndex;
		//cout << (*tempPass)[0].FullText.substr(lastIndex, dif);
		keyed += (*tempPass)[0].FullText.substr(lastIndex, dif);*/

		keyed = kwAnalyzer->getFilledWord();
		
		showCurrentEditingPath();
		if (kwAnalyzer->isSingleWord()) { changeDirectory(); handleKeyPress(); return; }
		
		// reassign availablewords with only those that passed this last key
		//availableWords = tempPass;
		//printKeys(availableWords, keyedNum);
		printKeys(words, keyedNum);
	//}
	
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
