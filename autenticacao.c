#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <sys/random.h>
#include <errno.h>

//Tamanho definido do salt
#define SALT_SIZE 16
//Tamanho grande para ser utilizado em algumas strings
#define MAX 2048
//Tamanho máximo das strings padrão de dados comuns (contando com o último caractere NULL)
#define MAX_DADOS 51
//Tamanho máximo da string do identificador (contando com o último caractere NULL)
#define MAX_IDENTIFICADOR 16
//Tamanho máximo da string da senha (contando com o último caractere NULL)
#define MAX_SENHA 31
//Parâmetro da função crypt
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
* Não pode conter mais de 2 números repetidos em sequência;
* Não pode conter caracteres que não sejam alfanuméricos, caracteres especiais ou espaço.
**/

/**
    ❖ Coordenador:
        ➢ Dados dos Estudantes: ver dados;
        ➢ Disciplina: ver descrição da disciplina; alterar descrição da disciplina;
            matricular estudante;
        ➢ Notas: ver notas; alterar notas.
    ❖ Professor:
        ➢ Dados dos Estudantes: ver dados;
        ➢ Disciplina: ver descrição da disciplina; alterar descrição da disciplina;
        ➢ Notas: ver notas; alterar notas.
    ❖ Estudante:
        ➢ Dados dos Estudantes: ver dados*;
        ➢ Disciplina: ver descrição da disciplina; matricular estudante*;
        ➢ Notas: ver notas*
        *Somente próprias informações
 */

//Ponteiro para os arquivos
FILE *ponteiroArquivos = NULL;
char temp[MAX * 5]; //Variável temporária para gravação de valores descartáveis.

char arquivoUsuarios[] = "dados.txt";
char arquivoNotas[] = "notas.txt";
char arquivoDisciplina[] = "disciplina.txt";
char arquivoCurso[] = "curso.txt";

//Declaração da estrutura do usuário
struct Usuario u;

//Declaração da estrutura da disciplina
struct Disciplina d;

//Declaração da estrutura das notas
struct Nota n;

// ### - Ajustar descrição das funções inserindo descrição dos parâmetros e retornos
// ### - Fazer validações nas entradas de dados com while(scanf(...) != X)

/*Declaração das funções*/

int pegarProximoId(char *arquivo);
void cadastrarUsuario();
short int autenticar(short int ehLogin);
void mostrarPolitica();
char *alternarCapitalLetras(char *string, int flag);
short int validarStringPadrao(char *string);
short int validarStringEmail(char *string);
short int validarIdentificador(char *identificador);
short int validarSenha(char *senha);
void gerarSalt();
void criptografarSenha();
void limparEstruturaUsuario();
void areaLogada();
void verDadosUsuario(int idUsuario);
short int excluirDados();
void imprimirDecoracao();
short int testarArquivo(char *nomeArquivo);
void editarDadosUsuario();
void coletarDados(short int nome, short int sobrenome, short int email, short int identificador, short int senha, short int papel);
void atualizarLinhaArquivo(char *arquivo, char *linhaObsoleta, char *linhaAtualizada);
short int inserirDadosPadrao(char *arquivo);
int selecionarUsuario(short int idPapelProcurado);
char *descreverNomePapel(short int idPapel);
int selecionarDisciplina(short int idCurso);
void pausarPrograma();
void operarDisciplina(int idDisciplina, short int verDescricao, short int alterarDescricao, short int alterarProfessor);
void matricularEstudanteDisciplina(int idEstudante, int idDisciplina);
void operarNotas(int idEstudante, int idDisciplina, short int verNotas, short int alterarNotas);

/**
 * Estrutura para organização dos dados do usuário
 */
struct Usuario
{
    int codigo;
    char nome[MAX_DADOS];
    char sobrenome[MAX_DADOS];
    char email[MAX_DADOS];
    char identificador[MAX_IDENTIFICADOR];
    char salt[SALT_SIZE + 1];
    char senha[MAX_SENHA];
    char *senhaCriptografada;
    short int papel;
    char linhaUsuario[MAX]; //Essa string é utilizada para encontrar a linha do usuário no arquivo
    char linhaAtualizadaUsuario[MAX];
};

/**
 * Estrutura para organização dos dados da disciplina
 */
struct Disciplina
{
    int codigo;
    char nome[MAX_DADOS];
    int idCurso;
    int idProfessor;
    char descricao[MAX * 4];
};

/**
 * Estrutura para organização dos dados da nota
 */
struct Nota
{
    int codigo;
    float nota1;
    float nota2;
    int idEstudante;
    int idDisciplina;
};

/**
 * Função principal
 */
