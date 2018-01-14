#include <iostream>
#include <fstream>
#include <string>
#include <codecvt>
#include <sstream>
#include <deque>
#include <map>
#include <vector>
#include <chrono>
#define DEBUG 0
using namespace std;
class Decomposition {
public:
	map<const int, int> decomp;
	Decomposition(wstring chainOfLetters) {
		for (size_t i = 0; i < chainOfLetters.length(); i++) {
			// Non existant values are ok since they will take the default value of int (0)
			decomp[(int)chainOfLetters[i]]++;
		}
	}

	~Decomposition()
	{
		decomp.clear();
	}
	inline bool canCreateWord(const Decomposition& wordDecomposition) {
		for (auto const& letter : wordDecomposition.decomp) {
			int countDiff = decomp[letter.first] - letter.second;
			if (countDiff < 0)
				return false;
		}
		return true;
	}
	/*inline bool canCreateWord(const wstring& word) {
		Decomposition wordDecomposition(word);
		return canCreateWord(wordDecomposition);
	}*/
private:

};

int main(int argc, char* argv[]) {
#if !DEBUG
	if (argc != 2) {
		wcerr << "Provide ONE argument: " << "<path_to_file>" << endl;
		return 1;
	}
#endif
	auto start = chrono::high_resolution_clock::now();
	wcout << "Starting input decomposition...";
	auto inputFile =
#if DEBUG
		"in_example.txt"
#else
		argv[1]
#endif
		;
	wifstream inputReader(inputFile, ifstream::in);
	inputReader.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	vector<Decomposition*> seeds;
	// Worst case
	seeds.reserve(5000);

	
	wstring line;
	int seedCounter = 0;
	while (getline(inputReader,line)) {
		Decomposition* d = new Decomposition(line);
		seeds.push_back(d);
		++seedCounter;
	}
	inputReader.close();
	wcout << " DONE" << endl;
	// Read the big dictionnary
	wifstream dictionnaryReader("liste_des_mots.txt", ifstream::in);
	dictionnaryReader.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));

	wcout << "Allocating answer array";
	vector<int>* answer_array;
	const int MAX_INPUT_SIZE = 5000;
	answer_array = new vector<int>[seedCounter];
	for (size_t j = 0; j < seedCounter; j++)
	{
		// Big enough to have everything lined up
		answer_array->reserve(75000);
	}
	wcout << " DONE" << endl;

	// Reserve a 130,000 line array to contain words
	vector<Decomposition*> wordsFromDictionary;
	wordsFromDictionary.reserve(130000);

	wcout << "Starting read dictionnary";
	int lineIndex = 1;
	while (getline(dictionnaryReader, line)) {
		Decomposition* lineDecomposition = new Decomposition(line);
		wordsFromDictionary.push_back(lineDecomposition);
	}
	dictionnaryReader.close();
	wcout << "DONE" << endl;

	wcout << "Starting decomposing array" << endl;
	#pragma omp parallel for 
	for (int seedIndex = 0; seedIndex < seeds.size(); seedIndex++)
	{
		for (size_t l = 0; l < wordsFromDictionary.size(); l++)
		{
			if (seeds[seedIndex]->canCreateWord(*wordsFromDictionary[l]))
				answer_array[seedIndex].push_back(l+1);
		}
		wcout << seedIndex << "\r";
	}
	
	wcout << endl << "End decomposing array" << endl;

	ofstream solWriter("out.txt", ofstream::out);
	wcout << "Starting to write to disk ";
	stringstream ss(stringstream::out);
	for (size_t k = 0; k < seedCounter; k++)
	{
		for each (int p in answer_array[k])
		{
#if DEBUG
			wcout << p << " ";
#endif
			solWriter << p << " ";
		}
#if DEBUG
		wcout << endl;
#endif
		solWriter << endl;

		/*string line = ss.str();
		solWriter.write(line.c_str(), line.length());*/
	}

	solWriter.close();
	wcout << " DONE" << endl;
	for (size_t i = 0; i < seedCounter; i++)
	{
		answer_array[i].clear();
	}
	delete [] answer_array;
	for each (Decomposition* d in seeds)
	{
		delete d;
	}
#if DEBUG
	system("pause");
#endif
	auto finish = chrono::high_resolution_clock::now();
	chrono::duration<double> diff = finish - start;
	wcout << diff.count() << endl;
	return 0;
}