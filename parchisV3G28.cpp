/*
PARCHIS V3
Integrantes:
Miguel Antonio Amato Hermo (Ingenieria de Software)
Grupo D
*/

#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>
#include <fstream>
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

using namespace std;

//Constantes globales 
const int NUM_JUGADORES = 4;
const int NUM_FICHAS = 4;
const int NUM_CASILLAS = 68;

//Tipos enumerado
typedef enum { Amarillo, Azul, Rojo, Verde, Ninguno, Gris } tColor;
typedef int tFichas[NUM_FICHAS];

struct tJugador {
    tColor color;
    tFichas fichas;
};

typedef tJugador tJugadores[NUM_JUGADORES];

struct tCasilla {
    tColor calle1;
    tColor calle2;
};

typedef tCasilla tCasillas[NUM_CASILLAS];

struct tJuego {
    tCasillas casillas;
    tJugadores jugadores;
    tColor jugadorTurno;
    int tirada;
    int premio = 0;
    int seises = 0;
    int ultimaFichaMovida = 0;
};

//Funciones del parchis anterior
string colorACadena(tColor colorCad1);
bool esSeguro(int pos);
int salidaJugador(tColor casilla);
int zanataJugador(tColor meta);

//Funciones del parchis

void iniciar(tJuego& juego);
int cuantasEn(tJuego& juego, int casilla);
//Funcion que verifica cuantas fichas hay en una casilla.
int primeraEn(tJuego& juego, int casilla);
//Menor índice de las fichas del jugador que están en esa casilla
int segundaEn(tJuego& juego, int casilla);
//Mayor índice de las ficha del jugador que están en esa casilla
void saleFicha(tJuego& juego, bool& pasaTurno);
// Funcion que contiene todos los casos posibles en los que una ficha puede salir de su casa (casilla = -1).
void aCasita(tJuego& juego, int casilla);
//Funcion que envia una ficha comida por otra ficha de otro color a su respectiva casa.
bool procesa5(tJuego& juego, bool& pasaTurno);
// Funcion que, cuando un la tirada es 5, verifica si el jugador tiene una ficha en casa y si la tiene la envia a su casilla de salida con saleFicha.
int tirarDado();
//Funcion que devuelve un numero aleatorio del 1 al 6 que simula la tirada de un dado.
bool puente(tJuego& juego, int casilla);
//Funcion que verifica en que casilla se formo un puente con dos fichas.
bool puedeMover(tJuego& juego, int ficha, int& casilla);
//Funcion que simula la tirada de todas las fichas que se pueden mover del jugadorTurno y devuelve true si la ficha puede moverse a la casilla destino sin encontrarse con ningun problema (puentes, sobrepasarse de meta, etc.)
void mover(tJuego& juego, int ficha, int casilla, bool& pasaTurno);
//Luego de verificar que fichas se pueden mover con puedeMover, entra la funcion mover para que la ficha cambie su posicion a la casilla destino (casilla origen + tirada).
void abrirPuente(tJuego& juego, int casillaOrig, int casillaDest);
//Abre el puente que tenga el jugadorTurno en la casilla de origen
bool procesa6(tJuego& juego, bool& pasaTurno);
//Funcion que procesa las tiradas de seises del parchis
void cargar(tJuego& juego, ifstream& archivo);
//Funcion que hace el procedimiento de las pruebas del parchis (Modo debug)
bool todasEnMeta(const tFichas jugador);
//Funcion que verifica si todas las fichas del jugador estan en meta (casilla 108)

// PROTOTIPOS DE LOS SUBPROGRAMAS DE VISUALIZACIÓN DEL TABLERO

void mostrar(const tJuego& juego);
void setColor(tColor color);
void iniciaColores();

//El programa principal:

