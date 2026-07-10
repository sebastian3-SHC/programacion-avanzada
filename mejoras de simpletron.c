#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>

#define MEMORY_SIZE 1000
#define PROGRAM_FILE "programa.simp"
#define SENTINEL 9999
#define WORD_LIMIT 99999.0
#define EPSILON 1e-9
#define MAX_STRING_LENGTH 99
#define LINE_SIZE 512


#define READ 10
#define WRITE 11
#define READ_STRING 12
#define WRITE_STRING 13
#define NEWLINE 14
#define READ_FLOAT 15
#define WRITE_FLOAT 16


#define LOAD 20
#define STORE 21


#define ADD 30
#define SUBTRACT 31
#define DIVIDE 32
#define MULTIPLY 33
#define MODULO 34
#define POWER 35

#define BRANCH 40
#define BRANCHNEG 41
#define BRANCHZERO 42
#define HALT 43

typedef struct
{
    double memory[MEMORY_SIZE];

    double accumulator;

    int instructionCounter;
    int instructionRegister;
    int operationCode;
    int operand;

    int running;
    int fatalError;

} Simpletron;


void limpiarBufferFinal(char texto[])
{
    int longitud = strlen(texto);

    while(longitud > 0 &&
         (texto[longitud - 1] == '\n' ||
          texto[longitud - 1] == '\r'))
    {
        texto[longitud - 1] = '\0';
        longitud--;
    }
}


int leerLinea(char buffer[], int tamano)
{
    if(fgets(buffer, tamano, stdin) == NULL)
    {
        return 0;
    }

    limpiarBufferFinal(buffer);

    return 1;
}


int convertirDouble(const char texto[], double *valor)
{
    char *final;
    double numero;

    errno = 0;

    numero = strtod(texto, &final);

    if(texto == final || errno == ERANGE)
    {
        return 0;
    }

    while(isspace((unsigned char)*final))
    {
        final++;
    }

    if(*final != '\0' || !isfinite(numero))
    {
        return 0;
    }

    *valor = numero;

    return 1;
}

int esEntero(double valor)
{
    return fabs(valor - round(valor)) < EPSILON;
}


int valorEnRango(double valor)
{
    return isfinite(valor) &&
           valor >= -WORD_LIMIT &&
           valor <= WORD_LIMIT;
}


int direccionValida(int direccion)
{
    return direccion >= 0 && direccion < MEMORY_SIZE;
}


void imprimirPalabra(double valor)
{
    if(esEntero(valor))
    {
        printf("%+011lld", (long long)llround(valor));
    }
    else
    {
        printf("%+011.3f", valor);
    }
}


void vaciadoMemoria(const Simpletron *simpletron)
{
    int fila;
    int columna;

    printf("\n\nREGISTROS:\n");

    printf("accumulator          ");
    imprimirPalabra(simpletron->accumulator);
    printf("\n");

    printf("instructionCounter      %03d\n",
           simpletron->instructionCounter);

    printf("instructionRegister   %+06d\n",
           simpletron->instructionRegister);

    printf("operationCode            %02d\n",
           simpletron->operationCode);

    printf("operand                 %03d\n",
           simpletron->operand);

    printf("\nMEMORIA:\n");

    printf("      ");

    for(columna = 0; columna < 10; columna++)
    {
        printf("%11d ", columna);
    }

    printf("\n");

    for(fila = 0; fila < MEMORY_SIZE; fila += 10)
    {
        printf("%03d  ", fila);

        for(columna = 0; columna < 10; columna++)
        {
            imprimirPalabra(
                simpletron->memory[fila + columna]
            );

            printf(" ");
        }

        printf("\n");
    }
}


void errorFatal(Simpletron *simpletron, const char mensaje[])
{
    printf("\n*** ERROR FATAL: %s ***\n", mensaje);

    printf(
        "*** La ejecucion de Simpletron "
        "termino anormalmente ***\n"
    );

    simpletron->fatalError = 1;
    simpletron->running = 0;
}


