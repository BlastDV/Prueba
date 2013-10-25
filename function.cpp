//ola ke ase
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <fstream>
using namespace std;

// CINTA
struct simbolo
{
	simbolo* prox;
	simbolo* prev;
	string caracter;

	simbolo ()
	{
		prox= NULL;
		prev= NULL;
		caracter="";
	}

	int eslimite (int movimiento)
	{
		if (movimiento==-1)
		{
			if (prev==NULL)
				return -1;
		}
		else
		if (movimiento== 1)
		{
			if (prox==NULL)
				return 1;
		}
		return 0;
	}

	void blanco(int movimiento, simbolo* &actual)
	{
		if (movimiento!=0)
		{
			if (movimiento==-1)
			{
				prev= new simbolo;
				prev->caracter=35;
				prev->prox= actual;
			}
			else
			if (movimiento==1)
			{
				prox= new simbolo;
				prox->caracter=35;
				prox->prev= actual;
			}
		}
	}
};

class chain
{
	public:
	simbolo* C;
	simbolo* cabezal;
	simbolo* ultimo;

	chain ()
	{
		C= NULL;
		cabezal= NULL;
		ultimo= NULL;
	}

	void agregarsimbolo (string caracter)
	{
		simbolo*  simbolo_aux= new simbolo();
		
		simbolo_aux->caracter= caracter;

		if (C==NULL)
		{
			C= simbolo_aux;
			cabezal= C;
			ultimo= C;
		}
		else
		{
			simbolo_aux->prev= ultimo;
			ultimo->prox= simbolo_aux;
			ultimo= simbolo_aux;
		}
	}
};

// FIN CINTA

// TRANSICIONES
struct estado;

struct trans
{
	string toread;
	string towrite;
	int movement;
	estado* destino;
	trans* prox;

	trans ()
	{
		toread="";
		towrite="";
		movement= -2;
		destino= NULL;
		prox= NULL;
	}
};

class paths
{
	public:
	trans* P;
	trans* actual;
	trans* ultimo;

	paths ()
	{
		P= NULL;
		actual= NULL;
		ultimo= NULL;
	}

	void agregartransicion (string toread, string towrite, estado* destino, string movement)
	{
		trans*  trans_aux= new trans();

		trans_aux->toread= toread;
		trans_aux->towrite= towrite;
		trans_aux->destino= destino;
		if (movement=="L")
			trans_aux->movement= -1;
		if (movement=="R")
			trans_aux->movement= 1;
		if (movement=="S")
			trans_aux->movement= 0;

		if (P==NULL)
		{
			P= trans_aux;
			actual= P;
			ultimo= P;
		}
		else
		{
			ultimo->prox= trans_aux;
			ultimo= trans_aux;
		}
	}
};
// FIN TRANSICIONES

// ESTADOS
struct estado
{
	int id;
	bool inicial;
	bool final;
	paths Transiciones;
	estado* prox;

	estado ()
	{
		id=0;
		inicial= false;
		final= false;
		prox= NULL;
	}
};

class estados
{
	public:
	estado* E;
	estado* actual;
	estado* ultimo;
	estado* inicial;

	estados ()
	{
		E= NULL;
		actual= NULL;
		ultimo= NULL;
		inicial= NULL;
	}

	void agregarestado (int id, bool inicial, bool final)
	{
		estado*  estado_aux= new estado();

		estado_aux->id= id;
		estado_aux->inicial= inicial;
		estado_aux->final= final;

		if (E==NULL)
		{
			E= estado_aux;
			actual= E;
			ultimo= E;
		}
		else
		{
			ultimo->prox= estado_aux;
			ultimo= estado_aux;
		}
	}
};
// FIN ESTADOS

// FIN DE ESTRUCTURAS

