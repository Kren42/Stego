// Jacob Kesler, Luis Campuzano, Zheng Chen
// Final Project - 4/14/2018
// BMP and JPG Steganogrpahy
// https://github.com/Kren42/Stego

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
using namespace std;

// Function prototypes
void launchMenu();
bool fileExists(string);
int getHeaderSize(string);
int getMaxFilesize(string, int);
int getHiddenFileSize(string);
void bmpStego(string, string, string, string);
void bmpUnstego(string, string);
bool checkPassword(string, string);
bitset<64> passwordToBits(string);
void jpegAppend(string, string, string);
void jpegSplit(string, string);

int main()
{
	launchMenu();
}

// Generates text-based menu for the application
void launchMenu() {
	while (true) {
		int choice = -1;
		cout << "Welcome to Simple Steganography.\nCurrent Supported filetypes: BMP, JPEG.\n";
		cout << "Please select an option:\n";
		cout << "1. Hide data in BMP file.\n";
		cout << "2. Unhide data in BMP file.\n";
		cout << "3. Hide data in JPG file.\n";
		cout << "4. Unhide data in JPG file.\n";
		cout << "5. Exit.\n";
		cin >> choice;
		if (choice == 1) {
			string carrier;
			string payload;
			string output;
			string password;
			cout << "Hide data selected.\n";
			cout << "Please enter the path of the carrier file: \n";
			cin >> carrier;
			while (!fileExists(carrier)) {
				cout << "Please enter a vaild path or 'x' to exit.\n";
				cin >> carrier;
				if (carrier == "x") {
					return;
				}
			}
			int maxFilesize = getMaxFilesize(carrier, getHeaderSize(carrier));
			cout << "The maximum size of the payload is: " << maxFilesize << " bytes.\n";
			cout << "Please enter the path to the payload file: \n";
			cin >> payload;
			while (!fileExists(payload)) {
				cout << "Please enter a valid path or 'x' to exit.\n";
				cin >> payload;
				if (payload == "x") {
					return;
				}
			}
			int payloadSize = getHiddenFileSize(payload);
			if (payloadSize > maxFilesize) {
				cout << "The carrier file is not large enough for that payload.\n";
				cout << "The payload is " << payloadSize - maxFilesize << " bytes too large.\n";
				return;
			}
			cout << "Please enter a path, including filename, for the output file: \n";
			cin >> output;
			cout << "Please enter an 8 character password.\n";
			cin >> password;
			while (password.size() != 8) {
				cout << "Password must be 8 characters.\n";
				cin >> password;
			}
			bmpStego(carrier, payload, output, password);
			cout << "Done!\n";
		}
		else if (choice == 2) {
			string source;
			string password;
			string output;
			cout << "Unhide data selected.\n";
			cout << "Please enter the path to the file with the payload: \n";
			cin >> source;
			while (!fileExists(source)) {
				cout << "Please enter a valid path or 'x' to exit.\n";
				cin >> source;
				if (source == "x") {
					return;
				}
			}
			cout << "Please enter the password to the file.\n";
			cin >> password;
			if (password.size() != 8 || !checkPassword(source, password)) {
				cout << "Incorrect Password!\n";
				return;
			}
			cout << "Please enter the output path and filename for the unhidden payload.\n";
			cin >> output;
			bmpUnstego(source, output);
			cout << "Done!\n";
		}
		else if (choice == 3) {
			string carrier, payload, output;
			cout << "JPEG Hide seleceted.\n";
			cout << "Enter the path to the carrier file.\n";
			cin >> carrier;
			while (!fileExists(carrier)) {
				cout << "Please enter a valid path or 'x' to exit.\n";
				cin >> carrier;
				if (carrier == "x") {
					return;
				}
			}
			cout << "Enter the path to the payload file.\n";
			cin >> payload;
			while (!fileExists(payload)) {
				cout << "Please enter a valid path or 'x' to exit.\n";
				cin >> payload;
				if (payload == "x") {
					return;
				}
			}
			cout << "Enter the path for the output.\n";
			cin >> output;
			jpegAppend(carrier, payload, output);
		}
		else if (choice == 4) {
			string carrier, output;
			cout << "JPEG Unhide discovered.\n";
			cout << "Enter the path to the carrier file: \n";
			cin >> carrier;
			while (!fileExists(carrier)) {
				cout << "Please enter a valid path or 'x' to exit.\n";
				cin >> carrier;
				if (carrier == "x") {
					return;
				}
			}
			cout << "Enter the path for the outpath file: \n";
			cin >> output;
			jpegSplit(carrier, output);
			cout << "Done!\n";
		}
		else if (choice == 5) {
			cout << "Exiting.\n";
			return;
		}
		else {
			cout << "Please enter a valid option.\n";
		}
	}
}

