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
#include <algorithm>
#include <numeric>

#define DEBUG 0

using namespace std;

std::wstring s2ws(const std::string& t_str)
{
	//setup converter
	typedef std::codecvt_utf8<wchar_t> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	return converter.from_bytes(t_str);
}

class WordDecomposition {
public:
	WordDecomposition(wstring chainOfLetters, bool track = false) {
		for (size_t i = 0; i < chainOfLetters.length(); i++) {
			// Non existant vaues are ok since they will take the default value of int (0)
			auto c = chainOfLetters[i];
			//cmap[(int)c] += 1;
			if (!(decomposition.count(c) > 0))
				decomposition[c] = 1;
			else
				decomposition[c] += 1;
		}
		//original = chainOfLetters;
		/*sorted = original;
		sort(sorted.begin(), sorted.end());*/
		//verifyCanCreate(chainOfLetters);
	}

	~WordDecomposition() {
		//decomposition.clear();
	}
	inline bool canCreateWord(WordDecomposition* wordDecomposition){
		/*if (sorted.length() != original.length()) {
			wcerr << "Mutation in string" << endl;
		}
		wstring cpy(sorted);
		int index = 0;
		for (auto it = wordDecomposition->sorted.begin(); it < wordDecomposition->sorted.end(); ++it, ++index) {
			auto found = cpy.find(*it);
			if (found == string::npos)
				return false;
			cpy.erase(cpy.begin()+found);
		}
		return true;*/
		//#pragma omp parallel for reduction(&&:x)
		//for (int i = 0; i < 256; i++) {
		//	//x = x && (( this->cmap[i] - wordDecomposition->cmap[i]) >= 0);
		//	if (cmap[i] < wordDecomposition->cmap[i])
		//		return false;
		//	/*if (val < 0)
		//		return true;*/
		//}
		//return true;

		for (auto const& letter : wordDecomposition->decomposition) {
			if (!(decomposition.count(letter.first) > 0))
				return false;
			//int initial = decomposition[letter.first];
			//int countDiff = initial - letter.second;
			if (this->decomposition[letter.first] < letter.second)
				return false;
		}
		/*auto myRep = getWstring(*this);
		auto otherRep = getWstring(wordDecomposition);
		sort(myRep.begin(), myRep.end());
		sort(otherRep.begin(), otherRep.end());
		if (myRep.compare(otherRep) != 0) {
			wcerr << "Wrong decomposition ERROR" << endl;
		}*/
		return true;
	}
private:
	map<const wchar_t, int> decomposition;
	//int cmap[256] = { 0 };
	/*wstring original;
	wstring sorted;
	static wstring getWstring(const WordDecomposition& wd) {
		wstring recomposition;
		for (auto const& letter : wd.decomposition) {
			for (int i = 0; i < letter.second; i++) {
				wstringstream ss;
				wstring s;
				char c = letter.first;
				ss << c;
				ss >> s;
				recomposition.append(s);
			}
		}
		return recomposition;
	}
	void verifyCanCreate(const wstring& chainOfLetters) {
		//sum += letter.second;
		wstring recomposition = WordDecomposition::getWstring(*this);
		int sum = recomposition.length();
		if (sum != chainOfLetters.length()) {
			wcerr << "Wrong decomposition" << endl;
		}
		wstring sortedDecomposition = recomposition;
		wstring sortedInput = chainOfLetters;
		sort(sortedDecomposition.begin(), sortedDecomposition.end());
		sort(sortedInput.begin(), sortedInput.end());
		if (sortedInput.compare(sortedDecomposition) != 0) {
			wcerr << "Wrong decomposition ERROR" << endl;
		}
	}*/
};

const bool USE_CHECKPOINTS = true;