int main() {
    // Variables V2
    int casilla;
    int ficha;
    bool pasaTurno = true;
    bool win = false;
    bool debug = false;
    bool jugada = false;
    bool puede;
    int cambioModo;

    // Variables V3
    tJuego juego;
    ifstream archivo("archivo.txt");
    bool endArchivo = false;

    cout << "Bienvenid@ a: Parchis V3!" << endl;
    cout << "Hecho por: Carlos Vesteiro y Miguel Amato" << endl << endl;
    cout << "Si quieres jugar el parchis en modo debug,  introduce  ----> 0 " << endl;
    cout << "Si quieres jugar el parchis en modo normal, introduce  ----> Cualquier otro valor" << endl << endl;
    cout << "Que modo quieres jugar? ----> ";
    cin >> cambioModo;
    if (cambioModo == 0) {
        debug = true;
    }

    iniciaColores();
    iniciar(juego);

    if (debug) {
        cargar(juego, archivo);
    }

    mostrar(juego);

    while (!win) {
        jugada = false;
        setColor(juego.jugadorTurno);
        cout << "Le toca al jugador: " << colorACadena(juego.jugadorTurno) << endl;
        pasaTurno = true;
        if (juego.premio != 0) {
            cout << "Tienes un premio de " << juego.premio << " movimientos extra!" << endl;
            if (juego.tirada >= 6) {
                pasaTurno = false;
            }
            juego.tirada = juego.premio;
            juego.premio = 0;
        }
        else {
            if (debug) {
                if (endArchivo) {
                    cout << "Dame tirada (0 para salir): ";
                    cin >> juego.tirada;
                }
                else {
                    archivo >> juego.tirada;
                    if (juego.tirada == -1) {
                        endArchivo = true;
                        archivo.close();
                        cout << "Dame tirada (0 para salir): ";
                        cin >> juego.tirada;
                    }
                    else {
                        cout << endl << "Ha salido un: " << juego.tirada << endl;
                    }
                }
            }
            else {
                juego.tirada = tirarDado();
                cout << "El jugador lanza los dados y sale un: " << juego.tirada << endl;
            }

        }

        if (juego.tirada == 0) {
            win = true;
        }

        if (juego.tirada == 5 && procesa5(juego, pasaTurno)) {
            juego.tirada = 0;
            jugada = true;
            cout << endl << "El jugador " << colorACadena(juego.jugadorTurno) << " tiene una ficha en casa, sale a calle." << endl << endl;;
        }

        else if (juego.tirada == 6 && procesa6(juego, pasaTurno)) {
            jugada = true;
        }


        if (!jugada) {
            puede = false;
            for (int f = 0; f < NUM_FICHAS; f++) {
                cout << f + 1 << ": ";
                casilla = juego.jugadores[juego.jugadorTurno].fichas[f];
                if (puedeMover(juego, f, casilla)) {
                    cout << "Puede ir a la casilla " << casilla << endl;
                    puede = true;
                }
                else
                    cout << "No se puede mover" << endl;
            }
            while (puede) {
                cout << "Ficha a mover: ";
                cin >> ficha;
                cin.get();
                ficha--;
                casilla = juego.jugadores[juego.jugadorTurno].fichas[ficha];
                if (ficha > 3 || ficha < 0) {
                    cout << "La ficha que has introducido no existe, intenta otra vez" << endl;
                }
                else if (puedeMover(juego, ficha, casilla)) {
                    mover(juego, ficha, casilla, pasaTurno);
                    puede = false;
                }
                else {
                    cout << "La ficha que has introducido no se puede mover, intenta otra vez" << endl;
                }
            }
        }

        system("pause");
        mostrar(juego);

        if (pasaTurno == true) {
            juego.jugadorTurno = tColor((int(juego.jugadorTurno) + 1) % NUM_JUGADORES);
            juego.seises = 0;
        }
        if (todasEnMeta(juego.jugadores[juego.jugadorTurno].fichas)) {
            setColor(juego.jugadorTurno);
            cout << "Felicidades ha ganado el jugador " << colorACadena(juego.jugadorTurno) << "!!!" << endl;
            win = true;
        }
    }
    return 0;
}

// Subprogramas de la version 1