int cargarDesdeArchivo(Simpletron *simpletron,
                       const char nombreArchivo[])
{
    FILE *archivo;

    char linea[LINE_SIZE];
    char *inicio;

    int posicion = 0;
    int numeroLinea = 0;

    double valor;

    archivo = fopen(nombreArchivo, "r");

    if(archivo == NULL)
    {
        return 0;
    }

    while(fgets(linea, sizeof(linea), archivo) != NULL)
    {
        numeroLinea++;

        inicio = linea;

        while(isspace((unsigned char)*inicio))
        {
            inicio++;
        }

        if(*inicio == '\0' ||
           *inicio == '\n' ||
           *inicio == '#')
        {
            continue;
        }

        limpiarBufferFinal(inicio);

        if(!convertirDouble(inicio, &valor) ||
           !valorEnRango(valor))
        {
            printf(
                "*** Linea invalida en %s, linea %d: %s ***\n",
                nombreArchivo,
                numeroLinea,
                inicio
            );

            fclose(archivo);

            memset(
                simpletron->memory,
                0,
                sizeof(simpletron->memory)
            );

            return -1;
        }

        if(posicion >= MEMORY_SIZE)
        {
            printf(
                "*** El archivo contiene mas de %d palabras ***\n",
                MEMORY_SIZE
            );

            fclose(archivo);

            memset(
                simpletron->memory,
                0,
                sizeof(simpletron->memory)
            );

            return -1;
        }

        if(esEntero(valor) &&
           (long long)llround(valor) == SENTINEL)
        {
            break;
        }

        simpletron->memory[posicion] = valor;

        posicion++;
    }

    fclose(archivo);

    printf(
        "*** Programa cargado desde %s (%d palabras) ***\n",
        nombreArchivo,
        posicion
    );

    return 1;
}


void cargarInteractivo(Simpletron *simpletron)
{
    char linea[LINE_SIZE];

    int posicion = 0;

    double valor;

    printf("\n*** Bienvenido a Simpletron ***\n");

    printf(
        "*** Introduzca una instruccion o palabra "
        "de datos por linea. ***\n"
    );

    printf(
        "*** Escriba %d para terminar la carga. ***\n\n",
        SENTINEL
    );

    while(posicion < MEMORY_SIZE)
    {
        printf("%03d ? ", posicion);

        if(!leerLinea(linea, sizeof(linea)))
        {
            printf("\n*** Fin de entrada detectado ***\n");
            break;
        }

        if(!convertirDouble(linea, &valor) ||
           !valorEnRango(valor))
        {
            printf(
                "*** Entrada invalida. Use un numero "
                "entre -99999 y +99999. ***\n"
            );

            continue;
        }

        if(esEntero(valor) &&
           (long long)llround(valor) == SENTINEL)
        {
            break;
        }

        simpletron->memory[posicion] = valor;

        posicion++;
    }

    printf("\n*** Se termino de cargar el programa ***\n");
}


int leerEnteroEnRango(const char mensaje[],
                      long long minimo,
                      long long maximo,
                      long long *valor)
{
    char linea[LINE_SIZE];

    double numero;

    while(1)
    {
        printf("%s", mensaje);

        if(!leerLinea(linea, sizeof(linea)))
        {
            return 0;
        }

        if(convertirDouble(linea, &numero) &&
           esEntero(numero) &&
           numero >= minimo &&
           numero <= maximo)
        {
            *valor = (long long)llround(numero);

            return 1;
        }

        printf(
            "*** Introduzca un entero entre %lld y %lld. ***\n",
            minimo,
            maximo
        );
    }
}


int leerFlotanteEnRango(const char mensaje[],
                        double *valor)
{
    char linea[LINE_SIZE];

    double numero;

    while(1)
    {
        printf("%s", mensaje);

        if(!leerLinea(linea, sizeof(linea)))
        {
            return 0;
        }

        if(convertirDouble(linea, &numero) &&
           valorEnRango(numero))
        {
            *valor = numero;

            return 1;
        }

        printf(
            "*** Introduzca un numero entre "
            "-99999 y +99999. ***\n"
        );
    }
}


int guardarCadena(Simpletron *simpletron,
                  int direccionInicial)
{
    char texto[LINE_SIZE];

    int longitud;
    int i;
    int ascii;

    if(!direccionValida(direccionInicial))
    {
        errorFatal(
            simpletron,
            "Direccion inicial invalida para cadena"
        );

        return 0;
    }

    printf("? cadena: ");

    if(!leerLinea(texto, sizeof(texto)))
    {
        errorFatal(
            simpletron,
            "No fue posible leer la cadena"
        );

        return 0;
    }

    longitud = strlen(texto);

    if(longitud > MAX_STRING_LENGTH)
    {
        printf(
            "*** La cadena se recortara a %d caracteres. ***\n",
            MAX_STRING_LENGTH
        );

        longitud = MAX_STRING_LENGTH;

        texto[longitud] = '\0';
    }

    if(direccionInicial + longitud >= MEMORY_SIZE)
    {
        errorFatal(
            simpletron,
            "La cadena no cabe en la memoria"
        );

        return 0;
    }

    simpletron->memory[direccionInicial] =
        longitud * 1000;

    for(i = 0; i < longitud; i++)
    {
        ascii = (unsigned char)texto[i];

        simpletron->memory[
            direccionInicial + 1 + i
        ] = ((i + 1) * 1000) + ascii;
    }

    return 1;
}

