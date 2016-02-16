//*************************************************************
//  File: KeyWordAnalyzer.cpp
//  Date created: 1/17/2016
//  Date edited: 2/15/2016
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

			// automatically make the first letter a key
			keyword->Keys->push_back(keyword->FullText[0]);
			
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
				if (current.FullText[letter] == (*groups)[j][0].FullText[letter]) 
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
	vector<KeyWord>* KeyWordAnalyzer::filterKeys(string keys)
	{
		vector<KeyWord>* filtered = new vector<KeyWord>();
		
		int numKeys = strlen(keys);
		
		// check the keys against all of the keys in the m_completed
		for (int i = 0; i < m_completed->size(); i++)
		{
			KeyWord currentKeyWord = (*m_completed)[i];
			
			// first check for if passed has more keys than current
			if (numKeys > currentKeyWord->Keys->size()) { continue; }

			// go through each key one by one to see if it matches
			bool matches = true;
			for (int j = 0; j < numKeys; j++)
			{
				if (keys[j] != currentKeyWord.FullText[currentKeyWord.Keys[j]]) { matches = false; break; }
			}
			if (matches) { filtered->push_back(currentKeyWord); }
		}

		// set autofilled word
		if (filtered->size > 0) // TODO: also need to check for if nothing's been filtered? (same size as m_completed)
		{
			// get first word
			KeyWord first = (*filtered)[0];
			
			// get final key index that can use
			int lastIndex = first.Keys[numKeys - 1];

			// fill out string to index of that word
		}
	
		return filtered; // obviously change this
	}
}
