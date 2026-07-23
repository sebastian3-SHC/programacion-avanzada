# Proyecto Final: Simpletron

## Información del proyecto

**Alumno:** Sebastian Huerta Cortina

**Carrera:** Ingeniería en Software

**Materia:** Programación Avanzada

**Universidad:** UPAEP

**Periodo:** Verano 2026

---

# Descripción general

Este proyecto consiste en el desarrollo de una computadora virtual denominada **Simpletron**, capaz de ejecutar programas escritos en el lenguaje de máquina **SML (Simpletron Machine Language)**.

Además del simulador, el proyecto integra un compilador que traduce programas escritos en el lenguaje **Simple** hacia instrucciones SML, permitiendo posteriormente su ejecución dentro del simulador.

Durante el desarrollo también se implementó un módulo para el procesamiento de expresiones aritméticas, incluyendo validación de símbolos de agrupación, conversión de expresiones infijas a postfijas y evaluación de expresiones postfijas.

Todo el proyecto fue desarrollado en lenguaje **C**, utilizando Visual Studio Code y GitHub para el control de versiones.

---

# Objetivos

Los objetivos principales del proyecto son:

- Implementar una computadora virtual Simpletron.
- Ejecutar programas escritos en lenguaje SML.
- Detectar errores de ejecución.
- Implementar nuevas instrucciones para el simulador.
- Procesar expresiones aritméticas.
- Diseñar un compilador para el lenguaje Simple.
- Traducir programas Simple a lenguaje SML.
- Ejecutar los programas compilados dentro del simulador.

---

# Características implementadas

## Simulador Simpletron

El simulador implementa:

- Carga de programas SML desde archivo.
- Ejecución del ciclo Buscar-Decodificar-Ejecutar.
- Manejo del acumulador.
- Manejo del contador de instrucciones.
- Registro de instrucción.
- Opcode.
- Operando.
- Memory Dump completo.
- Validación de instrucciones.
- Validación de direcciones.
- Detección de división entre cero.
- Detección de overflow.
- Validación del rango permitido.
- Finalización mediante HALT.

---

## Instrucciones SML soportadas

| Opcode | Instrucción | Descripción |
|---------|-------------|-------------|
|10|READ|Leer entero|
|11|WRITE|Escribir entero|
|12|READSTRING|Leer cadena|
|13|WRITESTRING|Escribir cadena|
|14|NEWLINE|Salto de línea|
|20|LOAD|Cargar acumulador|
|21|STORE|Guardar acumulador|
|30|ADD|Suma|
|31|SUBTRACT|Resta|
|32|DIVIDE|División|
|33|MULTIPLY|Multiplicación|
|34|MOD|Residuo|
|35|POWER|Potencia|
|40|BRANCH|Salto incondicional|
|41|BRANCHNEG|Salto si es negativo|
|42|BRANCHZERO|Salto si es cero|
|43|HALT|Finalizar programa|

---

# Evaluación de expresiones

El sistema implementa:

- Validación de paréntesis.
- Validación de corchetes.
- Validación de llaves.
- Conversión de expresiones infijas a postfijas.
- Evaluación de expresiones postfijas.
- Manejo de operandos de varios dígitos.
- Respeto de precedencia de operadores.
- Respeto de asociatividad de operadores.

---

# Lenguaje Simple

El compilador reconoce los siguientes comandos:

- rem
- input
- let
- print
- goto
- if
- end

También soporta operadores:

```
+
-
*
/
^
```

Y operadores relacionales:

```
<
<=
>
>=
==
!=
```

Las variables son de una sola letra y las constantes son enteras.

---

# Funcionamiento del compilador

El compilador se desarrolla mediante dos pasadas.

## Primera pasada

Durante esta etapa se realiza:

- Lectura del programa fuente.
- Construcción de la tabla de símbolos.
- Registro de líneas.
- Registro de variables.
- Registro de constantes.
- Generación de instrucciones preliminares.
- Marcado de referencias pendientes.

## Segunda pasada

En esta etapa:

- Se resuelven referencias hacia adelante.
- Se actualizan instrucciones incompletas.
- Se genera el programa SML final.
- Se produce un archivo ejecutable por el simulador.

---

# Estructura del proyecto

```
simpletron-final/

README.md

docs/
    requerimientos.md
    diseno.md

src/
    simpletron/
    expresiones/
    compilador/

programas/
    simple/
    sml/

pruebas/
    entradas/
    salidas/

evidencias/
    ejecuciones/
    git-branch/
```

---

# Requisitos

- GCC
- Visual Studio Code
- Git
- GitHub

---

# Compilación

## Simulador

```bash
gcc simpletron.c -o simpletron -lm
```

## Evaluador de expresiones

```bash
gcc Integracion.c -o expresiones -lm
```

## Compilador

```bash
gcc compilador.c -o compilador -lm
```

---

# Ejecución

## Ejecutar Simpletron

```bash
simpletron.exe
```

## Ejecutar el compilador

```bash
compilador.exe
```

---

# Ejemplo de programa Simple

```
10 input a
20 input b
30 let c = a + b
40 print c
50 end
```

---

# Ejemplo de programa SML

```
+1090
+1091
+2090
+3091
+2192
+1192
+4300
```

---

# Ejemplo de ejecución

Entrada:

```
10
20
```

Salida:

```
30
```

---

# Manejo de errores

El simulador detecta:

- División entre cero.
- Opcode inválido.
- Dirección inválida.
- Overflow del acumulador.
- Valores fuera del rango permitido.
- Archivo inexistente.
- Instrucciones inválidas.

El compilador detecta:

- Comandos inválidos.
- Variables inválidas.
- Expresiones incorrectas.
- Líneas inexistentes.
- Errores de sintaxis básicos.

---

# Programas de prueba

El proyecto incluye programas de prueba para:

- Suma.
- Resta.
- Multiplicación.
- División.
- Potencia.
- Residuo.
- Condicionales.
- Ciclos.
- Lectura de datos.
- Escritura de datos.
- Conversión de expresiones.
- Compilación de programas Simple.

---

# Control de versiones

Durante el desarrollo se utilizó Git y GitHub para:

- Crear ramas locales.
- Crear ramas remotas.
- Realizar commits.
- Fusionar ramas.
- Sincronizar cambios mediante push y pull.
- Mantener un historial del desarrollo.

Las evidencias se encuentran en la carpeta:

```
evidencias/git-branch/
```

---

# Archivos principales

| Archivo | Descripción |
|----------|-------------|
|Simpletron.c|Simulador Simpletron|
|Integracion.c|Evaluación de expresiones|
|Compilador.c|Compilador de Simple a SML|
|README.md|Documentación principal|

---

# Tecnologías utilizadas

- Lenguaje C
- Visual Studio Code
- Git
- GitHub

---


# Autor

**Sebastian Huerta Cortina**

Ingeniería en Software

Materia: **Programación Avanzada**

Universidad Popular Autónoma del Estado de Puebla (UPAEP)

Periodo: **Verano 2026**

---

# Licencia

Proyecto desarrollado con fines académicos para la materia de **Programación Avanzada** de la carrera de **Ingeniería en Software** en la **UPAEP**.
