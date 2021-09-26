#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <unistd.h>
using namespace std;

struct Registro
{
  char codigo[5];
  char nombre[20];
  char carrera[15];
  int ciclo;
  int next = -2;
  char place[2];

  Registro(){};
  Registro(string codigo, string nombre, string carrera, int ciclo)
  {
    strcpy(this->codigo, codigo.c_str());
    strcpy(this->nombre, nombre.c_str());
    strcpy(this->carrera, carrera.c_str());
    this->ciclo = ciclo;
  }
};

istream &operator>>(istream &stream, Registro &record)
{
  stream.read(record.codigo, 5);
  stream.read(record.nombre, 20);
  stream.read(record.carrera, 15);
  stream.read((char *)&record.ciclo, sizeof(int));
  stream.read((char *)&record.next, sizeof(int));
  stream.read(record.place, 2);
  record.codigo[4] = '\0';
  record.nombre[19] = '\0';
  record.carrera[14] = '\0';
  record.place[1] = '\0';
  return stream;
}

ostream &operator<<(ostream &stream, Registro &record)
{
  stream.write(record.codigo, 4);
  stream << " ";
  stream.write(record.nombre, 19);
  stream << " ";
  stream.write(record.carrera, 14);
  stream << " ";
  stream.write((char *)&record.ciclo, sizeof(int));
  stream.write((char *)&record.next, sizeof(int));
  stream.write(record.place, 1);
  stream << " ";
  stream << '\n';
  return stream;
}

class FixedRecord
{
private:
  string fileName;
  string fileAux;

  Registro readrecord(int pos)
  {
    std::ifstream inFile;
    Registro record;
    std::string linea;
    inFile.open(fileName);
    inFile.seekg(pos * (sizeof(record) + 2), std::ios::beg);
    getline(inFile, linea) >> record;
    inFile.close();
    return record;
  }

  int searchPos(string key)
  {
    fstream file;
    file.open(fileName, ios::app);
    int totalSize = file.tellg();
    file.close();

    int l = 0;
    int u = totalSize - 1;
    while (u >= l)
    {
      int m = (l + u) / 2;
      Registro record = readrecord(m);
      string name;
      name += record.nombre;
      if (name < key)
      {
        u = m - 1;
      }
      else if (name > key)
      {
        l = m + 1;
      }
      else
      {
        return m;
      }
    }
    return 1000;
  }

public:
  FixedRecord(string fileName, string fileAux)
  {
    this->fileName = fileName;
    this->fileAux = fileAux;
  }

  void insertAll(vector<Registro> registros)
  {
    fstream file;
    file.open(this->fileName, ios::in | ios::out);
    Registro record;
    string line;
    bool empty = false;

    getline(file, line) >> record;
    if (record.next == -2)
    {
      empty = true;
    }

    if (empty)
    { //si el archivo principal esta vacio
      int position = 1000;
      for (auto registro : registros)
      {
        if (position > registro.next)
        {
          position = registro.next;
        }
      }
      while (position != -1)
      {
        Registro record;
        record = readrecord(position);
        file << record;
        position = record.next;
      }
    }
    else
    { // si el archivo principal contenia registros
      int lastPosition = record.next;
      while (getline(file, line) >> record)
      {
        if (record.place[0] == 'd')
        {
          lastPosition = record.next;
        }
        else
        {
          auto it = registros.begin();
          for (int i = 0; i < record.next; i++)
          {
            it++;
          }
          Registro newRecord = *it;
          file.seekp(lastPosition * (sizeof(record) + 1));
          file << newRecord;
        }
      }
    }
    file.close();
  }

  void add(Registro newRecord)
  {
    fstream aux;
    fstream file;
    aux.open(fileAux);
    file.open(fileName);
    int position = 1;
    char lastChar = 'd';
    Registro *lastRecord;
    while (position != -2 && position != -1)
    {
      Registro record;
      file.seekg((position - 1) * (sizeof(record)));
      file.read((char *)&record, sizeof(record));
      if (record.next == -2)
      { //si file.data esta vacio
        aux.seekg((position - 1) * (sizeof(record)));
        aux.read((char *)&record, sizeof(record));
        cout << "entree (if) - position = " << sizeof(record) << endl;
        cout << "record.next = " << aux.tellg() << endl;
        lastChar = 'a';
        if (record.next == -2)
        { //si aux.data esta vacio
          aux.close();
          newRecord.next = -1;
          newRecord.place[0] = 'd';
          ofstream aux;
          aux.open(fileAux, ios::out | ios::app);
          aux << newRecord;
          aux.close();
          position = -2;
          cout << "entree (if if) - position = " << position << endl;
        }
      }
      if (record.next != -2)
      {
        cout << "entree (else if) - position = " << position << endl;
        cout << "chris nombre = " << record.nombre << " ---" << endl;
        cout << "chris next = " << record.next << endl;
        cout << "ahris nombre = " << newRecord.nombre << " ---" << endl;
        string nameA, nameB;
        nameA += newRecord.nombre;
        nameB += record.nombre;
        if (nameA > nameB)
        {
          cout << "entre al mayor  " << endl;
          if (record.next == -1)
          {

            long lastPosition;
            if (lastChar == 'a')
            {
              lastPosition = aux.tellg();
            }
            else
            {
              lastPosition = file.tellg();
            }
            if (lastPosition > 0)
              lastPosition -= sizeof(record);
            else
              lastPosition++;
            cout << "last position" << lastPosition << endl;

            aux.close();
            fstream aux;
            aux.open(fileAux, ios::app);
            cout << "donde empezamos?" << aux.tellp() << endl;
            newRecord.next = -1;
            newRecord.place[0] = 'x';
            aux << newRecord;
            record.next = aux.tellp() / (sizeof(record) - 1);
            record.place[0] = 'a';
            position = -1;
            aux.close();
            if (lastChar == 'a')
            {
              fstream aux;
              aux.open(fileAux);
              aux.seekg(lastPosition);
              aux << record;
              aux.close();
            }
            else
            {
              file.seekp(lastPosition);
              file << record;
            }
            file.close();
          }
          else
          {
            sleep(1);
            cout << "entramos al else iffff" << endl;
            position = record.next;
            lastRecord = &record;
            if (record.place[0] == 'a')
            {
              aux.seekg((record.next - 1) * (sizeof(record) + 1));
              aux.read((char *)&record, sizeof(record));
            }
            else
            {
              file.seekg((record.next - 1) * (sizeof(record) + 1));
              file.read((char *)&record, sizeof(record));
            }
          }
        }
        else
        {
          sleep(1);
          cout << "entramos al ultimo else" << endl;
          if (position == 0)
          {
            newRecord.next = 1;
            newRecord.place[0] = 'd';
          }
          else
          {
            newRecord.next = lastRecord->next;
            newRecord.place[0] = lastRecord->place[0];
          }

          aux.close();
          ofstream aux;
          aux.open(fileAux, ios::app);
          if (record.codigo != lastRecord->codigo)
          {
            lastRecord->next = aux.tellp() / (sizeof(record) - 1);
            lastRecord->place[0] = 'a';
          }
          aux << newRecord;
          aux.close();
          position = -1;
        }
      }
    }
  }

  Registro search(string key)
  {
    int position = searchPos(key);
    Registro record = readrecord(position);
    return record;
  }

  vector<Registro> searchRange(string begin, string end)
  {
    int posBegin = searchPos(begin);
    int posEnd = searchPos(end);
    vector<Registro> registros;
    while (posBegin < posEnd)
    {
      Registro record = readrecord(posBegin);
      posBegin += sizeof(record);
      registros.push_back(record);
    }
    return registros;
  }
};