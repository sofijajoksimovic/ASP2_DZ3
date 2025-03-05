#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <random>
using namespace std;
class AddressFunction {
public:
    virtual int getAddress(long long int key, int address, int attempt) = 0;
};
class LinearHashing : public AddressFunction {
    int s;
public:
    LinearHashing(int s):s(s){}
     int getAddress(long long int key, int address, int attempt) override {
        return address + attempt * s;
    }
};

class hashTable{
    struct Ulaz{
        long long int key;
        string data;
        bool zauzeto;
    };
    int size;
    int brZauzetih=0;

    AddressFunction*funkcija;
    Ulaz* tabela;

    int brPokusajaPriUspesnom=0;
    long long int brPokusajaPriNeuspesnom=0;
    int brKljucevaNenadjenih=0, brNadjenih=0;
    double maxPopunjenost, maxNeusp, maxUsp;
    long long int maxKey=0;
    long long int minKey=LONG_LONG_MAX;

    int findKeyIndex(long long int key, int& pokusaj){
        long long int addr = key % size;
        while (true) {
            int ind = (funkcija->getAddress(key, addr, pokusaj))%size;
            if (tabela[ind].zauzeto and tabela[ind].key == key) {
                pokusaj++;
                return ind;
            }
            else if (!tabela[ind].zauzeto or (tabela[ind].zauzeto and tabela[ind].key==-2)) {
                pokusaj++;
                return -1;
            }
            pokusaj++;
        }
    }
    void resizeTable(){
        int newSize = size * 2;  // Double the size
        resetStatistics();
        for(int i=0;i<size;i++){
            if(tabela[i].zauzeto){
                findKey(tabela[i].key);
            }
        }
        if (avgAccessSuccess() > maxUsp || avgAccessUnsuccess() > maxNeusp || fillRatio() > maxPopunjenost) {
            Ulaz *newTable = new Ulaz[newSize];
            for (int i = 0; i < size; ++i) {
                if (tabela[i].zauzeto) {
                    int address = tabela[i].key % newSize;
                    int attempt = 0;
                    while(true) {
                        int index = (funkcija->getAddress(tabela[i].key, address, attempt)) % newSize;
                        if (!newTable[index].zauzeto or (newTable[index].zauzeto and tabela[index].key == -2)) {
                            newTable[index].key = tabela[i].key;
                            newTable[index].data = tabela[i].data;
                            newTable[index].zauzeto = true;
                            break;
                        }
                        attempt++;
                    }
                }
            }
            delete[] tabela;
            tabela = newTable;
            size = newSize;
        }
    }
public:
    long long int getMaxKey()const{
        return maxKey;
    }
    long long int getMinKey()const{
        return minKey;
    }
    hashTable(int size, AddressFunction*funkcija, double maxPopunjenost, double maxNeusp, double maxUsp):size(size), funkcija(funkcija), maxPopunjenost(maxPopunjenost), maxNeusp(maxNeusp),maxUsp(maxUsp){
        tabela = new Ulaz[size];
        for (int i = 0; i < size; ++i) {
            tabela[i].zauzeto = false;
        }
    }
    ~hashTable() {
        delete[] tabela;
    }
    string findKey(long long int key){
        int pokusaj=0;
        int index = findKeyIndex(key, pokusaj);
        if (index != -1) {
            brPokusajaPriUspesnom+=pokusaj;
            brNadjenih++;
            return tabela[index].data;
        }
        else{
            brPokusajaPriNeuspesnom+=pokusaj;
            brKljucevaNenadjenih++;
            return "0";
        }

    }
    bool insertKey(long long int key, string data){
        int address = key % size;
        int attempt = 0;
        while (true) {
            int index = (funkcija->getAddress(key, address, attempt))%size;
            if (!tabela[index].zauzeto or (tabela[index].zauzeto and tabela[index].key==-2)) {
                tabela[index].key = key;
                tabela[index].data = data;
                tabela[index].zauzeto = true;
                brZauzetih++;
                if(key<minKey){
                    minKey=key;
                }
                if(key>maxKey){
                    maxKey=key;
                }
                int i=size;
                resizeTable();
                if(i!=size)cout<<"Tabela je prosirena!"<<endl;
                return true;
            }
            else if (tabela[index].key == key) {
                return false;
            }
            attempt++;
        }
    }
    bool deleteKey(int key){
        int pokusaj=0;
        int ind = findKeyIndex(key, pokusaj);
        if (ind != -1) {
            tabela[ind].key=-2;
            tabela[ind].data="0";
            brZauzetih--;
            return true;
        }
        return false;
    }
    double avgAccessSuccess()const{
        if(brZauzetih>0)return (double)brPokusajaPriUspesnom/brNadjenih;
        else return 0;
    }
    double avgAccessUnsuccess()const{
        if(brKljucevaNenadjenih>0)return (long double)brPokusajaPriNeuspesnom/brKljucevaNenadjenih;
        else return 0;
    }
    void resetStatistics(){
        brPokusajaPriUspesnom=0;
        brPokusajaPriNeuspesnom=0;
        brKljucevaNenadjenih=0;
    }
    void clear(){
        for (int i = 0; i < size; ++i) {
            tabela[i].zauzeto = false;
        }
        resetStatistics();
        brZauzetih = 0;
    }
    int keyCount() const {
        return brZauzetih;
    }
    int tableSize()const{
        return size;
    }
    double fillRatio() const{
        return (double)brZauzetih /size;
    }
    friend ostream& operator<<(ostream& os, const hashTable& hes){
        for (int i = 0; i < hes.size; ++i) {
            if (!hes.tabela[i].zauzeto or(hes.tabela[i].zauzeto and hes.tabela[i].key==-2)) {
                os << "EMPTY" << endl;
            } else {
                os << "Key: " << hes.tabela[i].key << ", Data: " << hes.tabela[i].data << endl;
            }
        }
        return os;
    }
    long long int getNeusp()const{
        return brPokusajaPriNeuspesnom;
    }
};