string colorACadena(tColor colorCad1) {
    string color;
    if (colorCad1 == Rojo) {
        color = "Rojo";
    }
    else if (colorCad1 == Verde) {
        color = "Verde";
    }
    else if (colorCad1 == Amarillo) {
        color = "Amarillo";
    }
    else if (colorCad1 == Azul) {
        color = "Azul";
    }
    return color;
}

bool esSeguro(int pos) { //Asigna que casillas seran seguros 
    bool devolver;
    if (pos == 0) {
        devolver = true;
    }
    else if (pos == 5) {
        devolver = true;
    }
    else if (pos == 12) {
        devolver = true;
    }
    else if (pos == 17) {
        devolver = true;
    }
    else if (pos == 22) {
        devolver = true;
    }
    else if (pos == 29) {
        devolver = true;
    }
    else if (pos == 34) {
        devolver = true;
    }
    else if (pos == 39) {
        devolver = true;
    }
    else if (pos == 46) {
        devolver = true;
    }
    else if (pos == 51) {
        devolver = true;
    }
    else if (pos == 56) {
        devolver = true;
    }
    else if (pos == 63) {
        devolver = true;
    }
    else {
        devolver = false;
    }
    return devolver;
}

int salidaJugador(tColor color) //Asigna en que casilla saldra el jugador dependiendo de que color haya elegido
{
    int salida = 0;
    if (color == Rojo)
        salida = 39;
    else if (color == Verde)
        salida = 56;
    else if (color == Amarillo)
        salida = 5;
    else if (color == Azul)
        salida = 22;
    return salida;
}

int zanataJugador(tColor color) { // Asigna las zanatas de los respectivos colores
    int zanata = 0;
    if (color == Rojo)
        zanata = 34;
    else if (color == Verde)
        zanata = 51;
    else if (color == Amarillo)
        zanata = 0;
    else if (color == Azul)
        zanata = 17;
    return zanata;
}

// Subprogramas de la version 2

void iniciar(tJuego& juego) {
    srand(time(NULL));
    setColor(Gris);
    for (int i = 0; i < NUM_JUGADORES; i++)
        for (int j = 0; j < NUM_FICHAS; j++)
            juego.jugadores[i].fichas[j] = -1;
    for (int i = 0; i < NUM_CASILLAS; i++) {
        juego.casillas[i].calle1 = Ninguno;
        juego.casillas[i].calle2 = Ninguno;
    }
    juego.jugadorTurno = tColor(rand() % NUM_JUGADORES);
}

int cuantasEn(const tFichas jugador, int casilla) {
    int num = 0;
    for (int i = 0; i < NUM_FICHAS; i++) {
        if (jugador[i] == casilla)
            num++;
    }
    return num;
}

int primeraEn(const tFichas jugador, int casilla) {
    int pos = 0;
    bool primeraEn = false;

    while ((pos < NUM_FICHAS) && !primeraEn) {
        if (jugador[pos] == casilla)
            primeraEn = true;
        else
            pos++;
    }
    if (!primeraEn)
        pos = -1;
    return pos;
}

int segundaEn(const tFichas jugador, int casilla) {
    int pos = NUM_FICHAS - 1;
    bool segundaEn = false;

    while ((pos >= 0) && !segundaEn) {
        if (jugador[pos] == casilla)
            segundaEn = true;
        else
            pos--;
    }
    if (!segundaEn)
        pos = -1;
    return pos;

}

void saleFicha(tJuego& juego, bool& pasaTurno) {
    int salida = salidaJugador(juego.jugadorTurno);
    int ficha = primeraEn(juego.jugadores[juego.jugadorTurno].fichas, -1);
    tColor aux;
    juego.jugadores[juego.jugadorTurno].fichas[ficha] = salida;
    if (juego.casillas[salida].calle2 == juego.jugadorTurno) {
        aux = juego.casillas[salida].calle1;
        juego.casillas[salida].calle1 = juego.casillas[salida].calle2;
        juego.casillas[salida].calle2 = aux;
        aCasita(juego, salida);
        juego.premio = 20;
        pasaTurno = false;
    }
    else if (juego.casillas[salida].calle2 != Ninguno) {
        aCasita(juego, salida);
        juego.premio = 20;
        pasaTurno = false;
    }
    juego.casillas[salida].calle2 = juego.casillas[salida].calle1;
    juego.casillas[salida].calle1 = juego.jugadorTurno;
}

