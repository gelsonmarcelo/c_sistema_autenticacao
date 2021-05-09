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

//Ponteiro para o arquivo de dados
FILE *dados;

//Declaração da estrutura
struct usuario u;

void cadastrarUsuario();
char *tornarMaiusculo(char *string);
short int validarStringPadrao(char *string);
short int validarIdentificador(char *identificador);
short int validarSenha(char *senha);
int getProximoId();
void autenticar();
void mostrarPolitica();

/**
 * Função principal
 */
int main()
{
    setlocale(LC_ALL, "Portuguese");
    system("cls || clear");
    printf("\n*********************************************************************************");
    printf("\n_________________________________________________________________________________\n");
    printf("\n\t\t>> OLÁ, PROGRAMA INICIADO COM SUCESSO! <<\n\n");
    char entrada = '0';
    int op = 0, i = 0;

    //Menu de opções
    do
    {
        //Limpa o buffer do teclado para evitar que lixo de memória seja lido ao invés da entrada do usuário
        setbuf(stdin, NULL);

        printf("\n*********************************************************************************\n");
        printf("_________________________________________________________________________________\n");
        printf("> Informe um número para escolher uma opção e pressione ENTER:");
        printf("\n[1] Login");
        printf("\n[2] Cadastro");
        printf("\n[3] Ver a política de criação de identificadores e senhas");
        printf("\n[0] Encerrar programa");
        printf("\n*********************************************************************************\n");
        printf("_________________________________________________________________________________\n");
        printf("\n> Informe o número: ");
        scanf("%c", &entrada);
        system("cls || clear");

        //Converte o char para int para que possa ser verificado no switch
        op = entrada - '0';
        // op = 2;
        //### verificar porque nao está acessando o valor 3 na leitura de dados
        printf("%i '%c'", op, entrada);
        switch (op)
        {
        case 0:
            printf("\n# Sistema finalizado.\n");
            return 0;
        case 1:
            printf("\n*********************************************************************************\n");
            printf("_________________________________________________________________________________\n");
            printf("\n\t\t\t>> AUTENTICAÇÃO <<\n\n");
            printf("\n*********************************************************************************\n");
            printf("_________________________________________________________________________________\n");
            autenticar();
            break;
        case 2:
            printf("\n*********************************************************************************\n");
            printf("_________________________________________________________________________________\n");
            printf("\n\t\t\t>> CADASTRO <<\n\n");
            printf("\n*********************************************************************************\n");
            printf("_________________________________________________________________________________\n");
            cadastrarUsuario();
            break;
        case 3:
            printf("\n*********************************************************************************\n");
            printf("_________________________________________________________________________________\n");
            printf("\n\t\t\t>> POLÍTICA DE IDENTIFICADORES E SENHAS <<\n\n");
            printf("\n*********************************************************************************\n");
            printf("_________________________________________________________________________________\n");
            mostrarPolitica();
            break;
        default:
            printf("\n# OPÇÃO INVÁLIDA\n# Você digitou uma opção inválida, tente novamente!\n");
            break;
        }
    } while (1);

    return 0;
}

/**
 * Estrutura para organização dos valores do usuário
 */
struct usuario
{
    int codigo;
    char nome[50];
    char sobrenome[50];
    char email[30];
    char identificador[50];
    char senha[100];
};

/**
 * Zera os dados da estrutura para reutilização
 */
void limparEstrutura()
{
    u.codigo = '0';
    u.nome[0] = '\0';
    u.sobrenome[0] = '\0';
    u.email[0] = '\0';
    u.identificador[0] = '\0';
    u.senha[0] = '\0';
}

/**
 * Transforma a String para maiúscula, para facilitar comparações em dados
 * @return variavel string passada como parâmetro toda maiúscula
 */
char *tornarMaiusculo(char *string)
{
    int i = 0;
    //Passa por todos os caracteres da String
    for (i; string[i] != '\0'; i++)
    {
        //Converte o caractere para maiusculo e joga na mesma variavel
        string[i] = toupper(string[i]);
    }
    //Acrescenta o caractere nulo no final da string convertida
    string[i] = '\0';
    // printf("\n\n\n%s", string);
    return string;
}

/**
 * Busca no arquivo o último id usado e retorna o próximo ID a ser usado
 * @return valor do próximo ID a ser usado e 0 em caso de falha
 */