// Core del programa
void carpinteria (estados MT, string cadena)
{
	chain Cinta;

	// Llenamos la cinta con la cadena a leer
	int i=0;
	string aux;
	while (cadena[i]!=NULL)
	{
		aux= cadena[i];
		Cinta.agregarsimbolo (aux);
		i++;
	}
	// Ahora nos ubicamos en el estado inicial
	bool done= false;
	MT.actual= MT.E;
	while (MT.actual!= NULL && !done)
	{
		if (MT.actual->inicial)
		{
			done= true;
		}
		else
			MT.actual= MT.actual->prox;
	}

	// Y finalmente se empieza a procesar la cadena
	bool found= true;
	MT.actual->Transiciones.actual= MT.actual->Transiciones.P;
	while (found)
	{
		MT.actual->Transiciones.actual= MT.actual->Transiciones.P;
		bool done= false;
		while (MT.actual->Transiciones.actual!=NULL)
		{
			if (MT.actual->Transiciones.actual->toread== Cinta.cabezal->caracter || MT.actual->Transiciones.actual->toread=="%")
			{
				// Haciendo lo que ordena la maquina sobre la cinta
				Cinta.cabezal->caracter= MT.actual->Transiciones.actual->towrite;

				// Ahora para los movimientos, primero hay que fijarnos sino estamos moviendo el cabezal a una posicion inexistente de la lista
				// que representa la cinta. Asi evitamos un Segmentation Fault
				if (MT.actual->Transiciones.actual->movement==-1)
				{
					Cinta.cabezal->blanco (Cinta.cabezal->eslimite (-1), Cinta.cabezal);
					Cinta.cabezal= Cinta.cabezal->prev;
				}
				else
				{
					Cinta.cabezal->blanco (Cinta.cabezal->eslimite (1), Cinta.cabezal);
					Cinta.cabezal= Cinta.cabezal->prox;
				}

				// Y para el cambio de estado...
				MT.actual= MT.actual->Transiciones.actual->destino;
				done= true;

				break;
			}
			
			MT.actual->Transiciones.actual= MT.actual->Transiciones.actual->prox;
		}
		if (MT.actual->Transiciones.actual==NULL && !done)
		{
			found= false;
		}
	}

	// Por ultimo, analizamos el porque se interrumpio el procesamiento
	if (MT.actual->final) // ¿Alcanzamos el estado final?
	{
		//Cinta.cabezal= Cinta.C;
		string salida="";
		while (Cinta.cabezal!=NULL)
		{
			salida= salida + Cinta.cabezal->caracter;
			
			Cinta.cabezal= Cinta.cabezal->prox;
		}
		ofstream out ("mt.out", ios::app);
		out<<salida<<endl;
		out.close();
	}
	else
	{
		ofstream out ("mt.out", ios::app);
		out<<"reject "<<MT.actual->id<<endl;
		out.close();
	}
}
// Fin del Core