void aCasita(tJuego& juego, int casilla) {
    tColor color = juego.casillas[casilla].calle2;
    int ficha = segundaEn(juego.jugadores[color].fichas, casilla);
    juego.jugadores[color].fichas[ficha] = -1;
    juego.casillas[casilla].calle2 = Ninguno;
}

bool procesa5(tJuego& juego, bool& pasaTurno) {
    int salida = salidaJugador(juego.jugadorTurno);
    bool devolver = false;
    if (cuantasEn(juego.jugadores[juego.jugadorTurno].fichas, -1) > 0 && cuantasEn(juego.jugadores[juego.jugadorTurno].fichas, salida) < 2) {
        saleFicha(juego, pasaTurno);
        devolver = true;
    }
    return devolver;
}

int tirarDado() {
    int ran;
    srand(time(NULL));
    ran = rand() % 6 + 1;
    return ran;
}

bool puente(tJuego& juego, int casilla) {
    bool  devolver;
    if (juego.casillas[casilla].calle1 != Ninguno && casilla > -1 && casilla < 68 && juego.casillas[casilla].calle1 == juego.casillas[casilla].calle2)
        devolver = true;
    else
        devolver = false;
    return devolver;
}

bool puedeMover(tJuego& juego, int ficha, int& casilla) {
    int zanata = zanataJugador(juego.jugadorTurno);
    bool devolver = true;
    int casillaDev = casilla;
    int i = 0;

    if (casillaDev != -1 && casillaDev != 108) {
        while (i < juego.tirada && devolver) {
            if (casillaDev == zanata) {
                casillaDev = 101;
                i++;
            }

            else if (casillaDev > 100 && casillaDev <= 108) {
                casillaDev = casillaDev + 1;
                i++;
            }
            else if (casillaDev >= 0 && casillaDev < NUM_CASILLAS) {
                casillaDev = casillaDev + 1;
                if (casillaDev > NUM_CASILLAS - 1) {
                    casillaDev = casillaDev - NUM_CASILLAS;
                }
                i++;
            }

            if (cuantasEn(juego.jugadores[juego.jugadorTurno].fichas, casillaDev) == 2 && i == juego.tirada && casillaDev != 108) {
                devolver = false;
                i = juego.tirada + 1;
            }
            else if (casillaDev < NUM_CASILLAS && juego.casillas[casillaDev].calle1 != Ninguno && juego.casillas[casillaDev].calle2 != Ninguno && i == juego.tirada) {
                devolver = false;
                i = juego.tirada + 1;
            }
            else if (casillaDev == 108 && i != juego.tirada) {
                devolver = false;
                i = juego.tirada + 1;
            }
            else if (casillaDev < NUM_CASILLAS && esSeguro(casillaDev) && puente(juego, casillaDev)) {
                devolver = false;
                i = juego.tirada + 1;
            }
        }
    }
    else {
        devolver = false;
    }
    if (devolver) {
        casilla = casillaDev;
    }

    return devolver;
}

