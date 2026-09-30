#include<iostream>
#include<string>
#include<fstream>

using namespace std;

int main(){
	ofstream fileGhi("G:\\uy1\\CODE\\C plus\\dictionary\\tudien.txt");
	if(fileGhi.is_open()){
		fileGhi << "Hello" << ": " << "Xin chao" << endl;
		fileGhi <<"Computer" << ": " << "May tinh" << endl;
		fileGhi.close();
		cout << "[OK] Da luu tu vung vao file tudien.txt thanh cong!" << endl;
	}
	
	ifstream fileDoc("G:\\uy1\\CODE\\C plus\\dictionary\\tudien.txt");
	string tuTiengAnh, nghiaTiengViet;
	cout << "\n--- DANH SACH TU DOC TU FILE ---\n";
	
	while(getline(fileDoc, tuTiengAnh, ':') && getline(fileDoc, nghiaTiengViet)){
		cout << "Tu: " << tuTiengAnh << " -> Nghia: " << nghiaTiengViet << endl;
	}
	fileDoc.close();
	return 0;
}
