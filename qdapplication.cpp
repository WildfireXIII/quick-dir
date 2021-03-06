//*************************************************************
//  File: qdapplication.cpp
//  Date created: 1/17/2016
//  Date edited: 5/10/2016
//  Author: Nathan Martindale
//  Copyright © 2016 Digital Warrior Labs
//  Description: Console application that handles getting user's desired path
//*************************************************************

#include <iostream>
#include <string>
#include <vector>

#include <FileLayer/FileLayer.cpp>
#include <ConsoleLayer/ConsoleLayer.cpp>

#include "KeyWordAnalyzer.h"

using namespace std;
using namespace dwl;

static const int WORD_SIZE = 24;

static FileLayer* fl;
static ConsoleLayer* cl;

KeyWordAnalyzer* kwAnalyzer;
static vector<KeyWord>* words;
static vector<KeyWord>* availableWords;
string fullPath = ".\\";
string keyed = "";
string keys = "";
int keyedNum = 0;

int rows = 0;

bool shiftDown = false;

void printDirsBelow();
void changeDirectory();
void showCurrentEditingPath();
void printKeys(vector<KeyWord>* keywords, int highlightKeyIndex);
void homeCursor(int home);
void clearLines(int lines);
void printKey(KeyWord word, int highlightIndex);
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

// prints out single directory with keys higlighted
static void printKey(KeyWord word, int highlightIndex)
{
	for (int i = 0; i < word.FullText.length(); i++)
	{
		if (highlightIndex < word.Keys->size() && i == word.Keys->at(highlightIndex))
		{
			cl->setFGColor(cl->LIGHTWHITE);
			highlightIndex++;
			cout << word.FullText[i] << flush;
		}
		else
		{
			cl->setFGColor(cl->WHITE);
			cout << word.FullText[i] << flush;
		}
	}
	int spaceDif = WORD_SIZE - word.FullText.length();
	if (spaceDif > 0)
	{
		string spaces(spaceDif, ' ');
		cout << spaces << flush;
	}
}

// sends cursor to beginning of line TODO: this should really be put in the consolelayer library
static void homeCursor(int home)
{
	int x, y;
	cl->getCursorPos(&x, &y);
	cl->setCursorPos(home, y);
}

// DO NOT try putting this in consolelayerlibrary, this is too specific
static void clearLines(int lines)
{
	int width, height;
	cl->getConsoleSize(&width, &height);

	string spaces(width - 1, ' ');
	string lineClearing = "";
	
	for (int i = 0; i <= lines; i++)
	{
		lineClearing += spaces;
		lineClearing += "\n";
	}

	cout << lineClearing << flush;
}

// print out all of passed keys as directories
static void printKeys(vector<KeyWord>* keywords, int highlightKeyIndex)
{
	cl->storeCursorPos();
	int cursorLeft = 2 + fullPath.length() + keyed.length();
	cl->moveCursorDown(1);
	homeCursor(0);

	int numWords = keywords->size();
	int rowsNeeded = numWords / 3;

	if (numWords <= 10) { rowsNeeded = numWords; } // if under certain amount, just list all in one col

	int currentLeft = 0; 
	int currentRow = 0;

	int largerRows = rows;
	if (rowsNeeded > largerRows) { largerRows = rowsNeeded; }
	
	// clear lines
	clearLines(largerRows);
	
	// reset cursor pos, DON'T USE RESTORE CURSOR POS, unreliable when buffer size has been reached
	cl->moveCursorUp(largerRows + 2);
	cl->moveCursorRight(cursorLeft);
	cl->moveCursorDown(1);
	homeCursor(0);

	rows = rowsNeeded;
	
	// print stuff!
	for (int i = 0; i < keywords->size(); i++)
	{
		printKey(keywords->at(i), highlightKeyIndex);
		cl->moveCursorDown(1);
		homeCursor(currentLeft);
		currentRow++;
		if (currentRow > rowsNeeded)
		{
			cl->moveCursorUp(rowsNeeded + 1);
			cl->moveCursorRight(WORD_SIZE);
			currentRow = 0;
			currentLeft += WORD_SIZE;
			homeCursor(currentLeft);
		}
	}
	
	// reset cursor pos
	cl->moveCursorUp(currentRow + 1);
	homeCursor(0);
	cl->moveCursorRight(cursorLeft);
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
	for (int i = 0; i < width - 1; i++) { cout << " " << flush; }
	cl->setCursorPos(0, y);

	cout << "> " << fullPath << keyed << flush;
}

// assumes changeDirectory has already been called
static void finalizePath()
{
	string cmd = "cd " + fl->getCurrentPath();
	string currently = fl->getCWD();

	cl->storeCursorPos();
	homeCursor(0);
	cl->moveCursorDown(1);
	clearLines(rows);
	cl->restoreCursorPos();
	cl->moveCursorDown(1);
	homeCursor(0);
	cout.flush();
	
	fl->outputCWD("C:\\dwl\\tmp");
	return;
}

static void handleKeyPress()
{
	int press = cl->getNextKeyPress();
	string OLD_KEYS = keys; // in case useless key pressed

	bool justPressedShift = false; // set to true ONLY on keypress of shift (so shift bool doesn't reset right away)
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
	else if (press == 16) // shift // TODO: THIS IS A REALLY HACKY WAY OF DOING THINGS
	{
		if (shiftDown) { shiftDown = false; }
		else { shiftDown = true; }
		justPressedShift = true;
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
		if (press == 189)
		{
			if (shiftDown) { converted = '_'; }
			else { converted = '-'; }
		}

		keys += converted;
		keyedNum++;
	}
	words = kwAnalyzer->filterKeys(keys, false);
	if (!justPressedShift) { shiftDown = false; }

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
	ios_base::sync_with_stdio(false);
	words = new vector<KeyWord>();
	availableWords = new vector<KeyWord>();
	
	fl = new FileLayer();
	cl = new ConsoleLayer();

	// get some space to work with (so doesn't screw with stored cursor pos)
	cout << "\n\n" << flush;
	cl->moveCursorUp(2);
	cl->storeCursorPos();

	
	cout << "> " << flush;
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