void mover(tJuego& juego, int ficha, int casilla, bool& pasaTurno) {
    int casillaInicial = juego.jugadores[juego.jugadorTurno].fichas[ficha];

    if (juego.casillas[casillaInicial].calle1 == juego.casillas[casillaInicial].calle2 && juego.casillas[casillaInicial].calle2 != Ninguno) {
        juego.casillas[casillaInicial].calle2 = Ninguno;
    }
    else if (juego.casillas[casillaInicial].calle1 == juego.jugadorTurno && juego.casillas[casillaInicial].calle2 != Ninguno) {
        juego.casillas[casillaInicial].calle1 = juego.casillas[casillaInicial].calle2;
        juego.casillas[casillaInicial].calle2 = Ninguno;
    }
    else if (juego.casillas[casillaInicial].calle1 != Ninguno && juego.casillas[casillaInicial].calle2 == juego.jugadorTurno) {
        juego.casillas[casillaInicial].calle2 = Ninguno;
    }
    else {
        juego.casillas[casillaInicial].calle1 = Ninguno;
    }

    if (casilla == 108) {
        juego.premio = 10;
        pasaTurno = false;
    }
    else if (casilla < NUM_CASILLAS) {
        if (juego.casillas[casilla].calle1 == Ninguno) {
            juego.casillas[casilla].calle1 = juego.jugadorTurno;
        }
        else {
            if (esSeguro(casilla)) {
                juego.casillas[casilla].calle2 = juego.jugadorTurno;
            }
            else {
                if (juego.casillas[casilla].calle1 != Ninguno && juego.casillas[casilla].calle1 != juego.jugadorTurno) {
                    juego.casillas[casilla].calle2 = juego.casillas[casilla].calle1;
                    aCasita(juego, casilla);
                    juego.casillas[casilla].calle1 = juego.jugadorTurno;
                    juego.premio = 20;
                    pasaTurno = false;
                }
                else if (juego.casillas[casilla].calle1 == juego.jugadorTurno) {
                    juego.casillas[casilla].calle2 = juego.jugadorTurno;
                }
            }
        }
    }
    juego.jugadores[juego.jugadorTurno].fichas[ficha] = casilla;
    juego.ultimaFichaMovida = ficha;
}

void abrirPuente(tJuego& juego, int casillaOrig, int casillaDest) {
    bool aux = true;
    mover(juego, segundaEn(juego.jugadores[juego.jugadorTurno].fichas, casillaOrig), casillaDest, aux);
    cout << "Se ha abierto un puente en: " << casillaOrig << endl;
    juego.ultimaFichaMovida = segundaEn(juego.jugadores[juego.jugadorTurno].fichas, casillaOrig);
}

bool procesa6(tJuego& juego, bool& pasaTurno) {
    bool devolver = false;
    int ultimaFichaMovida = juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida];
    juego.seises++;
    if (cuantasEn(juego.jugadores[juego.jugadorTurno].fichas, -1) == 0) {
        juego.tirada = 7;
    }
    if (juego.seises == 3) {
        if (ultimaFichaMovida > 100) {
            cout << "Has sacado 3 seises consecutivos, como la ficha " << juego.ultimaFichaMovida + 1 << " esta en subida, no va a casa" << endl;
        }
        else if (cuantasEn(juego.jugadores[juego.jugadorTurno].fichas, -1) != 4) {
            if (juego.casillas[ultimaFichaMovida].calle2 == Ninguno) {
                juego.casillas[ultimaFichaMovida].calle1 = Ninguno;
                juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida] = -1;
                cout << "Has sacado 3 seises consecutivos, la ficha " << juego.ultimaFichaMovida + 1 << " se ha ido a casa" << endl;
            }
            else if (juego.casillas[ultimaFichaMovida].calle2 == juego.jugadorTurno) {
                juego.casillas[ultimaFichaMovida].calle2 = Ninguno;
                juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida] = -1;
                cout << "Has sacado 3 seises consecutivos, la ficha " << juego.ultimaFichaMovida + 1 << " se ha ido a casa" << endl;
            }
            else if (juego.casillas[ultimaFichaMovida].calle1 == juego.jugadorTurno && juego.casillas[ultimaFichaMovida].calle2 != juego.jugadorTurno) {
                juego.casillas[ultimaFichaMovida].calle1 = juego.casillas[ultimaFichaMovida].calle2;
                juego.casillas[ultimaFichaMovida].calle2 = Ninguno;
                juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida] = -1;
                cout << "Has sacado 3 seises consecutivos, la ficha " << juego.ultimaFichaMovida + 1 << " se ha ido a casa" << endl;
            }
        }

        pasaTurno = true;
        devolver = true;
    }

    else {
        int fichasPuentes = 0;
        int fichasMoverse = 0;
        int casillaPuente = 0;
        int casillaDest = 0;
        int casillaAMoverse;
        int ficha;
        for (int i = 0; i < NUM_FICHAS; i++) {
            if (puente(juego, juego.jugadores[juego.jugadorTurno].fichas[i])) {
                fichasPuentes++;
                casillaDest = juego.jugadores[juego.jugadorTurno].fichas[i];
                if (puedeMover(juego, i, casillaDest)) {
                    fichasMoverse++;
                    casillaPuente = juego.jugadores[juego.jugadorTurno].fichas[i];
                    casillaAMoverse = casillaDest;
                }
            }
        }
        if ((fichasPuentes == 2 || fichasPuentes == 4) && fichasMoverse == 2) {
            ficha = segundaEn(juego.jugadores[juego.jugadorTurno].fichas, casillaPuente);
            mover(juego, ficha, casillaAMoverse, pasaTurno);
            cout << "Se ha abierto el puente de la casilla " << casillaPuente << endl;
            devolver = true;
        }
        pasaTurno = false;
    }

    return devolver;
}

