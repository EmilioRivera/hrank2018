#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <thread>

#define DEBUG 0
const int WORD_DIC_SIZE = 128908;
//const int MAX_INPUT_SIZE = 5000;

using namespace std;
struct compareByStraightValue {
	bool operator()(const char& a, const char& b) const {
		return a < b;
	}
	std::size_t operator()(const char& k) const {
		return (unsigned char)k;
	}
};
class WordDecomposition {
public:
	WordDecomposition(const string& chainOfLetters, int lineFromDic = 0) {
		memset(c, 0, 80 * sizeof(unsigned char));
		memset(l, 0, 256 * sizeof(unsigned char));
		//decomposition.reserve(80);
		for (size_t i = 0; i < chainOfLetters.length(); i++) {
			// Non existant values are ok since they will take the default value of int (0)
			c[i] = ((unsigned char)chainOfLetters[i]);
			l[(unsigned char)chainOfLetters[i]]++;
			//decomposition[chainOfLetters[i]] += 1;
			//d[(unsigned char)chainOfLetters[i]] += 1;
		}
		length = chainOfLetters.length();
		if (lineFromDic != 0) {
			lineNumberInString.reserve(7);
			lineNumberInString = to_string(lineFromDic) + " ";
		}
	}

	~WordDecomposition() {

	}
	inline bool canCreateWord(const WordDecomposition& wordDecomposition){
		if (length < wordDecomposition.length)
			return false;
		/*for (auto const& letter : wordDecomposition.decomposition) {
			if (decomposition[letter.first] < letter.second)
				return false;
		}*/
		/*for (int i = 256 - 1; i >= 0; i--)
		{
			if (wordDecomposition.l[i] == 0)
				continue;
			if (l[i] < wordDecomposition.l[i])
				return false;
		}*/
		for (size_t i = 0; i < wordDecomposition.length; i++) {
			auto toSearch = wordDecomposition.c[i];
			if (l[toSearch] < wordDecomposition.l[toSearch])
				return false;
		}
		return true;
	}
	size_t length;
	unsigned char c[80];
	unsigned char l[256];
	//unordered_map<char, unsigned char, compareByStraightValue> decomposition;
	// String representation of the line this word comes from with trailing space
	string lineNumberInString;
	//unsigned char d[256] = { 0 };
};

const bool USE_CHECKPOINTS = false;
#define  _OPENMP_NOFORCE_MANIFEST
void writeSolution(int seedCount, string* lineOfStrings);
void decomposeDictionary(WordDecomposition** wordsFromDictionary);
void decomposeInput(char * inputFile, std::vector<WordDecomposition *> &seeds, int &seedCount);
void findWordDecompositions(const std::vector<WordDecomposition *> &seeds, WordDecomposition** wordsFromDictionary, string* lineOfStrings);

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
	//auto start = chrono::high_resolution_clock::now();
	
	// Vector containing data from input
	char* inputFile = argv[1];
	// Build word decomposition for input file
	//
	vector<WordDecomposition*> seeds;
	int seedCount = 0;
	decomposeInput(inputFile, seeds, seedCount);
	
	
	// Read the big dictionnary and decompose words
	//
	WordDecomposition** wordsFromDictionary = new WordDecomposition*[WORD_DIC_SIZE];
	/*wordsFromDictionary.reserve(WORD_DIC_SIZE);*/
	decomposeDictionary(wordsFromDictionary);

	// Creating answer structure
	//
	string* linesOfString = new string[seeds.size()];
	for (size_t i = 0; i < seeds.size(); i++) {
		// Cannot contain more than the size of dictionnary * 7 (6-digit number + 1 space) chars
		linesOfString[i].reserve(WORD_DIC_SIZE * 7);
	}

	// Actual processing of data
	//
	findWordDecompositions(seeds, wordsFromDictionary, linesOfString);

	// Write data to output file
	//
	writeSolution(seedCount, linesOfString);

	// Free objects
	//
	delete[] linesOfString;

	for (int i = 0; i < WORD_DIC_SIZE; i++) {
		delete wordsFromDictionary[i];
	}
	delete[] wordsFromDictionary;

	for each (WordDecomposition* d in seeds) {
		delete d;
	}

	return 0;
}

