#include <bits/stdc++.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits>
#include <fstream>
#include <iostream>
using namespace std;

//Tipo de dato para el key de los registros
typedef pair<int, int> mint;

//Declaración de las dos tablas hash a ser usadas
map<mint, int> firstPositionOfHash, lastPositionOfHash;

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
    printf("\n|%-10i|%-32s|%-32s\t|%-10i|%-16s\t|%-10i|%-12f|%-4c|", data->id, data->name, data->type, data->age, data->race, data->height, data->weight, data->sex);
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

//funcion para retornar el numero de pacientes en el archivo binario
int getNumberOfAnimals()
{
    ifstream archivo("binaries/dataDogs.dat", ios::in | ios::binary | ios::ate);
    int numberOfAnimals = archivo.tellg() / sizeof(Animal); //Hallamos el número de registros en el archivo
    archivo.close();
    return numberOfAnimals;
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
    archivo.close();
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
    system("clear");

    //El valor debe ser un int válido mayor a cero
    while (!cin.good() || num <= 0)
    {
        cout << "Por favor, digite un número entero mayor a cero: ";
        cin.clear();
        cin.ignore(INT_MAX, '\n');
        cin >> num;

        system("clear");
    }
    return num;
}

//Función para verificar un input correcto de un número decimal por parte del usuario
float getAndVerifyFloat()
{
    float num;
    cin >> num;
    system("clear");

    //El valor debe ser un float válido mayor a cero
    while (!cin.good() || num <= 0.0)
    {
        cout << "Por favor, digite un número decimal mayor a cero: ";
        cin.clear();
        cin.ignore(INT_MAX, '\n');
        cin >> num;

        system("clear");
    }
    return num;
}

//Función para verificar un input correcto de un string por parte del usuario. Recibe un int que define la longitud máxima del string.
string getAndVerifyString(int strLenght)
{
    string str = "";
    getline(cin,str);
    cin.clear();
    system("clear");

    //El valor debe ser menor a la longitud dada
    while (!cin.good() || str.length() > strLenght || str.length() == 0)
    {
        cout << "Por favor, digite un nombre de no más de " << strLenght << " carácteres: ";
        cin.clear();
        getline(cin,str);

        system("clear");
    }
    return str;
}

char getAndVerifySex()
{
    char sex;
    cin >> sex;
    system("clear");

    while (!cin.good() || (sex != 'm' && sex != 'M' && sex != 'f' && sex != 'F'))
    {
        cout << "Por favor, ingrese 'F' o 'M' únicamente: " << endl;

        cin.clear();
        cin.ignore(INT_MAX, '\n');
        cin >> sex;

        system("clear");
    }
    return sex;
}

//Función para la opción 1 de agregar un registro
void addAnimal()
{
    struct Animal newAnimal;

    //Se recibe el input del usuario para inicializar los valores del registro
    cout << "Digite el nombre de la mascota: " << endl;
    cin.ignore(INT_MAX, '\n');
    strcpy(newAnimal.name, getAndVerifyString(32).c_str());
    string newAnimalNameLowercase = newAnimal.name;
    transform(newAnimalNameLowercase.begin(), newAnimalNameLowercase.end(), newAnimalNameLowercase.begin(), [](unsigned char c) { return tolower(c); });

    cout << "Digite la especie: " << endl;
    strcpy(newAnimal.type, getAndVerifyString(32).c_str());

    cout << "Digite la raza: " << endl;
    strcpy(newAnimal.race, getAndVerifyString(16).c_str());

    cout << "Digite el sexo: " << endl;
    newAnimal.sex = getAndVerifySex();

    cout << "Digite la edad: " << endl;
    newAnimal.age = getAndVerifyInt();

    cout << "Digite la altura: " << endl;
    newAnimal.height = getAndVerifyInt();

    cout << "Digite el peso: " << endl;
    newAnimal.weight = getAndVerifyFloat();

    system("clear");

    mint newAnimalKey = hashString(newAnimalNameLowercase);

    fstream archivo("binaries/dataDogs.dat", ios::in | ios::out | ios::binary);

    //Determinar id de registro a agregar
    archivo.seekg(0, archivo.end);
    int numberOfAnimals = archivo.tellg() / sizeof(Animal);

    newAnimal.id = numberOfAnimals + 1;

    //No tiene registro siguiente, por lo que inicializamos el respectivo puntero con -1
    newAnimal.nextWithSameHash = -1;

    //Determinar puntero a registro anterior del registro nuevo
    if (lastPositionOfHash[newAnimalKey] != (int)NULL)
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

    cout << "Se guardo el registro del nuevo animal con exito" << endl;

    cout << "\nPresione ENTER para regresar al menú principal" << endl;

    //Se establece una pausa hasta que el usuario presione enter
    cin.ignore();
    cin.get();
}