int getProximoId()
{
    dados = fopen("dados.txt", "r");
    int id = 0, x, y;
    char linha[1000];

    //Validação para caso o arquivo não possa ser aberto.
    if (dados == NULL)
    {
        printf("\n# FALHA NOS DADOS - O arquivo de dados não pode ser aberto.");
        return 0;
    }

    while (!feof(dados))
    {
        //Lê as linhas até o final do arquivo
        fscanf(dados, "%i ° %[^\n]s ° %[^\n]s ° %[^\n]s ° %[^\n]s", &id, &linha, &linha, &linha, &linha);
    }
    fclose(dados);

    // printf("\n%i", id);
    //O id lido por último é o ID do último usuário cadastrado
    return id + 1;
}

/**
 * Verifica se a string passada como parâmetro contem somente caracteres alfabéticos ou espaço
 * @return 0 caso string inválida e 1 caso a string seja válida
 */
short int validarStringPadrao(char *string)
{
    //Loop para passar pelos caracteres da string
    for (int i = 0; i < strlen(string); i++)
    {
        //Usando a função isalpha da biblioteca ctype.h, é possível verificar se o caractere é alfabético e minusculo
        if (!isalpha(string[i]) && !isspace(string[i]))
        {
            printf("\n# CARACTERE INVÁLIDO: por favor insira somente caracteres alfabéticos nesse campo.\n# '%c' não é alfabético ou espaço!\n", string[i]);
            return 0;
        }
    }
    return 1;
}

/**
 * Função para cadastrar novos usuários
 */
void cadastrarUsuario()
{
    //Declara variavel que vai unir todos os valores para inserir no arquivo de uma só vez
    char linha[200];

    //Variavel que será o separador dos campos no arquivo
    char separador[] = " ° ";

    //Variavel string que recebera o valor de código (inteiro), para utilizar na concatenação
    char codigoString[5];

    //Define o ID do usuário que está se cadastrando
    u.codigo = getProximoId();
    //Converte o valor inteiro para String, permitindo usá-la posteriormente na função strcat
    sprintf(codigoString, "%d", u.codigo);

    //Recolhendo informações do cadastro
    printf("\n\n> Forneça as informações necessárias para efetuar o cadastro:\n");

    //loop para validação do nome
    do
    {
        printf("> Informe seu primeiro nome: ");
        setbuf(stdin, NULL);
        scanf("%s", &u.nome);
        scanf("%[^\n]s", &u.nome);
    } while (!validarStringPadrao(u.nome));
    tornarMaiusculo(u.nome);

    //loop validação sobrenome
    do
    {
        printf("> Agora, informe seu sobrenome: ");
        setbuf(stdin, NULL);
        scanf("%[^\n]s", &u.sobrenome);
    } while (!validarStringPadrao(u.sobrenome));
    tornarMaiusculo(u.sobrenome);

    //entrada do e-mail
    printf("> Informe seu e-mail: ");
    setbuf(stdin, NULL);
    scanf("%[^\n]s", &u.email);
    tornarMaiusculo(u.email);

    //entrada do identificador
    do
    {
        printf("> Crie seu login: ");
        setbuf(stdin, NULL);
        scanf("%[^\n]s", &u.identificador);
    } while (!validarIdentificador(u.identificador));

    //loop da senha
    do
    {
        printf("> Crie uma senha: ");
        setbuf(stdin, NULL);
        scanf("%[^\n]s", &u.senha);
    } while (!validarSenha(u.senha));

    system("cls || clear");

    //Abrir o arquivo com parâmetro de append, não sobrescreve as informações, apenas adiciona.
    dados = fopen("dados.txt", "a");

    //Validação para caso o arquivo não possa ser aberto.
    if (dados == NULL)
    {
        printf("\n# ERRO FATAL - O arquivo de dados não pode ser aberto.");
        return;
    }

    //zerar a variável antes de começar a utilizá-la, para evitar que tenha valores prévios gravados
    linha[0] = '\0';
    //Concatenção de valores na variavel para jogar no arquivo somente uma string
    strcat(linha, codigoString);
    strcat(linha, separador);
    strcat(linha, u.identificador);
    strcat(linha, separador);
    strcat(linha, u.senha);
    strcat(linha, separador);
    strcat(linha, u.nome);
    strcat(linha, separador);
    strcat(linha, u.sobrenome);
    strcat(linha, separador);
    strcat(linha, u.email);
    strcat(linha, "\n");
    //Inserir a string do arquivo
    fputs(linha, dados);

    printf("\n# Cadastro ralizado com sucesso!\n");

    //Fecha o arquivo
    fclose(dados);

    // printf("\n\nSeu nome completo é %s %s!\nSua senha é '%s'.\n", u.nome, u.sobrenome, u.senha);
    limparEstrutura();
}

/**
 * Função para verificar se o identificador cumpre com a política
 * @return 1 em caso de identificador válido e 0 caso inválido.
 */
