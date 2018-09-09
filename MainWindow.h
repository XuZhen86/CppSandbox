#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>

#include<QWidget>
#include<QVBoxLayout>
#include<QPlainTextEdit>
#include<QPushButton>
#include<QStatusBar>
#include<QProgressBar>
#include<QLabel>
#include<QSplitter>
#include<QToolBar>
#include<QDebug>
#include<QTimer>
#include<QTemporaryFile>
#include<QTextStream>
#include<QProcess>
#include<QStringList>
#include<QShortcut>
#include<QKeySequence>
#include<QStatusBar>

class MainWindow:public QMainWindow{
        Q_OBJECT

    public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    private:
    /*Central text edit splitter*/
    QPlainTextEdit *codeBox;
    const QString CODE_BOX_SAMPLE_CODE_FILE=":/MainWindow/codeBox/SampleCode.cpp";
    const QString CODE_BOX_PLACEHOLDER_TEXT="Insert code here";
    const QString CODE_BOX_FONT="Courier";

    QPlainTextEdit *resultBox;
    const QString RESULT_BOX_PLACEHOLDER_TEXT="Compiler output / Program output";

    QString tempCppFileName,tempExecFileName;
    QProcess *compileProcess,*runProcess;
    QShortcut *compileShortcut;

    private slots:
    void slot_compileProcess_finished(int exitCode,QProcess::ExitStatus exitStatus);
    void slot_runProcess_finished(int exitCode,QProcess::ExitStatus exitStatus);
    void slot_compileShortcut_activated();
};

#endif // MAINWINDOW_H
