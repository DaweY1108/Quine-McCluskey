/*
* Quine-McCluskey egyszer�s�t� algoritmus
* K�sz�tette: Varga D�vid Zsolt, Neptun k�d: OTKHSM
* Neumann J�nos Egyetem
*/

//Teszt 0,1,2,3,4,5,9,11,12,13,16,17,18,19,25,27
//Teszt 0,1,4,5,9,10,13,14,16,17,20,21,25,26,27,29

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
#include <map>
#include <Windows.h>

using namespace std;

struct mData {
	vector<string> binary;
	vector<string> decimal;
};

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);


mData minterms;
mData toPrint;
mData lastTable;
mData hazardFree;
mData simplest;
string mintermPrint;
vector<string> mintermList;
vector<string> helpFunction;
map<string, mData> mMap;
int varCount;
int steps = 0;

bool detailMode = false;
bool run = false;

void setup();
void color(int);
void getVarCount();
void getMintermData();
void mintermToVariable(string, string&, string&);
void sortByWeightNum(vector<string>& vec);
void print(mData);
void printTable(vector<string>);
void primeImplicantTable();
int getWeightNum(string);
bool compare(string, string);
bool isNumber(string);
bool isGrayCode(string, string);
bool foundMinterm(vector<string>, string);
bool isEqual(vector<string>, vector<string>);
string toBinary(int);
string replaceDontCares(string, string);
string getDontCareBits(string);
string toRomanNumber(int n);
vector<string> listToVec(string);
vector<string> simplifyHelpFuncton();
mData simplifyMinterms();

int main() {
	setup();
    while (run) {
        steps = 0;
        /*
        * Adatok bek�r�se
        */
        getVarCount();
        getMintermData();
        /*
        * Adatok ki�rat�sa
        */
        color(8);
        cout << "----------------------------------------------------------" << endl;
        color(10);
        cout << "Valtozok szama: " << varCount << endl;
        cout << "Mintermek: " << mintermPrint << endl;
        color(8);
        cout << "----------------------------------------------------------" << endl << endl;
        /*
        * Egyszer�s�t�s els� l�p�se
        */
        if (detailMode) cout << endl << endl << "------------------Egyszerusites 1. Lepese-----------------" << endl << endl;
        do {
            minterms = simplifyMinterms();
        } while (!isEqual(minterms.binary, simplifyMinterms().binary));
        hazardFree = minterms;
        color(8);
        if (detailMode) cout << endl << endl << "----------------------------------------------------------" << endl << endl;
        /*
        * Egyszer�s�t�s m�sodik l�p�se
        */
        if (detailMode) cout << endl << endl << "------------------Egyszerusites 2. Lepese-----------------" << endl << endl;
        primeImplicantTable();
        color(8);
        if (detailMode) cout << endl << endl << "----------------------------------------------------------" << endl << endl;
        /*
        * Eredm�ny ki�rat�sa a k�perny�re
        */
        cout << endl << endl << "--------------------Egyszerusites vege--------------------" << endl << endl;
        color(7);
        cout << "Egyszerusites befejezodott!" << endl << endl;
        cout << "Legegyszerubb alak:" << endl << endl;
		print(simplest);
        color(7);
        cout << "Legegyszerubb hazardmentes alak:" << endl << endl;
        print(hazardFree);
        color(8);
        cout << "----------------------------------------------------------" << endl << endl;
        /*
        * Egyszer�s�t�s v�ge
        */
        cout << "Ha ujabb egyszerusitest szeretnel elvegezni nyomd meg az ENTER gombot..." << endl;
        cout << "Ha ki szeretnel lepni a programbol, nyomd meg az ESC gombot...";
        int be = _getch();
        system("cls");
        if (be == 27) run = false;
    }
    return 0;
}

