#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX 500
#define EPSILON 0.000001f


typedef struct
{
    char elementos[MAX];
    int tope;
} PilaCaracteres;


void inicializarPilaCaracteres(PilaCaracteres *p)
{
    p->tope = -1;
}


int caracteresVacia(PilaCaracteres *p)
{
    return p->tope == -1;
}


int caracteresLlena(PilaCaracteres *p)
{
    return p->tope == MAX - 1;
}


int pushCaracter(PilaCaracteres *p, char valor)
{
    if(caracteresLlena(p))
    {
        return 0;
    }

    p->tope++;
    p->elementos[p->tope] = valor;

    return 1;
}


char popCaracter(PilaCaracteres *p)
{
    if(caracteresVacia(p))
    {
        return '\0';
    }

    return p->elementos[p->tope--];
}


char topCaracter(PilaCaracteres *p)
{
    if(caracteresVacia(p))
    {
        return '\0';
    }

    return p->elementos[p->tope];
}



typedef struct
{
    float elementos[MAX];
    int tope;
} PilaFlotantes;


void inicializarPilaFlotantes(PilaFlotantes *p)
{
    p->tope = -1;
}


int flotantesVacia(PilaFlotantes *p)
{
    return p->tope == -1;
}


int flotantesLlena(PilaFlotantes *p)
{
    return p->tope == MAX - 1;
}


int pushFlotante(PilaFlotantes *p, float valor)
{
    if(flotantesLlena(p))
    {
        return 0;
    }

    p->tope++;
    p->elementos[p->tope] = valor;

    return 1;
}


