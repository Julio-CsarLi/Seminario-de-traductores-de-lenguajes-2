import re

def analizador(codigo):
    # Separamos el texto en lineas, ya que es mas facil
    lineas = codigo.split('\n')
    numero_linea = 0

    for linea in lineas:
        numero_linea += 1
        
        # Si la linea está vacia pasamos a la siguiente
        if linea.strip() == "":
            continue

        # Definimos un solo patron con 3 grupos
        # Grupo 1: (\d+\.\d+)            -> Numeros Reales
        # Grupo 2: ([a-zA-Z][a-zA-Z0-9]*) -> Identificadores
        # Grupo 3: ([^\s])                -> Errores
        patron = r'(\d+\.\d+)|([a-zA-Z][a-zA-Z0-9]*)|([^\s])'

        # Buscamos en la linea actual esos grupos
        for coincidencia in re.finditer(patron, linea):
            
            # Preguntamos que grupo fue el que se encontro y lo imprimimos
            if coincidencia.group(1):
                print(f"Línea {numero_linea:<2} | REAL          : {coincidencia.group(1)}")
                
            elif coincidencia.group(2):
                print(f"Línea {numero_linea:<2} | IDENTIFICADOR : {coincidencia.group(2)}")
                
            elif coincidencia.group(3):
                print(f"Línea {numero_linea:<2} | >> ERROR LÉXICO: {coincidencia.group(3)}")

# Probamos el codigo
codigo_de_prueba = codigo_de_prueba = """
posX = 15.5
jugador1 = 0.0
a
gravedad981
Z
100
.50
99.
3.14.16
1erNivel
salto_maximo
"""


analizador(codigo_de_prueba)