void statistika(hashTable&h, long long int br, long long int keys[], string data[],int brUnetih){
    for( int k=0;k<brUnetih;k++){
        h.insertKey(keys[k],data[k]);
    }
    cout<<h;
    mt19937 generator(std::time(0));
    long long int searchKeys[br];
    uniform_int_distribution<long long> distribution(h.getMinKey(), h.getMaxKey());
    for(long long int k=0;k<br;k++){
        searchKeys[k] = distribution(generator);
        h.findKey(searchKeys[k]);
    }
    cout<<"Statistika je kreirana!"<<endl;
    cout<<"Prosecan broj pristupa pri uspesnoj pretrazi:"<<h.avgAccessSuccess()<<endl;
    cout<<"Broj pristupa pri neuspesnoj pretrazi:"<<h.getNeusp()<<endl;

}
int main() {
    cout<<"Dobrodosli!\n";
    double maxP, maxN, maxU;
    int size;
    cout << "Unesite velicinu tabele: "<<endl;
    cin >> size;
    int korak;
    cout << "Unesite korak linearnog pretrazivanja:"<<endl;
    cin >> korak;
    cout<<"Unesite granicu popunjenosti:"<<endl;
    cin>>maxP;
    cout<<"Unesite max prosecan broj pristupa prilikom neuspesne pretrage"<<endl;
    cin>>maxN;
    cout<<"Unesite max prosecan broj pristupa prilikom uspesne pretrage"<<endl;
    cin>>maxU;
    AddressFunction *linearHashing = new LinearHashing(korak);
    hashTable hashTable(size, linearHashing,  maxP,  maxN,  maxU);

    while(true) {
        cout << "Izaberite jednu od ponudjenih stavki:"
                "\n1.Umetanje kljuca u tabelu"
                "\n2.Brisanje kljuca iz tabele"
                "\n3.Pretrazivanje na kljuc iz tabele"
                "\n4.Brisanje sadrzaja tabele"
                "\n5.Ispis tabele"
                "\n6.Ispis stepena popunjenosti tabele"
                "\n7.Broj umetnutih kljuceva"
                "\n8.Prosecan broj pristupa prilikom uspesnog trazenja"
                "\n9.Prosecan broj trazenja prilikom neuspesnog trazenja"
                "\n10.Resetovanje statistike"
                "\n11.Statistka linearnog pretrazivanja"
                "\n12.Kraj programa." << endl;
        int a;
        int kljuc;
        string line;
        cin>>a;
        if(a==1) {
            cout << "Unseite kljuc-podatak koji zelite da unesete u tabelu:" << endl;
            cin>>kljuc;
            string data;
            cin>>data;
            hashTable.insertKey(kljuc, data);
        }
        else if (a==2) {
            cout << "Unseite kljuc koji zelite da obrisete iz tabelu:" << endl;
            cin >> kljuc;
            hashTable.deleteKey(kljuc);
        }
        else if(a==3) {
            cout << "Unseite kljuc na koji zelite da pretrazite tabelu:" << endl;
            cin >> kljuc;
            string str = hashTable.findKey(kljuc);
            cout << "Rezultat pretrage: " << str<<endl;
        }
        else if(a==4) {
            hashTable.clear();
            cout << "Sadrzaj tabele je obrisan!" << endl;
        }
        else if(a==5) {
            cout << hashTable;
        }
        else if(a==6) {
            cout << "Stepen popunjenosti: " << hashTable.fillRatio() << endl;
        }
        else if(a==7) {
            cout << "Broj zauzetih lokacija: " << hashTable.keyCount() << endl;
        }
        else if(a==8) {
            cout << "Prosecan broj pristupa prilikom dosadasnjeg uspesnog pretrazivanja: "
                 << hashTable.avgAccessSuccess() << endl;
        }
        else if(a==9) {
            cout << "Prosecan broj pristupa prilikom dosadasnjeg neuspesnog pretrazivanja: "
                 << hashTable.avgAccessUnsuccess() << endl;
        }
        else if(a==10) {
            hashTable.resetStatistics();
            cout << "Statistika je resetovana!" << endl;
        }
        else if(a==11) {
            cout << "Izaberite da li kljuceve zelite da umetnete sa standardnog ulaza(1) ili iz datoteke(2)?" << endl;
            cin >> a;
            int klj;
            long long int br;
            cout << "Unesite broj kljuceva koje zelite da unesete u tabelu" << endl;
            cin >> klj;
            long long int keys[klj];
            string data1[klj];
            if (a == 1) {
                cout << "Unesite parove kljuc-podatak:" << endl;
                for (int k = 0; k < klj; k++) {
                    cin >> keys[k] >> data1[k];
                }
            } else if (a == 2) {
                ifstream inputFile("dz3.txt");
                if (!inputFile.is_open()) {
                    return 1;
                }
                int count = 0;
                while (count < klj && getline(inputFile, line)) {
                    stringstream ss2(line);
                    ss2 >> data1[count] >> keys[count];
                    count++;
                }
                inputFile.close();
            } else {
                cout << "Greska!" << endl;
                return 0;
            }
            hashTable.clear();
            statistika(hashTable, klj*10, keys, data1, klj);
        }
        else if(a==12) {
            cout << "Kraj programa!" << endl;
            return 0;
        }
        else{
            cout<<"Nevalidna opcija!Pokusajte ponovo"<<endl;
        }
    }
}
