#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent){
    // Central text edit splitter
    QSplitter *textEditSplitter;
    setCentralWidget(textEditSplitter=new QSplitter(Qt::Orientation::Vertical,this));

    // Code font
    QFont codeFont(CODE_BOX_FONT);

    // Code Box
    textEditSplitter->addWidget(codeBox=new QPlainTextEdit(textEditSplitter));
    codeBox->setPlaceholderText(CODE_BOX_PLACEHOLDER_TEXT);
    codeBox->setCenterOnScroll(true);
    codeBox->setFont(codeFont);

    QFile sampleCodeFile(CODE_BOX_SAMPLE_CODE_FILE);
    if(sampleCodeFile.open(QIODevice::ReadOnly)){
        codeBox->setPlainText(sampleCodeFile.readAll());
        sampleCodeFile.close();
    }

    // Result Box
    textEditSplitter->addWidget(resultBox=new QPlainTextEdit(textEditSplitter));
    resultBox->setPlaceholderText(RESULT_BOX_PLACEHOLDER_TEXT);
    resultBox->setFont(codeFont);
    resultBox->setReadOnly(true);

    // Compiler process
    compileProcess=new QProcess(this);
    compileProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(compileProcess,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),this,&MainWindow::slot_compileProcess_finished);

    // Run process
    runProcess=new QProcess(this);
    runProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(runProcess,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),this,&MainWindow::slot_runProcess_finished);

    // compiler shortcut
    compileShortcut=new QShortcut(QKeySequence::Save,this);
    connect(compileShortcut,&QShortcut::activated,this,&MainWindow::slot_compileShortcut_activated);

    // status bar
    setStatusBar(new QStatusBar(this));
    statusBar()->showMessage("Ready");
}

MainWindow::~MainWindow(){
}

void MainWindow::slot_compileShortcut_activated(){
    compileShortcut->setEnabled(false);
    statusBar()->showMessage("Compiling...");
    qDebug()<<"slot_compileShortcut_activated()";

    QTemporaryFile tempCppFile("XXXXXX.cpp",this),tempExecFile(this);
    tempCppFile.open();
    tempExecFile.open();
    qDebug()<<tempCppFile.fileName()<<" "<<tempExecFile.fileName();

    if(!tempCppFile.isOpen()||!tempExecFile.isOpen()){
        resultBox->setPlainText("Cannot open file");
        return;
    }

    tempCppFile.setAutoRemove(false);
    tempExecFile.setAutoRemove(false);
    tempCppFileName=tempCppFile.fileName();
    tempExecFileName=tempExecFile.fileName();

    // flush data into cpp file
    QTextStream tempCppFileTS(&tempCppFile);
    tempCppFileTS<<codeBox->toPlainText();
    tempCppFileTS.flush();
    tempCppFile.close();

    // construct argument list
    QStringList argumentList;
    argumentList<<"-pipe"<<"-std=c++11"<<"-Wall"<<tempCppFileName<<"-o"<<tempExecFileName;

    // call system to compile
    compileProcess->start("g++",argumentList);

    qDebug()<<"compiler process fired";
}

void MainWindow::slot_compileProcess_finished(int exitCode,QProcess::ExitStatus exitStatus){
    statusBar()->showMessage("Compile finished");
    qDebug()<<"exitCode="<<exitCode<<" "<<exitStatus;
    resultBox->setPlainText(
                compileProcess->readAll()
                +QString("[exitCode=%1 exitStatus=%2]").arg(exitCode).arg(exitStatus));

    if(exitCode==0){
        runProcess->start(tempExecFileName);
        statusBar()->showMessage("Running...");
    }else{
        statusBar()->showMessage("Compile error");
        compileShortcut->setEnabled(true);
    }
}

void MainWindow::slot_runProcess_finished(int exitCode,QProcess::ExitStatus exitStatus){
    resultBox->setPlainText(
                runProcess->readAll()
                +QString("[exitCode=%1 exitStatus=%2]").arg(exitCode).arg(exitStatus));

    QFile tempFile(this);
    tempFile.setFileName(tempCppFileName);
    tempFile.remove();
    tempFile.setFileName(tempExecFileName);
    tempFile.remove();
    compileShortcut->setEnabled(true);
    statusBar()->showMessage("Ready");
}