int main()
{
    setlocale(LC_ALL, "Portuguese");

    //Verifica arquivos necessários para o programa iniciar
    if (testarArquivo(arquivoUsuarios) || testarArquivo(arquivoNotas) || testarArquivo(arquivoDisciplina) || testarArquivo(arquivoCurso))
    {
        printf("\n# ERRO FATAL - um arquivo de dados essencial não pode ser aberto, o programa não pode ser iniciado.\n");
        return 0;
    }

    //Se o arquivo do Curso ou da Disciplina estiver vazio vai inserir os dados default para trabalhar
    if (pegarProximoId(arquivoCurso) == 1 || pegarProximoId(arquivoDisciplina) == 1)
    {
        if (inserirDadosPadrao(arquivoCurso) || inserirDadosPadrao(arquivoDisciplina))
            return 0;
    }

    //

    imprimirDecoracao();
    printf("\n\t\t>> OLÁ, PROGRAMA INICIADO COM SUCESSO! <<\n");
    char entrada = '0';
    int op = 0;

    //Menu de opções
    do
    {
        pausarPrograma();

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
            limparEstruturaUsuario();
            printf("\n# SISTEMA FINALIZADO.\n");
            return 0;
        case 1:
            imprimirDecoracao();
            printf("\n\t\t\t>> AUTENTICAÇÃO <<\n\n");
            imprimirDecoracao();
            if (autenticar(1))
            {
                areaLogada();
                limparEstruturaUsuario();
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
 * Insere os dados padrão no arquivo de curso ou disciplina, o que for passado como parâmetro.
 * @return 1 em caso de falha ou 0 em caso de sucesso
 */
short int inserirDadosPadrao(char *arquivo)
{
    char StringDados[110]; //Tamanho máximo dos dados que informarei

    //Teste do arquivo
    if (testarArquivo(arquivo))
        return 1;

    ponteiroArquivos = fopen(arquivo, "w");

    if (!strcmp(arquivo, arquivoCurso))
    {
        //id | Nome
        strcpy(StringDados, "1 | Análise e Desenvolvimento de Sistemas\n");
    }
    else if (!strcmp(arquivo, arquivoDisciplina))
    {
        //id | Nome            | idCurso | idProfessor | Descrição
        strcpy(StringDados, "1 | Segurança da Informação | 1 | 0 | Descrição padrão da disciplina de Segurança da Informação\n");
    }
    else
    {
        printf("\n# ERRO - O arquivo passado como parâmetro não pode ser utilizado para essa função: %s\n", arquivo);
        fclose(ponteiroArquivos);
        return 1;
    }
    //Insere os dados padrão no arquivo
    if (fputs(StringDados, ponteiroArquivos) == EOF)
    {
        printf("\n# ERRO - Problema para inserir os dados no arquivo %s\n", arquivo);
        perror("# - ");
        fclose(ponteiroArquivos);
        return 1;
    }
    else
    {
        printf("\n# SUCESSO - Dados padrão inseridos no arquivo %s\n", arquivo);
    }

    fclose(ponteiroArquivos);
    return 0;
}

/**
 * Busca no arquivo o último ID cadastrado e retorna o próximo ID a ser usado
 * @return valor do próximo ID a ser usado e 0 em caso de falha
 */
int pegarProximoId(char *arquivo)
{
    //Teste do arquivo
    if (testarArquivo(arquivo))
        return 0;

    int id = 0;

    //Abrindo arquivo
    ponteiroArquivos = fopen(arquivo, "r");

    while (!feof(ponteiroArquivos))
    {
        //Lê as linhas até o final do arquivo, atribuindo o id da linha na variável id com formato inteiro
        fscanf(ponteiroArquivos, "%d | %[^\n]", &id, temp);
    }
    fclose(ponteiroArquivos);

    //O ID lido por último é o ID do último usuário cadastrado e será somado e retornado mais 1 para cadastrar o próximo
    return id + 1;
}

/**
 * Função para cadastrar novo usuário
 */
void cadastrarUsuario()
{
    //Validação para caso o arquivo não possa ser aberto.
    if (testarArquivo(arquivoUsuarios))
        return;

    u.codigo = pegarProximoId(arquivoUsuarios); //Define o ID do usuário que está se cadastrando

    /*Recolhendo informações do cadastro*/
    printf("\n> Forneça as informações necessárias para efetuar o cadastro:\n");
    coletarDados(1, 1, 1, 1, 1, 1);

    /*Finalizada a coleta dos dados, agora, a gravação no arquivo será feita*/
    //Validação para caso o arquivo não possa ser aberto.
    if (testarArquivo(arquivoUsuarios))
        return;

    //Abrir o arquivo com parâmetro "a" de append, não sobrescreve as informações, apenas adiciona.
    ponteiroArquivos = fopen(arquivoUsuarios, "a");

    //Passar dados cadastrados para a linha do usuário
    sprintf(u.linhaUsuario, "%d | %s | %s | %s | %s | %s | %s | %d\n", u.codigo, u.identificador, u.salt, u.senhaCriptografada, u.nome, u.sobrenome, u.email, u.papel);

    //Insere a string com todos os dados no arquivo
    if (fputs(u.linhaUsuario, ponteiroArquivos) == EOF)
    {
        perror("\n# ERRO - Problema para inserir os dados no arquivo!\n");
    }
    else
    {
        printf("\n# SUCESSO - Cadastro ralizado!\n");
    }

    fclose(ponteiroArquivos); //Fecha o arquivo
    limparEstruturaUsuario();
}

/**
 * Coletar informações do usuário
 */
void coletarDados(short int nome, short int sobrenome, short int email, short int identificador, short int senha, short int papel)
{
    char entradaTemp[MAX];
    //Variável para fazer a entrada de valores digitados e realizar a validação antes de atribuir à variável correta
    memset(&entradaTemp[0], 0, sizeof(entradaTemp)); //Limpando a variável para garantir que não entre lixo de memória

    if (nome)
    {
        //Loop para validação do nome, enquanto a função que valida a string retornar 0 (falso) o loop vai continuar (há uma negação na frente do retorno da função)
        do
        {
            printf("\n> Informe seu primeiro nome: ");
            //Limpa a entrada par evitar lixo
            setbuf(stdin, NULL);
            //Leitura do teclado
            scanf("%[^\n]", entradaTemp);
        } while (!validarStringPadrao(entradaTemp));
        //Se sair do loop é porque a string é válida e pode ser copiada para a variável correta que irá guardar
        strcpy(u.nome, entradaTemp);
        //Transformar o nome em maiúsculo para padronização do arquivo
        alternarCapitalLetras(u.nome, 1);
        //Limpar a variável temporária para receber a próxima entrada
        memset(&entradaTemp[0], 0, sizeof(entradaTemp));
    }

    if (sobrenome)
    {
        //Loop validação sobrenome
        do
        {
            printf("\n> Agora, informe seu sobrenome: ");
            //Limpar o buffer de entrada para evitar lixo de memória
            setbuf(stdin, NULL);
            //Leitura do sobrenome, o %[^\n] vai fazer com que o programa leia tudo que o usuário digitar exceto ENTER (\n), por padrão pararia de ler no caractere espaço
            scanf("%[^\n]", entradaTemp);
        } while (!validarStringPadrao(entradaTemp));
        //Quando sai do loop é porque a string lida é válida e seu conteudo pode ser copiado para a variável que irá guardar (u.sobrenome)
        strcpy(u.sobrenome, entradaTemp);
        //Torna palavra maiúscula
        alternarCapitalLetras(u.sobrenome, 1);
        //Limpar a variável temporária
        memset(&entradaTemp[0], 0, sizeof(entradaTemp));
    }

    if (email)
    {
        //Loop validação do e-mail
        do
        {
            printf("\n> Informe seu e-mail: ");
            setbuf(stdin, NULL);
            scanf("%[^\n]", entradaTemp);
        } while (!validarStringEmail(entradaTemp));
        strcpy(u.email, entradaTemp);
        alternarCapitalLetras(u.email, 1);
        memset(&entradaTemp[0], 0, sizeof(entradaTemp));
    }

    if (identificador || senha)
    {
        imprimirDecoracao();
        //Mostra a política para o usuário saber o que deve conter no seu identificador e senha
        mostrarPolitica();
    }

    if (identificador)
    {
        //Loop de validação do identificador
        do
        {
            printf("\n> Crie seu login: ");
            setbuf(stdin, NULL);
            scanf("%[^\n]", entradaTemp);
        } while (!validarIdentificador(entradaTemp));
        strcpy(u.identificador, entradaTemp);
        memset(&entradaTemp[0], 0, sizeof(entradaTemp));
    }

    if (senha)
    {
        //Loop de validação da senha
        do
        {
            setbuf(stdin, NULL);
            //getpass é usado para desativar o ECHO do console e não exibir a senha sendo digitada pelo usuário, retorna um ponteiro apontando para um buffer contendo a senha terminada em '\0' (NULL)
            strcpy(entradaTemp, getpass("\n> Crie uma senha: "));
        } while (!validarSenha(entradaTemp));
        strcpy(u.senha, entradaTemp);
        memset(&entradaTemp[0], 0, sizeof(entradaTemp));
        printf("\n# SUCESSO - Senha aprovada.\n");

        //Gerar o valor pseudoaleatório do salt desse usuário
        gerarSalt();

        //Realiza a criptografia da senha com a função 'crypt'
        criptografarSenha();
    }

    if (papel)
    {
        //Loop para escolha do papel assumido
        do
        {
            printf("\n> Informe o número referente ao papel que lhe descreve: ");
            printf("\n[1] Coordenador");
            printf("\n[2] Professor");
            printf("\n[3] Estudante\n> ");
            setbuf(stdin, NULL);
            u.papel = 0;
            scanf("%d", &u.papel);

            if (u.papel > 0 && u.papel < 4)
            {
                break;
            }
            else
            {
                printf("\n# FALHA - Escolha um número dentre as opções!\n");
            }
        } while (1);
    }
}

/**
 * Relizar a autenticação do usuário
 * @return 1 em caso de sucesso e; 0 em outros casos
 */
short int autenticar(short int ehLogin)
{
    //Variaveis que guardam os dados lidos nas linhas do arquivo
    char identificadorArquivo[MAX_IDENTIFICADOR], saltArquivo[SALT_SIZE + 1], criptografiaArquivo[120], usuarioArquivo[MAX_DADOS], sobrenomeArquivo[MAX_DADOS], emailArquivo[MAX_DADOS];
    int codigoArquivo = 0, papelArquivo = 0;

    /*Coleta do login e senha*/
    printf("\n> Informe suas credenciais:\n# LOGIN: ");
    //Limpa o buffer de entrada par evitar lixo
    setbuf(stdin, NULL);
    //Realiza a leitura até o usuário pressionar ENTER
    scanf("%[^\n]", &u.identificador);
    //Lê a senha com ECHO desativado e copia a leitura para a variável u.senha do usuário
    strcpy(u.senha, getpass("# SENHA: "));

    //Validação para caso o arquivo não possa ser aberto.
    if (testarArquivo(arquivoUsuarios))
        return 0;

    //Abrir o arquivo com parâmetro "r" de read, apenas lê.
    ponteiroArquivos = fopen(arquivoUsuarios, "r");

    //Loop que passa por todas as linhas do arquivo
    while (!feof(ponteiroArquivos))
    {

        //Lê as linhas até o final do arquivo
        fscanf(ponteiroArquivos, "%d | %s | %s | %s | %s | %s | %s | %d", &codigoArquivo, identificadorArquivo, saltArquivo, criptografiaArquivo, usuarioArquivo, sobrenomeArquivo, emailArquivo, &papelArquivo);
        //Copia o salt lido do arquivo nessa linha para a variável u.salt de onde o criptografarSenha() irá usar
        strcpy(u.salt, saltArquivo);
        //Criptografa a senha que o usuário digitou com o salt que foi lido na linha
        criptografarSenha();
        //Se o identificador lido no arquivo e a senha digitada, criptografada com o salt da linha do arquivo forem iguais, autentica o usuário
        if (!strcmp(identificadorArquivo, u.identificador) && !strcmp(criptografiaArquivo, u.senhaCriptografada))
        {
            //Se for login basta acertar um identificador e senha qualquer salvo no arquivo
            if (ehLogin)
            {
                //Copiando dados para a estrutura
                u.codigo = codigoArquivo;
                strcpy(u.nome, usuarioArquivo);
                strcpy(u.sobrenome, sobrenomeArquivo);
                strcpy(u.email, emailArquivo);
                strcpy(u.identificador, identificadorArquivo);
                strcpy(u.salt, saltArquivo);
                strcpy(u.senhaCriptografada, criptografiaArquivo);
                u.papel = papelArquivo;
                //Salvar o formato da linha do usuário autenticado
                sprintf(u.linhaUsuario, "%d | %s | %s | %s | %s | %s | %s | %d\n", u.codigo, u.identificador, u.salt, u.senhaCriptografada, u.nome, u.sobrenome, u.email, u.papel);
                printf("\n\n# SUCESSO - Autenticação realizada!\n\n");

                fclose(ponteiroArquivos); //Fecha o arquivo
                return 1;                 //Retorna 1, true
            }
            //Se não for login, a autenticação só poderá liberar o acesso caso o usuário autenticando seja o usuário que está logado
            else if (u.codigo == codigoArquivo)
            {
                printf("\n\n# SUCESSO - Acesso autorizado.\n\n");
                fclose(ponteiroArquivos); //Fecha o arquivo
                return 1;                 //Retorna 1, true
            }
            else
            {
                printf("\n\n# ACESSO NÃO AUTORIZADO - Esse usuário não pode realizar essa operação!\n\n");
                return 0;
            }
        }
    }
    //Se sair do loop é porque não autenticou
    fclose(ponteiroArquivos);
    /*Apenas se for autenticação para login, deve limpar os dados caso falhe a autenticação, 
    se for autenticação quando o usuário já está logado, não pode limpar os dados caso erre a senha.*/
    if (ehLogin)
        limparEstruturaUsuario();

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
    char identificadorMaiusculo[MAX_IDENTIFICADOR];   //Variável que irá guardar o identificador convertido para maiúsculo, para simplificar a comparação com o nome e sobrenome
    char identificadorArquivo[MAX_IDENTIFICADOR];     //Variável para guardar o identificador recebido do arquivo
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

    //Validação para caso o arquivo não possa ser aberto.
    if (testarArquivo(arquivoUsuarios))
        return 0;

    //Abrir o arquivo com parâmetro "r" de read, apenas lê.
    ponteiroArquivos = fopen(arquivoUsuarios, "r");

    //Passa pelas linhas do arquivo
    while (!feof(ponteiroArquivos))
    {
        //Lê linha por linha colocando os valores no formato, nas variáveis
        fscanf(ponteiroArquivos, "%s | %s | %[^\n]", temp, identificadorArquivo, temp);
        alternarCapitalLetras(identificadorArquivo, 1); //Transformar o identificador recebido do arquivo em maiúsculo também para comparar com o que está tentando cadastrar
        //Realizando a comparação dos identificadores
        if (!strcmp(identificadorArquivo, identificadorMaiusculo))
        {
            //Se entrar aqui o identificador já foi utilizado
            printf("\n# IDENTIFICADOR INVÁLIDO - Já está sendo utilizado!\n");
            fclose(ponteiroArquivos);
            return 0;
        }
    }
    //Fecha o arquivo
    fclose(ponteiroArquivos);

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
    char confirmacaoSenha[MAX_SENHA]; //Variável que receberá a confirmação da senha

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
    buffer = malloc(SALT_SIZE + 1);

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
        // printf("\n§ > %d : '%c'", i, u.salt[i]);
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
 * Zera os dados da estrutura do usuário para reutilização
 */
void limparEstruturaUsuario()
{
    memset(&u.nome[0], 0, sizeof(u.nome));
    memset(&u.sobrenome[0], 0, sizeof(u.sobrenome));
    memset(&u.email[0], 0, sizeof(u.email));
    memset(&u.identificador[0], 0, sizeof(u.identificador));
    memset(&u.salt[0], 0, sizeof(u.salt));
    memset(&u.senha[0], 0, sizeof(u.senha));
    memset(&u.senhaCriptografada[0], 0, sizeof(u.senhaCriptografada));
    memset(&u.linhaUsuario[0], 0, sizeof(u.linhaUsuario));
    u.papel = '0';
    u.codigo = '0';
    // printf("\n# A ESTRUTURA DO USUÁRIO FOI LIMPA.\n");
}

/**
 * Zera os dados da estrutura da disciplina para reutilização
 */
void limparEstruturaDisciplina()
{
    memset(&d.nome[0], 0, sizeof(d.nome));
    memset(&d.descricao[0], 0, sizeof(d.descricao));
    d.idCurso = '0';
    d.idProfessor = '0';
    d.codigo = '0';
}

/**
 * Opções para o usuário autenticado
 */
// ### - Criar loops para que o usuário possa repetir algumas operações sem ter que acessar a opção do menu cada vez.
void areaLogada()
{
    char entrada[] = "00"; //Recebe a entrada que o usuário digitar
    int op = 0;            //Recebe o valor da entrada convertido para int para usar no switch

    imprimirDecoracao();

    printf("\n\t\t\tBEM-VINDO %s %s!\n", descreverNomePapel(u.papel), u.nome);

    //Menu de opções
    do
    {
        pausarPrograma();
        //Limpar a variável para evitar lixo de memória nas repetições
        memset(&entrada[0], 0, sizeof(entrada));

        system("cls || clear");
        imprimirDecoracao();
        printf("\n\t\t\tLOGADO COMO %s.\n", u.nome);
        //Qualquer papel tem acesso
        printf("\n> Informe um número para escolher uma opção e pressione ENTER:");
        printf("\n___________________________________");
        printf("\n[0] ENCERRAR PROGRAMA");
        printf("\n[1] LOGOUT");
        printf("\n[2] EXCLUIR MEU CADASTRO");
        printf("\n___________________________________");
        printf("\n[3] Ver meus dados");
        printf("\n[4] Editar meus dados");
        printf("\n[5] Ver descrição da disciplina");
        printf("\n[6] Ver notas");
        //Coordenador e Professor tem acesso
        if (u.papel == 1 || u.papel == 2)
        {
            printf("\n[7] Ver dados estudantes");
            printf("\n[8] Alterar descrição de disciplina");
            printf("\n[9] Alterar notas");
        }
        //Coordenador tem acesso
        if (u.papel == 1)
        {
            printf("\n[10] Matricular estudante");
            printf("\n[11] Definir professor em disciplina");
        }
        //Coordenador e estudante tem acesso
        if (u.papel == 3)
        {
            printf("\n[7] Matricular-se em disciplina");
        }

        imprimirDecoracao();
        printf("\n> Informe o número: ");
        scanf("%[0-9]", entrada);

        //Converte o char para int para que possa ser verificado no switch
        op = atoi(entrada);

        switch (op)
        {
        case 0:
            if (!strcmp(entrada, "0"))
            {
                limparEstruturaUsuario();
                system("cls || clear");
                printf("\n# SISTEMA FINALIZADO.\n");
                exit(0);
            }
            else //O usuário digitou um caractere não inteiro
            {
                printf("\n# OPÇÃO INVÁLIDA\n# Você digitou uma opção inválida, tente novamente!\n");
                setbuf(stdin, NULL);
                getchar();
            }
            break;
        case 1:
            printf("\n# LOGOUT - Você saiu.\n");
            return;
        case 2:
            imprimirDecoracao();
            printf("\n\t\t\t>> EXCLUIR CONTA <<\n\n");
            if (excluirDados())
            {
                limparEstruturaUsuario();
                getchar();
                return;
            }
            else
            {
                printf("\n# OPERAÇÃO CANCELADA\n");
                getchar();
            }

            break;
        case 3:
            imprimirDecoracao();
            printf("\n\t\t\t>> MEUS DADOS <<\n\n");
            verDadosUsuario(u.codigo);
            imprimirDecoracao();
            break;
        case 4:
            imprimirDecoracao();
            printf("\n\t\t\t>> EDITAR MEUS DADOS <<\n\n");
            imprimirDecoracao();
            editarDadosUsuario(u.codigo);
            break;
        case 5:
            imprimirDecoracao();
            printf("\n\t\t>> DESCRIÇÃO DA DISCIPLINA <<\n\n");
            imprimirDecoracao();
            operarDisciplina(selecionarDisciplina(1), 1, 0, 0);
            break;
        case 6:
            imprimirDecoracao();
            printf("\n\t\t\t>> NOTAS <<\n\n");
            imprimirDecoracao();
            //Se o usuário logado for estudante, mostra apenas as notas dele
            if (u.papel == 3)
            {
                printf("\n# SELECIONE A DISCIPLINA QUE DESEJA VER SUAS NOTAS\n");
                operarNotas(u.codigo, selecionarDisciplina(1), 1, 0);
            }
            else //Se for outro papel, o usuário seleciona de qual estudante quer ver as notas
            {
                printf("\n# SELECIONE A DISCIPLINA E O ESTUDANTE QUE DESEJA VER AS NOTAS\n");
                operarNotas(selecionarUsuario(3), selecionarDisciplina(1), 1, 0);
            }
            break;
        case 7:
            imprimirDecoracao();
            if (u.papel != 3)
            {
                printf("\n\t\t\t>> VER DADOS DE ESTUDANTES <<\n\n");
                imprimirDecoracao();
                printf("\n# SELECIONE O ESTUDANTE QUE DESEJA VER OS DADOS\n");
                verDadosUsuario(selecionarUsuario(3));
            }
            else
            {
                printf("\n\t\t>> MATRICULAR EM DISCIPLINA <<\n\n");
                imprimirDecoracao();
                printf("\n# SELECIONE A DISCIPLINA PARA MATRICULAR-SE\n");
                matricularEstudanteDisciplina(u.codigo, selecionarDisciplina(1));
            }
            break;
        default:
            if (u.papel == 1 || u.papel == 2)
            {
                switch (op)
                {
                case 8:
                    imprimirDecoracao();
                    printf("\n\t\t>> ALTERAR DESCRIÇÃO DA DISCIPLINA <<\n\n");
                    imprimirDecoracao();
                    printf("\n# SELECIONE A DISCIPLINA QUE DESEJA ALTERAR A DESCRIÇÃO\n");
                    operarDisciplina(selecionarDisciplina(1), 1, 1, 0);
                    break;
                case 9:
                    imprimirDecoracao();
                    printf("\n\t\t>> ALTERAR NOTAS DOS ESTUDANTES <<\n\n");
                    imprimirDecoracao();
                    printf("\n# SELECIONE A DISCIPLINA E O ESTUDANTE QUE DESEJA ALTERAR AS NOTAS\n");
                    operarNotas(selecionarUsuario(3), selecionarDisciplina(1), 1, 1);
                    break;
                default:
                    if (u.papel == 1)
                    {
                        switch (op)
                        {
                        case 10:
                            imprimirDecoracao();
                            printf("\n\t\t\t>> MATRICULAR ESTUDANTE <<\n\n");
                            imprimirDecoracao();
                            printf("\n# SELECIONE A DISCIPLINA E O ESTUDANTE QUE DESEJA MATRICULAR\n");
                            matricularEstudanteDisciplina(selecionarUsuario(3), selecionarDisciplina(1));
                            break;
                        case 11:
                            imprimirDecoracao();
                            printf("\n\t\t>> DEFINIR PROFESSOR PARA DISCIPLINA <<\n\n");
                            imprimirDecoracao();
                            printf("\n# SELECIONE UMA DISCIPLINA E UM PROFESSOR PARA ATRIBUIR\n");
                            operarDisciplina(selecionarDisciplina(1), 0, 0, 1);
                            break;
                        default:
                            printf("\n# OPÇÃO INVÁLIDA\n# Você digitou uma opção inválida, tente novamente!\n");
                            break;
                        }
                    }
                    else
                    {
                        printf("\n# OPÇÃO INVÁLIDA\n# Você digitou uma opção inválida, tente novamente!\n");
                    }
                    break;
                }
            }
            else
            {
                printf("\n# OPÇÃO INVÁLIDA\n# Você digitou uma opção inválida, tente novamente!\n");
            }
            break;
        }
    } while (1);
}

/**
 * Imprime os dados do usuário
 * @param id do usuário que se deseja exibir os dados
 */
void verDadosUsuario(int idUsuario)
{
    if (idUsuario != u.codigo)
    {
        //Teste do arquivo
        if (testarArquivo(arquivoUsuarios))
            return;

        char nome[MAX_DADOS], sobrenome[MAX_DADOS], email[MAX_DADOS], identificador[MAX_IDENTIFICADOR], salt[SALT_SIZE + 1], senhaCriptografada[120];
        int idPapel = 0, idLido = 0;

        //Abrindo arquivo
        ponteiroArquivos = fopen(arquivoUsuarios, "r");

        while (!feof(ponteiroArquivos))
        {
            //Lê as linhas até o final do arquivo, atribuindo o id da linha na variável id com formato inteiro
            fscanf(ponteiroArquivos, "%d | %s | %s | %s | %s | %s | %s | %d\n", &idLido, identificador, salt, senhaCriptografada, nome, sobrenome, email, &idPapel);

            //Se encontrar a linha do ID do usuário procurado para o loop
            if (idLido == idUsuario)
            {
                break;
            }
        }
        fclose(ponteiroArquivos);

        //Se sair do loop e o idLido por último não for o do usuário que foi passado como parâmetro, significa que não encontrou o usuário buscado.
        if (idLido != idUsuario)
        {
            printf("\n# ERRO - Os dados do usuário selecionado não puderam ser localizados!");
            return;
        }

        printf("\n¬ Código: %d", idUsuario);
        printf("\n¬ Nome Completo: %s %s", nome, sobrenome);
        printf("\n¬ E-mail: %s", email);
        printf("\n¬ Login: %s", identificador);
        printf("\n¬ Salt: %s", salt);
        printf("\n¬ Senha criptografada: %s", senhaCriptografada);
        printf("\n¬ Papel: %s\n", descreverNomePapel(idPapel));
    }
    else
    {
        printf("\n¬ Código: %d", idUsuario);
        printf("\n¬ Nome Completo: %s %s", u.nome, u.sobrenome);
        printf("\n¬ E-mail: %s", u.email);
        printf("\n¬ Login: %s", u.identificador);
        printf("\n¬ Salt: %s", u.salt);
        printf("\n¬ Senha criptografada: %s", u.senhaCriptografada);
        printf("\n¬ Papel: %s\n", descreverNomePapel(u.papel));
    }
}

/**
 * Excluir os dados do usuário do arquivo de dados
 * @return 1 se o cadastro foi deletado e 0 se foi cancelado pelo usuário ou por falha no arquivo
 */
short int excluirDados()
{
    printf("# AVISO - Tem certeza que deseja excluir seu cadastro?\n# Essa ação não pode ser desfeita! [s/n]\n>");
    setbuf(stdin, NULL);
    if (getchar() != 's')
    {
        return 0;
    }

    //Validação para caso os arquivos não possa ser acessados.
    if (testarArquivo(arquivoUsuarios) || testarArquivo("transferindo.txt"))
        return 0;

    ponteiroArquivos = fopen(arquivoUsuarios, "r"); //Arquivo de entrada
    FILE *saida = fopen("transferindo.txt", "w");   //Arquivo de saída
    char texto[MAX];                                //Uma string grande para armazenar as linhas lidas

    //Loop pelas linhas do arquivo
    while (fgets(texto, MAX, ponteiroArquivos) != NULL)
    {
        //Se a linha sendo lida no arquivo for diferente da linha do usuário atual, ela será copiada para o arquivo de saída
        if (strcmp(u.linhaUsuario, texto))
        {
            fputs(texto, saida);
        }
    }
    //Fechar arquivos
    fclose(ponteiroArquivos);
    fclose(saida);

    //Excluir arquivo original, contendo a linha dos dados do usuário
    remove(arquivoUsuarios);
    //Renomeia o arquivo onde foram passadas as linhas que não seriam excluidas para o nome do arquivo de dados de entrada
    rename("transferindo.txt", arquivoUsuarios);
    printf("\n# SUCESSO - Seu cadastro foi deletado.\n");
    return 1;
}

/**
 * Apenas imprime as linhas de separação
 */
void imprimirDecoracao()
{
    printf("\n_________________________________________________________________________________");
    printf("\n*********************************************************************************\n");
}

/**
 *  Verifica se o arquivo passado como parâmetro pode ser criado/utilizado
 *  @param *nomeArquivo: nome do arquivo que se deseja testar
 *  @return int 1 caso o arquivo não possa ser acessado e 0 caso contrário
 */
short int testarArquivo(char *nomeArquivo)
{
    FILE *arquivo = fopen(nomeArquivo, "a");
    if (arquivo == NULL)
    {
        printf("\n# ERRO - O arquivo '%s' não pode ser acessado, verifique.\n", nomeArquivo);
        return 1;
    }
    fclose(arquivo);
    return 0;
}

/**
 * Disponibiliza um menu para o usuário escolher que dados do cadastro ele quer editar e salva 
 * no arquivo de dados quando o usuário escolher a opção de Salvar no menu.
 */
void editarDadosUsuario()
{
    //Validação para caso os arquivos não possa ser acessados.
    if (testarArquivo(arquivoUsuarios))
        return;

    char entrada = '0'; //Recebe a entrada que o usuário digitar
    int op = 0;         //Recebe o valor da entrada convertido para int para usar no switch

    //Menu de opções
    do
    {
        pausarPrograma();
        //Limpar a variável para evitar lixo de memória nas repetições
        entrada = '\0';

        system("cls || clear");
        imprimirDecoracao();
        printf("\n\t\t\tEDITAR MEUS DADOS\n");
        printf("\n> Informe um número para escolher a informação que deseja editar e pressione ENTER:");
        printf("\n[1] Nome");
        printf("\n[2] Sobrenome");
        printf("\n[3] E-mail");
        printf("\n[4] Identificador");
        printf("\n[5] Senha");
        printf("\n[6] Papel");
        printf("\n_______________________________");
        printf("\n[7] SAIR E GUARDAR ALTERAÇÕES");
        printf("\n[0] ENCERRAR PROGRAMA");
        imprimirDecoracao();
        printf("\n> Informe o número: ");
        scanf("%[0-9]", &entrada);

        //Converte o char para int para que possa ser verificado no switch
        op = entrada - '0';

        switch (op)
        {
        case 0:
            limparEstruturaUsuario();
            system("cls || clear");
            printf("\n# SISTEMA FINALIZADO.\n");
            exit(0);
        case 1:
            coletarDados(1, 0, 0, 0, 0, 0);
            break;
        case 2:
            coletarDados(0, 1, 0, 0, 0, 0);
            break;
        case 3:
            coletarDados(0, 0, 1, 0, 0, 0);
            break;
        case 4:
            coletarDados(0, 0, 0, 1, 0, 0);
            break;
        case 5:
            if (autenticar(0))
                coletarDados(0, 0, 0, 0, 1, 0);
            break;
        case 6:
            coletarDados(0, 0, 0, 0, 0, 1);
            break;
        case 7:
            //Preparar linha com dados do usuário para inserir no arquivo
            sprintf(u.linhaAtualizadaUsuario, "%d | %s | %s | %s | %s | %s | %s | %d\n", u.codigo, u.identificador, u.salt, u.senhaCriptografada, u.nome, u.sobrenome, u.email, u.papel);
            atualizarLinhaArquivo(arquivoUsuarios, u.linhaUsuario, u.linhaAtualizadaUsuario);
            //Atualizar a string da linha que está no arquivo agora
            strcpy(u.linhaUsuario, u.linhaAtualizadaUsuario);
            // sprintf(u.linhaUsuario, "%d | %s | %s | %s | %s | %s | %s | %d\n", u.codigo, u.identificador, u.salt, u.senhaCriptografada, u.nome, u.sobrenome, u.email, u.papel);
            return;
        default:
            printf("\n# OPÇÃO INVÁLIDA\n# Você digitou uma opção inválida, tente novamente!\n");
            break;
        }
    } while (1);
}

/**
 * Faz a atualização do arquivo, procurando pela linha obsoleta e trocando pela linha atualizada.
 */
// ### - Fazer validação correta se a função conseguiu realizar todas as operações
void atualizarLinhaArquivo(char *arquivo, char *linhaObsoleta, char *linhaAtualizada)
{
    //Validação antes de acessar os arquivos.
    if (testarArquivo(arquivo) || testarArquivo("transferindo.txt"))
        return;

    FILE *entrada = fopen(arquivo, "r");          //Arquivo de entrada
    FILE *saida = fopen("transferindo.txt", "w"); //Arquivo de saída
    char texto[MAX];                              //Uma string grande para armazenar as linhas lidas

    //Loop pelas linhas do arquivo
    while (fgets(texto, MAX, entrada) != NULL)
    {
        //Se a linha sendo lida no arquivo for diferente da linha do usuário atual, ela será apenas copiada para o arquivo de saída
        if (strcmp(linhaObsoleta, texto))
        {
            fputs(texto, saida);
            // printf("\n§ Linha é diferente copiada: '%s'\n§ Linha procurada: '%s'", texto, linhaObsoleta);
        }
        else
        {
            fputs(linhaAtualizada, saida);
            // printf("\n§ Linha é igual substituida: '%s' > '%s'", linhaObsoleta, linhaAtualizada);
        }
    }
    //Fechar arquivos
    fclose(entrada);
    fclose(saida);

    //Excluir arquivo original, contendo a linha com os dados obsoletos
    remove(arquivo);
    //Renomeia o arquivo onde foram passadas as linhas, inclusive a linha atualizada, para o nome do arquivo de dados de entrada
    rename("transferindo.txt", arquivo);
    printf("\n# SUCESSO - Os dados foram atualizados.\n");
}

/**
 * Lista todos os usuários de um grupo específico passado no parâmetro inteiro 
 * [1-Coordenadores, 2-Professores ou 3-Estudantes] 
 * e dá opção de escolha ao usuário.
 * @return id do usuário escolhido dentre os listados ou 0 em caso de falha
 */
int selecionarUsuario(short int idPapelProcurado)
{
    //Teste do arquivo
    if (testarArquivo(arquivoUsuarios))
        return 0;

    int listaIds[MAX], idSelecionado = 0, idPapelLido = 0, idUsuarioLido = 0, contador = 0;
    char nomeUsuarioLido[MAX_DADOS], sobrenomeUsuarioLido[MAX_DADOS];

    //Abrindo arquivo
    ponteiroArquivos = fopen(arquivoUsuarios, "r");

    printf("\n\t\t*** LISTANDO USUÁRIOS COM PAPEL: %s ***\n", descreverNomePapel(idPapelProcurado));
    while (!feof(ponteiroArquivos))
    {
        idUsuarioLido = 0;
        idPapelLido = 0;

        //Lê as linhas até o final do arquivo, atribuindo o id da linha na variável id com formato inteiro
        fscanf(ponteiroArquivos, "%d | %s | %s | %s | %s | %s | %s | %d\n", &idUsuarioLido, temp, temp, temp, nomeUsuarioLido, sobrenomeUsuarioLido, temp, &idPapelLido);

        if (idPapelLido == idPapelProcurado)
        {
            listaIds[contador] = idUsuarioLido;
            printf("\n[%d] %s %s", listaIds[contador], nomeUsuarioLido, sobrenomeUsuarioLido);
            contador++;
        }
    }
    fclose(ponteiroArquivos);

    // Encerra a função se não haviam usuários cadastrados com o papel escolhido
    if (contador == 0)
    {
        printf("\n# INFO - Não há usuários cadastrados com esse papel\n");
        return 0;
    }

    //Solicita que o usuário selecione um ID e valida se esse ID está entre os listados (permitidos)
    do
    {
        printf("\n\n> Selecione o número que corresponde ao usuário desejado: ");
        setbuf(stdin, NULL);
        scanf("%d", &idSelecionado);
        for (int i = 0; i < contador; i++)
        {
            if (idSelecionado == listaIds[i])
            {
                return idSelecionado;
            }
        }
        printf("\n# FALHA - Esse ID não está na listagem, tente novamente informando um ID que está na lista mostrada.");
    } while (1);

    return idSelecionado;
}

/** Retorna o nome do papel com base no id passado como parâmetro 
 * [1-Coordenador, 2-Professor ou 3-Estudante]
 * @return nome do papel
 */
char *descreverNomePapel(short int idPapel)
{
    if (idPapel == 1)
    {
        return "Coordenador";
    }
    else if (idPapel == 2)
    {
        return "Professor";
    }

    return "Estudante";
}

/**
 * Lista todos as disciplinas de um curso específico passado no parâmetro inteiro 
 * e solicita para o usuário escolher a desejada.
 * @param idCurso o ID do curso para listar as disciplinas
 * @return id da disciplina escolhido dentre as listadas ou 0 em caso de falha
 */
int selecionarDisciplina(short int idCurso)
{
    //Teste do arquivo
    if (testarArquivo(arquivoDisciplina))
        return 0;

    int listaIds[MAX], idSelecionado = 0, idCursoLido = 0, idDisciplinaLida = 0, contador = 0;
    char nomeDisciplinaLida[MAX_DADOS];

    //Abrindo arquivo
    ponteiroArquivos = fopen(arquivoDisciplina, "r");

    printf("\n\t\t*** LISTANDO DISCIPLINAS DO CURSO ***\n");
    while (!feof(ponteiroArquivos))
    {
        idDisciplinaLida = 0;
        idCursoLido = 0;

        //Lê as linhas até o final do arquivo, atribuindo o id da linha na variável id com formato inteiro
        fscanf(ponteiroArquivos, "%d | %[^|] | %d | %[^\n]", &idDisciplinaLida, nomeDisciplinaLida, &idCursoLido, temp);

        if (idCursoLido == idCurso)
        {
            listaIds[contador] = idDisciplinaLida;
            printf("\n[%d] %s", listaIds[contador], nomeDisciplinaLida);
            contador++;
        }
    }
    fclose(ponteiroArquivos);

    // Encerra a função se não haviam disciplinas cadastrados no curso escolhido
    if (contador == 0)
    {
        printf("\n# INFO - Não há disciplinas cadastrados nesse curso\n");
        return 0;
    }

    //Solicita que o usuário selecione um ID e valida se esse ID está entre os listados (permitidos)
    do
    {
        printf("\n\n> Selecione o número que corresponde à disciplina desejada: ");
        setbuf(stdin, NULL);
        scanf("%d", &idSelecionado);
        for (int i = 0; i < contador; i++)
        {
            if (idSelecionado == listaIds[i])
            {
                return idSelecionado;
            }
        }
        printf("\n# FALHA - Esse ID não está na listagem, tente novamente informando um ID que está na lista mostrada.");
    } while (1);

    return idSelecionado;
}

/**
 * Para o programa para que o usuário possa ler mensagens antes de limpar a tela
 */
void pausarPrograma()
{
    printf("\nPressione ENTER para continuar...\n");
    //Limpa o buffer do teclado para evitar que lixo de memória (geralmente ENTER) seja lido ao invés da entrada do usuário
    setbuf(stdin, NULL);
    //Apenas uma pausa para que as informações fiquem na tela
    getchar();
    //Limpa o ENTER digitado no getchar()
    setbuf(stdin, NULL);
}

/**
 * Essa função é capaz de exibir a descrição da disciplina, alterar a descrição da disciplina e 
 * alterar o professor da disciplina.
 * @param idDisciplina ID da disciplina que deseja realizar uma das operações
 * @param verDescrição flag booleana para visualizar a descrição da disciplina
 * @param alterarDescricao flag booleana para alterar a descrição da disciplina
 * @param alterarProfessor flag booleana para alterar o professor da disciplina
 */
void operarDisciplina(int idDisciplina, short int verDescricao, short int alterarDescricao, short int alterarProfessor)
{
    //Teste do arquivo
    if (testarArquivo(arquivoDisciplina))
        return;

    //Abrindo arquivo
    ponteiroArquivos = fopen(arquivoDisciplina, "r");

    while (!feof(ponteiroArquivos))
    {
        //Lê as linhas até o final do arquivo, atribuindo o id da linha na variável id com formato inteiro
        fscanf(ponteiroArquivos, "%d | %[^|] | %d | %d | %[^\n]", &d.codigo, d.nome, &d.idCurso, &d.idProfessor, d.descricao);

        if (d.codigo == idDisciplina)
        {
            //Fechar o arquivo já que não é mais necessário
            fclose(ponteiroArquivos);

            if (verDescricao)
                printf("\n# Descrição da disciplina %s\n¬ \"%s\"\n", d.nome, d.descricao);

            if (alterarDescricao || alterarProfessor)
            {
                char linhaAntiga[MAX * 5], linhaAtualizada[MAX * 5];
                sprintf(linhaAntiga, "%d | %s| %d | %d | %s\n", d.codigo, d.nome, d.idCurso, d.idProfessor, d.descricao);
                // printf("\n§ Linha Antiga: '%s'", linhaAntiga);

                if (alterarDescricao)
                {
                    //Colher a nova descrição e validar se não está vazia
                    do
                    {
                        memset(&d.descricao[0], 0, sizeof(d.descricao));
                        printf("\n> Insira a nova descrição da disciplina [Pressione ENTER para finalizar]: ");
                        setbuf(stdin, NULL);
                        scanf("%[^\n]", d.descricao);

                        //Valida se a descrição não está vazia
                        if (!strcmp(d.descricao, ""))
                        {
                            printf("\n# FALHA - A descrição da disciplina não pode ficar vazia.\n");
                        }
                        else
                        {
                            break;
                        }
                    } while (1);
                }

                if (alterarProfessor)
                {
                    d.idProfessor = selecionarUsuario(2);
                }

                sprintf(linhaAtualizada, "%d | %s| %d | %d | %s\n", d.codigo, d.nome, d.idCurso, d.idProfessor, d.descricao);
                // printf("\n§ Linha Atualizada: '%s'", linhaAtualizada);

                atualizarLinhaArquivo(arquivoDisciplina, linhaAntiga, linhaAtualizada);
            }
            return;
        }
    }
    fclose(ponteiroArquivos);

    // Se chegar aqui não encontrou a disciplina selecionada
    printf("\n# ERRO - A disciplina solicitada não foi localizada\n");
}

/**
 * Matricula o estudante com ID passado no primeiro parâmetro na disciplina passada com ID no segundo parêmetro.
 * Criando o registro do usuário no arquivo de notas
 */
void matricularEstudanteDisciplina(int idEstudante, int idDisciplina)
{
    //Validação para caso o arquivo não possa ser aberto.
    if (testarArquivo(arquivoNotas))
        return;

    //Verificar se o estudante já está matriculado
    ponteiroArquivos = fopen(arquivoNotas, "r");
    //Passa pelas linhas verificando se encontra o id do estudante e disciplina, que serão matriculados.
    while (!feof(ponteiroArquivos))
    {
        //Lê as linhas até o final do arquivo, os dados para as variáveis da estrutura
        fscanf(ponteiroArquivos, "%d | %f | %f | %d | %d", &n.codigo, &n.nota1, &n.nota2, &n.idEstudante, &n.idDisciplina);

        if (n.idEstudante == idEstudante && n.idDisciplina == idDisciplina)
        {
            //Fechar o arquivo já que não é mais necessário
            fclose(ponteiroArquivos);
            printf("\n# FALHA - Esse estudante já está matriculado nessa disciplina!\n");
            return;
        }
    }
    fclose(ponteiroArquivos);

    /*Inicia o processo de matrícula do aluno*/
    char linhaNota[MAX]; //Variável que guarda a linha com os dados para inserir no arquivo de notas
    int codigo = pegarProximoId(arquivoNotas);

    //Abrir o arquivo com parâmetro "a" de append, não sobrescreve as informações, apenas adiciona.
    ponteiroArquivos = fopen(arquivoNotas, "a");

    //Passar dados cadastrados para a variável que será inserida no arquivo
    sprintf(linhaNota, "%d | %.2f | %.2f | %d | %d\n", codigo, -1.0, -1.0, idEstudante, idDisciplina);

    //Insere a string com todos os dados no arquivo
    if (fputs(linhaNota, ponteiroArquivos) == EOF)
    {
        perror("\n# ERRO - Problema para inserir os dados no arquivo!\n");
    }
    else
    {
        printf("\n# SUCESSO - Matrícula efetuada!\n");
    }

    fclose(ponteiroArquivos); //Fecha o arquivo
}

/**
 * Essa função pode exibir e/ou alterar as notas de determinado estudante.
 * @param idEstudante ID do estudante que deseja realizar uma das operações
 * @param idDisciplina ID da disciplina que deseja realizar uma das operações
 * @param verNotas flag booleana para exibir as notas do estudante selecionado
 * @param alterarNotas flag booleana para alterar as notas do estudante selecionado
 */
void operarNotas(int idEstudante, int idDisciplina, short int verNotas, short int alterarNotas)
{
    //Teste do arquivo
    if (testarArquivo(arquivoNotas))
        return;

    ponteiroArquivos = fopen(arquivoNotas, "r"); //Abrindo arquivo

    while (!feof(ponteiroArquivos))
    {
        //Lê as linhas até o final do arquivo, atribuindo o os valores da linha nas variáveis
        fscanf(ponteiroArquivos, "%d | %f | %f | %d | %d", &n.codigo, &n.nota1, &n.nota2, &n.idEstudante, &n.idDisciplina);

        if (n.idEstudante == idEstudante && n.idDisciplina == idDisciplina)
        {
            //Fechar o arquivo já que não é mais necessário
            fclose(ponteiroArquivos);

            if (verNotas)
            {
                if (n.nota1 < 0)
                {
                    printf("\n¬ Nota 1: -");
                }
                else
                {
                    printf("\n¬ Nota 1: %.2f", n.nota1);
                }

                if (n.nota2 < 0)
                {
                    printf("\n¬ Nota 2: -\n");
                }
                else
                {
                    printf("\n¬ Nota 2: %.2f\n", n.nota2);
                }
            }

            if (alterarNotas)
            {
                char linhaAntiga[MAX], linhaAtualizada[MAX];
                int op = 0;
                sprintf(linhaAntiga, "%d | %.2f | %.2f | %d | %d\n", n.codigo, n.nota1, n.nota2, n.idEstudante, n.idDisciplina);
                // printf("\n§ Linha Antiga: '%s'", linhaAntiga);

                printf("\n> Escolha qual nota quer alterar: ");
                printf("\n[1] Nota 1");
                printf("\n[2] Nota 2\n> ");
                //Loop para colher as notas e validar
                do
                {
                    setbuf(stdin, NULL);
                    scanf("%d", &op);

                    if (op == 1)
                    {
                        do
                        {
                            // ### - Fazer validação para quando o usuário digitar ,
                            printf("\n> Insira a nota 1...\n[A nota deve ser entre 0 e 10, números decimais devem ser escritos com . (ponto final) ao invés de , (vírgula)]\n[Caso informe valores inválidos o sistema solicitará a nota novamente]\n> ");
                            setbuf(stdin, NULL);
                            while (scanf("%f", &n.nota1) != 1)
                            {
                                printf("\n# FALHA - Ocorreu um erro na leitura. Digite novamente:\n> ");
                                setbuf(stdin, NULL);
                            };
                        } while (n.nota1 < 0 || n.nota1 > 10);
                        break;
                    }
                    else if (op == 2)
                    {
                        do
                        {
                            printf("\n> Insira a nota 2...\n[A nota deve ser entre 0 e 10, números decimais devem ser escritos com . ao invés de ,]\n[Caso informe valores inválidos o sistema solicitará a nota novamente]\n> ");
                            setbuf(stdin, NULL);
                            while (scanf("%f", &n.nota2) != 1)
                            {
                                printf("\n# FALHA - Ocorreu um erro na leitura. Digite novamente:\n> ");
                                setbuf(stdin, NULL);
                            };
                        } while (n.nota2 < 0 || n.nota2 > 10);
                        break;
                    }
                    else
                    {
                        printf("\n# FALHA - Digite o número correspondente à nota que quer alterar da lista exibida!\n> ");
                    }
                } while (1);

                sprintf(linhaAtualizada, "%d | %.2f | %.2f | %d | %d\n", n.codigo, n.nota1, n.nota2, n.idEstudante, n.idDisciplina);

                atualizarLinhaArquivo(arquivoNotas, linhaAntiga, linhaAtualizada);
            }

            return;
        }
    }
    fclose(ponteiroArquivos);

    // Se chegar aqui não encontrou a linha com o estudante e disciplina solicitados
    printf("\n# ERRO - Estudante não está matriculado na disciplina solicitada.\n");
}