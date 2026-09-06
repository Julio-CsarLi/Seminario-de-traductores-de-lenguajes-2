#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <regex>
#include <cmath>
#include <iomanip> // NUEVO: Librería para alinear columnas

using namespace std;

// =========================================================
// 1. PILA DE OBJETOS
// =========================================================
class ElementoPila {
public:
    virtual string getToken() { return ""; }
    virtual int getEstado() { return -1; }
};

class Terminal : public ElementoPila {
    string simbolo;
public:
    Terminal(string s) { simbolo = s; }
    string getToken() override { return simbolo; }
};

class NoTerminal : public ElementoPila {
    string simbolo;
public:
    NoTerminal(string s) { simbolo = s; }
    string getToken() override { return simbolo; }
};

class Estado : public ElementoPila {
    int estado;
public:
    Estado(int e) { estado = e; }
    int getEstado() override { return estado; }
};

class Pila {
    list<ElementoPila*> lista;
public:
    void push(ElementoPila* x) { lista.push_front(x); }
    void pop() { lista.erase(lista.begin()); }
    ElementoPila* top() { return *lista.begin(); }

    // MODIFICADO: Ahora regresa un string en lugar de imprimir directo
    string obtenerTexto() {
        string res = "";
        for (auto it = lista.rbegin(); it != lista.rend(); it++) {
            if ((*it)->getEstado() != -1) res += to_string((*it)->getEstado()) + " ";
            else res += (*it)->getToken() + " ";
        }
        return res;
    }
};

// =========================================================
// 2. ANALIZADOR LÉXICO
// =========================================================
struct Token { string lexema; int tipo; };

vector<Token> lexico(string codigo) {
    vector<pair<int, regex>> reglas = {
        {3, regex("^\"[^\"]*\"")}, {4, regex("^(int|float|void)\\b")},
        {19, regex("^if\\b")}, {20, regex("^while\\b")}, {21, regex("^return\\b")}, {22, regex("^else\\b")},
        {0, regex("^[a-zA-Z][a-zA-Z0-9]*")}, {2, regex("^\\d+\\.\\d+")}, {1, regex("^\\d+")},
        {11, regex("^(==|!=)")}, {7, regex("^(<=|>=|<|>)")}, {9, regex("^&&")}, {8, regex("^\\|\\|")},
        {10, regex("^!")}, {18, regex("^=")}, {5, regex("^(\\+|\\-)")}, {6, regex("^(\\*|/)")},
        {12, regex("^;")}, {13, regex("^,")}, {14, regex("^\\(")}, {15, regex("^\\)")},
        {16, regex("^\\{")}, {17, regex("^\\}")}, {23, regex("^\\$")}
    };

    vector<Token> tokens;
    string restante = codigo;
    smatch m;

    while (!restante.empty()) {
        if (regex_search(restante, m, regex("^\\s+"))) { restante = m.suffix(); continue; }

        bool ok = false;
        for (auto& r : reglas) {
            if (regex_search(restante, m, r.second)) {
                tokens.push_back({m.str(), r.first});
                restante = m.suffix();
                ok = true; break;
            }
        }
        if (!ok) { cout << "Error lexico: " << restante[0] << "\n"; restante = restante.substr(1); }
    }
    return tokens;
}

// =========================================================
// 3. PROGRAMA PRINCIPAL
// =========================================================
int main() {
    ifstream archivo("compilador.lr");
    if (!archivo.is_open()) {
        cout << "Error: No se encuentra 'compilador.lr'.\n"; return 1;
    }

    int n_reglas;
    archivo >> n_reglas;
    vector<int> idRegla(n_reglas), lonRegla(n_reglas);
    vector<string> nomRegla(n_reglas);
    for (int i = 0; i < n_reglas; i++) archivo >> idRegla[i] >> lonRegla[i] >> nomRegla[i];

    int filas, columnas;
    archivo >> filas >> columnas;
    vector<vector<int>> tabla(filas, vector<int>(columnas));
    for (int f = 0; f < filas; f++) {
        for (int c = 0; c < columnas; c++) {
            archivo >> tabla[f][c];
        }
    }
    archivo.close();

    // Código de prueba correcto (declaración + asignación dentro de una función)
    string codigo = "int main ( ) { int a ; a = 10 ; } $";
    vector<Token> tokens = lexico(codigo);

    Pila pila;
    pila.push(new Estado(0));
    int cursor = 0;

    cout << "INICIANDO COMPILADOR...\n\n";

    while (true) {
        int estado = pila.top()->getEstado();
        Token t = tokens[cursor];
        int accion = tabla[estado][t.tipo];

        // --- INICIO DE ZONA DE IMPRESIÓN ORDENADA ---
        string texto_pila = pila.obtenerTexto();

        // Si la pila mide más de 65 caracteres, le recortamos el inicio y ponemos "..."
        // para que no desborde la terminal
        if (texto_pila.length() > 65) {
            texto_pila = "..." + texto_pila.substr(texto_pila.length() - 62);
        }

        // setw(75) fuerza a que el espacio mida exactamente 75 caracteres rellenando con espacios blancos
        cout << left << setw(75) << ("Pila: " + texto_pila)
             << "| Entrada: " << setw(10) << t.lexema;

        if (accion > 0) cout << " | Accion: d" << accion << "\n";
        else if (accion < -1) cout << " | Accion: r" << (abs(accion) - 2) << " (" << nomRegla[abs(accion)-2] << ")\n";
        else if (accion == -1) cout << " | Accion: Aceptar\n";
        else cout << " | Accion: ERROR\n";
        // --- FIN DE ZONA DE IMPRESIÓN ORDENADA ---

        if (accion > 0) {
            pila.push(new Terminal(t.lexema));
            pila.push(new Estado(accion));
            cursor++;
        }
        else if (accion == -1) {
            cout << "\n¡CADENA ACEPTADA! El programa es sintacticamente correcto.\n";
            break;
        }
        else if (accion < -1) {
            int r = abs(accion) - 2;
            for (int i = 0; i < lonRegla[r] * 2; i++) pila.pop();

            int salto = tabla[pila.top()->getEstado()][idRegla[r]];

            pila.push(new NoTerminal(nomRegla[r]));
            pila.push(new Estado(salto));
        }
        else {
            cout << "\nERROR SINTACTICO CERCA DE: '" << t.lexema << "'\n";
            break;
        }
    }
    return 0;
}
