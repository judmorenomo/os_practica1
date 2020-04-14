#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits>
#include <fstream>
#include <iostream>
#include <map>
#include <climits>
#include <algorithm>
#include <termios.h>
#include <stdio_ext.h>
using namespace std;

//Tipo de dato para el key de los registros
typedef pair<int, int> mint;

//Declaración de las dos tablas hash a ser usadas
map<mint, int> firstPositionOfHash, lastPositionOfHash;

//Función para limpiar la consola
void clearConsole()
{
    system("clear");
}

//Funcion que imita em metodo getch() de windows
//Utilizada para realizar una pausa hasta que el usuario presione una tecla
int mygetch()
{
    struct termios oldt,
        newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

//Estructura de un registro
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

//Función para imprimir un registro
void printAnimal(struct Animal *data)
{
    printf("|%10d|%32s|%32s|%10d|%16s|%10d|%12.1f|%4c|\n", data->id, data->name, data->type, data->age, data->race, data->height, data->weight, data->sex);
}

//Declaraciones y operaciones para generación del key compuesto final
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

//Función que toma el nombre de un registro y devuelve el key final
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

//Estructura que representa la forma en la que las tablas hash fueron almacenadas en el archivo hash.bin y que permite su recuperación
struct HashRepresentation
{
    int firstKey;
    int secondKey;
    int firstPosition;
    int lastPosition;
};

int totalNumberOfExistingAnimals;
//funcion para retornar el numero de pacientes en el archivo binario
void getNumberOfAnimals()
{
    ifstream archivo("binaries/numberOfAnimals.dat", ios::in | ios::binary | ios::ate);
    archivo.seekg(0, ios::beg);
    archivo.read((char *)&totalNumberOfExistingAnimals, sizeof(int)); //Hallamos el número de registros en el archivo
    archivo.close();
}

//Mostrar cuantos animales validos hay en el sistema
void printNumberOfExistingAnimals()
{
    cout << "Actualmente existen " << totalNumberOfExistingAnimals << " registros " << endl;
}

//Guardar en el fichero la cantidad de animales que hay
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
    r = fwrite(&totalNumberOfExistingAnimals, sizeof(int), 1, apFile);
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

//Reducir el número de animales
void reduceNumberOfAnimals()
{
    totalNumberOfExistingAnimals--;
}

//Incrementar el número de animales
void increaseNumberOfAnimals()
{
    totalNumberOfExistingAnimals++;
}

//Función que, dado un index (posición en el archivo), carga la estructura correspondiente en memoria y devuelve un puntero a la misma
Animal *recoverAnimalInIndex(int itemNum)
{
    struct Animal *m;
    m = (Animal *)malloc(sizeof(Animal));
    ifstream archivo("binaries/dataDogs.dat", ios::in | ios::binary | ios::ate);
    int numberOfAnimals = archivo.tellg() / sizeof(Animal); //Hallamos el número de registros en el archivo
    if (itemNum >= numberOfAnimals)
    {
        m->id = -1;
        return m;
    }
    archivo.seekg(sizeof(Animal) * itemNum, ios::beg);
    archivo.read((char *)m, sizeof(Animal));
    return m;
}

//Función para recuperar los hashes del archivo hash.bin y cargarlos en memoria
void recoverHashes()
{
    HashRepresentation m;
    ifstream archivo("binaries/hash.bin", ios::in | ios::binary | ios::ate);
    int numberOfHashes = archivo.tellg() / sizeof(HashRepresentation);
    for (int i = 0; i < numberOfHashes; i++)
    {
        archivo.seekg(sizeof(HashRepresentation) * i, ios::beg);
        archivo.read((char *)&m, sizeof(HashRepresentation));

        firstPositionOfHash[{m.firstKey, m.secondKey}] = m.firstPosition;
        lastPositionOfHash[{m.firstKey, m.secondKey}] = m.lastPosition;
    }
}

//Función para verificar un input correcto de un número entero por parte del usuario
int getAndVerifyInt()
{
    int num;
    cin >> num;
    clearConsole();

    //El valor debe ser un int válido mayor a cero
    while (!cin.good() || num <= 0)
    {
        cout << "Por favor digite un número entero mayor a cero: ";
        cin.clear();
        cin.ignore(INT_MAX, '\n');
        cin >> num;

        clearConsole();
    }
    return num;
}

//Función para verificar un input correcto de un número decimal por parte del usuario
float getAndVerifyFloat()
{
    float num;
    cin >> num;
    clearConsole();

    //El valor debe ser un float válido mayor a cero
    while (!cin.good() || num <= 0.0)
    {
        cout << "Por favor, digite un número decimal mayor a cero: ";
        cin.clear();
        cin.ignore(INT_MAX, '\n');
        cin >> num;

        clearConsole();
    }
    return num;
}

//Función para verificar un input correcto de un string por parte del usuario. Recibe un int que define la longitud máxima del string.
string getAndVerifyString(int strLenght)
{
    string str = "";
    getline(cin, str);
    cin.clear();
    clearConsole();

    //El valor debe ser menor a la longitud dada
    while (!cin.good() || str.length() > strLenght || str.length() == 0)
    {
        cout << "Por favor, digite un nombre de no más de " << strLenght << " carácteres: ";
        cin.clear();
        getline(cin, str);

        clearConsole();
    }
    return str;
}

//Función que decide si un input para el sexo es válido
bool animalSexInputIsValid(string input)
{
    return input == "f" || input == "F" || input == "m" || input == "M";
}

//Función que solicita y verifica input del sexo por parte del usuario
char getAndVerifyAnimalSexInput()
{
    string input = " ";

    while (!animalSexInputIsValid(input))
    {
        cout << "Por favor ingrese 'F' o 'M' únicamente: " << endl;
        cin >> input;
        clearConsole();
    }
    return input[0];
}

//Función que solicita los atributos de un animal y lo retorna
Animal readNewAnimal()
{
    struct Animal newAnimal;

    //Se recibe el input del usuario para inicializar los valores del registro
    cout << "Digite el nombre de la mascota: " << endl;
    cin.ignore(INT_MAX, '\n');
    strcpy(newAnimal.name, getAndVerifyString(32).c_str());
    string newAnimalNameLowercase = newAnimal.name;

    cout << "Digite la especie: " << endl;
    strcpy(newAnimal.type, getAndVerifyString(32).c_str());

    cout << "Digite la raza: " << endl;
    strcpy(newAnimal.race, getAndVerifyString(16).c_str());

    cout << "Digite el sexo: " << endl;
    newAnimal.sex = getAndVerifyAnimalSexInput();

    cout << "Digite la edad: " << endl;
    newAnimal.age = getAndVerifyInt();

    cout << "Digite la altura: " << endl;
    newAnimal.height = getAndVerifyInt();

    cout << "Digite el peso: " << endl;
    newAnimal.weight = getAndVerifyFloat();

    return newAnimal;
    clearConsole();
}

//Función para la opción 1 de agregar un registro
void addAnimal()
{
    Animal newAnimal = readNewAnimal();
    mint newAnimalKey = hashString(newAnimal.name);

    fstream archivo("binaries/dataDogs.dat", ios::in | ios::out | ios::binary);

    //Determinar id de registro a agregar
    archivo.seekg(0, archivo.end);
    int numberOfAnimals = archivo.tellg() / sizeof(Animal);

    newAnimal.id = numberOfAnimals + 1;

    //No tiene registro siguiente, por lo que inicializamos el respectivo puntero con -1
    newAnimal.nextWithSameHash = -1;

    //Determinar puntero a registro anterior del registro nuevo
    if (lastPositionOfHash.find(newAnimalKey) != lastPositionOfHash.end())
    { //Ya existe una estructura con el mismo hash
        //Determinar puntero a registro anterior del registro nuevo para que apunte al último registro previo
        newAnimal.previousWithSameHash = lastPositionOfHash[newAnimalKey];

        //Actualizar puntero a registro siguiente del registro previo
        struct Animal prevAnimal;
        prevAnimal = *recoverAnimalInIndex(lastPositionOfHash[newAnimalKey]);
        prevAnimal.nextWithSameHash = (newAnimal.id) - 1;

        //Actualizar registro previo en el archivo
        archivo.seekp(sizeof(Animal) * lastPositionOfHash[newAnimalKey], ios::beg);
        archivo.write((char *)&prevAnimal, sizeof(Animal));

        //Actualizar hash de último registro con mismo hash para apuntar al nuevo registro
        lastPositionOfHash[newAnimalKey] = newAnimal.id - 1;
    }
    else //No existía una estructura con el mismo hash
    {
        //No tiene registro anterior, por lo que inicializamos el respectivo puntero con -1
        newAnimal.previousWithSameHash = -1;

        //El nuevo registro es el ambos el primero y último registro con ese hash
        firstPositionOfHash[newAnimalKey] = newAnimal.id - 1;
        lastPositionOfHash[newAnimalKey] = newAnimal.id - 1;
    }

    //Agregar estructura al final del archivo
    archivo.seekp(0, archivo.end);
    archivo.write((char *)&newAnimal, sizeof(Animal));

    archivo.close();

    cout << "Se guardó el registro del nuevo animal con exito" << endl;

    //como el guardado es exitoso se aumenta el numero de registros
    increaseNumberOfAnimals();

    //Se establece una pausa hasta que el usuario presione alguna tecla
    cout << "\nPresione alguna tecla para regresar al menú principal" << endl;
    __fpurge(stdin);
    mygetch();
}

//Función que imprime los nombres de las columnas al mostrar registros
void printShowAnimalsHeader()
{
    printf("|%4s%s%4s|%13s%s%13s|%14s%s%14s|%3s%s%3s|%6s%s%6s|%2s%s%2s|%4s%s%4s|%s|\n",
           " ", "ID", " ",
           " ", "Nombre", " ",
           " ", "Tipo", " ",
           " ", "Edad", " ",
           " ", "Raza", " ",
           " ", "Altura", " ",
           " ", "Peso", " ",
           "Sexo");
}

//Verificar si el input de yes/no del usuario es correcto
bool userYesNoInputIsValid(string input)
{
    return input == "n" || input == "N" || input == "s" || input == "S";
}

//Función para la opción 2 de buscar un registro por ID
void searchAnimalByID()
{
    //se muestra el numero de registros actuales
    printNumberOfExistingAnimals();

    //Se pide y se registra el ID del paciente a buscar
    int IDofAnimalToSearch;
    cout << "Digite el ID del paciente a buscar: ";
    IDofAnimalToSearch = getAndVerifyInt();

    struct Animal *recordOfAnimal;
    //Se busca al animal y se asigna a una estructura del mismo tipo
    recordOfAnimal = recoverAnimalInIndex(IDofAnimalToSearch - 1);

    //Si la ID de la estructura es negativa significa que no está dicho animal en el registro
    if (recordOfAnimal->id == -1)
    {
        cout << "No existe animal registrado con ese ID" << endl;

        free(recordOfAnimal);

        //Se establece una pausa hasta que el usuario presione alguna tecla
        cout << "\nPresione alguna tecla para regresar al menú principal" << endl;
        __fpurge(stdin);
        mygetch();

        return;
    }

    //El usuario puede decidir si regresar al menú o abrir el archivo
    //Si ingresa un valor distinto se le pedira que ingrese un valor correcto y se mantendra el menú
    string userSelection = "";
    while (!userYesNoInputIsValid(userSelection))
    {
        printShowAnimalsHeader();
        printAnimal(recordOfAnimal);
        cout << "\n\n¿Desea abrir el registro médico del paciente? S/N" << endl;

        cout << "Por favor ingrese sólo S o N\n";
        cin >> userSelection;
        clearConsole();
    }

    if (userSelection == "s" || userSelection == "S")
    {
        //Se establece un string para pasar por consola que cree el archivo
        //Si ya existe un archivo con el mismo nombre no ocurrira nada
        string filename = "reports/" + to_string(recordOfAnimal->id) + "file.txt";
        string sysout = "touch " + filename;

        //Se establece una variable del tipo indicado para pasar a la funcion system
        const char *systemChar = sysout.c_str();

        //Se ejecuta el comando
        system(systemChar);

        //Se sigue el procedimiento anterior esta vez para crear el archivo
        sysout = "nano " + filename;

        systemChar = sysout.c_str();

        system(systemChar);

        clearConsole();

        //Se establece una pausa hasta que el usuario presione alguna tecla
        cout << "\nPresione alguna tecla para regresar al menú principal" << endl;
        __fpurge(stdin);
        mygetch();
    }

    //Se libera el malloc declarado en la funcion recoverAnimalInIndex()
    free(recordOfAnimal);
}

//Función para la opción 3 de buscar un registro por nombre
void searchAnimalByName()
{
    //Se pide y almacena el nombre a buscar
    string nameOfAnimal = "";

    cout << "Digite el nombre a buscar: ";

    cin.ignore(INT_MAX, '\n');
    nameOfAnimal = getAndVerifyString(32);

    //Si se llega al final del mapa significa que no hay tal nombre registrado en el archivo
    if (firstPositionOfHash.find(hashString(nameOfAnimal)) == firstPositionOfHash.end())
    {

        clearConsole();

        cout << "El nombre no se encuentra registrado" << endl;

        //Se establece una pausa hasta que el usuario presione alguna tecla
        cout << "\nPresione alguna tecla para regresar al menú principal" << endl;
        __fpurge(stdin);
        mygetch();

        return;
    }

    clearConsole();
    //Se realiza el hash del nombre y con la key se busca en la tabla cargada de las primeras posiciones
    //en el archivo de un determinado hash
    int recordPosition = firstPositionOfHash[hashString(nameOfAnimal)];

    printShowAnimalsHeader();

    //Se crea un apuntador de tipo struct Animal para guardar cada estructura del archivo cuyo nombre es
    //el que se busca, de tal forma que solo se cargue en memoria esta estructura durante el proceso
    struct Animal *record;

    //Se ejecuta un while que corre mientras aun halla mas estructuras cuyos nombres tienen el mismo hash
    while (true)
    {
        //Se toma la estructura actual en la que estamos
        record = recoverAnimalInIndex(recordPosition);

        printAnimal(record);

        //Se cambia la posicion a la siguiente con el mismo hash
        recordPosition = record->nextWithSameHash;

        //Si la posicion es -1 es que no hay ningun otro con el mismo hash por lo que se corta el ciclo
        if (recordPosition == -1)
        {
            break;
        }
    }

    //Se libera el malloc declarado en la funcion recoverAnimalInIndex()
    free(record);

    //Se establece una pausa hasta que el usuario presione alguna tecla
    cout << "\nPresione alguna tecla para regresar al menú principal" << endl;
    __fpurge(stdin);
    mygetch();
}

//Función para la opción 4 de borrar un registro
void deleteAnimalWithID()
{
    //se muestra el numero de registros actuales
    printNumberOfExistingAnimals();

    //Se pide y registra la ID del paciente a borrar
    int animalIDtoDelete;
    cout << "Digite el ID del paciente a borrar: ";
    animalIDtoDelete = getAndVerifyInt();

    //Se resta una unidad al ID para poder usarlo como index en el buscador
    //Se establece un apuntador del tipo struct Animal que contenga la estructura que se busca borrar
    struct Animal *recordToDelete;
    recordToDelete = recoverAnimalInIndex(animalIDtoDelete - 1);

    //Si la ID de la estructura recibida es -1 significa que el paciente no existe
    if (recordToDelete->id == -1)
    {

        cout << "No existe animal registrado con ese ID" << endl;

        //Se libera el malloc declarado en la funcion recoverAnimalInIndex()
        free(recordToDelete);

        //Se establece una pausa hasta que el usuario presione alguna tecla
        cout << "\nPresione alguna tecla para regresar al menú principal" << endl;
        __fpurge(stdin);
        mygetch();

        return;
    };

    //Se guarda el key del hash para el nombre de la estructura a borrar
    mint AnimalKey = hashString(recordToDelete->name);

    //Se inicializa un archivo de inscripcion y lectura binario
    fstream archivo("binaries/dataDogs.dat", ios::in | ios::out | ios::binary);

    //Se guardan el index de las estructuras siguiente y anterior con el mismo hash
    int recordToDeleteNexthash = recordToDelete->nextWithSameHash;
    int recordToDeletePrevhash = recordToDelete->previousWithSameHash;

    //Si la estructura a borrar tiene siguiente con mismo hash
    if (recordToDeleteNexthash != -1)
    {
        //Se establece un apuntador del tipo struct Animal que contenga la estructura siguiente
        struct Animal *nextRecord = recoverAnimalInIndex(recordToDeleteNexthash);
        //Se establece el index de la estructura anterior para esta estructura como el index anterior de
        //la estructura a borrar
        nextRecord->previousWithSameHash = recordToDeletePrevhash;

        //Se reescribe la estructura en el archivo
        archivo.seekp(sizeof(Animal) * recordToDeleteNexthash, ios::beg);
        archivo.write((char *)nextRecord, sizeof(Animal));

        //Se libera el malloc declarado en la funcion recoverAnimalInIndex()
        free(nextRecord);

        //Si la estructura a borrar es la primera de ese hash, entonces ahora la primera es la siguiente
        if (firstPositionOfHash[AnimalKey] == (animalIDtoDelete - 1))
        {
            firstPositionOfHash[AnimalKey] = recordToDeleteNexthash;
        }
    }
    //Si no tiene una estructura siguiente
    else
    {
        //Y es la primera estructura de su hash, se elimina el registro
        if (firstPositionOfHash[AnimalKey] == (animalIDtoDelete - 1))
        {
            firstPositionOfHash.erase(AnimalKey);
        }
    }

    //Si la estructura a borrar tiene anterior con mismo hash
    if (recordToDeletePrevhash != -1)
    {
        //Se establece un apuntador del tipo struct Animal que contenga la estructura anterior
        struct Animal *recordPrev = recoverAnimalInIndex(recordToDeletePrevhash);
        //Se establece el index de la estructura siguiente para esta estructura como el index siguiente de
        //la estructura a borrar
        recordPrev->nextWithSameHash = recordToDeleteNexthash;

        //Se reescribe la estructura en el archivo
        archivo.seekp(sizeof(Animal) * recordToDeletePrevhash, ios::beg);
        archivo.write((char *)recordPrev, sizeof(Animal));

        //Se libera el malloc declarado en la funcion recoverAnimalInIndex()
        free(recordPrev);

        //Si la estructura a borrar es la ultima de ese hash, entonces ahora la ultima es la anterior
        if (lastPositionOfHash[AnimalKey] == (animalIDtoDelete - 1))
        {
            lastPositionOfHash[AnimalKey] = recordToDeletePrevhash;
        }
    }
    //Si no tiene una estructura anterior
    else
    {
        //Y es la ultima estructura de su hash, se elimina el registro
        if (lastPositionOfHash[AnimalKey] == (animalIDtoDelete - 1))
        {
            lastPositionOfHash.erase(AnimalKey);
        }
    }

    //Se establece la id del registro a borrar a -1 al igual que su next y prev hash
    recordToDelete->id = -1;
    recordToDelete->nextWithSameHash = -1;
    recordToDelete->previousWithSameHash = -1;

    //Se modifica la estructura en el archivo, con ID = -1 no sera reconocida por las operaciones
    archivo.seekp(sizeof(Animal) * (animalIDtoDelete - 1), ios::beg);
    archivo.write((char *)recordToDelete, sizeof(Animal));

    //Se cierra el archivo
    archivo.close();
    //Se libera el malloc declarado en la funcion recoverAnimalInIndex()
    free(recordToDelete);

    //Se establece el string del comando a usar para borrar cualquier txt asociado al registro
    string sysstrg = "rm ./reports/" + to_string(animalIDtoDelete) + "file.txt";

    //Se declara el tipo de variable para pasar a la funcion system()
    const char *systemChar = sysstrg.c_str();

    //Se ejecuta el comando
    system(systemChar);

    clearConsole();

    cout << "Se borró con éxito el elemento" << endl;

    //Se reduce el numero guardado con el numero de registros totales
    reduceNumberOfAnimals();

    //se muestra el numero de registros actuales
    printNumberOfExistingAnimals();

    //Se establece una pausa hasta que el usuario presione alguna tecla
    cout << "\nPresione alguna tecla para regresar al menú principal" << endl;
    __fpurge(stdin);
    mygetch();
}

//Variables para imprimir las opciones existentes
const int numberOfOptions = 5;
string menuOptions[5] = {"Ingresar un paciente al registro",
                         "Buscar un paciente por ID",
                         "Mostrar pacientes por nombre",
                         "Borrar un paciente con ID especifica",
                         "Salir del programa"};

//Función para imprimir el menú principal
void printMenu()
{
    string mainHeader = string(15, '-') + "Bienvenido" + string(15, '-');
    string instruction = "Por favor ingrese el número correspondiente a la opción que desea ejecutar \n";
    printf("%35s\n%s\n", mainHeader.c_str(), instruction.c_str());
    for (int i = 0; i < numberOfOptions; i++)
    {
        printf("(%d) %s\n\n", i + 1, menuOptions[i].c_str());
    }
    cout << "Por favor ingrese solo una de las opciones" << endl;
    printf("Opción: ");
}

//Validar que la opción que ingresa el usuario exista
bool userMenuSelectionIsValid(string selection)
{
    for (int i = 0; i < numberOfOptions; i++)
    {
        if (selection == to_string(i + 1))
            return true;
    }
    return false;
}

//Función para el menú principal
string getUserMenuSelection()
{
    //Variable de seleccion para el menú
    string menuSelection = "";

    while (!userMenuSelectionIsValid(menuSelection))
    {
        clearConsole();
        printMenu();
        cin >> menuSelection;
    }

    return menuSelection;
}

//Variable que indica si el programa debe continuar
bool working = true;

//Ejecutar la operación ingresada por el usuario
void doOperation(int userSelection)
{
    clearConsole();
    switch (userSelection)
    {
    case 1:
    {
        addAnimal();
        break;
    }
    case 2:
    {
        searchAnimalByID();
        break;
    }

    case 3:
    {
        searchAnimalByName();
        break;
    }
    case 4:
    {
        deleteAnimalWithID();
        break;
    }
    case 5:
    {
        // Si el usuario desea cerrar el programa se establece la variable de trabajo a falso y se guarda la cantidad de animales
        saveNumberOfAnimals();
        working = false;
    }
    default:
        return;
    }
}

int main()
{
    //Se lee el archivo con las tablas hash para cargarlas en memoria
    recoverHashes();
    getNumberOfAnimals();

    //Se ejecuta el programa principal
    //Mientras esta variable sea true, el programa está trabajando
    while (working)
    {
        int integerMenuSelection = stoi(getUserMenuSelection());
        doOperation(integerMenuSelection);
    }
}