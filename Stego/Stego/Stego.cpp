// Steganography Application Entry Point
// Start with RAW BMP files. They are not compressed so should be easier to work with.

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <bitset>

using namespace std;

int getHeaderSize(string);

int getMaxFilesize(string, int);

int getHiddenFileSize(string);

void doTheStego(string, string, string);

int main()
{
	string filePath = "c:\\users\\jake\\source\\repos\\stego\\bmptest.bmp";
	string outputPath = "c:\\test\\binary.lol";
	string hiddenFile = "c:\\test\\hideme.txt";
	/*cout << "Please enter the name of the host file you want to use: ";
	getline(cin, filename);
	cout << filename;
	*/
	int headerSize = getHeaderSize(filePath);
	cout << headerSize << "\n";
	cout << getMaxFilesize(filePath, headerSize) << "\n";
	cout << getHiddenFileSize(hiddenFile) << "\n";
	bitset<32> fileSize = 165444;
	cout << fileSize << "\n";
	doTheStego(filePath, hiddenFile, outputPath);
	cin >> outputPath; //Too lazy to put a pause in. This is just to pause to troubleshoot.
}

int getHeaderSize(string path) {
	char * memblock = new char [4];
	ifstream header;
	header.open(path, ios::binary | ios::in);
	if (!header.is_open()) {
		cout << "error!";
		return -1;
	}
	header.seekg(10);
	header.read(memblock, 4);
	return memblock[0];
	header.close();
}

int getMaxFilesize(string path, int headerSize){
	int availableSize;
	ifstream file(path, ios::binary | ios::ate);
	if (!file.is_open()) {
		return -1;
	}
	availableSize = ((int)file.tellg() - headerSize) / 8;
	file.close();
	return availableSize;
}

int getHiddenFileSize(string path) {
	int totalSize;
	ifstream file(path, ios::binary | ios::ate);
	if (!file.is_open()) {
		return -1;
	}
	totalSize = (int)file.tellg();
	file.close();
	return totalSize;
}

void doTheStego(string hostFile, string hiddenFile,string outputPath) {
	char * hostMem;
	char * hiddenMem;
	ifstream::pos_type size;
	int hostPos = 0;
	bitset<32> hiddenFileSize = getHiddenFileSize(hiddenFile); //First 32 changed bits are going to be the hidden file size
	int headerSpace = getHeaderSize(hostFile);
	
	ifstream host(hostFile, ios::binary | ios::ate);
	ifstream hidden(hiddenFile, ios::binary | ios::in);

	ofstream output(outputPath);
	size = host.tellg();

	hostMem = new char[size];
	host.seekg(0, ios::beg);
	host.read(hostMem, size);
	for ();
}