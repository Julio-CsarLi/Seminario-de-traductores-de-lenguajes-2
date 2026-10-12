import re

#Analizador lexico

class Token:
    def __init__(self, lexema, tipo):
        self.lexema = lexema
        self.tipo = tipo

    def __repr__(self):
        return f"Token(Tipo: {self.tipo:2} | Lexema: '{self.lexema}')"

def analizador_lexico(codigo):
    #Diccionario de reglas
    reglas = [
        (3, r'^"[^"]*"'),              # Cadenas
        (4, r'^(int|float|void)\b'),   # Tipos de dato
        (19, r'^if\b'), (20, r'^while\b'), (21, r'^return\b'), (22, r'^else\b'),
        (0, r'^[a-zA-Z][a-zA-Z0-9]*'), # Identificadores
        (2, r'^\d+\.\d+'),             # Flotantes
        (1, r'^\d+'),                  # Enteros
        (11, r'^(==|!=)'),             # Igualdad
        (7, r'^(<=|>=|<|>)'),          # Relacionales
        (9, r'^&&'), (8, r'^\|\|'), (10, r'^!'), # Logicos
        (18, r'^='),                   # Asignacion
        (5, r'^(\+|-)'), (6, r'^(\*|/)'), # Aritmeticos
        (12, r'^;'), (13, r'^,'),      # Delimitadores
        (14, r'^\('), (15, r'^\)'), (16, r'^\{'), (17, r'^\}'), 
        (23, r'^\$')                   # Fin de archivo
    ]

    tokens_encontrados = []
    cursor = 0

    while cursor < len(codigo):
        if codigo[cursor].isspace():
            cursor += 1
            continue
        
        match_encontrado = False
        texto_restante = codigo[cursor:]

        #Evaluar el texto
        for tipo, patron in reglas:
            coincidencia = re.match(patron, texto_restante)
            if coincidencia:
                lexema = coincidencia.group(0)
                tokens_encontrados.append(Token(lexema, tipo))
                cursor += len(lexema)
                match_encontrado = True
                break
        
        #Errores Lexicos
        if not match_encontrado:
            caracter_invalido = codigo[cursor]
            print(f"\n[!] ERROR LÉXICO: Carácter no reconocido '{caracter_invalido}' en la posición {cursor}.")
            return None 

    return tokens_encontrados

#Analizador sintactico

def cargar_tabla_lr(ruta_archivo):
    try:
        with open(ruta_archivo, 'r') as archivo:
            datos = archivo.read().split()
            
        n_reglas = int(datos[0])
        reglas = []
        idx = 1
        
        for _ in range(n_reglas):
            reglas.append({
                'id': int(datos[idx]),
                'lon': int(datos[idx+1]),
                'nombre': datos[idx+2]
            })
            idx += 3
            
        filas = int(datos[idx])
        columnas = int(datos[idx+1])
        idx += 2
        
        tabla = []
        for f in range(filas):
            fila_actual = []
            for c in range(columnas):
                fila_actual.append(int(datos[idx]))
                idx += 1
            tabla.append(fila_actual)
            
        return reglas, tabla
    except FileNotFoundError:
        print(f"[!] Error: No se encontró el archivo '{ruta_archivo}'.")
        return None, None

def analizador_sintactico(tokens, reglas, tabla):
    pila = [0]
    cursor = 0
    
    print(f"\n{'PILA':<90} | {'ENTRADA':<10} | {'ACCION'}")
    print("-" * 130)

    while True:
        estado_actual = pila[-1]
        token_actual = tokens[cursor]
        tipo_token = token_actual.tipo
        lexema_actual = token_actual.lexema
        
        accion = tabla[estado_actual][tipo_token]
        
        texto_pila = " ".join(str(x) for x in pila)
        print(f"{texto_pila:<90} | {lexema_actual:<10} | ", end="")

        if accion > 0:
            print(f"d{accion} (Desplazar)")
            pila.append(lexema_actual)
            pila.append(accion)
            cursor += 1
            
        elif accion < -1:
            id_regla = abs(accion) - 2
            regla = reglas[id_regla]
            
            print(f"r{id_regla} (Reducir por: {regla['nombre']})")
            
            elementos_a_sacar = regla['lon'] * 2
            for _ in range(elementos_a_sacar):
                pila.pop()
                
            estado_descubierto = pila[-1]
            salto = tabla[estado_descubierto][regla['id']]
            
            pila.append(regla['nombre'])
            pila.append(salto)
            
        elif accion == -1:
            #Aceptacion
            print("ACEPTAR")
            print("\nAnalizis sintactico exitoso")
            break
            
        else:
            #Error sintactico
            print("ERROR")
            print(f"\nError sintactico: Error en:  '{lexema_actual}'.")
            break

#Ejecucion del programa

if __name__ == "__main__":
    # 1. Se carga la configuracion del lenguaje
    reglas_gramaticales, matriz_lr = cargar_tabla_lr("compilador.lr")
    
    if matriz_lr:
        # 2. Definimos el codigo a evaluar
        nombre_archivo = "prueba1.txt"
        #codigo_fuente = "int main ( ) { int a ; a = 10 ; } $"
        
        try:
            with open(nombre_archivo, "r") as archivo:
                codigo_fuente = archivo.read()

            print("=== iniciando traductor ===")
            print(f"Leyendo archivo: {nombre_archivo}")
            print(f"Código fuente: {codigo_fuente}")
        
        # 3. Ejecucion del analisis lexico
            lista_de_tokens = analizador_lexico(codigo_fuente)
        
            if lista_de_tokens:
                # 4. Ejecucion del analisis sintactico
                analizador_sintactico(lista_de_tokens, reglas_gramaticales, matriz_lr)

        except FileNotFoundError:
            print(f"Error: No se encontro el archivo '{nombre_archivo}'.")