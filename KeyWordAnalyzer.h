//*************************************************************
//  File: KeyWordAnalyzer.h
//  Date created: 1/17/2016
//  Date edited: 1/18/2016
//  Author: Nathan Martindale
//  Copyright © 2016 Digital Warrior Labs
//  Description: 
//*************************************************************

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class KeyWord
{
	public:
		string FullText;
		vector<int>* Keys;
};

class KeyWordAnalyzer
{
	private:
		// vector<vector<KeyWord> >* m_groups; // DON'T NEED, just single master group to start out with (prob don't even need it here)
		vector<KeyWord>* m_completed;
	
	public:
		KeyWordAnalyzer();
		KeyWordAnalyzer(vector<string>* wordList);
		~KeyWordAnalyzer();

		vector<KeyWord>* getKeyWords();
		
		void analyzeGroup(vector<KeyWord> group, int letter);
};
