#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <iomanip> // Alinear columnas

using namespace std;

// Elementos de la pila

class ElementoPila {
public:
    virtual void muestra() {}
    virtual string obtenerCadena() { return ""; }
    virtual int getEstado() { return -1; }
};

class Terminal : public ElementoPila {
protected:
    string simbolo;
public:
    Terminal(string s) {
        this->simbolo = s;
    }
    void muestra() {
        cout << simbolo << " ";
    }
    string obtenerCadena() override {
        return simbolo + " ";
    }
};

class NoTerminal : public ElementoPila {
protected:
    string simbolo;
public:
    NoTerminal(string s) {
        this->simbolo = s;
    }
    void muestra() {
        cout << simbolo << " ";
    }
    string obtenerCadena() override {
        return simbolo + " ";
    }
};

class Estado : public ElementoPila {
protected:
    int estado;
public:
    Estado(int e) {
        this->estado = e;
    }
    void muestra() {
        cout << estado << " ";
    }
    string obtenerCadena() override {
        return to_string(estado) + " ";
    }
    int getEstado() {
        return estado;
    }
};

class Pila {
private:
    list<ElementoPila*> lista;
public:
    void push(ElementoPila* x) {
        lista.push_front(x);
    }

    ElementoPila* pop() {
        ElementoPila* x = *lista.begin();
        lista.erase(lista.begin());
        return x;
    }

    ElementoPila* top() {
        return *lista.begin();
    }

    void muestra() {
        list<ElementoPila*>::reverse_iterator it;
        for (it = lista.rbegin(); it != lista.rend(); it++) {
            (*it)->muestra();
        }
    }

    // Juntamos el texto de la pila en un string
    string obtenerCadenaPila() {
        string resultado = "";
        list<ElementoPila*>::reverse_iterator it;
        for (it = lista.rbegin(); it != lista.rend(); it++) {
            resultado += (*it)->obtenerCadena();
        }
        return resultado;
    }
};

// Analizador sintactico

string nombre_token(int token) {
    if (token == 0) return "id";
    if (token == 1) return "+";
    if (token == 2) return "$";
    return "";
}

int main() {
    vector<int> entrada = {0, 1, 0, 2};

// Matriz
    string tabla[5][4] = {
        {"d2", "",    "",     "1"},
        {"",   "",    "acc",  "" },
        {"",   "d3",  "",     "" },
        {"d4", "",    "",     "" },
        {"",   "",    "r1",   "" }
    };

    Pila pila;
    pila.push(new Estado(0));

    int cursor = 0;

    cout << "Simulacion con pila de objetos\n\n";

    while (true) {
        int estado_actual = pila.top()->getEstado();
        int token_actual = entrada[cursor];
        string accion = tabla[estado_actual][token_actual];

        // Imprimir
        string texto_pila = "Pila: " + pila.obtenerCadenaPila();

        // Reserva 30 espacios para la pila, si la pila mide menos, rellena con espacios blanco
        cout << left << setw(30) << texto_pila
             << "| Entrada: " << setw(5) << nombre_token(token_actual)
             << " | Accion: " << accion << "\n";

        if (accion == "") {
            cout << "\nError sintactico\n";
            break;
        }
        else if (accion == "acc") {
            cout << "\nLa cadena fue aceptada\n";
            break;
        }
        else if (accion == "d2") {
            pila.push(new Terminal(nombre_token(token_actual)));
            pila.push(new Estado(2));
            cursor++;
        }
        else if (accion == "d3") {
            pila.push(new Terminal(nombre_token(token_actual)));
            pila.push(new Estado(3));
            cursor++;
        }
        else if (accion == "d4") {
            pila.push(new Terminal(nombre_token(token_actual)));
            pila.push(new Estado(4));
            cursor++;
        }
        else if (accion == "r1") {
            for (int i = 0; i < 6; i++) {
                pila.pop();
            }

            int nuevo_estado = pila.top()->getEstado();
            string salto_texto = tabla[nuevo_estado][3];

            pila.push(new NoTerminal("E"));
            pila.push(new Estado(stoi(salto_texto)));
        }
    }

    return 0;
}