void waitAndWrite(int* work, string* linesOfString, int workLimit) {
	FILE* outFile = fopen("out.txt", "w");
	int lastWrittenLine = -1;
	// Todo change this value

	while (lastWrittenLine < workLimit - 1) {
		if (work[lastWrittenLine + 1] == 1) {
			fprintf(outFile, linesOfString[lastWrittenLine + 1].c_str());
			fprintf(outFile, "\n");
			lastWrittenLine++;
		}
	}
	fclose(outFile);
}

inline void findWordDecompositions(const std::vector<WordDecomposition *> &seeds, WordDecomposition** wordsFromDictionary, string* linesOfString)
{
	int* doneList = (int*) calloc(seeds.size(), sizeof(int));
	std::thread writerThread(waitAndWrite, doneList, linesOfString, seeds.size());
	//if (USE_CHECKPOINTS) wcout << "Starting decomposing array" << endl;
	// The seed size makes it so that our input data may vary in size (3 to 80), so we
	// use a dynamic scheduling so that threads that finish faster than others
	// are used for the next inputs.
	#pragma omp parallel for
	for (int seedIndex = 0; seedIndex < seeds.size(); seedIndex++) {
		for (int l = 0; l < WORD_DIC_SIZE; l++) {
			WordDecomposition* w = wordsFromDictionary[l];
			if (seeds[seedIndex]->canCreateWord(*w)) {
				linesOfString[seedIndex].append(w->lineNumberInString);
			}
		}
		// Tell thread to start to work
		doneList[seedIndex] = 1;
		//if (USE_CHECKPOINTS) wcout << seedIndex << "\r";
	}
	writerThread.join();
	delete[] doneList;
	//if (USE_CHECKPOINTS) wcout << endl << "End decomposing array" << endl;
}

inline void decomposeInput(char * inputFile, std::vector<WordDecomposition *> &seeds, int &seedCount)
{
	// In worst case, we'll have 5000 inputs
	const int WORST_CASE_MAX_INPUT = 5000;

	// DO NOT use UTF 8 for values that are encoded in ANSI
	// If only values encoded from 0 to 255 are used, ANSI
	// characters fit into a 'char' instead of two, which makes
	// everything faster
	ifstream inputReader(inputFile, ifstream::in);
	// If data ever is in UTF-8...
	//inputReader.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));

	seeds.reserve(WORST_CASE_MAX_INPUT);
	vector<string> allLines;
	allLines.reserve(WORST_CASE_MAX_INPUT);
	if (USE_CHECKPOINTS) wcout << "Starting input decomposition...";
	string line;
	while (getline(inputReader, line)) {
		allLines.push_back(line);
	}
	inputReader.close();

	seedCount = allLines.size();
	for (int i = 0; i < allLines.size(); i++) {
		std::sort(allLines[i].rbegin(), allLines[i].rend());
		WordDecomposition* d = new WordDecomposition(allLines[i]);
		seeds.push_back(d);
	}

	//if (USE_CHECKPOINTS) wcout << " DONE" << endl;
}
inline void decomposeDictionary(WordDecomposition** wordsFromDictionary)
{
	//if (USE_CHECKPOINTS) wcout << "Starting read dictionnary";
	ifstream dictionnaryReader("liste_des_mots_ansi.txt", ifstream::in);
	//dictionnaryReader.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	string dictionaryLine;
	string allDicLines[WORD_DIC_SIZE];
	int i = 0;
	while (getline(dictionnaryReader, dictionaryLine)) {
		allDicLines[i] = dictionaryLine;
		++i;
	}
	dictionnaryReader.close();
	#pragma omp parallel for
	for (int i = 0; i < WORD_DIC_SIZE; i++) {
		sort(allDicLines[i].rbegin(), allDicLines[i].rend());
		wordsFromDictionary[i] = new WordDecomposition(allDicLines[i], i + 1);
	}
	//if (USE_CHECKPOINTS) wcout << "DONE" << endl;
}

inline void writeSolution(int seedCount, string* lineOfStrings)
{
	// Use C stdio for faster write speed
	//FILE* outFile = fopen("out.txt", "w");
	/*for (size_t i = 0; i < seedCount; i++) {
		fprintf(outFile, lineOfStrings[i].c_str());
		fprintf(outFile, "\n");
	}
	fclose(outFile);*/
	//if (USE_CHECKPOINTS) wcout << " DONE" << endl;
}