void load_data ()
{
	int n_casos;
	string linea;

	ifstream in ("mt.in");

	getline (in, linea);
	n_casos= atoi (linea.c_str());              //Lee la cantidad de casos de prueba
	
	for (int i=0; i<n_casos; i++)
	{
		ofstream out ("mt.out", ios::app);
		out<<"Caso #"<<i+1<<":"<<endl;
		out.close();
		
		int transicionesm, estadosm, simbolose, simbolosc, finalesm;

		getline (in, linea);
		// # Transiciones
		transicionesm= atoi (linea.substr (0, linea.find (32)).c_str());
		linea= linea.substr (linea.find (32)+1, strlen (linea.c_str())).c_str();
		// # Estados
		estadosm= atoi (linea.substr (0, linea.find (32)).c_str());
		linea= linea.substr (linea.find (32)+1, strlen (linea.c_str())).c_str();
		// # Simbolos de Entrada
		simbolose= atoi (linea.substr (0, linea.find (32)).c_str());
		linea= linea.substr (linea.find (32)+1, strlen (linea.c_str())).c_str();
		// # Simbolos de Cinta
		simbolosc= atoi (linea.substr (0, linea.find (32)).c_str());
		linea= linea.substr (linea.find (32)+1, strlen (linea.c_str())).c_str();
		// # Estados Finales
		finalesm= atoi (linea.substr (0, linea.find (32)).c_str());

		// Una vez obtenidos estos datos, ya podemos ir dando vida a nuestra estructura de datos
		estados MT;
		for (int e=0; e<estadosm; e++)
		{
			MT.agregarestado (e, false, false);
		}
		// Ahora tenemos todos los estados de la MT creados

		// Ignoremos las líneas donde se expresan ambos alfabetos
		getline (in, linea);
		getline (in, linea);

		// Ahora a lo mas importante, crear el mapeado de transiciones
		for (int t=0; t< transicionesm; t++)
		{
			int partida_aux, destino_aux;
			string encinta, acinta, movimiento_aux;
			getline (in, linea);

			// Obtenemos el ID del estado de partida de la transicion
			partida_aux= atoi (linea.substr (0, linea.find (32)).c_str());
			linea= linea.substr (linea.find (32)+1, strlen (linea.c_str())).c_str();
			// Ahora el caracter que debe estar en la cinta
			encinta= linea.substr (0, linea.find (32)).c_str();
			linea= linea.substr (linea.find (32)+1, strlen (linea.c_str())).c_str();
			// Luego el destino logico de la transicion
			destino_aux= atoi (linea.substr (0, linea.find (32)).c_str());
			linea= linea.substr (linea.find (32)+1, strlen (linea.c_str())).c_str();
			// Despues el caracter a escribir en la cinta
			acinta= linea.substr (0, linea.find (32)).c_str();
			linea= linea.substr (linea.find (32)+1, strlen (linea.c_str())).c_str();
			// Y finalmente el movimiento del cabezal
			movimiento_aux= linea.substr (0, linea.find (32)).c_str();

			// Ahora con todos los datos, hay que hacer un postprocesamiento sobre las variables con el sufijo _aux
			// Para el nodo inicial y el de destino
			MT.actual= MT.E;
			bool done= false;
			while (MT.actual!= NULL && !done)
			{
				if (MT.actual->id==partida_aux)
				{
					estado* actual_aux= MT.E;
					while (actual_aux!=NULL && !done)
					{
						if (actual_aux->id==destino_aux)
						{
							MT.actual->Transiciones.agregartransicion (encinta, acinta, actual_aux, movimiento_aux);
							done= true;
						}
						actual_aux= actual_aux->prox;
					}
				}
				MT.actual= MT.actual->prox;
			}
			MT.actual= MT.E;
		}

		// Con la maquina de turing casi totalmente construida, solo resta establecer los estados inicial y finales
		// y finalmente se podra iniciar con las pruebas
		
		getline (in, linea);

		for (int f=0; f<finalesm; f++)
		{
			int final;
			final= atoi (linea.substr (0, linea.find (32)).c_str());
			linea= linea.substr (linea.find (32)+1, strlen (linea.c_str())).c_str();
			
			bool done= false;
			while (MT.actual!= NULL && !done)
			{
				if (MT.actual->id==final)
				{
					MT.actual->final= true;
					done= true;
				}
				MT.actual= MT.actual->prox;
			}
			MT.actual= MT.E;
		}
		// Listos los estados finales

		getline (in, linea);

		int q0, n_cadenas;
		
		q0= atoi (linea.substr (0, linea.find (32)).c_str());
		linea= linea.substr (linea.find (32)+1, strlen (linea.c_str())).c_str();
		n_cadenas= atoi (linea.substr (0, linea.find (32)).c_str());
		linea= linea.substr (linea.find (32)+1, strlen (linea.c_str())).c_str();

		bool done= false;
		while (MT.actual!= NULL && !done)
		{
			if (MT.actual->id==q0)
			{
				MT.actual->inicial= true;
				done= true;
			}
			MT.actual= MT.actual->prox;
		}
		MT.actual= MT.E;
		// Finalmente queda todo leido y ahora viene el procesamiento de las cadenas
		for (int c=0; c< n_cadenas; c++)
		{
			getline (in, linea);
			carpinteria (MT, linea);
		}
	}	
}

int main ()
{
	load_data ();
	cout<<"Listo!"<<endl;
}
