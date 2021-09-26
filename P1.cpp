#include "P1.h"

int main()
{
  FixedRecord registros("datos.dat", "aux.dat");

  Registro record("0001", "lhristian", "computacion", 5);

  registros.add(record);
  //Registro otro = registros.search("jhristian");
  //cout << otro.carrera;
  return 0;
}