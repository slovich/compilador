#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QTextCursor>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>
#include <QXmlStreamWriter>
#include <Qstack>
#include "dialogsobre.h"
#include "analisadorlexico.h"
#include "analisadorsintatico.h"
#include "token.h"
#include "realcelexico.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void inicializarModels(QString, QStringList);
    void gerarResumo();
    void gerarCodigo();
    void gerarListaTokens();

private slots:
    void on_treeView_dir_clicked(const QModelIndex &index);
    void on_pushButton_selecionar_clicked();
    void on_pushButton_exportar_clicked();
    void on_pushButton_exportar_xml_sintatico_clicked();
    void on_actionSair_2_triggered();    
    void on_actionSobre_triggered();

private:
    void gerarArvoreSintatica();
    Ui::MainWindow *ui;
    QString system_root, caminhoArquivoAnalisado, codigoFonteAnalisado, xmlSintatico;
    QStringList ext;
    QFileSystemModel *dirmodel, *filemodel;
    QVector<Token> tokens;
    RealceLexico *realceLexico;
    QStandardItemModel *modeloTokens = nullptr, *modeloArvoreSintatica = nullptr;
};
#endif // MAINWINDOW_H
