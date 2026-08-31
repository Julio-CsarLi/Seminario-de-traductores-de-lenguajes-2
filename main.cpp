#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cctype>

using namespace std;

struct Regla {
    int longitud_derecha; // Cuantos elementos se sacan de la pila
    int columna_izq;      // Columna de las variable
};

// Matriz de columnas ejercicio 1
string tabla_ejercicio1[5][4] = {
    {"d2", "",    "",     "1"}, // Estado 0
    {"",   "",    "acc",  "" }, // Estado 1
    {"",   "d3",  "",     "" }, // Estado 2
    {"d4", "",    "",     "" }, // Estado 3
    {"",   "",    "r1",   "" }  // Estado 4
};

// Reglas ejercicio 1:
Regla reglas_ejercicio1[2] = {
    {0, 0}, // Regla 0
    {3, 3}  // Regla 1
};

// matriz de columnas ejercicio 2
string tabla_ejercicio2[5][4] = {
    {"d2", "",    "",     "1"}, // Estado 0
    {"",   "",    "acc",  "" }, // Estado 1
    {"",   "d3",  "r2",   "" }, // Estado 2
    {"d2", "",    "",     "4"}, // Estado 3
    {"",   "",    "r1",   "" }  // Estado 4
};

// Reglas ejercicio 2:
Regla reglas_ejercicio2[3] = {
    {0, 0}, // Regla 0
    {3, 3}, // Regla 1
    {1, 3}  // Regla 2
};


// Algoritmo

// Analizador lexico:
vector<int> escanear_tokens(const string& entrada) {
    vector<int> tokens;
    for (size_t i = 0; i < entrada.length(); i++) {
        if (isalpha(entrada[i])) {
            // Si es una letra, avanzamos hasta que termine la palabra
            while (i + 1 < entrada.length() && isalpha(entrada[i+1])) { i++; }
            tokens.push_back(0); // 0 = id
        } else if (entrada[i] == '+') {
            tokens.push_back(1); // 1 = +
        }
    }
    tokens.push_back(2); // 2 = $ (fin de la cadena)
    return tokens;
}

// Imprimir la pila como texto
string pila_a_string(const vector<int>& pila) {
    string res = "";
    for (int estado : pila) res += to_string(estado) + " ";
    return res;
}

// Analizador sintactico
void analizar_LR1(string tabla[5][4], Regla reglas[], const string& entrada) {
    vector<int> tokens = escanear_tokens(entrada);
    vector<int> pila;
    pila.push_back(0); // El estado inicial siempre es 0
    int cursor = 0;

    cout << "\nAnalizando cadena: " << entrada << "\n";
    cout << left << setw(25) << "Pila" << setw(15) << "Token Actual" << "Accion\n";
    cout << string(60, '-') << "\n";

    while (true) {
        int estado_actual = pila.back();
        int token_actual = tokens[cursor];
        string accion = tabla[estado_actual][token_actual];

        // Se traduce el numero de token a texto para su impresion
        string nombre_token = (token_actual == 0) ? "id" : (token_actual == 1) ? "+" : "$";
        cout << left << setw(25) << pila_a_string(pila) << setw(15) << nombre_token;

        // Validar
        if (accion == "") {
            cout << ">> ERROR SINTACTICO. Casilla [" << estado_actual << ", " << nombre_token << "] vacia.\n";
            break;
        }
        else if (accion == "acc") {
            cout << "r0 (ACEPTADO)\n";
            cout << "\nRESULTADO: La cadena es VALIDA.\n";
            break;
        }
        else if (accion[0] == 'd') {
            // Desplazamiento
            cout << accion << "\n";
            int nuevo_estado = stoi(accion.substr(1));
            pila.push_back(nuevo_estado);
            cursor++; // Avanzar al siguiente token
        }
        else if (accion[0] == 'r') {
            // Reduccion
            cout << accion << " (Sacando " << reglas[stoi(accion.substr(1))].longitud_derecha << " elementos) -> ";

            int num_regla = stoi(accion.substr(1));
            int cantidad_a_sacar = reglas[num_regla].longitud_derecha;

            // Estados de la pila
            for(int i = 0; i < cantidad_a_sacar; i++) {
                pila.pop_back();
            }

            // Realizar el Salto o goto
            int estado_tope_nuevo = pila.back();
            int columna_variable = reglas[num_regla].columna_izq;
            string salto = tabla[estado_tope_nuevo][columna_variable];

            pila.push_back(stoi(salto));
            cout << "Salto a " << salto << "\n";
        }
    }
}

// Funcion
int main() {
    cout << "========================================\n";
    cout << "           PRUEBA EJERCICIO 1           \n";
    cout << "       Gramatica: E -> id + id          \n";
    cout << "========================================\n";
    analizar_LR1(tabla_ejercicio1, reglas_ejercicio1, "hola+mundo");

    cout << "\n\n========================================\n";
    cout << "           PRUEBA EJERCICIO 2           \n";
    cout << "     Gramatica: E -> id + E | id        \n";
    cout << "========================================\n";
    analizar_LR1(tabla_ejercicio2, reglas_ejercicio2, "a+b+c+d+e+f");

    return 0;
}
