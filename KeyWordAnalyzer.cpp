//*************************************************************
//  File: KeyWordAnalyzer.cpp
//  Date created: 1/17/2016
//  Date edited: 5/10/2016
//  Author: Nathan Martindale
//  Copyright © 2016 Digital Warrior Labs
//  Description: 
//*************************************************************

#include "KeyWordAnalyzer.h"

using namespace std;

namespace dwl
{
	KeyWordAnalyzer::KeyWordAnalyzer() { }
	KeyWordAnalyzer::KeyWordAnalyzer(vector<string>* wordList) 
	{ 
		analyzeKeyWords(wordList);
	}

	vector<KeyWord>* KeyWordAnalyzer::getKeyWords() { return m_completed; }
	string KeyWordAnalyzer::getFilledWord() { return m_userWord; }

	bool KeyWordAnalyzer::isSingleWord() { return m_singleWord; }

	vector<KeyWord>* KeyWordAnalyzer::analyzeKeyWords(vector<string>* wordList)
	{
		m_completed = new vector<KeyWord>();
		m_userWord = "";	

		// add all of them into one big group
		vector<KeyWord>* group = new vector<KeyWord>();

		for (int i = 0; i < wordList->size(); i++)
		{
			KeyWord* keyword = new KeyWord();
			keyword->FullText = (*wordList)[i];
			keyword->Keys = new vector<int>();

			// automatically make the first letter a key, if only one word
			if (wordList->size() == 1) { keyword->Keys->push_back(0); }
			
			group->push_back(*keyword);
		}
		
		analyzeGroup(*group, 0);

		return m_completed;
	}

	// TODO: don't forget to check if letter is past end word length (if so, just add it to m_completed. User will have to hit enter or find some other way to deal with it if there's still ambiguity)
	void KeyWordAnalyzer::analyzeGroup(vector<KeyWord> group, int letter)
	{
		//cout << "STARTING ANALYSIS ON LETTER " << letter << endl; // DEBUG
		// ending conditions
		if (group.size() == 0) { return; }

		vector<vector<KeyWord> >* groups = new vector<vector<KeyWord> >();

		bool wordEndReached = false;
		
		// parcel out into groups
		for (int i = 0; i < group.size(); i++)
		{
			// check the letter of current analyzing one with that of each group
			KeyWord current = group[i];
			//cout << "---- Checking letter of " << current.FullText << endl; // DEBUG

			// make sure haven't reached end of word
			if (letter >= current.FullText.length()) 
			{ 
				wordEndReached = true;
				m_completed->push_back(current); continue; 
			}

			bool added = false; // if it found a group with same letter or not
			for (int j = 0; j < groups->size(); j++)
			{
				//cout << "Being checked with " << (*groups)[j][0].FullText << endl; // DEBUG
				//cout << "comparing '" << current.FullText[letter] << "' with '" << (*groups)[j][0].FullText[letter] << "'" << endl; // DEBUG
				// TODO TODO TODO TODO: this didn't used to have toLower in it 
				if (tolower(current.FullText[letter]) == tolower((*groups)[j][0].FullText[letter])) 
				{
					//cout << "It's a match!" << endl; // DEBUG
					(*groups)[j].push_back(current);
					added = true;
					break;
				}
			}
			if (!added) // didn't find a group for this letter, make a new group and add to list of groups
			{
				//cout << "No match, new group created" << endl; // DEBUG
				vector<KeyWord>* newGroup = new vector<KeyWord>();
				newGroup->push_back(current);
				groups->push_back(*newGroup);
				//cout << "(group count: " << groups->size() << ")" << endl; // DEBUG
			}
		}
		
		// if only one group, don't add, just pass it in with the next letter
		if (groups->size() == 1 && !wordEndReached) 
		{ 
			//cout << "Only 1 group, passing on to the next letter" << endl; // DEBUG
			analyzeGroup((*groups)[0], letter + 1); 
			return;
		}
		
		//add keys
		for (int i = 0; i < groups->size(); i++)
		{
			for (int j = 0; j < (*groups)[i].size(); j++) { (*groups)[i][j].Keys->push_back(letter); }
			
			// if any groups with only one in it, it's finalized 
			if ((*groups)[i].size() == 1) { m_completed->push_back((*groups)[i][0]); continue; }
			
			// analyze the sub group (if conditional above wasn't true)
			analyzeGroup((*groups)[i], letter + 1);
		}
	}

	// TODO: add boolean for user to say "that's it" for key letters? (esentially what tab right now is currently doing)
	vector<KeyWord>* KeyWordAnalyzer::filterKeys(string keys, bool caseSensitive)
	{
		vector<KeyWord>* filtered = new vector<KeyWord>();
		//cout << "Filtering! (for " << keys << ")" << endl; // DEBUG
		
		int numKeys = strlen(keys.c_str());
		
		// check the keys against all of the keys in the m_completed
		for (int i = 0; i < m_completed->size(); i++)
		{
			KeyWord currentKeyWord = (*m_completed)[i];
			//cout << "-- Checking(" << currentKeyWord.FullText << ")" << endl; // DEBUG
			
			//cout << "-- -- keyssize:" << currentKeyWord.Keys->size() << endl; // DEBUG
			//cout << "-- -- keys:"; // DEBUG
			//for (int i = 0; i < currentKeyWord.Keys->size(); i++)
			//{
				//cout << currentKeyWord.FullText[currentKeyWord.Keys->at(i)]; // DEBUG
			//}
			//cout << endl; // DEBUG
			// first check for if passed has more keys than current
			if (numKeys > currentKeyWord.Keys->size()) { continue; }

			// go through each key one by one to see if it matches
			bool matches = true;
			for (int j = 0; j < numKeys; j++)
			{
				if (caseSensitive && keys[j] != currentKeyWord.FullText[currentKeyWord.Keys->at(j)]) { cout << "WE'RE IN CASE SENSITIVE MODE" << endl; matches = false; break; }
				else if (!caseSensitive && tolower(keys[j]) != tolower(currentKeyWord.FullText[currentKeyWord.Keys->at(j)])) { matches = false; break; }
			}
			if (matches) { filtered->push_back(currentKeyWord); }
		}

		//TODO: if filteredsize IS 0, means we typed invalid character for it, so take it off
		// set autofilled word 
		m_userWord = "";
		if (filtered->size() > 0) // TODO: also need to check for if nothing's been filtered? (same size as m_completed)
		{
			// get first word
			KeyWord first = (*filtered)[0];
			
			// get final key index that can use
			int lastIndex = 0;
			if (numKeys < first.Keys->size()) { lastIndex = first.Keys->at(numKeys); }
			else { lastIndex = strlen(first.FullText.c_str()); } // just use whole word at this point

			// fill out string to index of that word
			for (int i = 0; i < lastIndex; i++) // was <=
			{
				m_userWord += first.FullText[i];
			}
		}
		//cout << "filtered size: " << filtered->size() << endl; // DEBUG
		
		if (filtered->size() == 1) { m_singleWord = true; }
		else { m_singleWord = false; }
		//cout << "Filtered size: " << filtered->size() << endl; // DEBUG
		return filtered; 
	}
}