/*
* Program ind�t�si be�ll�t�sai
*/
void setup() {
    system("title Quine McCluskey v2.0 by.: Varga David Zsolt");
    color(7);
    cout << "Udvozollek a Quine McCluskey programban." << endl;
    color(14);
    cout << "ENTER > Program inditasa normal modban" << endl << "Q     > Program inditasa levezeteses modban";
    char key = _getch();
    if (key == 'Q' || key == 'q') detailMode = true;
    system("cls");
    if (detailMode) {
        system("title Quine McCluskey v2.0 by.: Varga David Zsolt - Levezeteses Mod");
    }
    run = true;
}

/*
* Mintermek bek�r�se, majd elt�rol�sa
*/
void getMintermData() {
    minterms.binary.clear();
    minterms.decimal.clear();
    mintermList.clear();
    string temp = "";
    mintermPrint = "";
	
    color(10);
	cout << "Kerlek add meg a mintermeket vesszovel elvalasztva! pl.: 1,4,5,6" << endl;
	cout << "Egy minterm erteke 0 es " << pow(2, varCount) - 1 << " kozott lehet!" << endl << endl;
    color(8);
    cout << "i=";
    cin >> temp;

    istringstream is(temp);
    string m;
    int n;
    
    while (getline(is, m, ',')) {
        if (isNumber(m)) {
            n = stoi(m);
            int max = pow(2, varCount) - 1;
            if (n <= max && n >= 0) {
                minterms.binary.push_back(toBinary(n));
                minterms.decimal.push_back(m);
                mintermList.push_back(m);
                mintermPrint = temp;
            }
            else {
                system("cls");
                color(12);
                cout << "A mintermek erteke 0 es " << max << " kozott lehet. Kerlek, probald ujra!" << endl;
                is.clear();
                getMintermData();
            }
        }
        else {
            system("cls");
            color(12);
            cout << "Csak szamokat adhatsz meg vesszovel elvalasztva. Kerlek, probald ujra!" << endl;
            is.clear();
            getMintermData();
        }
    }
    toPrint = minterms;
    system("cls");
}

