#include <bits/stdc++.h>
using namespace std;
const int MAXN = 100010;
typedef pair<int, int> mint;
typedef long long ll;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

int generateInteger(int fr, int to)
{
    uniform_int_distribution<int> dis(fr, to);
    return dis(rng);
}

float generateDouble(int fr, int to)
{
    uniform_real_distribution<float> dis(fr, to);
    return dis(rng);
}

int numberOfAnimalsToGenerate;
map<mint, int> firstPositionOfHash, lastPositionOfHash;
vector<string> names;
string types[4] = {"dog", "cat", "turtle", "canary"};

int minWeightByType[4] = {100, 100, 100, 100};
int maxWeightByType[4] = {200, 200, 200, 200};

int minHeightByType[4] = {100, 100, 100, 100};
int maxHeightByType[4] = {200, 200, 200, 200};

int minRaceByType[4] = {1, 1, 1, 1};
int maxRaceByType[4] = {200, 200, 200, 200};

int minAgeByType[4] = {1, 1, 1};
int maxAgeByType[4] = {20, 20, 20, 20};

char genre[2] = {'M', 'F'};

const int MODS[] = {1001864327, 1001265673};
const mint BASE(256, 256), ZERO(0, 0), ONE(1, 1);
inline int add(int a, int b, const int &mod) { return a + b >= mod ? a + b - mod : a + b; }
inline int mul(int a, int b, const int &mod) { return 1ll * a * b % mod; }

inline mint operator+(const mint a, const mint b)
{
    return {add(a.first, b.first, MODS[0]), add(a.second, b.second, MODS[1])};
}
inline mint operator*(const mint a, const mint b)
{
    return {mul(a.first, b.first, MODS[0]), mul(a.second, b.second, MODS[1])};
}

mint hashString(string s)
{
    mint ans = ZERO;
    for (int i = 1; i < s.length() + 1; i++)
    {
        if (s[i - 1] >= 'A' && s[i - 1] <= 'Z')
            s[i - 1] = tolower(s[i - 1]);
        ans = ans * BASE + mint{s[i - 1], s[i - 1]};
    }
    return ans;
}

struct Animal
{
    int id;
    int age;
    int height;
    float weight;
    char sex;
    int nextWithSameHash;
    int previousWithSameHash;
    char name[32];
    char type[32];
    char race[16];
};

struct HashRepresentation
{
    int firstKey;
    int secondKey;
    int firstPosition;
    int lastPosition;
};

Animal animalsArray[10000010];
HashRepresentation hashesArray[100010];

int saveAnimals(void *arr)
{
    FILE *apFile;
    int r;
    string auxi = "";
    apFile = fopen("binaries/dataDogs.dat", "w+");
    if (apFile == NULL)
    {
        perror("error fopen:");
        exit(-1);
    }
    r = fwrite(arr, sizeof(Animal), numberOfAnimalsToGenerate, apFile);
    if (r <= 0)
    {
        perror("error fwrite");
        exit(-1);
    }
    r = fclose(apFile);
    if (r < 0)
    {
        perror("error fclose: ");
        exit(-1);
    }
    return 1;
}

int numberOfHashes = 0;
int saveHashes(void *arr)
{
    FILE *apFile;
    int r;
    apFile = fopen("binaries/hash.bin", "w+");
    if (apFile == NULL)
    {
        perror("error fopen:");
        exit(-1);
    }
    r = fwrite(arr, sizeof(HashRepresentation), numberOfHashes, apFile);
    if (r <= 0)
    {
        perror("error fwrite");
        exit(-1);
    }
    r = fclose(apFile);
    if (r < 0)
    {
        perror("error fclose: ");
        exit(-1);
    }
    return 1;
}

void generateAnimals()
{
    int id = 1;
    for (int i = 0; i < numberOfAnimalsToGenerate; i++)
    {
        int indexOfAnimal = generateInteger(0, 3);

        string name = names[generateInteger(0, names.size() - 1)];

        string type = types[indexOfAnimal];

        stringstream ss;
        int raceNumber = generateInteger(minRaceByType[indexOfAnimal], maxRaceByType[indexOfAnimal]);
        ss << raceNumber;
        string race = type + ss.str();

        int age = generateInteger(minAgeByType[indexOfAnimal], maxAgeByType[indexOfAnimal]);

        int height = generateInteger(minHeightByType[indexOfAnimal], maxHeightByType[indexOfAnimal]);

        float weight = generateDouble(minWeightByType[indexOfAnimal], maxWeightByType[indexOfAnimal]);

        char sex = genre[generateInteger(0, 1)];
        int nextWithSameHash = -1;
        int previousWithSameHash = -1;
        Animal newMascot = {id, age, height, weight, sex, nextWithSameHash, previousWithSameHash};
        strcpy(newMascot.name, name.c_str());
        strcpy(newMascot.type, type.c_str());
        strcpy(newMascot.race, race.c_str());

        animalsArray[i] = newMascot;

        mint nameHash = hashString(name);

        if (firstPositionOfHash.find(nameHash) == firstPositionOfHash.end())
        {
            firstPositionOfHash[nameHash] = i;
        }
        if (lastPositionOfHash.find(nameHash) == lastPositionOfHash.end())
        {
            lastPositionOfHash[nameHash] = i;
        }
        else
        {
            int indexOfLastOne = lastPositionOfHash[nameHash], indexOfNewLastOne = i;
            animalsArray[indexOfLastOne].nextWithSameHash = indexOfNewLastOne;
            animalsArray[indexOfNewLastOne].previousWithSameHash = indexOfLastOne;
            lastPositionOfHash[nameHash] = indexOfNewLastOne;
        }
        id++;
    }
}

void getNumberOfAnimalsToRead()
{
    cin >> numberOfAnimalsToGenerate;
    cin.ignore();
}

void readNames()
{
    string nameAux;
    while (getline(cin, nameAux))
    {
        names.push_back(nameAux);
    }
}

void generateHashesArray()
{
    map<mint, int>::iterator it = firstPositionOfHash.begin();
    for (; it != firstPositionOfHash.end(); it++)
    {
        int firstKey = it->first.first, secondKey = it->first.second, firstPosition = it->second;
        int lastPosition = lastPositionOfHash[{firstKey, secondKey}];
        hashesArray[numberOfHashes++] = {firstKey, secondKey, firstPosition, lastPosition};
    }
}

void saveNumberOfAnimals()
{
    FILE *apFile;
    int r;
    string auxi = "";
    apFile = fopen("binaries/numberOfAnimals.dat", "w+");
    if (apFile == NULL)
    {
        perror("error fopen:");
        exit(-1);
    }
    r = fwrite(&numberOfAnimalsToGenerate, sizeof(Animal), 1, apFile);
    if (r <= 0)
    {
        perror("error fwrite");
        exit(-1);
    }
    r = fclose(apFile);
    if (r < 0)
    {
        perror("error fclose: ");
        exit(-1);
    }
}

int main()
{
    freopen("in.txt", "r", stdin);
    //freopen("out.txt", "w", stdout);
    system("rm -r reports && mkdir reports");
    getNumberOfAnimalsToRead();
    readNames();
    generateAnimals();
    generateHashesArray();
    saveAnimals(animalsArray);
    saveHashes(hashesArray);
    saveNumberOfAnimals();
}