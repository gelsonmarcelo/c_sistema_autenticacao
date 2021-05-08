#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

/**
* - Identificador
* Para criação do identificador, não pode ser utilizado nome, sobrenome ou email;
* Pode conter somente caracteres alfanuméricos e ponto final;
* Deve ter no mínimo 5 caracteres e no máximo 15;
* - Senha
* Deve conter no mínimo 8 caracteres e no máximo 30;
* Deve conter, no mínimo, 2 caracteres especiais;
* Deve conter números e letras;
* Deve conter pelo menos uma letra maiúscula e uma minúscula;
* Não pode conter mais de 2 números ordenados em sequência;
* Não pode conter números repetidos em sequência;
* Não pode conter caracteres que não sejam alfanuméricos, caracteres especiais ou espaço.
**/

/**
 * Função para verificar se o identificador cumpre com a política
 */
int validaIdentificador(char *id, char *nome, char *sobrenome, char *email)
{
    printf("\nSeu identificador está sendo analisada de acordo com a política de criação de identificadores...\n");

    //Verifica tamanho do identificador
    if (strlen(id) < 5 || strlen(id) > 15)
    {
        printf("\n# IDENTIFICADOR INVÁLIDO - Não contém tamanho permitido (mínimo 5 e máximo 15)\n");
        return 0;
    }

    //Loop para passar pelos caracteres do identificador
    for (int i = 0; i < strlen(id); i++)
    {
        if (isalpha(id[i]))
        {
            printf("\n%c é Alfabético.", id[i]);
        }else if(id[i] == '.'){
            printf("\n%c é Ponto.", id[i]);
        }else{
            printf("\n# IDENTIFICADOR INVÁLIDO - Contém caracteres não permitidos\n");
        }

        if(strcmp(id, nome) == 0){

        }
    }
}
/**
 * Função para verificar se a senha cumpre com a política de senhas 
 */
int validaSenha(char *senha)
{
    printf("\nSua senha está sendo analisada de acordo com a política de criação de senhas...\n");
    //Contadores dos tipos de caracteres
    int contMinusculas = 0, contMaiusculas = 0, contNumeros = 0, contEspeciais = 0;

    //Verifica tamanho da senha
    if (strlen(senha) < 8 || strlen(senha) > 30)
    {
        printf("\n# SENHA INVÁLIDA - Não contém tamanho permitido (mínimo 8 e máximo 30)\n");
        return 0;
    }

    //Loop para passar pelos caracteres da senha
    for (int i = 0; i < strlen(senha); i++)
    {
        //Sequencia de IFs para verificar cada caractere da senha
        //Usando a função islower da biblioteca ctype.h, é possível verificar se o caractere é alfabético e minusculo
        if (islower(senha[i]))
        {
            printf("\n%c é Alfabético: minúsculo.", senha[i]);
            contMinusculas++;
        }
        //Usando a função isupper da biblioteca ctype.h, é possível verificar se o caractere é alfabético e maiúsculo
        else if (isupper(senha[i]))
        {
            printf("\n%c é Alfabético: maiúsculo.", senha[i]);
            contMaiusculas++;
        }
        //Usando a função isalpha da biblioteca ctype.h, é possível verificar se o caractere é um digito
        else if (isdigit(senha[i]))
        {
            printf("\n%c é Numérico.", senha[i]);
            contNumeros++;

            //Verifica se a senha contém números ordenados em sequência (ascendente ou descendente)
            if (((senha[i] - '0') + 1 == senha[i + 1] - '0' && (senha[i] - '0') + 2 == senha[i + 2] - '0') || ((senha[i] - '1') == senha[i + 1] - '0' && (senha[i] - '2') == senha[i + 2] - '0'))
            {
                printf("\n# SENHA INVÁLIDA - | %c | faz parte de números ordenados em sequência\n", senha[i]);
                return 0;
            }

            //Verifica se a senha contém números repetidos em sequência
            if (senha[i] == senha[i + 1])
            {
                printf("\n# SENHA INVÁLIDA - | %c | faz parte de números repetidos em sequência\n", senha[i]);
                return 0;
            }
        }
        //Senão é um caractere especial ou outro tipo
        else if (ispunct(senha[i]) || isspace(senha[i]))
        {
            printf("\n> | %c | é um caractere especial/espaço.", senha[i]);
            contEspeciais++;
        }
        else
        {
            printf("\n# SENHA INVÁLIDA - Sua senha contém caracteres que nao são nem alfanuméricos nem especiais ou espaço.");
            printf("\n# Verifique a digitação e tente novamente.\n# Caracteres permitidos:");
            printf("\n#\tEspeciais: ! \" # $ %% & ' ( ) * + , - . / : ; < = > ? @ [ \\ ] ^ _ ` { | } ~");
            printf("\n#\tNuméricos: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9");
            printf("\n#\tAlfabéticos: a b c d e f g h i j k l m n o p q r s t u v w x y z \n#\t\t\tA B C D E F G H I J K L M N O P Q R S T U V W X Y Z");
            return 0;
        }

    } //Fim do for que passa pela senha

    //Valida a quantidade de caracteres especiais
    if (contEspeciais < 2)
    {
        printf("\n# SENHA INVÁLIDA - não contém caracteres especiais suficientes\n");
        return 0;
    }
    //Verifica se contém números e letras
    if ((contMinusculas + contMaiusculas) == 0 || contNumeros == 0)
    {
        printf("\n# SENHA INVÁLIDA - não contém letras e números\n");
        return 0;
    }
    //Verifica se contém minúsculas
    if (contMinusculas == 0)
    {
        printf("\n# SENHA INVÁLIDA - não contém qualquer letra minúscula\n");
        return 0;
    }
    //Verifica se contém maiúsculas
    if (contMaiusculas == 0)
    {
        printf("\n# SENHA INVÁLIDA - não contém qualquer letra maiúscula\n");
        return 0;
    }

    printf("\nValidação da senha finalizada...\n");

    return 1;
}