// Checks if a file exists
bool fileExists(string path) {
	std::ifstream infile(path);
	return infile.good();
}

// Gets the size of the header from a BMP file
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

// Calculates the max file size that a BMP carrier file can hide
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

//Gets the size of a file
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

// Does LSB Steganography on BMP files
void bmpStego(string hostFile, string hiddenFile,string outputPath, string password) {
	int hiddenFileSize = getHiddenFileSize(hiddenFile);
	bitset<32> bitsHiddenFileSize = hiddenFileSize;
	bitset<64> bitsPassword = passwordToBits(password);
	int headerSpace = getHeaderSize(hostFile);
	int maxFileSize = getMaxFilesize(hostFile, headerSpace);
	ifstream host(hostFile, ios::binary);
	ifstream hidden(hiddenFile, ios::binary);
	ofstream output(outputPath, ios::out | ios::binary);
	
	vector<char> hostBuffer((istreambuf_iterator<char>(host)), (istreambuf_iterator<char>()));
	vector<char> hiddenBuffer((istreambuf_iterator<char>(hidden)), (istreambuf_iterator<char>()));
	int hiddenCounter = 8;
	int bitSetCounter = 0;
	int sizebitSetCounter = 0;
	bitset<8> currentHiddenByte;

	for (int i = 0; i < hostBuffer.size(); i++) {
		if (i < headerSpace) {
			output << hostBuffer[i];
		} 
		else if (i >= headerSpace && i < headerSpace + 64) {
			bitset<8> currentByte = hostBuffer[i];
			if (currentByte[0] != bitsPassword[bitSetCounter]) {
				currentByte.flip(0);
			}
			output << hex << char(currentByte.to_ulong());
			bitSetCounter++;
		}
		else if (i >= headerSpace + 64 && i < headerSpace + 96) {
			bitset<8> currentByte = hostBuffer[i];
			if (currentByte[0] != bitsHiddenFileSize[sizebitSetCounter]) {
				currentByte.flip(0);
			}
			output << hex << char(currentByte.to_ulong());
			sizebitSetCounter++;
		}
		else if (i >= headerSpace + 96 && i < headerSpace + (hiddenFileSize*8) + 96) {
			if (hiddenCounter > 7) {
				hiddenCounter = 0;
				currentHiddenByte = hiddenBuffer[0];
				hiddenBuffer.erase(hiddenBuffer.begin());
			}
			bitset<8> currentHostByte = hostBuffer[i];
			if (currentHostByte[0] != currentHiddenByte[hiddenCounter]) {
				currentHostByte.flip(0);
			}
			output << hex << char(currentHostByte.to_ulong());
			hiddenCounter++;
		}
		else {
			output << hostBuffer[i];
		}
	}
}

