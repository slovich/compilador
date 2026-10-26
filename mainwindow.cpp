#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setTabEnabled(1, false);              //desabilita aba não utilizada
    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidget_detalhes->setCurrentIndex(0);
    setWindowIcon(QIcon(":/img/assets/img/icone.png"));
    system_root = "C:/";                                //define o caminho inicial dos models
    ext << "*.jack";                                    //define as extensões dos arquivos que devem ser exibidos
    inicializarModels(system_root, ext);
    realceLexico = new RealceLexico(ui->textEdit_codigo->document());

    connect(ui->listView_file->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=](const QItemSelection &selected, const QItemSelection &deselected){
        bool temSelecao = ui->listView_file->selectionModel()->hasSelection();  // Verifica se existe pelo menos um item selecionado na lista
        ui->pushButton_selecionar->setEnabled(temSelecao);                      // Habilita ou desabilita o botão com base nisso
    });

    //Monitora a seleção do listView (para habilitar/desabilitar quando o usuário clica nos itens)
    connect(ui->listView_file->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=](const QItemSelection &selected, const QItemSelection &deselected) {
        bool temSelecao = ui->listView_file->selectionModel()->hasSelection();
        ui->pushButton_selecionar->setEnabled(temSelecao);
    });

    //Monitora quando o usuário clica/seleciona um novo diretório no treeView
    connect(ui->treeView_dir, &QTreeView::clicked, this, [=](const QModelIndex &index) {
        // Garante que sempre que trocar de diretório no treeView os botões desligam
        // até que um arquivo válido seja explicitamente focado/clicado na lista.
        ui->pushButton_selecionar->setEnabled(false);
        //Limpar totalmente a seleção anterior do listView ao trocar de pasta:
        ui->listView_file->clearSelection();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::inicializarModels(QString system_root, QStringList ext)
{
    dirmodel = new QFileSystemModel(this);
    dirmodel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dirmodel->setRootPath(system_root);                         //Define o diretório inicial da lista de arquivos pelo conteudo da 'String' caminho
    ui->treeView_dir->setModel(dirmodel);                       //Aplica o 'model' "filemodel" à 'listview' de arquivos

    filemodel = new QFileSystemModel(this);
    filemodel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    filemodel->setRootPath(system_root);
    ui->listView_file->setModel(filemodel);

    filemodel->setNameFilters(ext);                             //aplica o filtro à lista de arquivos
    filemodel->setNameFilterDisables(false);
}

/*void MainWindow::on_treeView_dir_clicked(const QModelIndex &index)
{
    QString novocaminho = dirmodel->fileInfo(index).absolutePath();
    ui->listView_file->setRootIndex(filemodel->setRootPath(novocaminho));
}*/

void MainWindow::on_treeView_dir_clicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    // Usa o caminho do próprio diretório selecionado.
    const QString novoCaminho = dirmodel->filePath(index);
    const QModelIndex indiceRaiz = filemodel->setRootPath(novoCaminho);
    ui->listView_file->setRootIndex(indiceRaiz);
}



void MainWindow::on_pushButton_selecionar_clicked()
{
    ui->tabWidget->setTabEnabled(1, false);
    QModelIndex indice = ui->listView_file->currentIndex();
    if (!indice.isValid()) {
        QMessageBox::warning(
            this,
            "Seleção de arquivo",
            "Selecione um arquivo .jack antes de continuar."
            );
        return;
    }
    //Obtém o caminho do arquivo selecionado.
    QString caminhoArquivo = filemodel->filePath(indice);
    //Abre o arquivo.
    QFile arquivo(caminhoArquivo);
    if (!arquivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(
            this,
            "Erro ao abrir arquivo",
            "Não foi possível abrir o arquivo:\n" + caminhoArquivo
            );
        return;
    }
    //Lê todo o conteúdo para um QString.
    QTextStream entrada(&arquivo);
    QString codigoFonte = entrada.readAll();
    arquivo.close();
    codigoFonteAnalisado = codigoFonte;
    //Cria o analisador com o código-fonte lido.
    AnalisadorLexico analisador(codigoFonte);
    //Executa a análise léxica.
    QString erro;
    if (!analisador.analisar(tokens, erro)) {
        QMessageBox::warning(
            this,
            "Erro léxico",
            erro
            );
        return;
    }
    // Se a análise foi concluída com sucesso.
    caminhoArquivoAnalisado = caminhoArquivo;
    gerarResumo();
    gerarCodigo();
    gerarListaTokens();
    // Habilita a exportação dos tokens analisados.
    ui->pushButton_exportar->setEnabled(true);

    // Executa a análise sintática após o sucesso da análise léxica.
    AnalisadorSintatico analisadorSintatico(tokens);
    const bool sintaxeValida = analisadorSintatico.analisar();

    // Armazena o XML e a mensagem de erro da análise sintática.
    xmlSintatico = analisadorSintatico.obterXML();
    const QString erroSintatico = analisadorSintatico.obterErro();

    if (sintaxeValida) {
        ui->tabWidget->setTabEnabled(1, true);

        // Exibe o XML produzido pelo analisador sintático.
        ui->textEdit_xml->setPlainText(xmlSintatico);
        ui->textEdit_xml->setReadOnly(true);
        // Gera a representação hierárquica da árvore sintática.
        gerarArvoreSintatica();

        QMessageBox::information(
            this,
            "Análise concluída",
            QString("Análise léxica: concluída com sucesso.\n"
                    "Análise sintática: concluída com sucesso.\n"
                    "Quantidade de tokens: %1")
                .arg(tokens.size())
            );
    } else {
        ui->tabWidget->setTabEnabled(1, false);

        QMessageBox::warning(
            this,
            "Resultado das análises",
            QString("Análise léxica: concluída com sucesso.\n"
                    "Quantidade de tokens: %1\n\n"
                    "Análise sintática: falhou.\n%2")
                .arg(tokens.size())
                .arg(erroSintatico)
            );
    }
}

void MainWindow::gerarResumo() {
    // Nome do arquivo analisado.
    QFileInfo arquivoInfo(caminhoArquivoAnalisado);
    ui->label_9->setText(arquivoInfo.fileName());
    // Situação da análise.
    ui->label_10->setText("Concluída com sucesso");
    // Quantidade de tokens por categoria.
    int totalKeywords = 0;
    int totalSimbolos = 0;
    int totalConstantesInteiras = 0;
    int totalConstantesTexto = 0;
    int totalIdentificadores = 0;
    for (const Token& token : tokens) {
        switch (token.tipo) {
        case TipoToken::Keyword:
            ++totalKeywords;
            break;

        case TipoToken::Symbol:
            ++totalSimbolos;
            break;

        case TipoToken::IntegerConstant:
            ++totalConstantesInteiras;
            break;

        case TipoToken::StringConstant:
            ++totalConstantesTexto;
            break;

        case TipoToken::Identifier:
            ++totalIdentificadores;
            break;
        }
    }
    // Total geral de tokens.
    ui->label_11->setText(QString::number(tokens.size()));
    // Quantidade de palavras-chave.
    ui->label_12->setText(QString::number(totalKeywords));
    // Quantidade de símbolos.
    ui->label_13->setText(QString::number(totalSimbolos));
    // Quantidade de constantes inteiras.
    ui->label_14->setText(QString::number(totalConstantesInteiras));
    // Quantidade de constantes de texto.
    ui->label_15->setText(QString::number(totalConstantesTexto));
    // Quantidade de identificadores.
    ui->label_16->setText(QString::number(totalIdentificadores));
}

void MainWindow::gerarCodigo() {
    // Exibe o código-fonte do arquivo analisado.
    ui->textEdit_codigo->setPlainText(codigoFonteAnalisado);
    // Impede alterações manuais no código exibido.
    ui->textEdit_codigo->setReadOnly(true);
    realceLexico->definirTokens(tokens);
    // Posiciona o cursor no início do código.
    QTextCursor cursor = ui->textEdit_codigo->textCursor();
    cursor.movePosition(QTextCursor::Start);
    ui->textEdit_codigo->setTextCursor(cursor);
}


void MainWindow::gerarListaTokens()
{
    // Cria o modelo apenas na primeira execução.
    if (modeloTokens == nullptr) {
        modeloTokens = new QStandardItemModel(this);
        ui->tableView_tokens->setModel(modeloTokens);
    }
    // Limpa os dados da análise anterior.
    modeloTokens->clear();
    // Define as colunas e os cabeçalhos.
    modeloTokens->setColumnCount(4);
    modeloTokens->setHorizontalHeaderLabels({
        "Nº",
        "Linha",
        "Categoria",
        "Lexema"
    });
    modeloTokens->setRowCount(tokens.size());
    // Preenche a tabela com os tokens analisados.
    for (int i = 0; i < tokens.size(); ++i) {
        const Token& token = tokens.at(i);
        QString categoria;
        switch (token.tipo) {
        case TipoToken::Keyword:
            categoria = "keyword";
            break;
        case TipoToken::Symbol:
            categoria = "symbol";
            break;
        case TipoToken::IntegerConstant:
            categoria = "integerConstant";
            break;
        case TipoToken::StringConstant:
            categoria = "stringConstant";
            break;
        case TipoToken::Identifier:
            categoria = "identifier";
            break;
        }
        modeloTokens->setItem(
            i, 0,
            new QStandardItem(QString::number(i + 1))
            );
        modeloTokens->setItem(
            i, 1,
            new QStandardItem(QString::number(token.linha))
            );
        modeloTokens->setItem(
            i, 2,
            new QStandardItem(categoria)
            );
        modeloTokens->setItem(
            i, 3,
            new QStandardItem(token.lexema)
            );
    }
    // Ajusta a apresentação das colunas.
    ui->tableView_tokens->setColumnWidth(0, 55);
    ui->tableView_tokens->setColumnWidth(1, 65);
    ui->tableView_tokens->setColumnWidth(2, 150);
    ui->tableView_tokens->horizontalHeader()
        ->setStretchLastSection(true);
    // Impede a edição manual dos dados.
    ui->tableView_tokens->setEditTriggers(
        QAbstractItemView::NoEditTriggers
        );
    // Seleciona linhas inteiras.
    ui->tableView_tokens->setSelectionBehavior(
        QAbstractItemView::SelectRows
        );
    // Permite selecionar apenas uma linha por vez.
    ui->tableView_tokens->setSelectionMode(
        QAbstractItemView::SingleSelection
        );
}

void MainWindow::on_pushButton_exportar_clicked()
{
    // Verifica se existe uma análise válida para exportar.
    if (tokens.isEmpty() || caminhoArquivoAnalisado.isEmpty()) {
        QMessageBox::warning(
            this,
            "Exportação XML",
            "Não existem tokens disponíveis para exportação."
            );
        return;
    }

    // Sugere o nome do XML com base no arquivo Jack analisado.
    QFileInfo arquivoInfo(caminhoArquivoAnalisado);

    QString nomeSugerido =
        arquivoInfo.absolutePath()
        + "/"
        + arquivoInfo.completeBaseName()
        + ".xml";

    // Solicita ao usuário o destino do arquivo.
    QString caminhoXML = QFileDialog::getSaveFileName(
        this,
        "Salvar arquivo XML",
        nomeSugerido,
        "Arquivos XML (*.xml)"
        );

    // O usuário cancelou a janela.
    if (caminhoXML.isEmpty()) {
        return;
    }

    // Acrescenta a extensão .xml, se necessário.
    if (!caminhoXML.endsWith(".xml", Qt::CaseInsensitive)) {
        caminhoXML += ".xml";
    }

    // Abre o arquivo de destino.
    QFile arquivoXML(caminhoXML);

    if (!arquivoXML.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(
            this,
            "Erro na exportação",
            "Não foi possível criar o arquivo XML:\n"
                + caminhoXML
            );
        return;
    }

    // Configura o escritor XML.
    QXmlStreamWriter xml(&arquivoXML);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);

    // Elemento raiz exigido pelo formato de tokens.
    xml.writeStartElement("tokens");

    // Escreve cada token na ordem em que foi reconhecido.
    for (const Token& token : tokens) {
        QString categoria;

        switch (token.tipo) {
        case TipoToken::Keyword:
            categoria = "keyword";
            break;

        case TipoToken::Symbol:
            categoria = "symbol";
            break;

        case TipoToken::IntegerConstant:
            categoria = "integerConstant";
            break;

        case TipoToken::StringConstant:
            categoria = "stringConstant";
            break;

        case TipoToken::Identifier:
            categoria = "identifier";
            break;
        }

        xml.writeStartElement(categoria);

        // O formato nand2tetris exige espaços ao redor do lexema.
        xml.writeCharacters(" " + token.lexema + " ");

        xml.writeEndElement();
    }

    xml.writeEndElement(); // </tokens>

    arquivoXML.close();

    // Confirma o resultado da gravação.
    if (arquivoXML.error() != QFileDevice::NoError) {
        QMessageBox::critical(
            this,
            "Erro na exportação",
            "Ocorreu um erro ao gravar o arquivo XML."
            );
        return;
    }

    QMessageBox::information(
        this,
        "Exportação concluída",
        "O arquivo XML foi salvo com sucesso em:\n"
            + caminhoXML
        );
}