int imprimirCadena(Simpletron *simpletron,
                   int direccionInicial)
{
    int longitud;
    int i;

    long long palabra;

    int posicionGuardada;
    int posicionEsperada;
    int ascii;

    if(!direccionValida(direccionInicial))
    {
        errorFatal(
            simpletron,
            "Direccion inicial invalida para cadena"
        );

        return 0;
    }

    if(!esEntero(
        simpletron->memory[direccionInicial]
    ))
    {
        errorFatal(
            simpletron,
            "Longitud invalida para cadena"
        );

        return 0;
    }

    longitud =
        (int)llround(
            simpletron->memory[direccionInicial]
        ) / 1000;

    if(longitud < 0 ||
       longitud > MAX_STRING_LENGTH ||
       direccionInicial + longitud >= MEMORY_SIZE)
    {
        errorFatal(
            simpletron,
            "Longitud de cadena invalida"
        );

        return 0;
    }

    for(i = 0; i < longitud; i++)
    {
        if(!esEntero(
            simpletron->memory[
                direccionInicial + 1 + i
            ]
        ))
        {
            errorFatal(
                simpletron,
                "Palabra no entera dentro de la cadena"
            );

            return 0;
        }

        palabra = llround(
            simpletron->memory[
                direccionInicial + 1 + i
            ]
        );

        posicionGuardada = palabra / 1000;
        ascii = palabra % 1000;

        posicionEsperada = i + 1;

        if(posicionGuardada != posicionEsperada ||
           ascii < 0 ||
           ascii > 255)
        {
            errorFatal(
                simpletron,
                "Formato ASCII de cadena invalido"
            );

            return 0;
        }

        putchar((unsigned char)ascii);
    }

    return 1;
}

int comprobarResultado(Simpletron *simpletron,
                       double resultado)
{
    if(!valorEnRango(resultado))
    {
        errorFatal(
            simpletron,
            "Desbordamiento del acumulador"
        );

        return 0;
    }

    simpletron->accumulator = resultado;

    return 1;
}