short int validarIdentificador(char *identificador)
{
    //Verifica tamanho do identificador
    if (strlen(identificador) < 5 || strlen(identificador) > 15)
    {
        printf("\n# IDENTIFICADOR INVÁLIDO - Não contém tamanho permitido (mínimo 5 e máximo 15)\n");
        return 0;
    }

    //Loop para passar pelos caracteres do identificador
    for (int i = 0; i < strlen(identificador); i++)
    {
        if (!isalpha(identificador[i]) && identificador[i] != '.')
        {
            printf("\n# IDENTIFICADOR INVÁLIDO - Contém caracteres não permitidos\n# O identificador pode conter somente caracteres alfanuméricos e ponto final.");
            return 0;
        }
        //Verificar se o identificador possui o nome, sobrenome ou email cadastrados.
        // if (&identificador[i] == &)
        // {
        // }
    }

    //Se o identificador for igual ao nome, sobrenome ou e-mail, é inválido.
    if (!strcmp(identificador, u.nome) || !strcmp(identificador, u.sobrenome) || !strcmp(identificador, u.email))
    {
        printf("\n# IDENTIFICADOR INVÁLIDO - Identificador não pode ser seu nome, sobrenome ou e-mail!\n");
        return 0;
    }

    return 1;
}

/** Função para verificar se a senha cumpre com a política de senhas
 * @return valor 1 em caso de senha válida e 0 caso inválida
 */
short int validarSenha(char *senha)
{
    //Contadores dos tipos de caracteres
    int contMinusculas = 0, contMaiusculas = 0, contNumeros = 0, contEspeciais = 0;
    printf("\n###");

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
            // printf("\n%c é Alfabético: minúsculo.", senha[i]);
            contMinusculas++;
        }
        //Usando a função isupper da biblioteca ctype.h, é possível verificar se o caractere é alfabético e maiúsculo
        else if (isupper(senha[i]))
        {
            // printf("\n%c é Alfabético: maiúsculo.", senha[i]);
            contMaiusculas++;
        }
        //Usando a função isalpha da biblioteca ctype.h, é possível verificar se o caractere é um digito
        else if (isdigit(senha[i]))
        {
            // printf("\n%c é Numérico.", senha[i]);
            contNumeros++;

            //Verifica se a senha contém +2 números ordenados em sequência (ascendente ou descendente)
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
        //Verificando se o caractere é especial ou espaço
        else if (ispunct(senha[i]) || isspace(senha[i]))
        {
            // printf("\n> | %c | é um caractere especial/espaço.", senha[i]);
            contEspeciais++;
        }
        else
        {
            printf("\n\n# SENHA INVÁLIDA - Sua senha contém caracteres que nao são nem alfanuméricos nem especiais ou espaço.");
            printf("\n# Verifique a digitação e tente novamente.\n# Caracteres permitidos:");
            printf("\n#\tEspeciais: ! \" # $ %% & ' ( ) * + , - . / : ; < = > ? @ [ \\ ] ^ _ ` { | } ~");
            printf("\n#\tNuméricos: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9");
            printf("\n#\tAlfabéticos: a b c d e f g h i j k l m n o p q r s t u v w x y z \n#\t\t\tA B C D E F G H I J K L M N O P Q R S T U V W X Y Z\n");
            return 0;
        }
    } //Fim do for que passa pelos cracteres da senha

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

    printf("\n# Sua senha está de acordo com a política e foi aprovada!\n");

    return 1;
}

/**
 * Relizar a autenticação do usuário
 */
void autenticar()
{
    printf("> Informe seus dados\n> Login: ");
    scanf("%s", &u.nome);
    printf("> Senha: ");
    scanf("%s", &u.senha);

    printf("# Agora você está logado!");
}

/**
 * Imprime a política com as regras para criação de identificadores e senhas
 */
void mostrarPolitica()
{
    printf("\n\tIDENTIFICADOR/LOGIN");
    printf("\n-Não pode ser utilizado nome, sobrenome ou email;");
    printf("\n-Pode conter somente caracteres alfanuméricos e ponto final;");
    printf("\n-Deve ter no mínimo 5 caracteres e no máximo 15;");
    printf("\n\n\tSENHA");
    printf("\n-Deve conter no mínimo 8 caracteres e no máximo 30;");
    printf("\n-Deve conter, no mínimo, 2 caracteres especiais;");
    printf("\n-Deve conter números e letras;");
    printf("\n-Deve conter pelo menos uma letra maiúscula e uma minúscula;");
    printf("\n-Não pode conter mais de 2 números ordenados em sequência;");
    printf("\n-Não pode conter números repetidos em sequência;");
    printf("\n-Não pode conter caracteres que não sejam alfanuméricos, especiais ou espaço.\n");
}