//Extracts the payload from a BMP carrier file
void bmpUnstego(string stegoFile, string outputPath) {
	int headerSpace = getHeaderSize(stegoFile);
	bitset<32> sizeBits;
	int sizeCounter = 0;
	ifstream stego(stegoFile, ios::binary);
	ofstream output(outputPath, ios::out | ios::binary);
	vector<char> stegoBuffer((istreambuf_iterator<char>(stego)), (istreambuf_iterator<char>()));
	int extractCounter = 0;
	bitset<8> extractedByte;
	if (stego.fail()) {
		cout << "Unable to open file.\n";
		return;
	}
	for (int i = headerSpace+64; i < headerSpace + 96; i++) {
		bitset<8> currentByte = stegoBuffer[i];
		if (currentByte[0] != sizeBits[sizeCounter]) {
			sizeBits.flip(sizeCounter);
		}
		sizeCounter++;
	}
	int size = int(sizeBits.to_ulong());
	for (int i = headerSpace + 96; i <= headerSpace + 96 + (size * 8); i++) {
		if (extractCounter > 7) {
			extractCounter = 0;
			output << hex << char(extractedByte.to_ulong());
		}
		bitset<8> currentByte = stegoBuffer[i];
		if (currentByte[0] != extractedByte[extractCounter]) {
			extractedByte.flip(extractCounter);
		}
		extractCounter++;
	}
}

// Finds the password in a carrier file and compares it to the entered password. Numbers in the password may not work.
bool checkPassword(string stegoFile, string password) {
	int headerSpace = getHeaderSize(stegoFile);
	bitset<64> inputPasswordBits = passwordToBits(password);
	bitset<64> correctPasswordBits;
	ifstream stego(stegoFile, ios::binary);
	vector<char> stegoBuffer((istreambuf_iterator<char>(stego)), (istreambuf_iterator<char>()));
	int passwordCounter = 0;

	for (int i = headerSpace; i < headerSpace + 64; i++) {
		bitset<8> currentByte = stegoBuffer[i];
		if (currentByte[0] != correctPasswordBits[passwordCounter]) {
			correctPasswordBits.flip(passwordCounter);
		}
		passwordCounter++;
	}
	if (correctPasswordBits == inputPasswordBits) {
		return true;
	}
	else {
		return false;
	}
}

// Changes the password to a size 64 bitset
bitset<64> passwordToBits(string password) {
	bitset<64> passwordBits;
	int bitsCounter = 0;
	for (int i = 0; i < 8; i++) {
		bitset<8> currentChar = bitset<8>(password[i]);
		for (int x = 0; x < 8; x++) {
			if (passwordBits[bitsCounter] != currentChar[x]) {
				passwordBits.flip(bitsCounter);
			}
			bitsCounter++;
		}
	}
	return passwordBits;
}

// Appends a file to the end of a jpeg file
void jpegAppend(string carrier, string payload, string outputPath) {
	ifstream stego(carrier, ios::binary);
	ifstream hidden(payload, ios::binary);
	ofstream output(outputPath, ios::out | ios::binary);

	output << stego.rdbuf() << hidden.rdbuf();
}

// Splits the jpeg and the appended file
void jpegSplit(string stegoFile, string outputPath) {
	ifstream stego(stegoFile, ios::binary);
	vector<char> stegoBuffer((istreambuf_iterator<char>(stego)), (istreambuf_iterator<char>()));
	ofstream output(outputPath, ios::out | ios::binary);
	int EOFCount = 0;
	int SOFCount = 0;
	bool endOfJpeg = false;
	// JPEGs start with FFD8 and end with FFD9. This detects those to determine the end of the JPEG file.
	bitset<8> firstByteEOF(string("11111111"));
	bitset<8> secondByteEOF(string("11011001"));
	bitset<8> secondByteSOF(string("11011000"));
	for (int i = 0; i < stegoBuffer.size(); i++) {
		bitset<8> currentByte = stegoBuffer[i];
		if (!endOfJpeg && i < stegoBuffer.size() - 2) {
			bitset<8> nextByte = stegoBuffer[i + 1];
			if (currentByte == firstByteEOF && nextByte == secondByteSOF) {
				SOFCount++;
			}
			if (currentByte == firstByteEOF && nextByte == secondByteEOF) {
				EOFCount++;
			}
			if (SOFCount > 0 && SOFCount == EOFCount) {
				endOfJpeg = true;
				i += 2;
			}
		}
		if (endOfJpeg) {
			output << stegoBuffer[i];
		}
	}
}