void writeSolution(int seedCounter, std::vector<std::vector<int>*>& answer_array);
void decomposeDictionary(std::vector<WordDecomposition *> &wordsFromDictionary);
void decomposeInput(char * inputFile, std::vector<WordDecomposition *> &seeds, int &seedCounter);
void findWordDecompositions(std::vector<WordDecomposition *> &seeds, std::vector<WordDecomposition *> &wordsFromDictionary, std::vector<std::vector<int>*>& answer_array);
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
	char* inputFile = argv[1];
	
	// Build word decomposition for input file
	//
	vector<WordDecomposition*> seeds;
	int seedCounter = 0;
	decomposeInput(inputFile, seeds, seedCounter);
	
	// Creating answer structure
	//
	if (USE_CHECKPOINTS) wcout << "Allocating answer array";
	const int MAX_INPUT_SIZE = 5000;
	vector<vector<int>*> answer_array;
	answer_array.reserve(seedCounter);
	for (int j = 0; j < seedCounter; j++) {
		// Big enough to have everything lined up
		vector<int>* a = new vector<int>();
		a->reserve(75000);
		answer_array.push_back(a);
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
	for (size_t i = 0; i < answer_array.size(); i++) {
		answer_array[i]->clear();
		delete answer_array[i];
	}
	//delete [] answer_array;
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

inline void findWordDecompositions(std::vector<WordDecomposition *> &seeds, std::vector<WordDecomposition *> &wordsFromDictionary, std::vector<std::vector<int>*>& answer_array)
{
	if (USE_CHECKPOINTS) wcout << "Starting decomposing array" << endl;
	#pragma omp parallel for
	for (int seedIndex = 0; seedIndex < seeds.size(); seedIndex++) {
		for (int l = 0; l < wordsFromDictionary.size(); l++) {
			WordDecomposition* el = wordsFromDictionary[l];
			if (seeds[seedIndex]->canCreateWord(el)) {
				answer_array[seedIndex]->push_back(l+1);
			}
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
		WordDecomposition* d = new WordDecomposition(line, true);
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

	wifstream dictionnaryReader("C:\\f\\liste_des_mots.txt", ifstream::in);
	dictionnaryReader.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	int lineIndex = 1;
	wstring dictionaryLine;
	while (getline(dictionnaryReader, dictionaryLine)) {
		WordDecomposition* lineDecomposition = new WordDecomposition(dictionaryLine);
		wordsFromDictionary.push_back(lineDecomposition);
		lineIndex++;
	}
	dictionnaryReader.close();
	if (USE_CHECKPOINTS) wcout << "DONE" << endl;
}

inline void writeSolution(int seedCounter, std::vector<std::vector<int>*>& answer_array)
{
	const bool useWof = false;
	wofstream os;
	FILE* outFile;
	if (useWof) {
		os.open("C:\\f\\out_wofstream.txt", ofstream::out);
		os.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	} else
		outFile = fopen("C:\\f\\out.txt", "w");
	FILE* statsFile = fopen("C:\\f\\stats.txt", "w");
	//wstringstream ss;
	// Use C stdio for faster write speed
	wcout << "Seed counter"  << seedCounter << endl;
	if (USE_CHECKPOINTS) wcout << "Starting to write to disk ";
	for (size_t k = 0; k < answer_array.size(); k++) {
		auto el = answer_array[k];
		for (int ansIndex = 0; ansIndex < el->size(); ansIndex++) {
			int p = (*el)[ansIndex];
			//wcout << p << " ";
			if (useWof)
				os << p << " ";
			else
				fprintf(outFile, "%d ", p);
		}
		fprintf(statsFile, "%d\n", el->size());
		//wcout << endl;
		// Prevent last empty line
		//if (k + 1 < answer_array.size())
		if (useWof)
			os << endl;
		else
			fprintf(outFile, "\n");
	}
	if (useWof)
		os.close();
	else
		fclose(outFile);
	fclose(statsFile);
	/*wstring con = ss.str();
	os.write(con.c_str(), con.length());*/
	if (USE_CHECKPOINTS) wcout << " DONE" << endl;
}
