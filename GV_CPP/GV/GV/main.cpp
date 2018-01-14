#include <iostream>
#include <fstream>
#include <string>
#include <codecvt>
#include <sstream>
#include <deque>
#include <map>
#include <vector>
#include <chrono>
#include <cstdio>
#include "main.h"
#define DEBUG 0

using namespace std;

class WordDecomposition {
public:
	WordDecomposition(wstring chainOfLetters) {
		for (size_t i = 0; i < chainOfLetters.length(); i++) {
			// Non existant values are ok since they will take the default value of int (0)
			decomposition[(int)chainOfLetters[i]]++;
		}
	}

	~WordDecomposition() {
		decomposition.clear();
	}
	inline bool canCreateWord(const WordDecomposition& wordDecomposition) {
		for (auto const& letter : wordDecomposition.decomposition) {
			int countDiff = decomposition[letter.first] - letter.second;
			if (countDiff < 0)
				return false;
		}
		return true;
	}
private:
	map<const int, int> decomposition;
};

const bool USE_CHECKPOINTS = true;

void decomposeDictionary(std::vector<WordDecomposition *> &wordsFromDictionary);
void decomposeInput(char * inputFile, std::vector<WordDecomposition *> &seeds, int &seedCounter);
void findWordDecompositions(std::vector<WordDecomposition *> &seeds, std::vector<WordDecomposition *> &wordsFromDictionary, std::vector<int> * answer_array);
void printMetrics(std::chrono::time_point<std::chrono::steady_clock> &start);

// A big sequential and flat solution.
// Pretty rusty in C++ but does the job =]
int main(int argc, char* argv[]) {
#if !DEBUG
	if (argc != 2) {
		wcerr << "Provide ONE argument: " << "<path_to_file>" << endl;
		return 1;
	}
#endif
	// For performance metrics
	auto start = chrono::high_resolution_clock::now();

	// Vector containing data from input
	auto inputFile = argv[1];
	
	// Build word decomposition for input file
	//
	vector<WordDecomposition*> seeds;
	int seedCounter = 0;
	decomposeInput(inputFile, seeds, seedCounter);
	
	// Creating answer structure
	//
	if (USE_CHECKPOINTS) wcout << "Allocating answer array";
	const int MAX_INPUT_SIZE = 5000;
	vector<int>* answer_array;
	answer_array = new vector<int>[seedCounter];
	for (int j = 0; j < seedCounter; j++) {
		// Big enough to have everything lined up
		answer_array->reserve(75000);
	}
	if (USE_CHECKPOINTS) wcout << " DONE" << endl;

	// Read the big dictionnary and decompose words
	//
	vector<WordDecomposition*> wordsFromDictionary;
	decomposeDictionary(wordsFromDictionary);

	// Actual processing of data
	//
	findWordDecompositions(seeds, wordsFromDictionary, answer_array);

	// Write data to output file
	//
	writeSolution(seedCounter, answer_array);

	// Free objects
	//
	for (size_t i = 0; i < seedCounter; i++) {
		answer_array[i].clear();
	}
	delete [] answer_array;
	for each (WordDecomposition* d in seeds) {
		delete d;
	}

	printMetrics(start);

	return 0;
}

inline void printMetrics(std::chrono::time_point<std::chrono::steady_clock> &start)
{
	auto finish = chrono::high_resolution_clock::now();
	chrono::duration<double> diff = finish - start;
	if (USE_CHECKPOINTS) wcout << diff.count() << endl;
}

inline void findWordDecompositions(std::vector<WordDecomposition *> &seeds, std::vector<WordDecomposition *> &wordsFromDictionary, std::vector<int> * answer_array)
{
	if (USE_CHECKPOINTS) wcout << "Starting decomposing array" << endl;
	#pragma omp parallel for 
	for (int seedIndex = 0; seedIndex < seeds.size(); seedIndex++) {
		for (size_t l = 0; l < wordsFromDictionary.size(); l++) {
			if (seeds[seedIndex]->canCreateWord(*wordsFromDictionary[l]))
				answer_array[seedIndex].push_back(l + 1);
		}
		if (USE_CHECKPOINTS) wcout << seedIndex << "\r";
	}
	if (USE_CHECKPOINTS) wcout << endl << "End decomposing array" << endl;
}

inline void decomposeInput(char * inputFile, std::vector<WordDecomposition *> &seeds, int &seedCounter)
{
	// In worst case, we'll have 5000 inputs
	const int WORST_CASE_MAX_INPUT = 5000;
	// Prepare input reader in UTF8
	wifstream inputReader(inputFile, ifstream::in);
	inputReader.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));

	seeds.reserve(WORST_CASE_MAX_INPUT);

	if (USE_CHECKPOINTS) wcout << "Starting input decomposition...";
	wstring line;
	while (getline(inputReader, line)) {
		WordDecomposition* d = new WordDecomposition(line);
		seeds.push_back(d);
		++seedCounter;
	}
	inputReader.close();
	if (USE_CHECKPOINTS) wcout << " DONE" << endl;
}

inline void decomposeDictionary(std::vector<WordDecomposition *> &wordsFromDictionary)
{
	if (USE_CHECKPOINTS) wcout << "Starting read dictionnary";
	// Reserve a 130,000 line array to contain words
	wordsFromDictionary.reserve(130000);

	wifstream dictionnaryReader("liste_des_mots.txt", ifstream::in);
	dictionnaryReader.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	int lineIndex = 1;
	wstring dictionaryLine;
	while (getline(dictionnaryReader, dictionaryLine)) {
		WordDecomposition* lineDecomposition = new WordDecomposition(dictionaryLine);
		wordsFromDictionary.push_back(lineDecomposition);
	}
	dictionnaryReader.close();
	if (USE_CHECKPOINTS) wcout << "DONE" << endl;
}

inline void writeSolution(int seedCounter, std::vector<int> * answer_array)
{
	// Use C stdio for faster write speed
	FILE* outFile = fopen("out.txt", "w");
	if (USE_CHECKPOINTS) wcout << "Starting to write to disk ";
	for (size_t k = 0; k < seedCounter; k++) {
		stringstream ss(stringstream::out);
		for (int i = 0; i < answer_array[k].size(); i++) {
			int p = answer_array[k][i];
#if DEBUG
			wcout << p << " ";
#endif
			fprintf(outFile, "%d ", p);
		}
#if DEBUG
		wcout << endl;
#endif
		// Prevent last empty line
		if (k + 1 < seedCounter)
			fprintf(outFile, "\n");
	}
	fclose(outFile);
	if (USE_CHECKPOINTS) wcout << " DONE" << endl;
}
