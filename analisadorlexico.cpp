
#include "AnalisadorLexico.h"

AnalisadorLexico::AnalisadorLexico(const QString& codigoFonte)
    : codigoFonte(codigoFonte)
{
}

bool AnalisadorLexico::analisar(
    QVector<Token>& tokens,
    QString& erro) const
{
    tokens.clear();
    erro.clear();

    const QStringList palavrasChave = {
        "class", "constructor", "function", "method",
        "field", "static", "var", "int", "char",
        "boolean", "void", "true", "false", "null",
        "this", "let", "do", "if", "else", "while",
        "return"
    };

    const QString simbolos = "{}()[].,;+-*/&|<>=~";

    const int tamanho = codigoFonte.length();

    int posicao = 0;
    int linha = 1;
    int inicioLinha = 0;

    auto ehLetra = [](QChar caractere) {
        return (caractere >= QLatin1Char('a')
                && caractere <= QLatin1Char('z'))
               || (caractere >= QLatin1Char('A')
                   && caractere <= QLatin1Char('Z'));
    };

    auto ehDigito = [](QChar caractere) {
        return caractere >= QLatin1Char('0')
        && caractere <= QLatin1Char('9');
    };

    auto ehEspaco = [](QChar caractere) {
        return caractere == QLatin1Char(' ')
        || caractere == QLatin1Char('\t')
            || caractere == QLatin1Char('\r')
            || caractere == QLatin1Char('\n');
    };

    auto adicionarToken = [&](TipoToken tipo,
                              const QString& lexema,
                              int linhaToken,
                              int colunaToken) {
        tokens.append(Token{
            tipo,
            lexema,
            linhaToken,
            colunaToken
        });
    };

    while (posicao < tamanho) {
        const QChar atual = codigoFonte.at(posicao);

        // Ignora espaços em branco e atualiza a linha.
        if (ehEspaco(atual)) {
            if (atual == QLatin1Char('\n')) {
                ++linha;
            }

            ++posicao;

            if (atual == QLatin1Char('\n')) {
                inicioLinha = posicao;
            }

            continue;
        }

        // Comentários de linha: // ...
        if (atual == QLatin1Char('/')
            && posicao + 1 < tamanho
            && codigoFonte.at(posicao + 1) == QLatin1Char('/')) {

            posicao += 2;

            while (posicao < tamanho
                   && codigoFonte.at(posicao)
                          != QLatin1Char('\n')) {
                ++posicao;
            }

            continue;
        }

        // Comentários de bloco: /* ... */
        if (atual == QLatin1Char('/')
            && posicao + 1 < tamanho
            && codigoFonte.at(posicao + 1) == QLatin1Char('*')) {

            const int linhaInicio = linha;
            posicao += 2;

            bool fechado = false;

            while (posicao < tamanho) {
                if (codigoFonte.at(posicao) == QLatin1Char('\n')) {
                    ++linha;
                    ++posicao;
                    inicioLinha = posicao;
                    continue;
                }

                if (codigoFonte.at(posicao) == QLatin1Char('*')
                    && posicao + 1 < tamanho
                    && codigoFonte.at(posicao + 1)
                           == QLatin1Char('/')) {
                    posicao += 2;
                    fechado = true;
                    break;
                }

                ++posicao;
            }

            if (!fechado) {
                erro = QString(
                           "Comentário de bloco não terminado, "
                           "iniciado na linha %1."
                           ).arg(linhaInicio);

                tokens.clear();
                return false;
            }

            continue;
        }

        const int linhaToken = linha;
        const int colunaToken = posicao - inicioLinha;

        // Constantes de texto: "..."
        if (atual == QLatin1Char('"')) {
            ++posicao;
            const int inicio = posicao;

            while (posicao < tamanho
                   && codigoFonte.at(posicao)
                          != QLatin1Char('"')
                   && codigoFonte.at(posicao)
                          != QLatin1Char('\n')
                   && codigoFonte.at(posicao)
                          != QLatin1Char('\r')) {
                ++posicao;
            }

            if (posicao >= tamanho
                || codigoFonte.at(posicao) != QLatin1Char('"')) {
                erro = QString(
                           "Constante de texto não terminada "
                           "na linha %1."
                           ).arg(linhaToken);

                tokens.clear();
                return false;
            }

            const QString lexema =
                codigoFonte.mid(inicio, posicao - inicio);

            adicionarToken(
                TipoToken::StringConstant,
                lexema,
                linhaToken,
                colunaToken
                );

            ++posicao; // Aspas finais.
            continue;
        }

        // Identificadores e palavras-chave.
        if (ehLetra(atual)) {
            const int inicio = posicao;

            while (posicao < tamanho) {
                const QChar caractere = codigoFonte.at(posicao);

                if (!ehLetra(caractere)
                    && !ehDigito(caractere)
                    && caractere != QLatin1Char('_')) {
                    break;
                }

                ++posicao;
            }

            const QString lexema =
                codigoFonte.mid(inicio, posicao - inicio);

            const TipoToken tipo =
                palavrasChave.contains(lexema)
                    ? TipoToken::Keyword
                    : TipoToken::Identifier;

            adicionarToken(
                tipo,
                lexema,
                linhaToken,
                colunaToken
                );

            continue;
        }

        // Constantes inteiras.
        if (ehDigito(atual)) {
            const int inicio = posicao;

            while (posicao < tamanho
                   && ehDigito(codigoFonte.at(posicao))) {
                ++posicao;
            }

            const QString lexema =
                codigoFonte.mid(inicio, posicao - inicio);

            bool conversaoValida = false;
            const int valor = lexema.toInt(&conversaoValida);

            if (!conversaoValida || valor > 32767) {
                erro = QString(
                           "Constante inteira fora do intervalo "
                           "permitido (0 a 32767), na linha %1: %2."
                           ).arg(linhaToken).arg(lexema);

                tokens.clear();
                return false;
            }

            adicionarToken(
                TipoToken::IntegerConstant,
                lexema,
                linhaToken,
                colunaToken
                );

            continue;
        }

        // Símbolos da linguagem Jack.
        if (simbolos.contains(atual)) {
            adicionarToken(
                TipoToken::Symbol,
                QString(atual),
                linhaToken,
                colunaToken
                );

            ++posicao;
            continue;
        }

        // Caractere não reconhecido.
        erro = QString(
                   "Caractere inválido na linha %1, coluna %2: '%3'."
                   ).arg(linhaToken)
                   .arg(colunaToken)
                   .arg(atual);

        tokens.clear();
        return false;
    }

    return true;
}