float popFlotante(PilaFlotantes *p)
{
    if(flotantesVacia(p))
    {
        return 0.0f;
    }

    return p->elementos[p->tope--];
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


int agrupadoresCoinciden(char apertura, char cierre)
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
    PilaCaracteres pila;

    char simbolo;
    char apertura;

    int i;

    inicializarPilaCaracteres(&pila);

    for(i = 0; expresion[i] != '\0'; i++)
    {
        simbolo = expresion[i];

        if(esApertura(simbolo))
        {
            if(!pushCaracter(&pila, simbolo))
            {
                return 0;
            }
        }
        else if(esCierre(simbolo))
        {
            if(caracteresVacia(&pila))
            {
                return 0;
            }

            apertura = popCaracter(&pila);

            if(!agrupadoresCoinciden(apertura, simbolo))
            {
                return 0;
            }
        }
    }

    return caracteresVacia(&pila);
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
    int esperaOperando = 1;
    int tieneElementos = 0;
    int i;

    char simbolo;

    for(i = 0; expresion[i] != '\0'; i++)
    {
        simbolo = expresion[i];

        if(isspace((unsigned char)simbolo))
        {
            continue;
        }

        tieneElementos = 1;

        if(isdigit((unsigned char)simbolo))
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

    if(!tieneElementos || esperaOperando)
    {
        return 0;
    }

    return 1;
}



int convertirPostfijo(char expresion[], char resultado[])
{
    PilaCaracteres pila;

    char simbolo;
    char superior;

    int i;
    int j = 0;

    inicializarPilaCaracteres(&pila);

    for(i = 0; expresion[i] != '\0'; i++)
    {
        simbolo = expresion[i];

        if(isspace((unsigned char)simbolo))
        {
            continue;
        }

        if(isdigit((unsigned char)simbolo))
        {
            resultado[j] = simbolo;
            j++;
        }

        else if(esApertura(simbolo))
        {
            if(!pushCaracter(&pila, simbolo))
            {
                return 0;
            }
        }

     
        else if(esCierre(simbolo))
        {
            while(!caracteresVacia(&pila) &&
                  !esApertura(topCaracter(&pila)))
            {
                resultado[j] = popCaracter(&pila);
                j++;
            }

            if(caracteresVacia(&pila))
            {
                return 0;
            }

            superior = popCaracter(&pila);

            if(!agrupadoresCoinciden(superior, simbolo))
            {
                return 0;
            }
        }

        
        else if(esOperador(simbolo))
        {
            while(!caracteresVacia(&pila) &&
                  esOperador(topCaracter(&pila)) &&
                  prec(topCaracter(&pila), simbolo))
            {
                resultado[j] = popCaracter(&pila);
                j++;
            }

            if(!pushCaracter(&pila, simbolo))
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }

    while(!caracteresVacia(&pila))
    {
        superior = popCaracter(&pila);

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


void postfijo(char expresion[])
{
    char resultado[MAX];

    if(convertirPostfijo(expresion, resultado))
    {
        printf("%s\n", resultado);
    }
    else
    {
        printf("Error al convertir la expresion.\n");
    }
}


float convierte(char numero)
{
    return (float)(numero - '0');
}


float evalua(float opnd1, char sign, float opnd2)
{
    switch(sign)
    {
        case '+':
            return opnd1 + opnd2;

        case '-':
            return opnd1 - opnd2;

        case '*':
            return opnd1 * opnd2;

        case '/':
            return opnd1 / opnd2;

        case '^':
            return powf(opnd1, opnd2);

        default:
            return 0.0f;
    }
}


int evaluarPostfijo(char expresion[], float *resultado)
{
    PilaFlotantes pila;

    char simbolo;

    float opnd1;
    float opnd2;
    float valor;

    int i;

    inicializarPilaFlotantes(&pila);

    for(i = 0; expresion[i] != '\0'; i++)
    {
        simbolo = expresion[i];

        if(isspace((unsigned char)simbolo))
        {
            continue;
        }

        if(isdigit((unsigned char)simbolo))
        {
            valor = convierte(simbolo);

            if(!pushFlotante(&pila, valor))
            {
                printf("Error: la pila de operandos esta llena.\n");
                return 0;
            }
        }

        else if(esOperador(simbolo))
        {
            if(pila.tope < 1)
            {
                printf(
                    "Error: faltan operandos para el operador %c.\n",
                    simbolo
                );

                return 0;
            }

            opnd2 = popFlotante(&pila);
            opnd1 = popFlotante(&pila);

            if(simbolo == '/' && fabsf(opnd2) < EPSILON)
            {
                printf("Error: division entre cero.\n");
                return 0;
            }

            valor = evalua(opnd1, simbolo, opnd2);

            if(!isfinite(valor))
            {
                printf("Error: resultado numerico invalido.\n");
                return 0;
            }

            if(!pushFlotante(&pila, valor))
            {
                printf("Error al guardar el resultado parcial.\n");
                return 0;
            }
        }
        else
        {
            printf(
                "Error: simbolo invalido en la expresion postfija: %c\n",
                simbolo
            );

            return 0;
        }
    }

    if(pila.tope != 0)
    {
        printf(
            "Error: la expresion postfija tiene operandos sobrantes.\n"
        );

        return 0;
    }

    *resultado = popFlotante(&pila);

    return 1;
}


float evaluar(char expresion[])
{
    float resultado;

    if(!evaluarPostfijo(expresion, &resultado))
    {
        return NAN;
    }

    return resultado;
}



void eliminarSaltoLinea(char cadena[])
{
    cadena[strcspn(cadena, "\n")] = '\0';
}



void pruebasConversion(void)
{
    char entradas[][100] =
    {
        "(2+3)",
        "(2+3)*(4-1)",
        "((2+3)*4)",
        "2*(3+4)",
        "{2+[3*(4-1)]}"
    };

    char esperadas[][100] =
    {
        "23+",
        "23+41-*",
        "23+4*",
        "234+*",
        "2341-*+"
    };

    char obtenida[MAX];

    int cantidad;
    int correcto;
    int i;

    cantidad = sizeof(entradas) / sizeof(entradas[0]);

    printf("\nPRUEBAS DE CONVERSION\n");
    printf("-------------------------------------------------------------\n");
    printf("%-25s %-15s %-15s\n",
           "Infija",
           "Esperada",
           "Obtenida");
    printf("-------------------------------------------------------------\n");

    for(i = 0; i < cantidad; i++)
    {
        correcto = convertirPostfijo(
            entradas[i],
            obtenida
        );

        printf("%-25s %-15s ",
               entradas[i],
               esperadas[i]);

        if(correcto)
        {
            printf("%-15s", obtenida);
        }
        else
        {
            printf("%-15s", "ERROR");
        }

        if(correcto &&
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

    printf("-------------------------------------------------------------\n");
}



void pruebasEvaluacion(void)
{
    char entradas[][100] =
    {
        "234*5-+",
        "23+4*5-",
        "542-^5+",
        "52*2/",
        "32^32*-",
        "2345/*6++",
        "35+52-*43*-",
        "443/4547/*/+"
    };

    float esperadas[] =
    {
        9.0f,
        15.0f,
        30.0f,
        5.0f,
        3.0f,
        10.4f,
        12.0f,
        1.2666f
    };

    float resultado;

    int cantidad;
    int correcto;
    int i;

    cantidad = sizeof(esperadas) / sizeof(esperadas[0]);

    printf("\nPRUEBAS DE EVALUACION\n");
    printf("--------------------------------------------------\n");
    printf("%-20s %-15s %-15s\n",
           "Postfija",
           "Esperado",
           "Obtenido");
    printf("--------------------------------------------------\n");

    for(i = 0; i < cantidad; i++)
    {
        correcto = evaluarPostfijo(
            entradas[i],
            &resultado
        );

        printf("%-20s %-15.4f ",
               entradas[i],
               esperadas[i]);

        if(correcto)
        {
            printf("%-15.4f", resultado);
        }
        else
        {
            printf("%-15s", "ERROR");
        }

        if(correcto &&
           fabsf(resultado - esperadas[i]) < 0.001f)
        {
            printf(" CORRECTO");
        }
        else
        {
            printf(" ERROR");
        }

        printf("\n");
    }

    printf("--------------------------------------------------\n");
}



int main(void)
{
    char expresionInfija[MAX];
    char expresionPostfija[MAX];

    float resultado;

    printf("=====================================================\n");
    printf(" CONVERSION Y EVALUACION DE EXPRESIONES\n");
    printf("=====================================================\n");

    printf("\nOperandos permitidos:\n");
    printf("Numeros de un solo digito: 0 a 9\n");

    printf("\nOperadores permitidos:\n");
    printf("+  -  *  /  ^\n");

    printf("\nAgrupadores permitidos:\n");
    printf("( )  [ ]  { }\n");

    printf("\nEjemplo de entrada:\n");
    printf("(2+3)*4\n");

    printf("\nIntroduzca una expresion infija:\n");
    printf("> ");

    if(fgets(
        expresionInfija,
        sizeof(expresionInfija),
        stdin
    ) == NULL)
    {
        printf("\nNo fue posible leer la expresion.\n");
        return 1;
    }

    eliminarSaltoLinea(expresionInfija);

 
    if(!profundidad(expresionInfija))
    {
        printf("\nERROR\n");
        printf(
            "Los simbolos de agrupamiento "
            "no estan balanceados.\n"
        );

        return 1;
    }

   
    if(!estructuraValida(expresionInfija))
    {
        printf("\nERROR\n");
        printf(
            "La estructura de la expresion "
            "es incorrecta.\n"
        );

        printf(
            "Recuerde utilizar operandos "
            "de un solo digito.\n"
        );

        return 1;
    }


    if(!convertirPostfijo(
        expresionInfija,
        expresionPostfija
    ))
    {
        printf("\nNo fue posible convertir la expresion.\n");
        return 1;
    }

  
    if(!evaluarPostfijo(
        expresionPostfija,
        &resultado
    ))
    {
        printf("\nNo fue posible evaluar la expresion.\n");
        return 1;
    }

    printf("\nRESULTADOS\n");
    printf("-----------------------------------------------------\n");

    printf(
        "Expresion infija:          %s\n",
        expresionInfija
    );

    printf(
        "Expresion postfija:        %s\n",
        expresionPostfija
    );

    printf(
        "Profundidad de anidamiento: %d\n",
        profundidadMaxima(expresionInfija)
    );

    printf(
        "Resultado numerico:        %.4f\n",
        resultado
    );

    printf("-----------------------------------------------------\n");

    pruebasConversion();
    pruebasEvaluacion();

    return 0;
}