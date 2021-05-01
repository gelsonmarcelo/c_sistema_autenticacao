#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

/**
 * Função para verificar se a senha cumpre com a política de senhas 
 */
int validaSenha(char *senha)
{
    printf("\nSua senha está sendo analisada de acordo com a política de criação de senhas...\n");
    //Contadores dos tipos de caracteres
    int contLetras = 0, contNumeros = 0, contEspeciais = 0;

    //Loop para passar pelos caracteres da senha
    for (int i = 0; i < strlen(senha); i++)
    {
        //Usando a função isalpha da biblioteca ctype.h, é possível verificar se o caractere é alfabético
        if (isalpha(senha[i]))
        {
            printf("\n%c é Alfabético.", senha[i]);
            contLetras++;
        }
        //Usando a função isalpha da biblioteca ctype.h, é possível verificar se o caractere é um digito
        else if (isdigit(senha[i]))
        {
            printf("\n%c é Numérico.", senha[i]);
            contNumeros++;
        }
        //Senão é um caractere especial ou outro tipo
        else
        {
            printf("\n> | %c | é um caractere especial/branco.", senha[i]);
        }
    }

    printf("\nValidação da senha finalizada...\n");

    return 0;
}

int main()
{
    setlocale(LC_ALL, "Portuguese");
    system("cls || clear");
    printf("\n*********************************************************************************\n_________________________________________________________________________________\n");
    printf("\n\t\t>> BEM-VINDO AO SISTEMA DE AUTENTICAÇÃO <<\n\n");
    char nome[50], sobrenome[50], senha[50], email[50], entrada;
    int op = 0, i = 0;
    // FILE *input;

    //Menu de opções
    while (1)
    {
        //Limpa o buffer do teclado para evitar que lixo de memória seja lido ao invés da entrada do usuário
        setbuf(stdin, NULL);

        printf("\n*********************************************************************************\n_________________________________________________________________________________\n");
        printf("Informe um número para escolher uma opção e pressione ENTER:");
        printf("\n[1] Login");
        printf("\n[2] Cadastro");
        printf("\n[3] Encerrar");
        printf("\n_________________________________________________________________________________\n*********************************************************************************\n");
        printf("\nInforme o número: ");
        scanf("%c", &entrada);
        system("cls || clear");

        op = atoi(&entrada);

        switch (op)
        {
        case 1:
            printf("\n*********************************************************************************\n_________________________________________________________________________________\n");
            printf("\n\t\t\t>> Login <<\n\n");
            printf("\n*********************************************************************************\n_________________________________________________________________________________\n");
            printf("Informe seu login:\n");
            scanf("%s", &nome);
            printf("Senha:\n");
            scanf("%s", &senha);
            break;
        case 2:
            printf("\n*********************************************************************************\n_________________________________________________________________________________\n");
            printf("\n\t\t\t>> CADASTRO <<\n\n");
            printf("\n*********************************************************************************\n_________________________________________________________________________________\n");
            printf("> Para começar, informe seu primeiro nome:\n_");
            scanf("%s", &nome);
            printf("> Agora, informe seu sobrenome:\n_");
            setbuf(stdin, NULL);
            scanf("%[^\n]s", &sobrenome);
            printf("> Informe seu e-mail:\n_");
            scanf("%s", &email);

            printf("> Digite uma senha:\n_");
            setbuf(stdin, NULL);
            scanf("%[^\n]s", &senha);

            system("cls || clear");

            if (validaSenha(senha) == 0)
            {
                printf("\nCadastro ralizado com sucesso!");
            }
            else
            {
                printf("\nA senha escolhida não atende à política de senhas do sistema!\nUsuário não cadastrado!");
            }

            printf("\nSeu nome completo é %s %s!\nSua senha é '%s'.\n", nome, sobrenome, senha);
            break;
        case 3:
            printf("\nFinalizando aplicação...\n");
            return 0;
        default:
            printf("\nOhh, você digitou uma opção inválida, tente novamente!\n");
            break;
        }
    }

    return 0;
}