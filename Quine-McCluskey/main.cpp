/*
* K�sz�tette: Varga D�vid Zsolt
*/

#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <cstring>
#include <math.h>
#include <Windows.h>

using namespace std;

/*
* Debug �zenetek megjelen�t�se
*/
bool debugMode = false;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
int varCount;
int steps = 0;
string mintermList;

stringstream converter;
vector<string> minterms;
vector<string> primeImplicants;
vector<string> lastTable;

/*
* F�ggv�nyek defini�l�sa
*/
vector<string> simplifyMinterms();
string replaceDontCares(string, string);
string toBinary(int);
void getVarCount();
void getMinterms();
void mintermToVariable(string, string&, string&);
void print();
void printTable(vector<string>);
void color(int);
bool isNumber(string);
bool isGrayCode(string, string);
bool foundMinterm(vector<string>, string);
bool isEqual(vector<string>, vector<string>);



/*
* F� f�ggv�ny
*/
int main() {
    system("title Quine McCluskey v1.0 by.: Varga David");
    color(7);
    cout << "Udvozollek a Quine McCluskey programban." << endl;
    color(14);
    cout << "ENTER > Program inditasa normal modban" << endl << "Q     > Program inditasa levezeteses modban";
    char key = _getch();
    if (key == 'Q' || key == 'q') debugMode = true;
    system("cls");
    if (debugMode) {
        system("title Quine McCluskey by.: Varga David - Levezeteses Mod");
    }
    bool run = true;
    while (run) {
        steps = 0;
        minterms.clear();
        lastTable.clear();
        getVarCount();
        getMinterms();
        sort(minterms.begin(), minterms.end());
        color(8);
        cout << "----------------------------------------------------------" << endl;
        color(10);
        cout << "Valtozok szama: " << varCount << endl;
        cout << "Mintermek: " << mintermList << endl;
        color(8);
        cout << "----------------------------------------------------------" << endl << endl;
        do {
            minterms = simplifyMinterms();
            sort(minterms.begin(), minterms.end());
        } while (!isEqual(minterms, simplifyMinterms()));
        print();
        color(10);
        cout << endl;
        cout << "Ha ujabb egyszerusitest szeretnel elvegezni nyomd meg az ENTER gombot..." << endl;
        cout << "Ha ki szeretnel lepni a programbol, nyomd meg az ESC gombot...";
        int be = _getch();
        system("cls");
        if (be == 27) run = false;

    }
    return 0;
}

