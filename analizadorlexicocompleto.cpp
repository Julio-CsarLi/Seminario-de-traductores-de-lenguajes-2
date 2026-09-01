#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <iomanip>

using namespace std;

struct Token {
    string lexema;
    int tipo;
};

struct ReglaLexica {
    int tipo;
    regex patron;
};

// Analizador lexico
vector<Token> analizadorLexico(string fuente) {
    // Determinamos el orden
    // Buscamos siempre al principio de la cadena
    vector<ReglaLexica> reglas = {
        {3,  regex("^\"[^\"]*\"")},           // 3: cadena (texto entre comillas)
        {4,  regex("^(int|float|void)\\b")},  // 4: tipo (la \b asegura que sea palabra exacta)
        {19, regex("^if\\b")},                // 19: if
        {20, regex("^while\\b")},             // 20: while
        {21, regex("^return\\b")},            // 21: return
        {22, regex("^else\\b")},              // 22: else
        {0,  regex("^[a-zA-Z][a-zA-Z0-9]*")}, // 0: identificador
        {2,  regex("^\\d+\\.\\d+")},          // 2: real (entero.entero+)
        {1,  regex("^\\d+")},                 // 1: entero (digito+)
        {11, regex("^(==|!=)")},              // 11: opIgualdad
        {7,  regex("^(<=|>=|<|>)")},          // 7: opRelac
        {9,  regex("^&&")},                   // 9: opAnd
        {8,  regex("^\\|\\|")},               // 8: opOr
        {10, regex("^!")},                    // 10: opNot
        {18, regex("^=")},                    // 18: = (asignacion)
        {5,  regex("^(\\+|\\-)")},            // 5: opSuma (+, -)
        {6,  regex("^(\\*|/)")},              // 6: opMul (*, /)
        {12, regex("^;")},                    // 12: ;
        {13, regex("^,")},                    // 13: ,
        {14, regex("^\\(")},                  // 14: (
        {15, regex("^\\)")},                  // 15: )
        {16, regex("^\\{")},                  // 16: {
        {17, regex("^\\}")},                  // 17: }
        {23, regex("^\\$")}                   // 23: $
    };

    regex ignorar("^\\s+"); // Regla para ignorar espacios, tabulaciones y saltos de línea

    vector<Token> lista_tokens;
    string cadena_restante = fuente;

    while (!cadena_restante.empty()) {
        smatch coincidencia;

        // Ignorar todos los espacios en blanco
        if (regex_search(cadena_restante, coincidencia, ignorar)) {
            cadena_restante = coincidencia.suffix().str();
            continue;
        }

        bool coincidencia_encontrada = false;

        // Probamos la cadena contra la lista de reglas
        for (const auto& regla : reglas) {
            if (regex_search(cadena_restante, coincidencia, regla.patron)) {
                lista_tokens.push_back({coincidencia.str(), regla.tipo});
                cadena_restante = coincidencia.suffix().str();
                coincidencia_encontrada = true;
                break; // Volver a evaluar desde el inicio de las reglas
            }
        }

        // Manejo de errores lexicos
        if (!coincidencia_encontrada && !cadena_restante.empty()) {
            cout << ">> ERROR LEXICO: Simbolo desconocido '" << cadena_restante[0] << "'\n";
            // Avanzamos 1 caracter para que el programa no entre en bucle
            cadena_restante = cadena_restante.substr(1);
        }
    }

    return lista_tokens;
}

// Prueba
int main() {
    string codigo_fuente = R"(
        int velocidad = 10;
        float gravedad = 9.81;

        if (velocidad <= 15 && gravedad != 0) {
            return velocidad + 5;
        } else {
            cadena_test = "alerta";
        }
        $
    )";

    // Analizador
    vector<Token> tokens = analizadorLexico(codigo_fuente);

    // Imprimir tabla
    cout << "=======================================\n";
    cout << left << setw(20) << "Lexema" << "| " << "Tipo (Columna LR1)\n";
    cout << "=======================================\n";

    for (const auto& t : tokens) {
        cout << left << setw(20) << t.lexema << "| " << t.tipo << "\n";
    }

    return 0;
}