int main()
{
    setlocale(LC_ALL, "Portuguese");
    system("cls || clear");
    printf("\n*********************************************************************************\n_________________________________________________________________________________\n");
    printf("\n\t\t>> BEM-VINDO AO SISTEMA DE AUTENTICAÇÃO <<\n\n");
    char identificador[50], nome[50], sobrenome[50], senha[100], email[50], entrada, linha[200], separador[] = " ° ";
    int op = 0, i = 0;
    FILE *dados;

    //Menu de opções
    while (1)
    {
        //Limpa o buffer do teclado para evitar que lixo de memória seja lido ao invés da entrada do usuário
        setbuf(stdin, NULL);

        // printf("\n*********************************************************************************\n_________________________________________________________________________________\n");
        // printf("Informe um número para escolher uma opção e pressione ENTER:");
        // printf("\n[1] Login");
        // printf("\n[2] Cadastro");
        // printf("\n[3] Encerrar");
        // printf("\n_________________________________________________________________________________\n*********************************************************************************\n");
        // printf("\nInforme o número: ");
        // scanf("%c", &entrada);
        // system("cls || clear");

        //Converte o char para int para que possa ser verificado no Switch
        // op = entrada-'0';
        op = 2;

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
            //### - Fazer validação para que o usuário digite apenas caracteres válidos nessas informações.
            printf("> Para começar, informe seu primeiro nome:\n_");
            scanf("%[^\n]s", &nome);
            printf("> Agora, informe seu sobrenome:\n_");
            setbuf(stdin, NULL);
            scanf("%[^\n]s", &sobrenome);
            printf("> Informe seu e-mail:\n_");
            scanf("%s", &email);

            printf("> Digite uma senha:\n_");
            setbuf(stdin, NULL);
            scanf("%[^\n]s", &senha);

            system("cls || clear");

            if (validaSenha(senha) == 1)
            {
                //Abrir o arquivo com parâmetro de append, ou seja, não sobrescreve as informações, apenas adiciona.
                dados = fopen("dados.txt", "a");

                //Validação para caso o arquivo não possa ser aberto.
                if (dados == NULL)
                {
                    printf("\n# FALHA NOS DADOS - O arquivo de dados não pode ser aberto.");
                    break;
                }
                else
                {
                    //zerar a variável antes de começar a utilizá-la, para evitar que tenha valores prévios gravados
                    linha[0] = '\0';
                    //Concatenção de valores na variavel para jogar no arquivo somente uma string
                    strcat(linha,"\n");
                    strcat(linha, identificador);
                    strcat(linha, separador);
                    strcat(linha, senha);
                    strcat(linha, separador);
                    strcat(linha, nome);
                    strcat(linha, separador);
                    strcat(linha, sobrenome);
                    strcat(linha, separador);
                    strcat(linha, email);
                    //Inserir a string do arquivo
                    fputs(linha, dados);
                    printf("\nCadastro ralizado com sucesso!");
                }

                fclose(dados);
            }
            else
            {
                printf("\nA senha escolhida não atende à política de senhas do sistema!\nUsuário não cadastrado!");
            }

            printf("\n\nSeu nome completo é %s %s!\nSua senha é '%s'.\n", nome, sobrenome, senha);
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