void MainWindow::on_actionSair_2_triggered()
{
    this->close();
}


void MainWindow::on_actionSobre_triggered()
{
    DialogSobre *sobre =  new DialogSobre;
    sobre->show();
}


void MainWindow::on_pushButton_exportar_xml_sintatico_clicked()
{

}

void MainWindow::gerarArvoreSintatica()
{
    // Cria o modelo da árvore na primeira execução.
    if (modeloArvoreSintatica == nullptr) {
        modeloArvoreSintatica = new QStandardItemModel(this);
        ui->treeView_xml->setModel(modeloArvoreSintatica);
    }

    // Limpa os dados da análise anterior.
    modeloArvoreSintatica->clear();

    // Define o cabeçalho da árvore.
    modeloArvoreSintatica->setHorizontalHeaderLabels(
        QStringList() << "Estrutura sintática"
        );

    // Prepara a leitura do XML produzido pelo analisador sintático.
    QXmlStreamReader leitor(xmlSintatico);

    // Mantém o item correspondente ao elemento XML atual.
    QStack<QStandardItem *> pilha;

    while (!leitor.atEnd()) {
        leitor.readNext();

        if (leitor.isStartElement()) {
            // Cria um item com o nome da tag XML.
            QStandardItem *item =
                new QStandardItem(leitor.name().toString());

            item->setEditable(false);

            // Insere o item na posição hierárquica correta.
            if (pilha.isEmpty()) {
                modeloArvoreSintatica->appendRow(item);
            } else {
                pilha.top()->appendRow(item);
            }

            // O elemento recém-criado passa a ser o elemento atual.
            pilha.push(item);

        } else if (leitor.isCharacters()
                   && !leitor.text().toString().trimmed().isEmpty()) {

            // Cria um item para o conteúdo textual da tag.
            QStandardItem *itemTexto =
                new QStandardItem(leitor.text().toString().trimmed());

            itemTexto->setEditable(false);

            if (!pilha.isEmpty()) {
                pilha.top()->appendRow(itemTexto);
            }

        } else if (leitor.isEndElement()) {
            // Finaliza o elemento atual.
            if (!pilha.isEmpty()) {
                pilha.pop();
            }
        }
    }

    // Ajusta a apresentação da árvore.
    ui->treeView_xml->setEditTriggers(
        QAbstractItemView::NoEditTriggers
        );

    ui->treeView_xml->setUniformRowHeights(true);
    ui->treeView_xml->expandAll();
}

