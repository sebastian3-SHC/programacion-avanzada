#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 500

typedef struct
{
    char elementos[MAX];
    int tope;
} Pila;



void inicializarPila(Pila *p)
{
    p->tope = -1;
}


int isEmpty(Pila *p)
{
    return p->tope == -1;
}


int isFull(Pila *p)
{
    return p->tope == MAX - 1;
}


int push(Pila *p, char simbolo)
{
    if(isFull(p))
    {
        return 0;
    }

    p->tope++;
    p->elementos[p->tope] = simbolo;

    return 1;
}


char pop(Pila *p)
{
    if(isEmpty(p))
    {
        return '\0';
    }

    return p->elementos[p->tope--];
}


char stackTop(Pila *p)
{
    if(isEmpty(p))
    {
        return '\0';
    }

    return p->elementos[p->tope];
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


int coinciden(char apertura, char cierre)
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
    Pila p;

    char simbolo;
    char apertura;

    int i;

    inicializarPila(&p);

    for(i = 0; expresion[i] != '\0'; i++)
    {
        simbolo = expresion[i];

        if(esApertura(simbolo))
        {
            if(!push(&p, simbolo))
            {
                return 0;
            }
        }
        else if(esCierre(simbolo))
        {
            if(isEmpty(&p))
            {
                return 0;
            }

            apertura = pop(&p);

            if(!coinciden(apertura, simbolo))
            {
                return 0;
            }
        }
    }

    return isEmpty(&p);
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
    int jer1;
    int jer2;

    if(esApertura(op1))
    {
        return 0;
    }

    if(esCierre(op2))
    {
        return 1;
    }

    jer1 = jerarquia(op1);
    jer2 = jerarquia(op2);

    return jer1 >= jer2;
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



int convertirPostfijo(char expresion[], char resultado[])
{
    Pila p;

    char symb;
    char topSymb;

    int i;
    int j = 0;

    inicializarPila(&p);

    for(i = 0; expresion[i] != '\0'; i++)
    {
        symb = expresion[i];

        
        if(isspace((unsigned char)symb))
        {
            continue;
        }

        if(isalnum((unsigned char)symb))
        {
            resultado[j] = symb;
            j++;
        }

        else if(esApertura(symb))
        {
            if(!push(&p, symb))
            {
                return 0;
            }
        }

    
        else if(esCierre(symb))
        {
            while(!isEmpty(&p) &&
                  !esApertura(stackTop(&p)))
            {
                resultado[j] = pop(&p);
                j++;
            }

            if(isEmpty(&p))
            {
                return 0;
            }

            topSymb = pop(&p);

            if(!coinciden(topSymb, symb))
            {
                return 0;
            }
        }

      
        else if(esOperador(symb))
        {
            while(!isEmpty(&p) &&
                  esOperador(stackTop(&p)) &&
                  prec(stackTop(&p), symb))
            {
                resultado[j] = pop(&p);
                j++;
            }

            if(!push(&p, symb))
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }

   
    while(!isEmpty(&p))
    {
        topSymb = pop(&p);

      
        if(esApertura(topSymb))
        {
            return 0;
        }

        resultado[j] = topSymb;
        j++;
    }

    resultado[j] = '\0';

    return 1;
}



void postfijo(char expresion[])
{
    char resultado[MAX];

    if(!profundidad(expresion))
    {
        printf("Error: los agrupadores no estan balanceados.\n");
        return;
    }

    if(!estructuraValida(expresion))
    {
        printf("Error: la estructura de la expresion es invalida.\n");
        return;
    }

    if(convertirPostfijo(expresion, resultado))
    {
        printf("%s\n", resultado);
    }
    else
    {
        printf("Error: no se pudo convertir la expresion.\n");
    }
}



void eliminarSaltoLinea(char cadena[])
{
    cadena[strcspn(cadena, "\n")] = '\0';
}



void pruebasUnidad(void)
{
    char entradas[][100] =
    {
        "(A+B)",
        "(A+B)-(C-D)",
        "((A+B)*(C-(D-E))^(F+G))",
        "(((A+B)*C)-D)",
        "(A+(B*(C-D)))",
        "((A+B)-(C*D))*E",
        "(A+B+C)+(C+B+A)",
        "((A+A)+((A+A)+A)+A)",
        "{A+[B*(C-D)]}",
        "A*(B+C)"
    };

    char esperadas[][100] =
    {
        "AB+",
        "AB+CD--",
        "AB+CDE--*FG+^",
        "AB+C*D-",
        "ABCD-*+",
        "AB+CD*-E*",
        "AB+C+CB+A++",
        "AA+AA+A++A+",
        "ABCD-*+",
        "ABC+*"
    };

    char obtenida[MAX];

    int cantidad;
    int resultado;
    int i;

    cantidad = sizeof(entradas) / sizeof(entradas[0]);

    printf("\n");
    printf("PRUEBAS DE UNIDAD\n");
    printf("-----------------------------------------------------------------\n");
    printf("%-30s %-20s %-20s\n",
           "Entrada",
           "Esperada",
           "Obtenida");
    printf("-----------------------------------------------------------------\n");

    for(i = 0; i < cantidad; i++)
    {
        resultado = convertirPostfijo(
            entradas[i],
            obtenida
        );

        printf("%-30s %-20s ",
               entradas[i],
               esperadas[i]);

        if(resultado)
        {
            printf("%-20s", obtenida);
        }
        else
        {
            printf("%-20s", "ERROR");
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

    printf("-----------------------------------------------------------------\n");
}



int main(void)
{
    char expresion[MAX];
    char resultado[MAX];

    printf("==================================================\n");
    printf(" CONVERSION A POSTFIJO - FASE II\n");
    printf("==================================================\n");

    printf("\nOperadores permitidos:\n");
    printf("+  -  *  /  ^\n");

    printf("\nAgrupadores permitidos:\n");
    printf("( )  [ ]  { }\n");

    printf("\nEscriba una expresion en formato infijo:\n");
    printf("> ");

    if(fgets(expresion, sizeof(expresion), stdin) == NULL)
    {
        printf("\nNo fue posible leer la expresion.\n");
        return 1;
    }

    eliminarSaltoLinea(expresion);

    if(!profundidad(expresion))
    {
        printf("\nLa expresion no es valida.\n");
        printf("Los simbolos de agrupamiento no estan balanceados.\n");

        pruebasUnidad();

        return 1;
    }

    if(!estructuraValida(expresion))
    {
        printf("\nLa expresion no es valida.\n");
        printf("La estructura de operandos y operadores es incorrecta.\n");

        pruebasUnidad();

        return 1;
    }

    if(!convertirPostfijo(expresion, resultado))
    {
        printf("\nNo fue posible convertir la expresion.\n");

        pruebasUnidad();

        return 1;
    }

    printf("\nRESULTADO\n");
    printf("--------------------------------------------------\n");
    printf("Expresion infija:   %s\n", expresion);
    printf("Expresion postfija: %s\n", resultado);
    printf("--------------------------------------------------\n");

    pruebasUnidad();

    return 0;
}