bool todasEnMeta(const tFichas jugador) {
    bool devolver = false;
    if (cuantasEn(jugador, 108) == 4) {
        devolver = true;
    }
    return devolver;
}

//VOID MOSTRAR

void mostrar(const tJuego& juego) {
    int casilla, ficha;
    tColor jug;

    cout << "\x1b[2J\x1b[H"; // Se situa en la esquina superior izquierda
    setColor(Gris);
    cout << endl;

    // Filas con la numeración de las casillas...
    for (int i = 0; i < NUM_CASILLAS; i++)
        cout << i / 10;
    cout << endl;
    for (int i = 0; i < NUM_CASILLAS; i++)
        cout << i % 10;
    cout << endl;

    // Borde superior...
    for (int i = 0; i < NUM_CASILLAS; i++)
        cout << '>';
    cout << endl;

    // Primera fila de posiciones de fichas...
    for (int i = 0; i < NUM_CASILLAS; i++) {
        setColor(juego.casillas[i].calle2);
        if (juego.casillas[i].calle2 != Ninguno)
            cout << primeraEn(juego.jugadores[juego.casillas[i].calle2].fichas, i) + 1;
        else
            cout << ' ';
        setColor(Gris);
    }
    cout << endl;

    // "Mediana"   
    for (int i = 0; i < NUM_CASILLAS; i++)
        if (esSeguro(i))
            cout << 'o';
        else
            cout << '-';
    cout << endl;


    // Segunda fila de posiciones de fichas...
    for (int i = 0; i < NUM_CASILLAS; i++) {
        setColor(juego.casillas[i].calle1);
        if (juego.casillas[i].calle1 != Ninguno)
            cout << segundaEn(juego.jugadores[juego.casillas[i].calle1].fichas, i) + 1;
        else
            cout << ' ';
        setColor(Gris);
    }
    cout << endl;

    jug = Amarillo;
    // Borde inferior...
    for (int i = 0; i < NUM_CASILLAS; i++)
        if (i == zanataJugador(jug)) {
            setColor(jug);
            cout << "V";
            setColor(Gris);
        }
        else if (i == salidaJugador(jug)) {
            setColor(jug);
            cout << "^";
            setColor(Gris);
            jug = tColor(int(jug) + 1);
        }
        else
            cout << '>';
    cout << endl;

    // Metas y casas...
    for (int i = 0; i < NUM_FICHAS; i++) {
        casilla = 0;
        jug = Amarillo;
        setColor(jug);
        while (casilla < NUM_CASILLAS) {
            if (casilla == zanataJugador(jug)) {
                ficha = primeraEn(juego.jugadores[jug].fichas, 101 + i);
                if (ficha != -1) {
                    cout << ficha + 1;
                    if (cuantasEn(juego.jugadores[jug].fichas, 101 + i) > 1) {
                        ficha = segundaEn(juego.jugadores[jug].fichas, 101 + i);
                        if (ficha != -1) {
                            cout << ficha + 1;
                        }
                        else
                            cout << "V";
                    }
                    else
                        cout << "V";
                }
                else
                    cout << "VV";
                casilla++;
            }
            else if (casilla == salidaJugador(jug)) {
                if (juego.jugadores[jug].fichas[i] == -1) // En casa
                    cout << i + 1;
                else
                    cout << "^";
                jug = tColor(int(jug) + 1);
                setColor(jug);
            }
            else
                cout << ' ';
            casilla++;
        }
        cout << endl;
    }

    // Resto de metas...
    for (int i = 105; i <= 107; i++) {
        casilla = 0;
        jug = Amarillo;
        setColor(jug);
        while (casilla < NUM_CASILLAS) {
            if (casilla == zanataJugador(jug)) {
                ficha = primeraEn(juego.jugadores[jug].fichas, i);
                if (ficha != -1) {
                    cout << ficha + 1;
                    if (cuantasEn(juego.jugadores[jug].fichas, i) > 1) {
                        ficha = segundaEn(juego.jugadores[jug].fichas, i);
                        if (ficha != -1) {
                            cout << ficha + 1;
                        }
                        else
                            cout << "V";
                    }
                    else
                        cout << "V";
                }
                else
                    cout << "VV";
                casilla++;
                jug = tColor(int(jug) + 1);
                setColor(jug);
            }
            else
                cout << ' ';
            casilla++;
        }
        cout << endl;
    }

    casilla = 0;
    jug = Amarillo;
    setColor(jug);
    while (casilla < NUM_CASILLAS) {
        cout << ((juego.jugadores[jug].fichas[0] == 108) ? '1' : '.');
        cout << ((juego.jugadores[jug].fichas[1] == 108) ? '2' : '.');
        jug = tColor(int(jug) + 1);
        setColor(jug);
        cout << "               ";
        casilla += 17;
    }
    cout << endl;
    casilla = 0;
    jug = Amarillo;
    setColor(jug);
    while (casilla < NUM_CASILLAS) {
        cout << ((juego.jugadores[jug].fichas[2] == 108) ? '3' : '.');
        cout << ((juego.jugadores[jug].fichas[3] == 108) ? '4' : '.');
        jug = tColor(int(jug) + 1);
        setColor(jug);
        cout << "               ";
        casilla += 17;
    }
    cout << endl << endl;
    setColor(Gris);
}

