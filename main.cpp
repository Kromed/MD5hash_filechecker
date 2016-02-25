#include <fstream>
#include <strsafe.h>
#include <iostream>
#include <string>
#include <map>
#include "MD5.h"

using namespace std;

int main(){

	map<string, int>hashmap;       //карта хэшей
	map<string, int>NewHashes;     //карта новых хэшей я использовал map для теоретического будущего внесения сведений о файле...мб убрать
	map<string, int>::iterator im;
	
	ifstream infile("hashbase");											//ИНИЦИАЛИЗАЦИЯ БАЗЫ ХЭШЕЙ
	if (infile.is_open()){ cout << "Hashbase is found!" << endl; }
	else {cout << "Error: No hashbases found!" << endl;	system("Pause");return 0;}
	while (!infile.eof()){
		char b[80];
		infile.getline(b, sizeof(b));
		hashmap.insert(pair<string, int>(b, 0));  
	}
	infile.close();
	cout << "Hashbase init sucessfully. Initialised: " << hashmap.size() - 1 << " hashes" << endl;

	int DelCount;
	int AddHashes;

	char	 Path[MAX_PATH];
	TCHAR	sPath[MAX_PATH];
	size_t length_of_path;
	char	 FileName[MAX_PATH];
	TCHAR	sFileName[MAX_PATH];
	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;

	char hash[50];
	char* ptr;
	long int FileSize;
	HANDLE  hFile;
	HANDLE  hMapping;

	while (1){
		DelCount = 0;
		AddHashes = 0;
		hFind = INVALID_HANDLE_VALUE;
		Path[0] = '\0';

		cout << "Please insert name of directory for scan, or type 'q' for quit ";
		cin >> Path;
		if ((Path == "q") || (Path == "Q")) { cout << "foooo" << endl; break; }
		// Check that the input path plus 3 is not longer than MAX_PATH. Three characters are for the "\*" plus NULL appended below.
		StringCchLength(Path, MAX_PATH, &length_of_path);
		if (length_of_path > (MAX_PATH - 3)){ cout << "Directory path is too long!" << endl; continue; }
		// Prepare string for use with FindFile functions.  First, copy the string to a buffer, then append '\*' to the directory name.
		StringCchCopy(sPath, MAX_PATH, Path);
		StringCchCat(sPath, MAX_PATH, TEXT("\\*"));
		// Find the first file in the directory.
		hFind = FindFirstFile(sPath, &FindFileData);
		if (INVALID_HANDLE_VALUE == hFind) { cout << "No such directory!" << endl; continue; }

		do
		{
			if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
				
				//Prepare full filename - with path
				StringCchCopy(sFileName, MAX_PATH, Path);			
				StringCchCat(sFileName, MAX_PATH, TEXT("\\"));
				StringCchCat(sFileName, MAX_PATH, TEXT(FindFileData.cFileName));
				//call hash function
				cout << FindFileData.cFileName << " ";
				hFile = CreateFile(sFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
				ptr = (char*)MapViewOfFile(hMapping, FILE_MAP_WRITE, 0, 0, 0);
				FileSize = (long int)GetFileSize(hFile, 0);
				GetMD5(ptr, FileSize, hash);
				cout << hash;
				UnmapViewOfFile(ptr);
				CloseHandle(hMapping);
				CloseHandle(hFile);
				if (hashmap.find(hash) == hashmap.end())//есть такой хэш?
				{
					cout << " unique" << endl;
					hashmap.insert(pair<string, int>(hash, 0));
					NewHashes.insert(pair<string, int>(hash, 0));//нет - внести хэш в  hashmap и NewHashes
					AddHashes++;
				}
				else {
					cout << " not unique" << endl;
					remove(sFileName);//хэш такого файла уже есть - удалить файл
					DelCount++;
				}
			}
		} while (FindNextFile(hFind, &FindFileData) != 0);
		FindClose(hFind);
		cout << "Directory scanned sucessfully!" << endl << "Deleted " << DelCount << " files" << endl << "Added " << AddHashes << " hashes" << endl;
		
	
		ofstream AddNHs("hashbase", ios::app);  //открываем базу хэшей в режиме добавления             
		for (im = NewHashes.begin(); im != NewHashes.end(); im++){ AddNHs << (*im).first << endl; } //Дописывание новых хэшей в hashbase
		AddNHs.close();
	}

	return 0;
}