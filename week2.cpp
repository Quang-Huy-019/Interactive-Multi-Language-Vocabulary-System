#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<string>

using namespace std;

struct WordData{
	string english;
	string vietnamese;
	string type;
	int leitnerLevel;
	vector<string>synonyms;
	string example;
};

void saveToFile(const string& filename, const vector<WordData>& list){
	ofstream outFile(filename);
	if(!outFile.is_open()){
		cout << "Loi: Khong the mo file de ghi!" << endl;
		return;
	}
	for(const auto& word : list){
		outFile << word.english << "|"
				<< word.vietnamese << "|"
				<< word.type << "|"
				<< word.leitnerLevel << "|";
				
		for(size_t i = 0; i < word.synonyms.size(); i++){
			outFile << word.synonyms[i];
			if(i < word.synonyms.size() - 1){
				 outFile << ",";
			}
		}
		outFile << "|" << word.example << endl;
	}
	outFile.close();
	cout << "[OK] Da luu thanh cong " << list.size() << " tu vung vao file " << filename << endl;
}

vector<WordData>loadFromFile(const string & filename){
	vector<WordData>list;
	ifstream inFile(filename);
	if(!inFile.is_open()){
		cout << "Khong tim thay file " << filename << ". Se tao moi khi luu. \n";
		return list;
	}
	string line;
	string eng, viet, type, levelStr, synsStr, example;
	 while(getline(inFile, line)){
	 	if(line.empty()){
	 		continue;
		 }
	 	
	 	stringstream ss(line);
	 
	 	
	 	getline(ss, eng, '|');
	 	getline(ss, viet, '|');
	 	getline(ss, type, '|');
	 	getline(ss, levelStr, '|');
	 	getline(ss, synsStr, '|');
	 	getline(ss, example, '|');
	 	
	 	WordData w;
	 	w.english = eng;
	 	w.vietnamese = viet;
	 	w.type = type;
	 	w.example = example;
	 	w.leitnerLevel = stoi(levelStr);
	 	
	 	stringstream ssSyns(synsStr);
	 	string syn;
	 	while(getline(ssSyns, syn, ',')){
	 		if(!syn.empty()){
	 			 w.synonyms.push_back(syn);
			 }
		 }
	 	list.push_back(w);
	 }
	 inFile.close();
	 return list;
}

int main(){
	vector<WordData>sampleList = {
	{"Hello", "Xin chao", "GeneralWord", 1, {"hi", "welcome"}, "Hello world"},
	{"Algorithm", "Thuat toan", "TechnicalTerm", 2, {"procedure",  "formular"}, "Fast algorithm"}
};

		saveToFile("G:\\uy1\\CODE\\C plus\\project\\tudien_v2.txt", sampleList);
	vector<WordData>loadedList = loadFromFile("G:\\uy1\\CODE\\C plus\\project\\tudien_v2.txt");
	cout << "\n--- DANH SACH TU VUNG CO CAU TRUC DAY DU ---\n";
	for(const auto& w : loadedList){
		cout << "Tu: " << w.english << " | Nghia: " << w.vietnamese	<< " | Loai: " << w.type << " | Level Leitner: " << w.leitnerLevel << " | Dong nghia: ";
	for(const auto& s : w.synonyms) cout << s << " ";
	cout << "| Vi du: " << w.example << endl;
	}
	return 0;
}
