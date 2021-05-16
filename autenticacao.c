#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <sys/random.h>
#include <errno.h>

//Tamanho definido do sal
#define SALT_SIZE 16
//Tamanho máximo das strings de leitura
#define MAX 2048
//Parâmetro da função crypt
#define PARAMETRO_CRYPT "$6$rounds=20000$%s$"
//Variavel que será o separador dos campos no arquivo
#define SEPARADOR " | "

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
* Não pode conter mais de 2 números repetidos em sequência;
* Não pode conter caracteres que não sejam alfanuméricos, caracteres especiais ou espaço.
**/

//Ponteiro para o arquivo de dados
FILE *dados;

char nomeArquivo[] = "dados.txt";

//Declaração da estrutura
struct Usuario u;

/*Declaração das funções*/

int pegarProximoId();
void cadastrarUsuario();
short int autenticar();
void mostrarPolitica();
char *alternarCapitalLetras(char *string, int flag);
short int validarStringPadrao(char *string);
short int validarStringEmail(char *string);
short int validarIdentificador(char *identificador);
short int validarSenha(char *senha);
void gerarSalt();
void criptografarSenha();
void limparEstrutura();
void areaLogada();
void imprimirDados();
void excluirDados();
void imprimirDecoracao();

/**
 * Estrutura para organização dos dados do usuário
 */
struct Usuario
{
    int codigo;
    char nome[51];
    char sobrenome[51];
    char email[51];
    char identificador[16];
    char salt[SALT_SIZE + 1];
    char senha[31];
    char *senhaCriptografada;
    char linhaUsuario[MAX];
};

/**
 * Função principal
 */
