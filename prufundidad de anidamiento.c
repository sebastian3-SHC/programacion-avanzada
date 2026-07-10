#include <stdio.h>
#include <string.h>

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
    char simbolo;

    if(isEmpty(pila))
    {
        return '\0';
    }

    simbolo = pila->elementos[pila->tope];
    pila->tope--;

    return simbolo;
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

    char symb;
    char temp;

    int i;
    int valido = 1;

    inicializarPila(&pila);

    for(i = 0; expresion[i] != '\0'; i++)
    {
        symb = expresion[i];

        if(symb == '(' || symb == '[' || symb == '{')
        {
            if(!push(&pila, symb))
            {
                valido = 0;
                break;
            }
        }

        else if(symb == ')' || symb == ']' || symb == '}')
        {
            if(isEmpty(&pila))
            {
                valido = 0;
                break;
            }

            temp = pop(&pila);

            if(!simbolosCoinciden(temp, symb))
            {
                valido = 0;
                break;
            }
        }
    }

    if(!isEmpty(&pila))
    {
        valido = 0;
    }

    return valido;
}

int profundidadMaxima(char expresion[])
{
    int profundidadActual = 0;
    int maximaProfundidad = 0;
    int i;

    for(i = 0; expresion[i] != '\0'; i++)
    {
        if(expresion[i] == '(' ||
           expresion[i] == '[' ||
           expresion[i] == '{')
        {
            profundidadActual++;

            if(profundidadActual > maximaProfundidad)
            {
                maximaProfundidad = profundidadActual;
            }
        }
        else if(expresion[i] == ')' ||
                expresion[i] == ']' ||
                expresion[i] == '}')
        {
            profundidadActual--;
        }
    }

    return maximaProfundidad;
}


void eliminarSaltoLinea(char cadena[])
{
    cadena[strcspn(cadena, "\n")] = '\0';
}


void pruebasUnidad(void)
{
    char pruebas[][100] =
    {
        "A+B",
        "(A+B)",
        "((A+B)*C)",
        "(A*{B+C})",
        "(A*[B+C)}",
        "{A*[B+C))",
        "A+{B+[C-D]+(D/E)}",
        "A+{B+[C-D]+(D/E))",
        ")A+B(",
        "((A+B)"
    };

    int salidasEsperadas[] =
    {
        1,
        1,
        1,
        1,
        0,
        0,
        1,
        0,
        0,
        0
    };

    int cantidadPruebas;
    int resultado;
    int i;

    cantidadPruebas =
        sizeof(salidasEsperadas) /
        sizeof(salidasEsperadas[0]);

    printf("\nPRUEBAS DE UNIDAD\n");
    printf("-----------------------------------------------\n");
    printf("%-25s %-10s %-10s\n",
           "Expresion",
           "Esperado",
           "Obtenido");
    printf("-----------------------------------------------\n");

    for(i = 0; i < cantidadPruebas; i++)
    {
        resultado = profundidad(pruebas[i]);

        printf("%-25s %-10d %-10d",
               pruebas[i],
               salidasEsperadas[i],
               resultado);

        if(resultado == salidasEsperadas[i])
        {
            printf(" CORRECTO");
        }
        else
        {
            printf(" ERROR");
        }

        printf("\n");
    }

    printf("-----------------------------------------------\n");
}


int main(void)
{
    char expresion[MAX];

    int resultado;
    int maximaProfundidad;

    printf("=========================================\n");
    printf("  PROFUNDIDAD DE ANIDAMIENTO\n");
    printf("=========================================\n");

    printf("\nIntroduzca una expresion aritmetica:\n");
    printf("> ");

    if(fgets(expresion, sizeof(expresion), stdin) == NULL)
    {
        printf("\nNo fue posible leer la expresion.\n");
        return 1;
    }

    eliminarSaltoLinea(expresion);

    resultado = profundidad(expresion);

    if(resultado == 1)
    {
        maximaProfundidad = profundidadMaxima(expresion);

        printf("\nResultado: 1\n");
        printf("La expresion esta correctamente balanceada.\n");
        printf("Profundidad maxima de anidamiento: %d\n",
               maximaProfundidad);
    }
    else
    {
        printf("\nResultado: 0\n");
        printf("La expresion NO esta correctamente balanceada.\n");
    }

    pruebasUnidad();

    return 0;
}