/*
* V�ltoz�k sz�m�nak bek�r�se n > 0
*/
void getVarCount() {
    string temp;
    int number = 0;
	
	color(10);
	cout << "Kerlek add meg a valtozok szamat: ";
	cin >> temp;
	
    if (isNumber(temp)) {
        number = stoi(temp);
        if (number > 0) {
            varCount = number;
        }
        else {
            system("cls");
            color(12);
            cout << "Csak 0-nal nagyobb szamot adhatsz meg. Kerlek, probald ujra!" << endl;
            getVarCount();
        }
	}
	else {
        system("cls");
		color(12);
		cout << "Csak szamot adhatsz meg. Kerlek, probald ujra!" << endl;
		getVarCount();
	}
    system("cls");
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
mData simplifyMinterms() {
    if (detailMode && !isEqual(lastTable.binary, minterms.binary)) {
        color(10);
        cout << toRomanNumber(++steps) << ". Oszlop " << endl;
        color(7);
        cout << "--------------" << endl;
        color(10);
        for (int i = 0; i < toPrint.decimal.size(); i++) {
            cout << toPrint.decimal[i] << getDontCareBits(toPrint.binary[i]) << endl;
        }
        cout << endl;
        color(7);
        cout << "Osszevonhato mintermek keresese..." << endl;
        color(10);
    }


    mData simplifiedMinterms;
    mData tempToPrint;
    bool canSimplify = false;
    int size = minterms.binary.size();
    int* checkedMinterms = new int[size];

    /*
    * Mintermek �sszehasonl�t�sa
    */
    for (int i = 0; i < size; i++) {
        for (int j = i; j < size; j++) {
            /*
            * Szomsz�ds�g ellen�rz�se (Gray k�d)
            */
            if (isGrayCode(minterms.binary[i], minterms.binary[j])) {
                /*
                * Ellen�rz�tt mintermek megjel�l�se
                */
                checkedMinterms[i] = 1;
                checkedMinterms[j] = 1;
                /*
                * �sszevonhat� mintermek ki�rat�sa
                */
                if (detailMode && !isEqual(lastTable.binary, minterms.binary)) cout << minterms.decimal[i] << " es " << minterms.decimal[j] << " osszevonhato -> " << minterms.decimal[i] << "," << minterms.decimal[j] << getDontCareBits(replaceDontCares(minterms.binary[i], minterms.binary[j])) << endl;
				
                /*
                * �sszevont mintermek ellen�rz�se. Ha van ugyan olyan minterm a t�bl�ba, akkor a k�vetkez�t m�r nem �rja bele
                */
                if (!foundMinterm(simplifiedMinterms.binary, replaceDontCares(minterms.binary[i], minterms.binary[j]))) {
                    tempToPrint.decimal.push_back(minterms.decimal[i] + "," + minterms.decimal[j]);
                    tempToPrint.binary.push_back(replaceDontCares(minterms.binary[i], minterms.binary[j]));
                    simplifiedMinterms.decimal.push_back(minterms.decimal[i] + "," + minterms.decimal[j]);
                    simplifiedMinterms.binary.push_back(replaceDontCares(minterms.binary[i], minterms.binary[j]));
                }
                canSimplify = true;
            }
        }
    }

    /*
    * Nem �sszevonhat� mintermek be�r�sa az �j t�bl�ba
    */
    for (int i = 0; i < size; i++) {
        if (checkedMinterms[i] != 1 && !foundMinterm(simplifiedMinterms.binary, minterms.binary[i])) {
            simplifiedMinterms.decimal.push_back(minterms.decimal[i]);
            simplifiedMinterms.binary.push_back(minterms.binary[i]);
        }
    }

    /*
    * Mintermek egyszer�s�t�s�nek levezet�se
    */
    if (detailMode) {
        if (!isEqual(lastTable.binary, minterms.binary)) {
            if (canSimplify) {
                color(7);
                cout << "--------------\n\n\n";
                cout << "  |  |  |  |  " << endl;
                cout << "  V  V  V  V  \n\n\n";
            }
            else {
                color(7);
                cout << "Nincs tobb osszevonhato minterm..." << endl;
                cout << "--------------\n";
            }
        }
    }
    toPrint = tempToPrint;
    lastTable = minterms;
    delete[] checkedMinterms;
    /*
    * Visszat�r�s az egyszer�s�tett t�bl�val. L�p�s v�ge.
    */
    return simplifiedMinterms;
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
* A f�ggv�ny lek�ri, hogy melyek azok a bitek, amelyekkel nem foglalkozunk. pl.: 1X1X -> (1,4)
*/
string getDontCareBits(string minterm) {
    int id = minterm.size() - 1;
    bool foundDontCare = false;
    string s = "";
    for (int i = 0; i < minterm.size(); i++) {
        if (minterm[i] == 'X') {
            s += (to_string((int)pow(2, id)) + ",");
            foundDontCare = true;
        }
        id--;
    }
    if (foundDontCare) {
        s = "(" + s;
        s[s.size() - 1] = ')';
    }
    return s;
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
* A f�ggv�ny ellen�rzi, hogy a bemenet sz�m-e
*/
bool isNumber(string str) {
    for (int i = 0; i < str.length(); i++) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

/*
* Egyszer�s�t�s m�sodik l�p�se pr�mimplik�ns t�bl�zattal
*/
void primeImplicantTable() {
    string letters = "abcdefghijklmnopqrstuvwxyz";
    int row = minterms.binary.size();
    int column = mintermList.size();

    /*
    * Dinamikus t�mb l�trehoz�sa
    */
    int** xes = new int* [row];
    for (int i = 0; i < row; i++) {
		xes[i] = new int[column];
    }

    /*
    * T�mb felt�lt�se �res karakterekkel
    */
    for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
            xes[i][j] = ' ';
		}
    }
	
    /*
    * Pr�mimplik�ns t�bla felt�lt�se
    */
    for (int i = 0; i < minterms.decimal.size(); i++) {
        vector<string> vec = listToVec(minterms.decimal[i]);
        for (int j = 0; j < vec.size(); j++) {
            for (int k = 0; k < mintermList.size(); k++) {
                if (vec[j] == mintermList[k]) xes[i][k] = 'X';
            }
        }
    }

    /*
    * �sszevont elemek megjel�l�se egy kisbet�vel
    */
    color(10);
    for (int i = 0; i < minterms.decimal.size(); i++) {
        if (detailMode) cout << letters[i] << ", " << minterms.decimal[i] << getDontCareBits(minterms.binary[i]) << endl;
    }

    /*
    * Pr�mimplik�ns t�bl�zat ki�rat�sa
    */
    color(7);
    if (detailMode) cout << endl << "Primimplikans tablazat: " << endl << endl;
    color(10);

    for (int i = 0; i <= row; i++) {
        for (int j = 0; j <= column; j++) {
            if (j == 0 && i == 0) if (detailMode) cout << left << setw(5) << " ";
            if (j == 0 && i != 0) if (detailMode) cout << left << setw(5) << letters[i - 1];
            if (i == 0 && j != 0) if (detailMode) cout << left << setw(5) << mintermList[j - 1];
            if (i != 0 && j != 0) if (detailMode) cout << left << setw(5) << (char) xes[i - 1][j - 1];
        }
        if (detailMode) cout << endl;
    }

    /*
    * X seg�df�ggv�ny ki�rat�sa a pr�mimplik�ns t�bla alapj�n
    */
    color(7);
    if (detailMode) cout << endl << "Segedfuggveny: " << endl << endl;
    color(10);
    if (detailMode) cout << "X=";
    string temp;
    string functionS = "";
    for (int i = 0; i < column; i++) {
        temp = "";
        functionS += "(";
        for (int j = 0; j < row; j++) {
            if ((char)xes[j][i] == 'X') {
                temp += letters[j];
                functionS = functionS + letters[j] + "+";
            }
        }
        functionS[functionS.size() - 1] = ')';
        helpFunction.push_back(temp);
    }
    if (detailMode) cout << functionS << endl;

    /*
    * X seg�df�ggv�ny egyszer�s�t�se
    */
    helpFunction = simplifyHelpFuncton();

    /*
    * Egyszer�s�tett x seg�df�ggv�ny ki�rat�sa
    */
    color(7);
    if (detailMode) cout << endl << "Egyszerusitett segedfuggveny: " << endl << endl;
    color(10);
    if (detailMode) cout << "X=";
    for (int i = 0; i < helpFunction.size(); i++) {
        if (i + 1 < helpFunction.size()) {
            if (detailMode)cout << helpFunction[i] << "*";
        }
        else {
            if (detailMode)cout << helpFunction[i];
        }
    }

    /*
    * Legegyszer�bb alak ki�rat�sa
    */
    for (int i = 0; i < helpFunction.size(); i++) {
        if (helpFunction[i].size() > 1) {
            string s = helpFunction[i];
            string s2 = "";
            s2 += s[0];
            helpFunction[i] = s2;
        }
    }
    if (detailMode) cout << endl;
    color(7);
    if (detailMode) cout << endl << "Legegyszerubb alak: " << endl << endl;
    color(10);
    if (detailMode) cout << "Q=";
    for (int i = 0; i < helpFunction.size(); i++) {
        if (i + 1 < helpFunction.size()) {
            if (detailMode) cout << helpFunction[i] << "+";
        }
        else {
            if (detailMode) cout << helpFunction[i];
        }
    }

    /*
    * Legegyszer�bb alak adatainak bet�lt�se a t�rol�ba
    */
    simplest.binary.clear();
    simplest.decimal.clear();
    if (detailMode) cout << endl << endl;
    for (int i = 0; i < helpFunction.size(); i++) {
        string s = helpFunction[i];
        for (int j = 0; j < letters.size(); j++) {
            if (s[0] == letters[j]) {
                if (detailMode) cout << letters[j] << ", " << minterms.decimal[j] << getDontCareBits(minterms.binary[j]) << " > " << minterms.binary[j] << endl;
                simplest.binary.push_back(minterms.binary[j]);
                simplest.decimal.push_back(minterms.decimal[j]);
            }
        }
    }
}

/*
* Az X seg�df�ggv�ny egyszer�s�t�se a A(A+B) = A alapj�n.
* 1. L�p�s: Sorbarendez�s hossz alapj�n
* 2. L�p�s: Keres�s, hogy van e m�g ugyan olyan v�ltoz� benne, ha van, akkor azt a v�ltoz�t ki�ti
* 3. L�p�s: Megn�zi melyik v�ltoz�k nem lettek ki�tve, �s azokat beteszi egy �j vektorba
* 4. L�p�s: Visszat�r�s az egyszer�s�tett vektorral
*/
vector<string> simplifyHelpFuncton() {
    int size = helpFunction.size();
    sort(helpFunction.begin(), helpFunction.end(), compare);
    vector<string> temp;

	for (int i = 0; i < size; i++) {
		for (int j = i + 1; j < size; j++) {
			if (helpFunction[j].find(helpFunction[i]) != string::npos) {
				helpFunction[j] = "-";
			}
		}
	}
    for (int i = 0; i < size; i++) {
        if (helpFunction[i] != "-") {
            temp.push_back(helpFunction[i]);
        }
    }
    for (int i = 0; i < temp.size(); i++) {
        string s = temp[i];
		for (int j = i + 1; j < temp.size(); j++) {
            for (int k = 0; k < temp[i].size(); k++) {
                if (temp[j].find(s[k]) != string::npos) {
					temp.erase(temp.begin() + j);
                }
            }
		}
    }
    return temp;
}

/*
* Mintermek ki�rat�sa a k�perny�re
*/
void print(mData md) {
    string first = "    ", second = "Q = ";
    color(10);
    for (int i = 0; i < md.binary.size(); i++) {
        if (i + 1 < md.binary.size()) {
            mintermToVariable(md.binary[i], first, second);
            first += "   ";
            second += " + ";
        }
        else {
            mintermToVariable(md.binary[i], first, second);
        }
    }
    cout << first << endl << second << endl << endl;
    color(8);
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

/*
* A f�ggv�ny �talak�tja az arab sz�mokat r�mai sz�mokk�
*/
string toRomanNumber(int n) {
    string str_romans[] = {"M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I"};
    int values[] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
    string result = "";
    for (int i = 0; i < 13; ++i) {
        while (n - values[i] >= 0) {
            result += str_romans[i];
            n -= values[i];
        }
    }
    return result;
}

/*
* A f�ggv�ny kiszedi a lista elemeit egy vektorba
*/
vector<string> listToVec(string s) {
    istringstream is(s);
    string temp;
    vector<string> out;
    while (getline(is, temp, ',')) {
        out.push_back(temp);
    }
    return out;
}

/*
* Vektor sorbarendez�se s�lysz�m alapj�n
*/
void sortByWeightNum(vector<string>& vec) {
    int size = vec.size();
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (getWeightNum(vec[j]) > getWeightNum(vec[j + 1])) {
                swap(vec[j], vec[j + 1]);
            }
        }
    }
}

/*
* S�lysz�m kisz�m�t�sa
*/
int getWeightNum(string s) {
    int n = stoi(s);
    int count = 0;
    string bin = toBinary(n);
    for (int i = 0; i < bin.size(); i++) {
        if (bin[i] == '1') count++;
    }
    return count;
}

/*
* Konzol sz�n be�ll�t�sa
*/
void color(int id) {
    SetConsoleTextAttribute(hConsole, id);
}

/*
* �sszehaszonl�t�s sz�hossz alapj�n
*/
bool compare(string str1, string str2) {
    if (str1.length() < str2.length())
        return true;
    return false;
}


