#include <iostream>
#include <fstream>
#include <string>
#include <codecvt>
#include <sstream>
#include <deque>
#include <map>
#include <vector>

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
	inline bool canCreateWord(Decomposition& wordDecomposition) {
		for (auto const& letter : wordDecomposition.decomp) {
			int countDiff = decomp[letter.first] - letter.second;
			if (countDiff < 0)
				return false;
		}
		return true;
	}
	inline bool canCreateWord(const wstring& word) {
		Decomposition wordDecomposition(word);
		return canCreateWord(wordDecomposition);
	}
private:

};

int main() {

	wifstream inputReader("in_example.txt", ifstream::in);
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

	// Read the big dictionnary
	wifstream dictionnaryReader("liste_des_mots.txt", ifstream::in);
	dictionnaryReader.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));


	vector<int>* answer_array;
	const int MAX_INPUT_SIZE = 5000;
	answer_array = new vector<int>[seedCounter];
	for (size_t j = 0; j < seedCounter; j++)
	{
		// Big enough to have everything lined up
		answer_array->reserve(2000);
	}

	int lineIndex = 1;
	while (getline(dictionnaryReader, line)) {
		Decomposition lineDecomposition(line);
		for (size_t seedIndex = 0; seedIndex < seeds.size(); seedIndex++)
		{
			if (seeds[seedIndex]->canCreateWord(lineDecomposition))
				answer_array[seedIndex].push_back(lineIndex);
		}
		++lineIndex;
	}
	/*Decomposition a(L"matrice");
	Decomposition b(L"rice");
	if (b.canCreateWord(L"matrice"))
		wcout << "WRONG b can create a" << endl;

	if (a.canCreateWord(L"rice"))
		wcout << "GOOD a can create b" << endl;*/


	for (size_t k = 0; k < seedCounter; k++)
	{
		for each (int p in answer_array[k])
		{
			wcout << p << " ";
		}
		wcout << endl;
	}


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
	return 0;
}