int main()
{
    setlocale(LC_ALL, "Portuguese");

    //Verifica se o arquivo pode ser criado/utilizado
    dados = fopen(nomeArquivo, "a");
    if (dados == NULL)
    {
        printf("\n# FALHA AO INICIAR PROGRAMA\n# O arquivo de dados não pode ser manipulado, verifique.\n");
        return 0;
    }
    fclose(dados);
    imprimirDecoracao();
    printf("\n\t\t>> OLÁ, PROGRAMA INICIADO COM SUCESSO! <<\n");
    char entrada = '0';
    int op = 0;

    //Menu de opções
    do
    {
        printf("\nPressione ENTER para continuar...\n");
        setbuf(stdin, NULL);
        getchar();

        //Limpa o buffer do teclado para evitar que lixo de memória seja lido ao invés da entrada do usuário
        setbuf(stdin, NULL);

        //Limpar a variável para evitar lixo de memória nas repetições
        entrada = '\0';

        system("cls || clear");
        imprimirDecoracao();
        printf("\n\t\t>> MENU DE OPÇÕES <<\n");
        printf("\n> Informe um número para escolher uma opção e pressione ENTER:");
        printf("\n[1] Login");
        printf("\n[2] Cadastro");
        printf("\n[3] Ver a política de criação de identificadores e senhas");
        printf("\n[0] Encerrar programa");
        imprimirDecoracao();
        printf("\n> Informe o número: ");
        scanf("%[0-9]", &entrada);
        system("cls || clear");

        //Converte o char para int para que possa ser verificado no switch
        op = entrada - '0';

        switch (op)
        {
        case 0:
            printf("\n# SISTEMA FINALIZADO.\n");
            return 0;
        case 1:
            imprimirDecoracao();
            printf("\n\t\t\t>> AUTENTICAÇÃO <<\n\n");
            imprimirDecoracao();
            if (autenticar())
            {
                areaLogada();
                limparEstrutura();
            }
            break;
        case 2:
            imprimirDecoracao();
            printf("\n\t\t\t>> CADASTRO <<\n\n");
            imprimirDecoracao();
            cadastrarUsuario();
            break;
        case 3:
            imprimirDecoracao();
            printf("\n\t\t>> POLÍTICA DE IDENTIFICADORES E SENHAS <<\n\n");
            imprimirDecoracao();
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
 * Busca no arquivo o último ID cadastrado e retorna o próximo ID a ser usado
 * @return valor do próximo ID a ser usado e 0 em caso de falha
 */
int pegarProximoId()
{
    dados = fopen(nomeArquivo, "r");
    int id = 0;
    char linha[2048];

    //Validação para caso o arquivo não possa ser aberto.
    if (dados == NULL)
    {
        printf("\n# FALHA NOS DADOS - O arquivo de dados não pode ser aberto.");
        return 0;
    }
    while (!feof(dados))
    {
        //Lê as linhas até o final do arquivo, atribuindo o id da linha na variável id com formato inteiro
        fscanf(dados, "%d | %[^\n]", &id, linha);
    }
    fclose(dados);

    //O ID lido por último é o ID do último usuário cadastrado e será somado e retornado mais 1 para cadastrar o próximo
    return id + 1;
}

/**
 * Função para cadastrar novo usuário
 */
void cadastrarUsuario()
{
    //Variável para fazer a entrada de valores digitados e realizar a validação antes de atribuir à variável correta
    char temp[MAX];

    //Limpando a variável para garantir que não entre lixo de memória
    memset(&temp[0], 0, sizeof(temp));

    //Declara variável que vai concatenar todos os valores para inserir no arquivo de uma só vez
    char linha[MAX];

    //Variavel que receberá o valor de código (inteiro), em formato string para fazer a concatenação e inserir no arquivo
    char codigoString[16];

    //Define o ID do usuário que está se cadastrando
    u.codigo = pegarProximoId();

    //Converte o valor inteiro para String, permitindo usá-la posteriormente na função strcat
    sprintf(codigoString, "%d", u.codigo);

    /*Recolhendo informações do cadastro*/
    printf("\n> Forneça as informações necessárias para efetuar o cadastro:\n");
    //Loop para validação do nome, enquanto a função que valida a string retornar 0 (falso) o loop vai continuar (há uma negação na frente do retorno da função)
    do
    {
        printf("\n> Informe seu primeiro nome: ");
        //Limpa a entrada par evitar lixo
        setbuf(stdin, NULL);
        //Leitura do teclado
        scanf("%[^\n]", &temp);
    } while (!validarStringPadrao(temp));
    //Se sair do loop é porque a string é válida e pode ser copiada para a variável correta que irá guardar
    strcpy(u.nome, temp);
    //Transformar o nome em maiúsculo para padronização do arquivo
    alternarCapitalLetras(u.nome, 1);
    //Limpar a variável temporária para receber a próxima entrada
    memset(&temp[0], 0, sizeof(temp));

    //Loop validação sobrenome
    do
    {
        printf("\n> Agora, informe seu sobrenome: ");
        //Limpar o buffer de entrada para evitar lixo de memória
        setbuf(stdin, NULL);
        //Leitura do sobrenome, o %[^\n] vai fazer com que o programa leia tudo que o usuário digitar exceto ENTER (\n), por padrão pararia de ler no caractere espaço
        scanf("%[^\n]", &temp);
    } while (!validarStringPadrao(temp));
    //Quando sai do loop é porque a string lida é válida e seu conteudo pode ser copiado para a variável que irá guardar (u.sobrenome)
    strcpy(u.sobrenome, temp);
    //Torna palavra maiúscula
    alternarCapitalLetras(u.sobrenome, 1);
    //Limpar a variável temporária
    memset(&temp[0], 0, sizeof(temp));

    //Loop validação do e-mail
    do
    {
        printf("\n> Informe seu e-mail: ");
        setbuf(stdin, NULL);
        scanf("%[^\n]", &temp);
    } while (!validarStringEmail(temp));
    strcpy(u.email, temp);
    alternarCapitalLetras(u.email, 1);
    memset(&temp[0], 0, sizeof(temp));

    imprimirDecoracao();
    //Mostra a política para o usuário saber o que deve conter no seu identificador e senha
    mostrarPolitica();

    //Loop de validação do identificador
    do
    {
        printf("\n> Crie seu login: ");
        setbuf(stdin, NULL);
        scanf("%[^\n]", &temp);
    } while (!validarIdentificador(temp));
    strcpy(u.identificador, temp);
    memset(&temp[0], 0, sizeof(temp));

    //Loop de validação da senha
    do
    {
        setbuf(stdin, NULL);
        //getpass é usado para desativar o ECHO do console e não exibir a senha sendo digitada pelo usuário, retorna um ponteiro apontando para um buffer contendo a senha terminada em '\0' (NULL)
        strcpy(temp, getpass("\n> Crie uma senha: "));
    } while (!validarSenha(temp));
    strcpy(u.senha, temp);
    memset(&temp[0], 0, sizeof(temp));

    /*Finalizada a coleta dos dados, agora, a gravação no arquivo será feita*/
    //Abrir o arquivo com parâmetro "a" de append, não sobrescreve as informações, apenas adiciona.
    dados = fopen(nomeArquivo, "a");
    //Validação para caso o arquivo não possa ser aberto.
    if (dados == NULL)
    {
        printf("\n# ERRO FATAL - O arquivo de dados não pode ser aberto.\n");
        return;
    }
    //Gerar o valor pseudoaleatório do salt desse usuário
    gerarSalt();
    //Realiza a criptografia da senha com a função 'crypt'
    criptografarSenha();

    //Zerar a variável antes de começar a utilizá-la, para evitar que tenha valores prévios gravados no espaço de memória
    memset(&linha[0], 0, sizeof(linha));

    //Concatenção de valores na variável para jogar no arquivo somente uma string, de uma só vez
    strcat(linha, codigoString);
    strcat(linha, SEPARADOR);
    strcat(linha, u.identificador);
    strcat(linha, SEPARADOR);
    strcat(linha, u.salt);
    strcat(linha, SEPARADOR);
    strcat(linha, u.senhaCriptografada);
    strcat(linha, SEPARADOR);
    strcat(linha, u.nome);
    strcat(linha, SEPARADOR);
    strcat(linha, u.sobrenome);
    strcat(linha, SEPARADOR);
    strcat(linha, u.email);
    strcat(linha, "\n");

    //Insere a string com todos os dados no arquivo
    if (fputs(linha, dados) == EOF)
    {
        perror("\n# ERRO - Problema para inserir os dados no arquivo!\n");
    }
    else
    {
        printf("\n# SUCESSO - Cadastro ralizado!\n");
    }

    //Fecha o arquivo
    fclose(dados);

    //Limpa a estrutura com os dados
    limparEstrutura();
}

/**
 * Relizar a autenticação do usuário
 * @return 1 em caso de sucesso e; 0 em outros casos
 */
short int autenticar()
{
    //Variaveis que guardam os dados lidos nas linhas do arquivo
    char identificadorArquivo[16], saltArquivo[SALT_SIZE + 1], criptografiaArquivo[120], usuarioArquivo[51], sobrenomeArquivo[51], emailArquivo[51];
    int idArquivo = 0;

    /*Coleta do login e senha*/
    printf("\n> Informe seus dados:\n\n> LOGIN: ");
    //Limpa o buffer de entrada par evitar lixo
    setbuf(stdin, NULL);
    //Realiza a leitura até o usuário pressionar ENTER
    scanf("%[^\n]", &u.identificador);
    //Lê a senha com ECHO desativado e copia a leitura para a variável u.senha do usuário
    strcpy(u.senha, getpass("\n> SENHA: "));

    //Abrir o arquivo com parâmetro "r" de read, apenas lê.
    dados = fopen(nomeArquivo, "r");
    //Validação para caso o arquivo não possa ser aberto.
    if (dados == NULL)
    {
        printf("\n# ERRO FATAL - O arquivo de dados não pode ser aberto.\n");
        return 0;
    }
    //Loop que passa por todas as linhas do arquivo
    while (!feof(dados))
    {
        //Lê as linhas até o final do arquivo
        fscanf(dados, "%d | %s | %s | %s | %s | %s | %s", &idArquivo, identificadorArquivo, saltArquivo, criptografiaArquivo, usuarioArquivo, sobrenomeArquivo, emailArquivo);
        //Copia o salt lido do arquivo nessa linha para a variável u.salt de onde o criptografarSenha() irá usar
        strcpy(u.salt, saltArquivo);
        //Criptografa a senha que o usuário digitou com o salt que foi lido na linha
        criptografarSenha();
        //Se o identificador lido no arquivo e a senha digitada, criptografada com o salt da linha do arquivo forem iguais, autentica o usuário
        if (!strcmp(identificadorArquivo, u.identificador) && !strcmp(criptografiaArquivo, u.senhaCriptografada))
        {
            printf("\n\n# SUCESSO - Agora você está logado!\n\n");
            //Copiando dados para a estrutura
            u.codigo = idArquivo;
            strcpy(u.nome, usuarioArquivo);
            strcpy(u.sobrenome, sobrenomeArquivo);
            strcpy(u.email, emailArquivo);
            strcpy(u.identificador, identificadorArquivo);
            strcpy(u.salt, saltArquivo);
            strcpy(u.senhaCriptografada, criptografiaArquivo);

            //Salvar o formato da linha desse usuário autenticado para encontrar posteriormente
            sprintf(u.linhaUsuario, "%d | %s | %s | %s | %s | %s | %s\n", u.codigo, u.identificador, u.salt, u.senhaCriptografada, u.nome, u.sobrenome, u.email);
            //Fecha o arquivo
            fclose(dados);
            //Retorna 1, true
            return 1;
        }
    }
    //Se sair do loop é porque não autenticou
    fclose(dados);
    limparEstrutura();
    printf("\n# FALHA - Usuário e/ou senha incorretos!\n");
    //Retorna 0, false
    return 0;
}

/**
 * Imprime a política com as regras para criação de identificadores e senhas
 */
void mostrarPolitica()
{
    printf("\n\t\tIDENTIFICADOR/LOGIN\n");
    printf("\n-Não pode ser utilizado nome, sobrenome ou email;");
    printf("\n-Pode conter somente caracteres alfanuméricos e ponto final;");
    printf("\n-Deve ter no mínimo 5 caracteres e no máximo 15.");
    printf("\n\n\t\t\tSENHA\n");
    printf("\n-Deve conter no mínimo 8 caracteres e no máximo 30;");
    printf("\n-Deve conter, no mínimo, 2 caracteres especiais;");
    printf("\n-Deve conter números e letras;");
    printf("\n-Deve conter pelo menos uma letra maiúscula e uma minúscula;");
    printf("\n-Não pode conter mais de 2 números ordenados em sequência (Ex.: 123, 987);");
    printf("\n-Não pode conter mais de 2 números repetidos em sequência (Ex.: 555, 999);");
    printf("\n-Não pode conter caracteres que não sejam alfanuméricos (números e letras), especiais ou espaço.\n");
}

/**
 * Transforma a String para maiúscula ou minúscula, para facilitar comparações em dados
 * @param: String que será alternada para maiuscula ou minuscula, flag: 1 para maiuscula e diferente de 1 para minuscula
 * @return Variavel string passada como parâmetro maiúscula ou minúscula, dependendo da flag
 */
char *alternarCapitalLetras(char *string, int flag)
{
    //Declarei o i fora do for para pegar depois a última posição da string e adicionar o NULL
    int i = 0;

    //Passa por todos os caracteres da String
    for (i; string[i] != '\0'; i++)
    {
        if (flag == 1)
        {
            //Converte o caractere para maiusculo e sobrescreve no mesmo local, substituindo o caractere
            string[i] = toupper(string[i]);
        }
        else
        {
            //Converte o caractere para maiusculo e sobrescreve no mesmo local, substituindo o caractere
            string[i] = tolower(string[i]);
        }
    }
    //Acrescenta o caractere nulo no final da string convertida
    string[i] = '\0';

    //Retorna a mesma palavra, convertida
    return string;
}

/**
 * Verifica se a string passada como parâmetro contém somente caracteres alfabéticos e se o tamanho não ultrapassa de 50 caracteres
 * @return 1 caso a string seja válida e 0 caso string inválida
 */
short int validarStringPadrao(char *string)
{
    //Verifica o tamanho da string
    if (strlen(string) > 50)
    {
        printf("\n# QUANTIDADE LIMITE DE CARACTERES: por favor não ultrapasse a quantidade de 50 caracteres, você inseriu %d caracteres!\n", strlen(string));
        return 0;
    }
    //Loop para passar pelos caracteres da string
    for (int i = 0; i < strlen(string); i++)
    {
        //Usando a função isalpha da biblioteca ctype.h, é possível verificar se o caractere é alfabético
        if (!isalpha(string[i]))
        {
            printf("\n# CARACTERE INVÁLIDO: por favor insira somente caracteres alfabéticos nesse campo, sem espaços ou pontuações.\n# '%c' não é alfabético!\n", string[i]);
            return 0;
        }
    }
    //Se chegou aqui é válido
    return 1;
}

/**
 * Verifica se a string passada como parâmetro contém somente caracteres permitidos para e-mail
 * @return 1 caso a string seja válida e 0 caso string inválida
 */
short int validarStringEmail(char *string)
{
    //Variáveis para guardar as informações digitadas separadas
    char usuario[256], site[256], dominio[256];

    //Valida tamanho da string
    if (strlen(string) > 50)
    {
        printf("\n# QUANTIDADE LIMITE DE CARACTERES: por favor não ultrapasse a quantidade de 50 caracteres, você inseriu %d caracteres!\n", strlen(string));
        return 0;
    }

    // sscanf lê a entrada a partir da string no primeiro parâmetro, atribuindo para as variáveis. Retorna o número de campos convertidos e atribuídos com êxito.
    if (sscanf(string, "%[^@ \t\n]@%[^. \t\n].%3[^ \t\n]", usuario, site, dominio) != 3)
    {
        printf("\n# E-MAIL INVÁLIDO: por favor verifique o e-mail digitado\n");
        return 0;
    }

    //Se chegou aqui é porque a string é válida
    return 1;
}

/**
 * Função para verificar se o identificador cumpre com a política
 * @return 1 em caso de identificador válido e 0 caso inválido.
 */
short int validarIdentificador(char *identificador)
{
    char identificadorMaiusculo[MAX];                 //Variável que irá guardar o identificador convertido para maiúsculo, para simplificar a comparação com o nome e sobrenome
    char temp[MAX];                                   //Variável para guardar valores descartáveis que virão do arquivo
    char identificadorArquivo[16];                    //Variável para guardar o identificador recebido do arquivo
    strcpy(identificadorMaiusculo, identificador);    //Copiando o identificador para transformar em maiúsculo
    alternarCapitalLetras(identificadorMaiusculo, 1); //Tornando maiúsculo

    //Verifica tamanho do identificador
    if (strlen(identificador) < 5 || strlen(identificador) > 15)
    {
        printf("\n# IDENTIFICADOR INVÁLIDO - Não contém tamanho permitido (mínimo 5 e máximo 15)\n");
        return 0;
    }

    //Loop para passar pelos caracteres do identificador e verificar se contém caracteres válidos
    for (int i = 0; i < strlen(identificador); i++)
    {
        if (!isalnum(identificador[i]) && identificador[i] != '.')
        {
            printf("\n# IDENTIFICADOR INVÁLIDO - Contém caracteres não permitidos\n# O identificador pode conter somente caracteres alfanuméricos e ponto final.\n");
            return 0;
        }
    }

    //Se o identificador for igual ao nome ou sobrenome, é inválido.
    if (!strcmp(identificadorMaiusculo, u.nome) || !strcmp(identificadorMaiusculo, u.sobrenome))
    {
        printf("\n# IDENTIFICADOR INVÁLIDO - Identificador não pode ser seu nome ou sobrenome!\n");
        return 0;
    }

    /*Verifica se o identificador já foi utilizado*/
    //Abrir o arquivo com parâmetro "r" de read, apenas lê.
    dados = fopen(nomeArquivo, "r");
    //Validação para caso o arquivo não possa ser aberto.
    if (dados == NULL)
    {
        printf("\n# ERRO FATAL - O arquivo de dados não pode ser aberto.\n");
        return 0;
    }
    //Passa pelas linhas do arquivo
    while (!feof(dados))
    {
        //Lê linha por linha colocando os valores no formato, nas variáveis
        fscanf(dados, "%s | %s | %[^\n]", temp, identificadorArquivo, temp);
        alternarCapitalLetras(identificadorArquivo, 1); //Transformar o identificador recebido do arquivo em maiúsculo também para comparar com o que está tentando cadastrar
        //Realizando a comparação dos identificadores
        if (!strcmp(identificadorArquivo, identificadorMaiusculo))
        {
            //Se entrar aqui o identificador já foi utilizado
            printf("\n# IDENTIFICADOR INVÁLIDO - Já está sendo utilizado!\n");
            fclose(dados);
            return 0;
        }
    }
    //Fecha o arquivo
    fclose(dados);

    //Se chegou até aqui, passou pelas validações, retorna 1 - true
    return 1;
}

/** 
 * Função para verificar se a senha cumpre com a política de senhas
 * @return valor 1 em caso de senha válida e 0 caso inválida
 */
short int validarSenha(char *senha)
{
    //Contadores dos tipos de caracteres que a senha possui
    int contMinusculas = 0, contMaiusculas = 0, contNumeros = 0, contEspeciais = 0;
    char confirmacaoSenha[31]; //Variável que receberá a confirmação da senha

    //Verifica tamanho da senha
    if (strlen(senha) < 8 || strlen(senha) > 30)
    {
        printf("\n# SENHA INVÁLIDA - Não contém tamanho permitido (mínimo 8 e máximo 30)\n");
        return 0;
    }

    //Loop para passar pelos caracteres da senha
    for (int i = 0; i < strlen(senha); i++)
    {
        /*Sequencia de condições para verificar cada caractere da senha*/
        //Usando a função islower da biblioteca ctype.h, é possível verificar se o caractere é alfabético e minusculo
        if (islower(senha[i]))
        {
            contMinusculas++;
        }
        //Usando a função isupper da biblioteca ctype.h, é possível verificar se o caractere é alfabético e maiúsculo
        else if (isupper(senha[i]))
        {
            contMaiusculas++;
        }
        //Usando a função isdigit da biblioteca ctype.h, é possível verificar se o caractere é um digito
        else if (isdigit(senha[i]))
        {
            contNumeros++;

            //Verifica se a senha contém +2 números ordenados em sequência (ascendente ou descendente)
            if (((senha[i] - '0') + 1 == senha[i + 1] - '0' && (senha[i] - '0') + 2 == senha[i + 2] - '0') || ((senha[i] - '1') == senha[i + 1] - '0' && (senha[i] - '2') == senha[i + 2] - '0'))
            {
                printf("\n# SENHA INVÁLIDA - | %c | faz parte de números ordenados em sequência\n", senha[i]);
                return 0;
            }

            //Verifica se a senha contém +2 números repetidos em sequência
            if (senha[i] == senha[i + 1] && senha[i] == senha[i + 2])
            {
                printf("\n# SENHA INVÁLIDA - | %c | faz parte de números repetidos em sequência\n", senha[i]);
                return 0;
            }
        }
        //Verificando se o caractere é especial ou espaço
        else if (ispunct(senha[i]) || isspace(senha[i]))
        {
            contEspeciais++;
        }
        else
        {
            printf("\n\n# SENHA INVÁLIDA - Sua senha contém caracteres que nao são nem alfanuméricos nem especiais ou espaço.");
            printf("\n# Verifique a digitação e tente novamente.\n# Caracteres permitidos:");
            printf("\n#\tEspeciais: ! \" # $ %% & ' ( ) * + , - . / : ; < = > ? @ [ \\ ] ^ _ ` { | } ~ [ESPAÇO]");
            printf("\n#\tNuméricos: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9");
            printf("\n#\tAlfabéticos: a b c d e f g h i j k l m n o p q r s t u v w x y z \n#\t\t\tA B C D E F G H I J K L M N O P Q R S T U V W X Y Z\n");
            return 0;
        }
    } //Fim do loop que passa pelos caracteres da senha

    //Valida a quantidade de caracteres especiais
    if (contEspeciais < 2)
    {
        printf("\n# SENHA INVÁLIDA - Não contém caracteres especiais suficientes\n");
        return 0;
    }
    //Verifica se contém números e letras
    if ((contMinusculas + contMaiusculas) == 0 || contNumeros == 0)
    {
        printf("\n# SENHA INVÁLIDA - Não contém letras e números\n");
        return 0;
    }
    //Verifica se contém minúsculas
    if (contMinusculas == 0)
    {
        printf("\n# SENHA INVÁLIDA - Não contém qualquer letra minúscula\n");
        return 0;
    }
    //Verifica se contém maiúsculas
    if (contMaiusculas == 0)
    {
        printf("\n# SENHA INVÁLIDA - Não contém qualquer letra maiúscula\n");
        return 0;
    }
    //Solicita a confirmação da senha
    strcpy(confirmacaoSenha, getpass("\n> Confirme sua senha: "));
    //Compara as 2 senhas informadas, se forem diferentes vai retornar != 0, entrando na condição
    if (strcmp(confirmacaoSenha, senha))
    {
        printf("\n# FALHA - As senhas não coincidem.\n");
        return 0;
    }

    // printf("\n# SUCESSO - Sua senha está de acordo com a política e foi aprovada!\n");
    return 1;
}

/**
 * Gerar valor de salt
 */
void gerarSalt()
{

    char *buffer;                                                                                //Ponteiro onde serão armazenados os caracteres gerados aleatoriamente
    char listaCaracteres[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./"; //Lista de caracteres para serem escolhidos para o salt
    int retorno;                                                                                 //Para guardar a quantidade de caracteres gerados na função getrandom

    //Reservar espaço do tamanho do salt na memória
    buffer = malloc(SALT_SIZE);

    //Flag 0 para que a função utilize /dev/urandom - fonte de aleatoriedade do próprio Kernel.
    retorno = getrandom(buffer, SALT_SIZE, 0);

    //Verifica se a função retornou todo os bytes necessários
    if (retorno != SALT_SIZE)
    {
        perror("\n# FALHA - Erro ao obter caracteres para criação do salt\n");
    }

    //Loop para montagem da string do salt, escolhendo os caracteres
    for (int i = 0; i < SALT_SIZE; i++)
    {
        /*Seleciona 1 caractere da lista: converte o caractere do buffer para unsigned char (número) e faz
        MOD quantidade de caracteres da lista, o resultado será (resto da divisão) o índice que contém o caractere a ser usado.
        Evitando assim que surjam caracteres que não podem ser interpretados pela codificação do SO ou caracteres não permitidos
        para uso na função crypt, posteriormente.*/
        u.salt[i] = listaCaracteres[((unsigned char)buffer[i]) % (strlen(listaCaracteres))];
    }
    //Adiciona o caractere NULL na última posição da string salt
    u.salt[SALT_SIZE] = '\0';
}

/**
 * Criptografar a senha do usuário
 */
void criptografarSenha()
{
    //Reservar espaço de 120 bytes na memória, que é tamanho suficiente do retorno da função crypt
    u.senhaCriptografada = malloc(120);

    //Variável que armazena o valor do parâmetro (formatado) para função crypt
    char idSaltSenha[strlen(PARAMETRO_CRYPT) + SALT_SIZE];

    //Incluindo o valor do salt gerado na variável (idSaltSenha)
    sprintf(idSaltSenha, PARAMETRO_CRYPT, u.salt);

    //Envia o retorno do crypt para senhaCriptografada
    u.senhaCriptografada = crypt(u.senha, idSaltSenha);
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
    memset(&u.salt[0], 0, sizeof(u.salt));
    memset(&u.senha[0], 0, sizeof(u.senha));
    memset(&u.senhaCriptografada[0], 0, sizeof(u.senhaCriptografada));
    memset(&u.linhaUsuario[0], 0, sizeof(u.linhaUsuario));
    u.codigo = '0';
    // printf("\n# A ESTRUTURA FOI LIMPA.\n");
}

/**
 * Opções para o usuário autenticado
 */
void areaLogada()
{
    char entrada = '0'; //Recebe a entrada que o usuário digitar
    int op = 0;         //Recebe o valor da entrada convertido para int para usar no switch

    imprimirDecoracao();
    printf("\n\t\t\tBEM-VINDO %s!\n", u.nome);

    //Menu de opções
    do
    {
        printf("\nPressione ENTER para continuar...\n");
        //Limpa o buffer do teclado para evitar que lixo de memória (geralmente ENTER) seja lido ao invés da entrada do usuário
        setbuf(stdin, NULL);
        //Apenas uma pausa para que as informações fiquem na tela
        getchar();
        //Limpa o ENTER digitado no getchar()
        setbuf(stdin, NULL);
        //Limpar a variável para evitar lixo de memória nas repetições
        entrada = '\0';

        system("cls || clear");
        imprimirDecoracao();
        printf("\n\t\t\tLOGADO COMO %s.\n", u.nome);
        printf("\n> Informe um número para escolher uma opção e pressione ENTER:");
        printf("\n[1] Ver a política de criação de identificadores e senhas");
        printf("\n[2] Ver meus dados");
        printf("\n[3] Excluir meu cadastro");
        printf("\n[4] Logout");
        printf("\n[0] Encerrar programa");
        imprimirDecoracao();
        printf("\n> Informe o número: ");
        scanf("%[0-9]", &entrada);

        //Converte o char para int para que possa ser verificado no switch
        op = entrada - '0';

        switch (op)
        {
        case 0:
            limparEstrutura();
            printf("\n# SISTEMA FINALIZADO.\n");
            exit(0);
        case 1:
            imprimirDecoracao();
            printf("\n\t\t>> POLÍTICA DE IDENTIFICADORES E SENHAS <<\n\n");
            imprimirDecoracao();
            mostrarPolitica();
            break;
        case 2:
            imprimirDecoracao();
            printf("\n\t\t\t>> MEUS DADOS <<\n\n");
            imprimirDados();
            imprimirDecoracao();

            break;
        case 3:
            imprimirDecoracao();
            printf("\n\t\t\t>> EXCLUIR CONTA <<\n\n");
            excluirDados();
            limparEstrutura();
            return;
        case 4:
            printf("\n# LOGOUT - Você saiu.\n");
            return;
        default:
            printf("\n# OPÇÃO INVÁLIDA\n# Você digitou uma opção inválida, tente novamente!\n");
            break;
        }
    } while (1);
}

/**
 * Imprime os dados do usuário
 */
void imprimirDados()
{
    printf("\n> Código: %d", u.codigo);
    printf("\n> Nome: %s", u.nome);
    printf("\n> Sobrenome: %s", u.sobrenome);
    printf("\n> E-mail: %s", u.email);
    printf("\n> Login: %s", u.identificador);
    printf("\n> Salt: %s", u.salt);
    printf("\n> Senha criptografada: %s\n", u.senhaCriptografada);
}

/**
 * Excluir os dados do usuário do arquivo de dados
 */
void excluirDados()
{
    dados = fopen(nomeArquivo, "r");               //Arquivo de entrada
    FILE *saida = fopen("transferindo.txt", "w"); //Arquivo de saída
    char texto[MAX];                               //Uma string grande para armazenar as linhas lidas

    //Validação para caso o arquivo não possa ser aberto.
    if (dados == NULL || saida == NULL)
    {
        printf("\n# FALHA NOS DADOS - O arquivo de dados não pode ser aberto.");
        return;
    }
    //Loop pelas linhas do arquivo
    while (fgets(texto, MAX, dados) != NULL)
    {
        //Se a linha sendo lida no arquivo for diferente da linha do usuário atual, ela será copiada para o arquivo de saída
        if (strcmp(u.linhaUsuario, texto))
        {
            fputs(texto, saida);
        }
    }
    //Fechar arquivos
    fclose(dados);
    fclose(saida);

    //Excluir arquivo original, contendo a linha dos dados do usuário
    remove(nomeArquivo);
    //Renomeia o arquivo onde foram passadas as linhas que não seriam excluidas para o nome do arquivo de dados de entrada
    rename("transferindo.txt", nomeArquivo);
    printf("\n# SUCESSO - Seu cadastro foi deletado.\n");
}

/**
 * Apenas imprime as linhas de separação
 */
void imprimirDecoracao()
{
    printf("\n_________________________________________________________________________________");
    printf("\n*********************************************************************************\n");
}