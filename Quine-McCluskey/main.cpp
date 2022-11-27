/*
* Quine-McCluskey egyszerûsítõ algoritmus
* Készítette: Varga Dávid Zsolt, Neptun kód: OTKHSM
* Neumann János Egyetem
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
        * Adatok bekérése
        */
        getVarCount();
        getMintermData();
        /*
        * Adatok kiíratása
        */
        color(8);
        cout << "----------------------------------------------------------" << endl;
        color(10);
        cout << "Valtozok szama: " << varCount << endl;
        cout << "Mintermek: " << mintermPrint << endl;
        color(8);
        cout << "----------------------------------------------------------" << endl << endl;
        /*
        * Egyszerûsítés elsõ lépése
        */
        if (detailMode) cout << endl << endl << "------------------Egyszerusites 1. Lepese-----------------" << endl << endl;
        do {
            minterms = simplifyMinterms();
        } while (!isEqual(minterms.binary, simplifyMinterms().binary));
        hazardFree = minterms;
        color(8);
        if (detailMode) cout << endl << endl << "----------------------------------------------------------" << endl << endl;
        /*
        * Egyszerûsítés második lépése
        */
        if (detailMode) cout << endl << endl << "------------------Egyszerusites 2. Lepese-----------------" << endl << endl;
        primeImplicantTable();
        color(8);
        if (detailMode) cout << endl << endl << "----------------------------------------------------------" << endl << endl;
        /*
        * Eredmény kiíratása a képernyõre
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
        * Egyszerûsítés vége
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
* Program indítási beállításai
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
* Mintermek bekérése, majd eltárolása
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
* Változók számának bekérése n > 0
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
* Egyszerûsítõ algoritmus:
* 1. Az összes mintermet összehasonlítja
* 2. Szomszédos mintermek keresése (Gray-kód alapján)
* 3. Bejelöli, hogy melyik szomszédokat ellenõrizte
* 4. A szomszédokat is összehasonlítja, majd megnézi, hogy össze tud -e vonni biteket, ha igen, akkor összevonja és belerakja az új listába (Ha még nincs benne olyan)
* 5. Amelyik mintermeket nem tudta összehasonlítani, azt belerakja az új listába változatlanul (Ha még nincs benne olyan)
* 6. Visszatér az új listával
* 7. Addig fut az algoritmus, amíg nem talál több összevonható, mintermet
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
    * Mintermek összehasonlítása
    */
    for (int i = 0; i < size; i++) {
        for (int j = i; j < size; j++) {
            /*
            * Szomszédság ellenõrzése (Gray kód)
            */
            if (isGrayCode(minterms.binary[i], minterms.binary[j])) {
                /*
                * Ellenõrzött mintermek megjelölése
                */
                checkedMinterms[i] = 1;
                checkedMinterms[j] = 1;
                /*
                * Összevonható mintermek kiíratása
                */
                if (detailMode && !isEqual(lastTable.binary, minterms.binary)) cout << minterms.decimal[i] << " es " << minterms.decimal[j] << " osszevonhato -> " << minterms.decimal[i] << "," << minterms.decimal[j] << getDontCareBits(replaceDontCares(minterms.binary[i], minterms.binary[j])) << endl;
				
                /*
                * Összevont mintermek ellenõrzése. Ha van ugyan olyan minterm a táblába, akkor a következõt már nem írja bele
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
    * Nem összevonható mintermek beírása az új táblába
    */
    for (int i = 0; i < size; i++) {
        if (checkedMinterms[i] != 1 && !foundMinterm(simplifiedMinterms.binary, minterms.binary[i])) {
            simplifiedMinterms.decimal.push_back(minterms.decimal[i]);
            simplifiedMinterms.binary.push_back(minterms.binary[i]);
        }
    }

    /*
    * Mintermek egyszerûsítésének levezetése
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
    * Visszatérés az egyszerûsített táblával. Lépés vége.
    */
    return simplifiedMinterms;
}

/*
* A függvény ellenõrzi, hogy két minterm 1 bitben tér -e el
* pl.: mintermA = 0101, mintermB = 0100 akkor a függvény igazzal tér vissza
*/
bool isGrayCode(string mintermA, string mintermB) {
    int notEqualBits = 0;
    for (int i = 0; i < mintermA.length(); i++) {
        if (mintermA[i] != mintermB[i]) notEqualBits++;
    }
    return (notEqualBits == 1);
}

/*
* A függvény kicseréli "X" karakterre azokat a biteket, amelyek ellentétesek (ponált vagy negált)
* pl.: 0110 vagy 0111 -> 011X
* A kötõjelet azért írjuk, mivel azokat a biteket össze tudtuk vonni, ezért többet nem foglalkozunk velük
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
* A függvény lekéri, hogy melyek azok a bitek, amelyekkel nem foglalkozunk. pl.: 1X1X -> (1,4)
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
* A függvény lellenõrzi, hogy a minterm megtalálható e a megadott listában
*/
bool foundMinterm(vector<string> mVector, string minterm) {
    for (int i = 0; i < mVector.size(); i++) {
        if (mVector[i].compare(minterm) == 0) return true;
    }
    return false;
}

/*
* A függvény lellenõrzi, hogy a két minterm lista megegyezik
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
* Decimális szám konvertálása binárissá
*/
string toBinary(int i) {
    /*
    * Konvertálás
    */
    string binary = "";
    while (i > 0) {
        if ((i % 2) == 0) binary = "0" + binary;
        else binary = "1" + binary;
        i = i / 2;
    }
    /*
    * Változók számának alapján kigészíti a minterm bináris alakját nullákkal.
    * pl.: Változók száma 4, Minterm száma 3
    * 3 = 11
    * 2db 0 hozzáadása a változók száma miatt
    * eredmény: 0011
    */
    int remainZero = varCount - binary.length();
    for (int i = 0; i < remainZero; i++) {
        binary = "0" + binary;
    }
    return binary;
}

/*
* A függvény ellenõrzi, hogy a bemenet szám-e
*/
bool isNumber(string str) {
    for (int i = 0; i < str.length(); i++) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

/*
* Egyszerûsítés második lépése prímimplikáns táblázattal
*/
void primeImplicantTable() {
    string letters = "abcdefghijklmnopqrstuvwxyz";
    int row = minterms.binary.size();
    int column = mintermList.size();

    /*
    * Dinamikus tömb létrehozása
    */
    int** xes = new int* [row];
    for (int i = 0; i < row; i++) {
		xes[i] = new int[column];
    }

    /*
    * Tömb feltöltése üres karakterekkel
    */
    for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
            xes[i][j] = ' ';
		}
    }
	
    /*
    * Prímimplikáns tábla feltöltése
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
    * Összevont elemek megjelölése egy kisbetûvel
    */
    color(10);
    for (int i = 0; i < minterms.decimal.size(); i++) {
        if (detailMode) cout << letters[i] << ", " << minterms.decimal[i] << getDontCareBits(minterms.binary[i]) << endl;
    }

    /*
    * Prímimplikáns táblázat kiíratása
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
    * X segédfüggvény kiíratása a prímimplikáns tábla alapján
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
    * X segédfüggvény egyszerûsítése
    */
    helpFunction = simplifyHelpFuncton();

    /*
    * Egyszerûsített x segédfüggvény kiíratása
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
    * Legegyszerûbb alak kiíratása
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
    * Legegyszerûbb alak adatainak betöltése a tárolóba
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
* Az X segédfüggvény egyszerûsítése a A(A+B) = A alapján.
* 1. Lépés: Sorbarendezés hossz alapján
* 2. Lépés: Keresés, hogy van e még ugyan olyan változó benne, ha van, akkor azt a változót kiüti
* 3. Lépés: Megnézi melyik változók nem lettek kiütve, és azokat beteszi egy új vektorba
* 4. Lépés: Visszatérés az egyszerûsített vektorral
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
* Mintermek kiíratása a képernyõre
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
* Bináris mintermek átalakítása változókká
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
* A függvény átalakítja az arab számokat római számokká
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
* A függvény kiszedi a lista elemeit egy vektorba
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
* Vektor sorbarendezése súlyszám alapján
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
* Súlyszám kiszámítása
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
* Konzol szín beállítása
*/
void color(int id) {
    SetConsoleTextAttribute(hConsole, id);
}

/*
* Összehaszonlítás szóhossz alapján
*/
bool compare(string str1, string str2) {
    if (str1.length() < str2.length())
        return true;
    return false;
}


