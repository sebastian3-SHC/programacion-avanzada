#include <stdio.h>

#define READ 10
#define WRITE 11
#define LOAD 20
#define STORE 21
#define ADD 30
#define HALT 43

void vaciadomemoria(int accumulator,
                    int instructioncounter,
                    int instructionregister,
                    int operationcode,
                    int operand,
                    int memory[])
{
    int i, j;

    printf("\n\nREGISTROS:\n");
    printf("accumulator          %+05d\n", accumulator);
    printf("instructionCounter      %02d\n", instructioncounter);
    printf("instructionRegister  %+05d\n", instructionregister);
    printf("operationCode           %02d\n", operationcode);
    printf("operand                 %02d\n", operand);

    printf("\nMEMORIA:\n");
    printf("       0      1      2      3      4      5      6      7      8      9\n");

    for(i = 0; i < 100; i += 10)
    {
        printf("%02d  ", i);

        for(j = 0; j < 10; j++)
        {
            printf("%+05d ", memory[i + j]);
        }

        printf("\n");
    }
}

int main()
{
    int memory[100] = {0};

    int accumulator = 0;
    int instructioncounter = 0;
    int instructionregister = 0;
    int operationcode = 0;
    int operand = 0;

    int ejecutar = 1;

    /* Programa cargado como en Excel */

    memory[0] = 1007;   /* READ num1 */
    memory[1] = 1008;   /* READ num2 */
    memory[2] = 2007;   /* LOAD num1 */
    memory[3] = 3008;   /* ADD num2 */
    memory[4] = 2109;   /* STORE resultado */
    memory[5] = 1109;   /* WRITE resultado */
    memory[6] = 4300;   /* HALT */

    memory[7] = 0;      /* num1 */
    memory[8] = 0;      /* num2 */
    memory[9] = 0;      /* resultado */

    printf("*** SIMPLETRON ***\n");
    printf("*** Iniciando ejecucion ***\n\n");

    while(ejecutar)
    {
        instructionregister = memory[instructioncounter];

        operationcode = instructionregister / 100;
        operand = instructionregister % 100;

        switch(operationcode)
        {
            case READ:
                printf("? ");
                scanf("%d", &memory[operand]);
                instructioncounter++;
                break;

            case WRITE:
                printf("=>%d\n", memory[operand]);
                instructioncounter++;
                break;

            case LOAD:
                accumulator = memory[operand];
                instructioncounter++;
                break;

            case STORE:
                memory[operand] = accumulator;
                instructioncounter++;
                break;

            case ADD:
                accumulator += memory[operand];
                instructioncounter++;
                break;

            case HALT:
                printf("\n*** Terminó la ejecución de Simpletron ***\n");
                ejecutar = 0;
                break;

            default:
                printf("\n*** ERROR: Codigo de operacion invalido %02d ***\n",
                       operationcode);
                ejecutar = 0;
                break;
        }
    }

    vaciadomemoria(
        accumulator,
        instructioncounter,
        instructionregister,
        operationcode,
        operand,
        memory
    );

    return 0;
}