void ejecutar(Simpletron *simpletron)
{
    simpletron->running = 1;
    simpletron->fatalError = 0;

    printf(
        "\n*** Comienza la ejecucion del programa ***\n"
    );

    while(simpletron->running)
    {
        int avanzarContador = 1;

        double base;
        double exponente;
        double resultado;

        long long valorEntero;
        long long izquierda;
        long long derecha;

        if(!direccionValida(
            simpletron->instructionCounter
        ))
        {
            errorFatal(
                simpletron,
                "Contador de instrucciones fuera de rango"
            );

            break;
        }

        if(!esEntero(
            simpletron->memory[
                simpletron->instructionCounter
            ]
        ))
        {
            errorFatal(
                simpletron,
                "La instruccion actual no es un numero entero"
            );

            break;
        }

        simpletron->instructionRegister =
            (int)llround(
                simpletron->memory[
                    simpletron->instructionCounter
                ]
            );

        if(simpletron->instructionRegister < 0 ||
           simpletron->instructionRegister > 99999)
        {
            errorFatal(
                simpletron,
                "Palabra de instruccion fuera "
                "del rango 00000-99999"
            );

            break;
        }

        simpletron->operationCode =
            simpletron->instructionRegister / 1000;

        simpletron->operand =
            simpletron->instructionRegister % 1000;

        if(!direccionValida(simpletron->operand) &&
           simpletron->operationCode != NEWLINE &&
           simpletron->operationCode != HALT)
        {
            errorFatal(
                simpletron,
                "Operando fuera del rango 000-999"
            );

            break;
        }

        switch(simpletron->operationCode)
        {
            case READ:

                if(!leerEnteroEnRango(
                    "? entero: ",
                    -99999,
                    99999,
                    &valorEntero
                ))
                {
                    errorFatal(
                        simpletron,
                        "No fue posible leer el entero"
                    );

                    break;
                }

                simpletron->memory[
                    simpletron->operand
                ] = valorEntero;

                break;


            case WRITE:

                imprimirPalabra(
                    simpletron->memory[
                        simpletron->operand
                    ]
                );

                printf("\n");

                break;


            case READ_STRING:

                guardarCadena(
                    simpletron,
                    simpletron->operand
                );

                break;


            case WRITE_STRING:

                imprimirCadena(
                    simpletron,
                    simpletron->operand
                );

                break;


            case NEWLINE:

                printf("\n");

                break;


            case READ_FLOAT:

                if(!leerFlotanteEnRango(
                    "? flotante: ",
                    &resultado
                ))
                {
                    errorFatal(
                        simpletron,
                        "No fue posible leer el flotante"
                    );

                    break;
                }

                simpletron->memory[
                    simpletron->operand
                ] = resultado;

                break;


            case WRITE_FLOAT:

                printf(
                    "%.6f\n",
                    simpletron->memory[
                        simpletron->operand
                    ]
                );

                break;


            case LOAD:

                simpletron->accumulator =
                    simpletron->memory[
                        simpletron->operand
                    ];

                break;


            case STORE:

                simpletron->memory[
                    simpletron->operand
                ] = simpletron->accumulator;

                break;


            case ADD:

                comprobarResultado(
                    simpletron,
                    simpletron->accumulator +
                    simpletron->memory[
                        simpletron->operand
                    ]
                );

                break;


            case SUBTRACT:

                comprobarResultado(
                    simpletron,
                    simpletron->accumulator -
                    simpletron->memory[
                        simpletron->operand
                    ]
                );

                break;


            case DIVIDE:

                if(fabs(
                    simpletron->memory[
                        simpletron->operand
                    ]
                ) < EPSILON)
                {
                    errorFatal(
                        simpletron,
                        "Intento de dividir entre cero"
                    );
                }
                else
                {
                    comprobarResultado(
                        simpletron,
                        simpletron->accumulator /
                        simpletron->memory[
                            simpletron->operand
                        ]
                    );
                }

                break;


            case MULTIPLY:

                comprobarResultado(
                    simpletron,
                    simpletron->accumulator *
                    simpletron->memory[
                        simpletron->operand
                    ]
                );

                break;


            case MODULO:

                if(!esEntero(simpletron->accumulator) ||
                   !esEntero(
                       simpletron->memory[
                           simpletron->operand
                       ]
                   ))
                {
                    errorFatal(
                        simpletron,
                        "MODULO requiere operandos enteros"
                    );

                    break;
                }

                izquierda =
                    llround(simpletron->accumulator);

                derecha =
                    llround(
                        simpletron->memory[
                            simpletron->operand
                        ]
                    );

                if(derecha == 0)
                {
                    errorFatal(
                        simpletron,
                        "Intento de calcular modulo entre cero"
                    );

                    break;
                }

                comprobarResultado(
                    simpletron,
                    izquierda % derecha
                );

                break;


            case POWER:

                base = simpletron->accumulator;

                exponente =
                    simpletron->memory[
                        simpletron->operand
                    ];

                if(fabs(base) < EPSILON &&
                   exponente < 0)
                {
                    errorFatal(
                        simpletron,
                        "Cero no puede elevarse "
                        "a un exponente negativo"
                    );

                    break;
                }

                errno = 0;

                resultado = pow(base, exponente);

                if(errno == EDOM ||
                   errno == ERANGE ||
                   !isfinite(resultado))
                {
                    errorFatal(
                        simpletron,
                        "Resultado invalido "
                        "en exponenciacion"
                    );

                    break;
                }

                comprobarResultado(
                    simpletron,
                    resultado
                );

                break;


            case BRANCH:

                simpletron->instructionCounter =
                    simpletron->operand;

                avanzarContador = 0;

                break;


            case BRANCHNEG:

                if(simpletron->accumulator < 0)
                {
                    simpletron->instructionCounter =
                        simpletron->operand;

                    avanzarContador = 0;
                }

                break;


            case BRANCHZERO:

                if(fabs(
                    simpletron->accumulator
                ) < EPSILON)
                {
                    simpletron->instructionCounter =
                        simpletron->operand;

                    avanzarContador = 0;
                }

                break;


            case HALT:

                printf(
                    "\n*** Termino la ejecucion "
                    "de Simpletron ***\n"
                );

                simpletron->running = 0;
                avanzarContador = 0;

                break;


            default:

                errorFatal(
                    simpletron,
                    "Codigo de operacion invalido"
                );

                avanzarContador = 0;

                break;
        }

      
        if(simpletron->running &&
           avanzarContador)
        {
            simpletron->instructionCounter++;
        }
    }
}


int main(void)
{
    Simpletron simpletron = {0};

    int resultadoCarga;

    printf("*** SIMPLETRON MEJORADO ***\n");

    printf(
        "*** Memoria: %d posiciones "
        "(000-999) ***\n",
        MEMORY_SIZE
    );

    printf(
        "*** Formato de instruccion: "
        "OP de 2 digitos + direccion "
        "de 3 digitos ***\n\n"
    );

    
    resultadoCarga = cargarDesdeArchivo(
        &simpletron,
        PROGRAM_FILE
    );

   
    if(resultadoCarga == 0)
    {
        printf(
            "*** No se encontro %s; "
            "se usara carga interactiva. ***\n",
            PROGRAM_FILE
        );

        cargarInteractivo(&simpletron);
    }
    else if(resultadoCarga == -1)
    {
        printf(
            "*** El archivo es invalido; "
            "se usara carga interactiva. ***\n"
        );

        cargarInteractivo(&simpletron);
    }

    ejecutar(&simpletron);

    vaciadoMemoria(&simpletron);

    if(simpletron.fatalError)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}