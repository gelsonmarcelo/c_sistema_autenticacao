#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <sys/random.h>
//Tamanho definido do buffer
#define SALT_SIZE 32
#define PARAMETRO_CRYPT "$6$rounds=20000$%s$"
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

char nomeArquivo[] = "dados2.txt";

//Declaração da estrutura
struct usuario u;

void cadastrarUsuario();
char *tornarMaiusculo(char *string);
short int validarStringPadrao(char *string);
short int validarIdentificador(char *identificador);
short int validarSenha(char *senha);
int getProximoId();
short int autenticar();
void mostrarPolitica();
void gerarSal();
void criptografarSenha();
void setSal(char *sal);

/**
 * Estrutura para organização dos valores do usuário
 */
struct usuario
{
    int codigo;
    char nome[50];
    char sobrenome[50];
    char email[50];
    char identificador[50];
    char sal[SALT_SIZE + 1];
    char senha[50];
    char *senhaCriptografada;
};

/**
 * Função principal
 */
int main()
{
    setlocale(LC_ALL, "Portuguese");
    // system("cls || clear");
    printf("\n_________________________________________________________________________________");
    printf("\n*********************************************************************************\n");
    printf("\n\t\t>> OLÁ, PROGRAMA INICIADO COM SUCESSO! <<\n");
    char entrada = '0';
    int op = 0, i = 0;

    //Menu de opções
    do
    {
        //Limpa o buffer do teclado para evitar que lixo de memória seja lido ao invés da entrada do usuário
        setbuf(stdin, NULL);
        //Limpar a variável para evitar lixo de memória nas repetições
        entrada = '\0';

        printf("\n\n_________________________________________________________________________________");
        printf("\n*********************************************************************************\n");
        printf("> Informe um número para escolher uma opção e pressione ENTER:");
        printf("\n[1] Login");
        printf("\n[2] Cadastro");
        printf("\n[3] Ver a política de criação de identificadores e senhas");
        printf("\n[0] Encerrar programa");
        printf("\n_________________________________________________________________________________");
        printf("\n*********************************************************************************\n");
        printf("\n> Informe o número: ");
        // scanf("%s", &entrada);
        // system("cls || clear");

        //Converte o char para int para que possa ser verificado no switch
        // op = entrada - '0';
        op = 1;

        switch (op)
        {
        case 0:
            printf("\n# Sistema finalizado.\n");
            return 0;
        case 1:
            printf("\n_________________________________________________________________________________");
            printf("\n*********************************************************************************\n");
            printf("\n\t\t\t>> AUTENTICAÇÃO <<\n\n");
            printf("\n_________________________________________________________________________________");
            printf("\n*********************************************************************************\n");
            if(autenticar()){
                printf("\n\n\n\n\n\n\n\n\\t\tBem vindo usuário X\n");
            }
            break;
        case 2:
            printf("\n_________________________________________________________________________________");
            printf("\n*********************************************************************************\n");
            printf("\n\t\t\t>> CADASTRO <<\n\n");
            printf("\n_________________________________________________________________________________");
            printf("\n*********************************************************************************\n");
            cadastrarUsuario();
            break;
        case 3:
            printf("\n_________________________________________________________________________________");
            printf("\n*********************************************************************************\n");
            printf("\n\t\t>> POLÍTICA DE IDENTIFICADORES E SENHAS <<\n\n");
            printf("\n_________________________________________________________________________________");
            printf("\n*********************************************************************************\n");
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
 * Zera os dados da estrutura para reutilização
 */
void limparEstrutura()
{
    memset(&u.nome[0], 0, sizeof(u.nome));
    memset(&u.sobrenome[0], 0, sizeof(u.sobrenome));
    memset(&u.email[0], 0, sizeof(u.email));
    memset(&u.identificador[0], 0, sizeof(u.identificador));
    memset(&u.sal[0], 0, sizeof(u.sal));
    memset(&u.senha[0], 0, sizeof(u.senha));
    memset(&u.senhaCriptografada[0], 0, sizeof(u.senhaCriptografada));
    u.codigo = '0';
    // u.nome[0] = '\0';
    // u.sobrenome[0] = '\0';
    // u.email[0] = '\0';
    // u.identificador[0] = '\0';
    // u.sal[0] = '\0';
    // u.senha[0] = '\0';
    // u.senhaCriptografada[0] = '\0';
    printf("# A ESTRUTURA FOI LIMPA.\n");
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
    dados = fopen(nomeArquivo, "r");
    int id = 0;
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
        fscanf(dados, "%i ° %[^\n]s", &id, linha);
        printf("\n> %d + %s", id, linha);
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
    // ### - Remover essa função, posso setar que caracteres podem ser lidos na string do scanf com 'scanf(%[A−Z0−9], texto);'
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
    char linha[1000];

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
    // do
    // {
    //     printf("> Informe seu primeiro nome: ");
    //     setbuf(stdin, NULL);
    //     scanf("%s", &u.nome);
    //     scanf("%[^\n]s", &u.nome);
    // } while (!validarStringPadrao(u.nome));
    strcpy(u.nome, "MeuPrimeiroNome");
    tornarMaiusculo(u.nome);

    //loop validação sobrenome
    // do
    // {
    //     printf("> Agora, informe seu sobrenome: ");
    //     setbuf(stdin, NULL);
    //     scanf("%[^\n]s", &u.sobrenome);
    // } while (!validarStringPadrao(u.sobrenome));
    strcpy(u.sobrenome, "MeuSobrenome");
    tornarMaiusculo(u.sobrenome);

    //entrada do e-mail
    // printf("> Informe seu e-mail: ");
    // setbuf(stdin, NULL);
    // scanf("%[^\n]s", &u.email);
    strcpy(u.email, "email@hotmail.com");
    tornarMaiusculo(u.email);

    //entrada do identificador
    // do
    // {
    //     printf("> Crie seu login: ");
    //     setbuf(stdin, NULL);
    //     scanf("%[^\n]s", &u.identificador);
    // } while (!validarIdentificador(u.identificador));
    strcpy(u.identificador, "IdentificadorLegal");

    //loop da senha
    // do
    // {
    //     // printf("> Crie uma senha: ");
    //     setbuf(stdin, NULL);
    //     // scanf("%[^\n]s", &u.senha);
    //     strcpy(u.senha, getpass("> Crie uma senha: "));
    //     // printf("\n%s", u.senha);
    // } while (!validarSenha(u.senha));
    strcpy(u.senha, "53nh4_Forte!");

    // system("cls || clear");

    //Abrir o arquivo com parâmetro de append, não sobrescreve as informações, apenas adiciona.
    dados = fopen(nomeArquivo, "a");

    //Validação para caso o arquivo não possa ser aberto.
    if (dados == NULL)
    {
        printf("\n# ERRO FATAL - O arquivo de dados não pode ser aberto.");
        return;
    }

    gerarSal();

    criptografarSenha();

    //zerar a variável antes de começar a utilizá-la, para evitar que tenha valores prévios gravados
    linha[0] = '\0';
    //Concatenção de valores na variavel para jogar no arquivo somente uma string
    strcat(linha, codigoString);
    strcat(linha, separador);
    strcat(linha, u.identificador);
    strcat(linha, separador);
    strcat(linha, u.sal);
    strcat(linha, separador);
    strcat(linha, u.senhaCriptografada);
    strcat(linha, separador);
    strcat(linha, u.nome);
    strcat(linha, separador);
    strcat(linha, u.sobrenome);
    strcat(linha, separador);
    strcat(linha, u.email);
    strcat(linha, "\n");
    //Inserir a string do arquivo
    fputs(linha, dados);

    printf("\n# Cadastro ralizado com sucesso!\n> %s\n", linha);

    //Fecha o arquivo
    fclose(dados);

    // printf("\n\nSeu nome completo é %s %s!\nSua senha é '%s'.\n", u.nome, u.sobrenome, u.senha);
    limparEstrutura();
}

/**
 * Gerar valor de sal
 */
void gerarSal()
{
    //Ponteiro onde serão armazenados os caracteres gerados aleatoriamente
    char *buffer;
    //Para guardar a quantidade de caracteres gerados na função getrandom
    int retorno;

    //Reservar espaço de 32 bytes na memória
    buffer = malloc(SALT_SIZE);

    //Flag 0 para que a função utilize /dev/urandom - fonte de aleatoriedade do próprio Kernel.
    retorno = getrandom(buffer, SALT_SIZE, 0);
    //Verifica se a função retornou todo os bytes necessários
    if (retorno != SALT_SIZE)
    {
        perror("# Erro ao obter caracteres para criação do salt");
    }

    //Retirei o ':' estava causando problema na função crypt, fazendo-a retornar NULL quando aparecia no sal
    char lista_caracteres[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/,.-+=~`<>";

    // printf("Saída PRNG_v2 (unsigned char) = ");
    // for (int indice = 0; indice < SALT_SIZE; indice++)
    // {
    //     printf("%hhu ", buffer[indice]);
    // }
    printf("\n>Saída PRNG_v2 (char) = ");
    for (int i = 0; i < SALT_SIZE; i++)
    {
        //Seleciona 1 caractere da lista: converte o caractere do buffer para unsigned char (número) e faz
        //MOD quantidade de caracteres da lista, o resultado será o índice que contém o caractere usado.
        //Evitando assim que surjam caracteres que não podem ser interpretados pela codificação do SO.
        // printf("\n\nContador = %i \nSequencia do Sal: %s \nCaractere escolhido da lista: %c", i, u.sal, lista_caracteres[((unsigned char)buffer[i]) % (strlen(lista_caracteres))]);
        printf("%c ", lista_caracteres[((unsigned char)buffer[i]) % (strlen(lista_caracteres))]);
        u.sal[i] = lista_caracteres[((unsigned char)buffer[i]) % (strlen(lista_caracteres))];
    }
    u.sal[32] = '\0';
    printf("\n>Sal que foi para estrutura: %s", u.sal);
}

/**
 * Criptografar senha
 */
void criptografarSenha()
{

    //Reservar espaço de 120 bytes na memória
    u.senhaCriptografada = malloc(120);

    //Variavel que armazena o valor do parâmetro para função crypt
    char idSaltSenha[strlen(PARAMETRO_CRYPT) + SALT_SIZE];
    printf("\n>Tamanho alocado para o idSaltSenha: %d", strlen(PARAMETRO_CRYPT) + SALT_SIZE);

    //Incluindo o valor do sal no valor do parâmetro da função crypt e definindo na variavel idSaltSenha
    sprintf(idSaltSenha, PARAMETRO_CRYPT, u.sal);
    printf("\n>IdSaltSenha: size %d : %s", strlen(idSaltSenha), idSaltSenha);

    // ### - Verificar se o hash gerado está correto
    //Aqui, se a senha criptografada retornar NULL, fazer validação.
    u.senhaCriptografada = crypt(u.senha, idSaltSenha);
    printf("\n>>Salted Hash com SHA512 = %s\n", crypt(u.senha, idSaltSenha));
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
    // ### - Remover esse loop e selecionar a entrada no scanf(%[a-zA-Z.]s)
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
short int autenticar()
{
    char identificadorArquivo[50], saltArquivo[SALT_SIZE + 1], criptografiaArquivo[120], usuarioArquivo[50], sobrenomeArquivo[50], emailArquivo[50], temp[1000];
    int idArquivo = 0;
    printf("> Informe seus dados\n> Login: ");
    scanf("%s", &u.identificador);
    // printf("\n> Senha: ");
    // scanf("%s", &u.senha);
    strcpy(u.senha, getpass("\n> Senha: "));
    printf("\nLogin informado: %s | senha: %s", u.identificador, u.senha);

    //Abrir o arquivo com parâmetro de read, apenas lê.
    dados = fopen(nomeArquivo, "r");

    //Validação para caso o arquivo não possa ser aberto.
    if (dados == NULL)
    {
        printf("\n# ERRO FATAL - O arquivo de dados não pode ser aberto.");
        return 0;
    }

    while (!feof(dados))
    {
        //Lê as linhas até o final do arquivo
        fscanf(dados, "%d ° %s ° %s ° %s ° %[^\n]s", &idArquivo, identificadorArquivo, saltArquivo, criptografiaArquivo, temp);
        strcpy(u.sal, saltArquivo);
        criptografarSenha();

        printf("\n\n\n> ID arq: %d\nidentificador: %s\n\t> Sal: %s\n\t\t> Hash: %s\n\t\t\t> Temp: %s", idArquivo, identificadorArquivo, saltArquivo, criptografiaArquivo, temp);

        if (!strcmp(identificadorArquivo, u.identificador) && !strcmp(criptografiaArquivo, u.senhaCriptografada))
        {
            printf("\n\n\n\n\n\n\n# SUCESSO - Agora você está logado!\n\n\n\n\n\n");
            fclose(dados);
            limparEstrutura();
            return 1;
        }
        else
        {
            printf("\nNão é essa linha...");
        }
    }
    fclose(dados);
    limparEstrutura();
    printf("\n# FALHA - Usuário e/ou senha incorretos!\n");
    return 0;
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