/*
* Egyszer�s�t� algoritmus:
* 1. Az �sszes mintermet �sszehasonl�tja
* 2. Szomsz�dos mintermek keres�se (Gray-k�d alapj�n)
* 3. Bejel�li, hogy melyik szomsz�dokat ellen�rizte
* 4. A szomsz�dokat is �sszehasonl�tja, majd megn�zi, hogy �ssze tud -e vonni biteket, ha igen, akkor �sszevonja �s belerakja az �j list�ba (Ha m�g nincs benne olyan)
* 5. Amelyik mintermeket nem tudta �sszehasonl�tani, azt belerakja az �j list�ba v�ltozatlanul (Ha m�g nincs benne olyan)
* 6. Visszat�r az �j list�val
* 7. Addig fut az algoritmus, am�g nem tal�l t�bb �sszevonhat�, mintermet
*/
vector<string> simplifyMinterms() {
    if (debugMode && !isEqual(lastTable, minterms)) {
        color(10);
        cout << ++steps << ". Lepes " << endl;
        color(7);
        cout << "--------------" << endl;
        color(10);
        printTable(minterms);
        color(7);
        cout << endl << "Osszevonhato mintermek keresese..." << endl;
        color(10);
    }


    vector <string> simplifiedMinterms;
    vector <string> temp;
    bool canSimplify = false;
    int size = minterms.size();
    int* checkedMinterms = new int[size];

    /*
    * Mintermek �sszehasonl�t�sa
    */
    for (int i = 0; i < size; i++) {
        for (int j = i; j < size; j++) {
            /*
            * Szomsz�ds�g ellen�rz�se (Gray k�d)
            */
            if (isGrayCode(minterms[i], minterms[j])) {
                /*
                * Ellen�rz�tt mintermek megjel�l�se
                */
                checkedMinterms[i] = 1;
                checkedMinterms[j] = 1;
                /*
                * �sszevonhat� mintermek ki�rat�sa
                */
                if (debugMode && !isEqual(lastTable, minterms)) cout << minterms[i] << " es " << minterms[j] << " osszevonhato -> " << replaceDontCares(minterms[i], minterms[j]) << endl;
                temp.push_back(replaceDontCares(minterms[i], minterms[j]));
                /*
                * �sszevont mintermek ellen�rz�se. Ha van ugyan olyan minterm a t�bl�ba, akkor a k�vetkez�t m�r nem �rja bele
                */
                if (!foundMinterm(simplifiedMinterms, replaceDontCares(minterms[i], minterms[j]))) {
                    simplifiedMinterms.push_back(replaceDontCares(minterms[i], minterms[j]));
                }
                canSimplify = true;
            }
        }
    }

    /*
    * Nem �sszevonhat� mintermek be�r�sa az �j t�bl�ba
    */
    for (int i = 0; i < size; i++) {
        if (checkedMinterms[i] != 1 && !foundMinterm(simplifiedMinterms, minterms[i])) {
            temp.push_back(minterms[i]);
            simplifiedMinterms.push_back(minterms[i]);
        }
    }

    /*
    * Mintermek egyszer�s�t�s�nek levezet�se
    */
    if (debugMode) {
        if (!isEqual(lastTable, minterms)) {
            sort(temp.begin(), temp.end());
            if (canSimplify) {
                color(7);
                cout << endl << "Mintermek osszevonasa utan:" << endl;
                color(10);
                printTable(temp);
                color(7);
                cout << endl << "Azonos mintermek leegyszerusitese 1db-ra..." << endl;
                cout << "Leegyszerusites utan:" << endl;
                sort(simplifiedMinterms.begin(), simplifiedMinterms.end());
                color(10);
                printTable(simplifiedMinterms);
                color(7);
                cout << "--------------\n\n\n\n\n\n";
            }
            else {
                color(7);
                cout << "Nincs tobb osszevonhato minterm..." << endl;
                cout << "--------------\n";
            }
        }
    }


    lastTable = minterms;
    delete[] checkedMinterms;
    /*
    * Visszat�r�s az egyszer�s�tett t�bl�val. L�p�s v�ge.
    */
    return simplifiedMinterms;
}

/*
* Decim�lis sz�m konvert�l�sa bin�riss�
*/
string toBinary(int i) {
    /*
    * Konvert�l�s
    */
    string binary = "";
    while (i > 0) {
        if ((i % 2) == 0) binary = "0" + binary;
        else binary = "1" + binary;
        i = i / 2;
    }
    /*
    * V�ltoz�k sz�m�nak alapj�n kig�sz�ti a minterm bin�ris alakj�t null�kkal.
    * pl.: V�ltoz�k sz�ma 4, Minterm sz�ma 3
    * 3 = 11
    * 2db 0 hozz�ad�sa a v�ltoz�k sz�ma miatt
    * eredm�ny: 0011
    */
    int remainZero = varCount - binary.length();
    for (int i = 0; i < remainZero; i++) {
        binary = "0" + binary;
    }
    return binary;
}

/*
* V�ltoz�k sz�m�nak bek�r�se
*/
void getVarCount() {
    string be;
    color(10);
    cout << "Kerlek add meg a valtozok szamat: ";
    cin >> be;
    if (!isNumber(be)) {
        system("cls");
        color(12);
        cout << "Kerlek szamot adj meg!" << endl;
        getVarCount();
    }
    converter.clear();
    converter << be;
    converter >> varCount;
    if (varCount < 1) {
        system("cls");
        color(12);
        cout << "A valtozok szamanak 1 vagy tobbnek kell lennie!" << endl;
        getVarCount();
    }
    system("cls");
}

/*
* Mintermek bek�r�se
*/
void getMinterms() {
    minterms.clear();
    lastTable.clear();
    string input;
    color(10);
    cout << "Kerlek add meg a mintermeket vesszovel elvalasztva!" << endl;
    cout << "Egy mintermnek 0 es " << pow(2, varCount) - 1 << " kozott kell lennie es egy minterm maximum 1x szerepelhet!" << endl << endl;
    color(8);
    cout << "i="; cin >> input;
    mintermList = input;
    istringstream is(input);
    string minterm;
    int mintermNumber;
    /*
    * Mintermek sz�tv�laszt�sa
    */
    while (getline(is, minterm, ',')) {
        converter.clear();
        converter << minterm;
        converter >> mintermNumber;
        if (!isNumber(minterm)) {
            color(12);
            system("cls");
            cout << "Kerlek, vesszovel elvalaszott szamokat adj meg!" << endl;
            getMinterms();
        }
        if (mintermNumber >= pow(2, varCount) || mintermNumber < 0) {
            color(12);
            system("cls");
            cout << "Kerlek csak, 0 es " << pow(2, varCount) - 1 << " kozott adj meg szamot!" << endl;
            getMinterms();
        }
        minterms.push_back(toBinary(mintermNumber));
    }
    system("cls");
}