void setColor(tColor color) {
    switch (color) {
    case Azul:
        cout << "\x1b[34;107m";
        break;
    case Verde:
        cout << "\x1b[32;107m";
        break;
    case Rojo:
        cout << "\x1b[31;107m";
        break;
    case Amarillo:
        cout << "\x1b[33;107m";
        break;
    case Gris:
    case Ninguno:
        cout << "\x1b[90;107m";
        break;
    }
}

void iniciaColores() {
#ifdef _WIN32
    for (DWORD stream : {STD_OUTPUT_HANDLE, STD_ERROR_HANDLE}) {
        DWORD mode;
        HANDLE handle = GetStdHandle(stream);

        if (GetConsoleMode(handle, &mode)) {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(handle, mode);
        }
    }
#endif
}

void cargar(tJuego& juego, ifstream& archivo) {
    int jugador, casilla;

    for (int i = 0; i < NUM_JUGADORES; i++)
        for (int f = 0; f < NUM_FICHAS; f++) {
            archivo >> casilla;
            juego.jugadores[i].fichas[f] = casilla;
            if ((casilla >= 0) && (casilla < NUM_CASILLAS))
                if (juego.casillas[casilla].calle1 == Ninguno)
                    juego.casillas[casilla].calle1 = tColor(i);
                else
                    juego.casillas[casilla].calle2 = tColor(i);
        }
    archivo >> jugador;
    juego.jugadorTurno = tColor(jugador);
}

// :)
