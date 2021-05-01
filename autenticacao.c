#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

int validaSenha(char *senha)
{
    int contAlfabetico;
    printf("\nSua senha está sendo analisada de acordo com a política de criação de senhas...\n");

    for (int i = 0; i < strlen(senha); i++)
    {
        if(isalpha(senha[i])){
            printf("\n%c é alpha", senha[i]);
        }
    }
    

    return 1;
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
            scanf("%s", &sobrenome);
            printf("> Informe seu e-mail:\n_");
            scanf("%s", &email);

            printf("> Digite uma senha:\n_");
            scanf("%s", &senha);

            system("cls || clear");

            if (validaSenha(senha) == 0)
            {
                printf("\nCadastro ralizado com sucesso!");
            }
            else
            {
                printf("\nSenha horrível vc escolheu! Não cadastrado!");
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