//*************************************************************
//  File: KeyWordAnalyzer.h
//  Date created: 1/17/2016
//  Date edited: 2/15/2016
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

		string m_userKeys; // users typed KEYS ONLY
		string m_userWord; // users typed word (auto fills from keys)
		
		void analyzeGroup(vector<KeyWord> group, int letter);
	
	public:
		KeyWordAnalyzer();
		KeyWordAnalyzer(vector<string>* wordList);
		~KeyWordAnalyzer();

		vector<KeyWord>* getKeyWords();
		
		vector<KeyWord>* analyzeKeyWords(vector<string>* wordList);

		vector<KeyWord>* filterKeys
};
