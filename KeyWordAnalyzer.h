//*************************************************************
//  File: KeyWordAnalyzer.h
//  Date created: 1/17/2016
//  Date edited: 2/18/2016
//  Author: Nathan Martindale
//  Copyright � 2016 Digital Warrior Labs
//  Description: 
//*************************************************************

#include <iostream>
#include <vector>
#include <string>

using namespace std;

namespace dwl
{

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

			string m_userWord; // users typed word (auto fills from keys)
			bool m_singleWord; 
			
			void analyzeGroup(vector<KeyWord> group, int letter);
		
		public:
			KeyWordAnalyzer();
			KeyWordAnalyzer(vector<string>* wordList);
			~KeyWordAnalyzer();

			vector<KeyWord>* getKeyWords();
			string getFilledWord();
			bool isSingleWord();
			
			vector<KeyWord>* analyzeKeyWords(vector<string>* wordList);

			vector<KeyWord>* filterKeys(string keys, bool caseSensitive); // returns list of words that only match keys so far
	};
}
