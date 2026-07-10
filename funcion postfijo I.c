#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 500

typedef struct
{
    char elementos[MAX];
    int tope;
} Pila;


void inicializarPila(Pila *pila)
{
    pila->tope = -1;
}


int isEmpty(Pila *pila)
{
    return pila->tope == -1;
}


int isFull(Pila *pila)
{
    return pila->tope == MAX - 1;
}


int push(Pila *pila, char simbolo)
{
    if(isFull(pila))
    {
        return 0;
    }

    pila->tope++;
    pila->elementos[pila->tope] = simbolo;

    return 1;
}


char pop(Pila *pila)
{
    if(isEmpty(pila))
    {
        return '\0';
    }

    return pila->elementos[pila->tope--];
}


char stackTop(Pila *pila)
{
    if(isEmpty(pila))
    {
        return '\0';
    }

    return pila->elementos[pila->tope];
}



int esApertura(char simbolo)
{
    return simbolo == '(' ||
           simbolo == '[' ||
           simbolo == '{';
}


int esCierre(char simbolo)
{
    return simbolo == ')' ||
           simbolo == ']' ||
           simbolo == '}';
}


int simbolosCoinciden(char apertura, char cierre)
{
    if(apertura == '(' && cierre == ')')
    {
        return 1;
    }

    if(apertura == '[' && cierre == ']')
    {
        return 1;
    }

    if(apertura == '{' && cierre == '}')
    {
        return 1;
    }

    return 0;
}


int profundidad(char expresion[])
{
    Pila pila;

    char simbolo;
    char temporal;

    int i;

    inicializarPila(&pila);

    for(i = 0; expresion[i] != '\0'; i++)
    {
        simbolo = expresion[i];

        if(esApertura(simbolo))
        {
            if(!push(&pila, simbolo))
            {
                return 0;
            }
        }
        else if(esCierre(simbolo))
        {
            if(isEmpty(&pila))
            {
                return 0;
            }

            temporal = pop(&pila);

            if(!simbolosCoinciden(temporal, simbolo))
            {
                return 0;
            }
        }
    }

    return isEmpty(&pila);
}


int profundidadMaxima(char expresion[])
{
    int profundidadActual = 0;
    int maximaProfundidad = 0;
    int i;

    for(i = 0; expresion[i] != '\0'; i++)
    {
        if(esApertura(expresion[i]))
        {
            profundidadActual++;

            if(profundidadActual > maximaProfundidad)
            {
                maximaProfundidad = profundidadActual;
            }
        }
        else if(esCierre(expresion[i]))
        {
            profundidadActual--;
        }
    }

    return maximaProfundidad;
}



int esOperador(char simbolo)
{
    return simbolo == '+' ||
           simbolo == '-' ||
           simbolo == '*' ||
           simbolo == '/' ||
           simbolo == '^';
}


int jerarquia(char operador)
{
    if(operador == '^')
    {
        return 3;
    }

    if(operador == '*' || operador == '/')
    {
        return 2;
    }

    if(operador == '+' || operador == '-')
    {
        return 1;
    }

    return 0;
}


int prec(char op1, char op2)
{
    int jer1 = jerarquia(op1);
    int jer2 = jerarquia(op2);

    if(jer1 >= jer2)
    {
        return 1;
    }

    return 0;
}


int estructuraValida(char expresion[])
{
    int i;
    int esperaOperando = 1;
    int contieneElementos = 0;

    char simbolo;

    for(i = 0; expresion[i] != '\0'; i++)
    {
        simbolo = expresion[i];

        if(isspace((unsigned char)simbolo))
        {
            continue;
        }

        contieneElementos = 1;

        if(isalnum((unsigned char)simbolo))
        {
            if(!esperaOperando)
            {
                
                return 0;
            }

            esperaOperando = 0;
        }
        else if(esApertura(simbolo))
        {
            if(!esperaOperando)
            {
                
                return 0;
            }

            esperaOperando = 1;
        }
        else if(esCierre(simbolo))
        {
            if(esperaOperando)
            {
                
                return 0;
            }

            esperaOperando = 0;
        }
        else if(esOperador(simbolo))
        {
            if(esperaOperando)
            {
                
                return 0;
            }

            esperaOperando = 1;
        }
        else
        {
           
            return 0;
        }
    }

    if(!contieneElementos || esperaOperando)
    {
        return 0;
    }

    return 1;
}