/*
* A f�ggv�ny ellen�rzi, hogy k�t minterm 1 bitben t�r -e el
* pl.: mintermA = 0101, mintermB = 0100 akkor a f�ggv�ny igazzal t�r vissza
*/
bool isGrayCode(string mintermA, string mintermB) {
    int notEqualBits = 0;
    for (int i = 0; i < mintermA.length(); i++) {
        if (mintermA[i] != mintermB[i]) notEqualBits++;
    }
    return (notEqualBits == 1);
}

/*
* A f�ggv�ny ellen�rzi, hogy az adat sz�m e
*/
bool isNumber(string str) {
    for (int i = 0; i < str.length(); i++) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

/*
* A f�ggv�ny kicser�li "X" karakterre azokat a biteket, amelyek ellent�tesek (pon�lt vagy neg�lt)
* pl.: 0110 vagy 0111 -> 011X
* A k�t�jelet az�rt �rjuk, mivel azokat a biteket �ssze tudtuk vonni, ez�rt t�bbet nem foglalkozunk vel�k
*/
string replaceDontCares(string mintermA, string mintermB) {
    string a = mintermA;
    for (int i = 0; i < mintermA.length(); i++) {
        if (mintermA[i] != mintermB[i]) {
            a[i] = 'X';
        }
    }
    return a;
}

/*
* A f�ggv�ny lellen�rzi, hogy a minterm megtal�lhat� e a megadott list�ban
*/
bool foundMinterm(vector<string> mVector, string minterm) {
    for (int i = 0; i < mVector.size(); i++) {
        if (mVector[i].compare(minterm) == 0) return true;
    }
    return false;
}

/*
* A f�ggv�ny lellen�rzi, hogy a k�t minterm lista megegyezik
*/
bool isEqual(vector<string> vectorA, vector<string> vectorB) {
    if (vectorA.size() != vectorB.size()) return false;
    sort(vectorA.begin(), vectorA.end());
    sort(vectorB.begin(), vectorB.end());
    for (int i = 0; i < vectorA.size(); i++) {
        if (vectorA[i] != vectorB[i]) {
            return false;
        }
    }
    return true;
}

/*
* A k�vetkez� sorok sz�n�nek be�ll�t�sa
*/
void color(int color) {
    SetConsoleTextAttribute(hConsole, color);
}

/*
* Mintermlista ki�rat�sa t�bl�zatba
*/
void printTable(vector<string> temp) {
    if (!isEqual(lastTable, minterms)) {
        for (int i = 0; i < temp.size(); i++) {
            string ki = temp[i];
            for (int j = 0; j < temp[i].size(); j++) {
                cout << ki[j] << " ";
            }
            cout << endl;
        }
    }
}

/*
* Mintermek ki�rat�sa a k�perny�re
*/
void print() {
    string first = "    ", second = "Q = ";
    cout << endl;
    color(8);
    cout << "----------------------------------------------------------" << endl;
    color(7);
    cout << "Egyszerusites befejezodott! Eredmeny: " << endl;
    color(10);
    for (int i = 0; i < minterms.size(); i++) {
        if (i + 1 < minterms.size()) {
            mintermToVariable(minterms[i], first, second);
            first += "   ";
            second += " + ";
        }
        else {
            mintermToVariable(minterms[i], first, second);
        }
    }
    cout << first << endl << second << endl << endl;
    color(8);
    cout << "----------------------------------------------------------" << endl;
}

/*
*
* Bin�ris mintermek �talak�t�sa v�ltoz�kk�
*             __ _
* pl.: 0010 = ABCD
*
*/
void mintermToVariable(string bin, string& firstLine, string& secondLine) {
    string variables = "ABCDEFGHIKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < bin.length(); i++) {
        switch (bin[i])
        {
        case '0':
            firstLine += "_";
            secondLine += variables[i];
            break;
        case '1':
            firstLine += " ";
            secondLine += variables[i];
            break;
        default:
            break;
        }
    }
}