//Función para la opción 2 de buscar un registro por ID
void searchAnimalByID()
{
    //Se pide y se registra el ID del paciente a buscar
    int IDofAnimalToSearch;
    int numberOfAnimalinRecord = getNumberOfAnimals();
    cout << "El numero de animales archivados es: " << numberOfAnimalinRecord << endl;
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

        cout << "\nPresione ENTER para regresar al menú principal" << endl;

        //Se establece una pausa hasta que el usuario presione enter
        cin.ignore();
        cin.get();

        return;
    }

    //Se establece un while para el menú de decisión del usuario
    while (true)
    {

        cout << "Se encontro el siguiente paciente: " << endl;

        cout << "|    ID    |            Nombre              |                Tipo               |   Edad   |        Raza        |  Altura  |    Peso    |Sexo|";
        printAnimal(recordOfAnimal);

        //El usuario puede decidir si regresar al menú o abrir el archivo
        //Si ingresa un valor distinto se le pedira que ingrese un valor correcto y se mantendra el menú
        cout << "\n\n¿Desea abrir el registro medico del paciente? S/N" << endl;

        string userSelection = "";

        cin >> userSelection;

        if (userSelection == "n" || userSelection == "N")
        {
            break;
        }
        else if (userSelection == "s" || userSelection == "S")
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

            system("clear");

            cout << "\nPresione ENTER para regresar al menú principal" << endl;

            //Se establece una pausa hasta que el usuario presione enter
            cin.ignore();
            cin.get();

            break;
        }

        system("clear");

        cout << "Por favor, Digite solo S o N" << endl;
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

    cin.ignore();
    getline(cin,nameOfAnimal);

    transform(nameOfAnimal.begin(), nameOfAnimal.end(), nameOfAnimal.begin(), [](unsigned char c) { return tolower(c); });

    //Se realiza el hash del nombre y con la key se busca en la tabla cargada de las primeras posiciones
    //en el archivo de un determinado hash
    int recordPosition = firstPositionOfHash[hashString(nameOfAnimal)];

    //Si se Pos es NULL significa que no hay tal nombre registrado en el archivo
    if (recordPosition == (int)NULL)
    {

        system("clear");

        cout << "El nombre no se encuentra registrado" << endl;

        cout << "\nPresione ENTER para regresar al menú principal" << endl;

        //Se establece una pausa hasta que el usuario presione enter
        cin.ignore();

        return;
    }

    system("clear");

    //Se muestra por pantalla el encabezado de la tabla antes de mostrar los
    //elementos de la misma
    cout << "|    ID    |            Nombre              |                Tipo               |   Edad   |        Raza        |  Altura  |    Peso    |Sexo|";

    //Se crea un apuntador de tipo struct Animal para guardar cada estructura del archivo cuyo nombre es
    //el que se busca, de tal forma que solo se cargue en memoria esta estructura durante el proceso
    struct Animal *record;

    string recordName = "";

    //Se establece una variable para contar el numero de registros
    int numberOfRecords = 0;

    //Se ejecuta un while que corre mientras aun halla mas estructuras cuyos nombres tienen el mismo hash
    while (true)
    {
        //Se toma la estructura actual en la que estamos
        record = recoverAnimalInIndex(recordPosition);

        recordName = record->name;

        transform(recordName.begin(), recordName.end(), recordName.begin(), [](unsigned char c) { return tolower(c); });

        //Se verifica que tenga el nombre que se busca, esto porque pueden haber nombres con el mismo hash
        if (recordName == nameOfAnimal)
        {
            //Se muestra la estructura por pantalla
            printAnimal(record);
            numberOfRecords++;
        }

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

    cout << "\nNumero de registros total: " << numberOfRecords << endl;

    cout << "\nPresione ENTER para regresar al menú principal" << endl;

    //Se establece una pausa hasta que el usuario presione enter
    cin.ignore();
}

//Función para la opción 4 de borrar un registro
void deleteAnimalWithID()
{
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

        cout << "\nPresione ENTER para regresar al menú principal" << endl;

        //Se establece una pausa hasta que el usuario presione enter
        cin.ignore();
        cin.get();

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
        //Y es la primera estructura de su hash, se inicializa la tabla a NULL
        if (firstPositionOfHash[AnimalKey] == (animalIDtoDelete - 1))
        {
            firstPositionOfHash[AnimalKey] = (int)NULL;
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
        //Y es la ultima estructura de su hash, se inicializa la tabla a NULL
        if (lastPositionOfHash[AnimalKey] == (animalIDtoDelete - 1))
        {
            lastPositionOfHash[AnimalKey] = (int)NULL;
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

    system("clear");

    cout << "Se borró con éxito el elemento" << endl;

    cout << "\nPresione ENTER para regresar al menú principal" << endl;

    //Se establece una pausa hasta que el usuario presione enter
    cin.ignore();
    cin.get();
}

//Función para el menú principal
string getUserMenuSelection()
{
    //Variable de seleccion para el menú
    string menuSelection = "";

    while (true)
    {

        cout << "---------------Bienvenido---------------" << endl;
        cout << "\nPor favor, ingrese el numero correspondiente a la opcion que desea ejecutar" << endl;
        cout << "\n(1) Ingresar un paciente al registro" << endl;
        cout << "(2) Buscar un paciente por ID" << endl;
        cout << "(3) Mostrar pacientes por nombre" << endl;
        cout << "(4) Borrar un paciente con ID especifica" << endl;
        cout << "(5) Salir del programa\n"
             << endl;

        cout << "Opcion: ";

        cin >> menuSelection;

        if (menuSelection == "1" || menuSelection == "2" || menuSelection == "3" || menuSelection == "4" || menuSelection == "5")
        {
            break;
        }
        else
        {
            system("clear");
            cout << "\nPor favor, digite solo una de las opciones" << endl;
            continue;
        }

        system("clear");
    }

    return menuSelection;
}
void mainProgram()
{
    //Mientras esta variable sea true, el programa está trabajando
    bool working = true;

    while (working)
    {
        system("clear");

        int integerMenuSelection = stoi(getUserMenuSelection());

        system("clear");

        switch (integerMenuSelection)
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
            // Si el usuario desea cerrar el programa se establece la variable de trabajo a falso
            working = false;
        }
        default:
            break;
        }
    }
}

int main()
{
    //Se lee el archivo con las tablas hash para cargarlas en memoria
    recoverHashes();
    //Se ejecuta el programa principal
    mainProgram();
    return 0;
}