int postfijo(char expresion[], char resultado[])
{
    Pila operadores;

    char simbolo;
    char superior;

    int i;
    int j = 0;

    inicializarPila(&operadores);

    for(i = 0; expresion[i] != '\0'; i++)
    {
        simbolo = expresion[i];

       
        if(isspace((unsigned char)simbolo))
        {
            continue;
        }

       
        if(isalnum((unsigned char)simbolo))
        {
            resultado[j] = simbolo;
            j++;
        }

       
        else if(esApertura(simbolo))
        {
            if(!push(&operadores, simbolo))
            {
                return 0;
            }
        }

     
        else if(esCierre(simbolo))
        {
            while(!isEmpty(&operadores) &&
                  !esApertura(stackTop(&operadores)))
            {
                resultado[j] = pop(&operadores);
                j++;
            }

            if(isEmpty(&operadores))
            {
                return 0;
            }

            superior = pop(&operadores);

            if(!simbolosCoinciden(superior, simbolo))
            {
                return 0;
            }
        }

  
        else if(esOperador(simbolo))
        {
            while(!isEmpty(&operadores) &&
                  esOperador(stackTop(&operadores)) &&
                  prec(stackTop(&operadores), simbolo))
            {
                resultado[j] = pop(&operadores);
                j++;
            }

            if(!push(&operadores, simbolo))
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }

  
    while(!isEmpty(&operadores))
    {
        superior = pop(&operadores);

        if(esApertura(superior))
        {
            return 0;
        }

        resultado[j] = superior;
        j++;
    }

    resultado[j] = '\0';

    return 1;
}


void eliminarSaltoLinea(char cadena[])
{
    cadena[strcspn(cadena, "\n")] = '\0';
}

void pruebasPostfijo(void)
{
    char entradas[][100] =
    {
        "A+B*C",
        "A*B+C",
        "A+B*C-D",
        "A^B*C-D",
        "A-B*C^D",
        "A^B^C^D",
        "(A+B)*(C-D)",
        "A*(B+C)",
        "A+{B*[C-D]}",
        "((A+B)*C)"
    };

    char esperadas[][100] =
    {
        "ABC*+",
        "AB*C+",
        "ABC*+D-",
        "AB^C*D-",
        "ABCD^*-",
        "AB^C^D^",
        "AB+CD-*",
        "ABC+*",
        "ABCD-*+",
        "AB+C*"
    };

    char obtenida[MAX];

    int cantidad;
    int resultado;
    int i;

    cantidad = sizeof(entradas) / sizeof(entradas[0]);

    printf("\n");
    printf("PRUEBAS DE UNIDAD\n");
    printf("------------------------------------------------------------\n");
    printf("%-22s %-18s %-18s\n",
           "Entrada",
           "Esperada",
           "Obtenida");
    printf("------------------------------------------------------------\n");

    for(i = 0; i < cantidad; i++)
    {
        resultado = postfijo(entradas[i], obtenida);

        printf("%-22s %-18s ",
               entradas[i],
               esperadas[i]);

        if(resultado)
        {
            printf("%-18s", obtenida);
        }
        else
        {
            printf("%-18s", "ERROR");
        }

        if(resultado &&
           strcmp(obtenida, esperadas[i]) == 0)
        {
            printf(" CORRECTO");
        }
        else
        {
            printf(" ERROR");
        }

        printf("\n");
    }

    printf("------------------------------------------------------------\n");
}



int main(void)
{
    char expresion[MAX];
    char expresionPostfija[MAX];

    int balanceada;
    int estructura;
    int conversion;
    int maximaProfundidad;

    printf("==============================================\n");
    printf(" CONVERSION DE EXPRESIONES A POSTFIJO\n");
    printf("==============================================\n");

    printf("\nOperadores permitidos:\n");
    printf("+  -  *  /  ^\n");

    printf("\nAgrupadores permitidos:\n");
    printf("( )  [ ]  { }\n");

    printf("\nIntroduzca una expresion infija:\n");
    printf("> ");

    if(fgets(expresion, sizeof(expresion), stdin) == NULL)
    {
        printf("\nNo fue posible leer la expresion.\n");
        return 1;
    }

    eliminarSaltoLinea(expresion);

    balanceada = profundidad(expresion);

    if(!balanceada)
    {
        printf("\nResultado: expresión inválida.\n");
        printf("Los símbolos de agrupamiento no están balanceados.\n");

        pruebasPostfijo();

        return 1;
    }

    estructura = estructuraValida(expresion);

    if(!estructura)
    {
        printf("\nResultado: expresión inválida.\n");
        printf("La estructura de operandos y operadores es incorrecta.\n");

        pruebasPostfijo();

        return 1;
    }

    maximaProfundidad = profundidadMaxima(expresion);

    conversion = postfijo(
        expresion,
        expresionPostfija
    );

    if(!conversion)
    {
        printf("\nNo fue posible convertir la expresión.\n");

        pruebasPostfijo();

        return 1;
    }

    printf("\nRESULTADOS\n");
    printf("----------------------------------------------\n");
    printf("Expresion infija:   %s\n", expresion);
    printf("Expresion postfija: %s\n", expresionPostfija);
    printf("Expresion válida:   1\n");
    printf("Profundidad maxima: %d\n", maximaProfundidad);
    printf("----------------------------------------------\n");

    pruebasPostfijo();

    return 0;
}