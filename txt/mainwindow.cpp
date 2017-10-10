#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextDocument>
#include <QMessageBox>
#include <QString>
#include <QPushButton>
#include <QTextStream>
#include <QFileDialog>
#include <QCloseEvent>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QTextEdit>
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QDate>
#include <QTime>
#include <QFontDialog>
#include <QFont>
#include <QAction>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statusBar()->hide();
    setWindowTitle(tr("无标题 - 记事本"));
    ifSave = false;
    connect(ui->textEdit->document(),&QTextDocument::contentsChanged,
            this,&MainWindow::doucumentWasModified);
    //打印
    connect(ui->actionPageSet,SIGNAL(triggered()),this,SLOT(doPrintPreview()));
    connect(ui->actionPrinter,SIGNAL(triggered()),this,SLOT(doPrint()));
    //文本处理相关按键状态设置
    connect(ui->textEdit,SIGNAL(copyAvailable(bool)),ui->actionCopy,SLOT(setEnabled(bool)));
    connect(ui->textEdit,SIGNAL(copyAvailable(bool)),ui->actionCut,SLOT(setEnabled(bool)));
    connect(ui->textEdit->document(),SIGNAL(undoAvailable(bool)),ui->actionUndo,SLOT(setEnabled(bool)));
    connect(ui->textEdit,SIGNAL(copyAvailable(bool)),ui->actionDelete,SLOT(setEnabled(bool)));
    /*参数类型对不上
    connect(ui->textEdit,&QTextEdit::copyAvailable,ui->actionCut,&QTextEdit::setEnabled);
    connect(ui->textEdit,&QTextEdit::undoAvailable,ui->actionUndo,&QTextEdit::setEnabled);*/
    resize(750,750);
    //设置初始状态
    //bool hasSelection = ui->textEdit->textCursor().hasSelection();
    ui->actionCut->setEnabled(false);
    ui->actionCopy->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionDelete->setEnabled(false);
    //状态栏
    connect(ui->textEdit,SIGNAL(cursorPositionChanged()),this,SLOT(showColRow()));

    ui->textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    ui->textEdit->setWordWrapMode(QTextOption::NoWrap);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::loadFile(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this,tr("记事本"),tr("无法读取文件%1：\n%2.")
                             .arg(filename).arg(file.errorString()));
    }
    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ui->textEdit->setText(in.readAll());
    QApplication::restoreOverrideCursor();
}

void MainWindow::saveFile(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this,tr("记事本"),tr("无法写入文件%1：\n%2.")
                             .arg(filename).arg(file.errorString()));
    }
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << ui->textEdit->toPlainText();
    QApplication::restoreOverrideCursor();
    ifSave = true;
}

void MainWindow::saveAs()
{
    QString filename = QFileDialog::getSaveFileName(this,tr("另存为"),tr("*.txt"));
    if(filename.isEmpty())
        ;
    else
        saveFile(filename);
}



void MainWindow::on_actionStatus_triggered(bool checked)
{
    if(checked == true)
        statusBar()->show();
    else
        statusBar()->hide();
}


void MainWindow::on_actionNew_triggered()
{
    ifSave = false;
    if(maybeSave())
    {
        ui->textEdit->document()->clear();
        setWindowTitle(tr("无标题 - 记事本"));
    }
    curFile.clear();//新建后清空原有路径

}

bool MainWindow::maybeSave()
{
    if(ui->textEdit->document()->isModified())
    {
        QMessageBox box;
        box.setWindowTitle(tr("记事本"));
        box.setText(tr("是否将更改保存到%1").arg(windowTitle()));
        //box.setText(tr("是否将更改保存到无标题"));
        box.setIcon(QMessageBox::Warning);
        QPushButton *yesbtn = box.addButton(tr("是(&Y)"),QMessageBox::YesRole);
        QPushButton *nobtn = box.addButton(tr("否(&N)"),QMessageBox::NoRole);
        box.addButton(tr("取消"),QMessageBox::RejectRole);
        box.exec();
        if(box.clickedButton() == yesbtn)
        {
            if(!curFile.isEmpty())
                saveFile(curFile);
            else
                saveAs();
            return true;//return save();
        }
        else if(box.clickedButton() == nobtn)
            return true;
        else
            return false;
    }
    return true;
}

void MainWindow::on_actionOpen_triggered()
{
    if(ifSave)
    {
        ui->textEdit->document()->setModified(false);
    }
    if(maybeSave())
    {
        QString filename = QFileDialog::getOpenFileName(this);
        if(!filename.isEmpty())
        {
            loadFile(filename);
        }
    curFile = QFileInfo(filename).canonicalFilePath();
    if(!curFile.isEmpty())
    {
        setWindowTitle(QFileInfo(curFile).fileName());
    }
    }
}

void MainWindow::on_actionSave_triggered()
{
    if(!curFile.isEmpty())
        saveFile(curFile);
    else
        saveAs();
}

void MainWindow::on_actionSaveAs_triggered()
{
    saveAs();
}

void MainWindow::doucumentWasModified()
{
    if(ui->textEdit->document()->isModified())
        ifSave = false;//文本被更改，修改ifSave，即未保存状态
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(maybeSave())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu;
    QAction *undo = menu->addAction(tr("撤销(&U)"),ui->textEdit,&QTextEdit::undo,QKeySequence::Undo);
    undo->setEnabled(ui->textEdit->document()->isUndoAvailable());

    menu->addSeparator();

    QAction *cut = menu->addAction(tr("剪切(&T)"),ui->textEdit,&QTextEdit::cut,QKeySequence::Cut);
    cut->setEnabled(ui->textEdit->textCursor().hasSelection());

    QAction *copy = menu->addAction(tr("复制(&C)"),ui->textEdit,&QTextEdit::copy,QKeySequence::Copy);
    copy->setEnabled(ui->textEdit->textCursor().hasSelection());

    QAction *paste = menu->addAction(tr("粘贴(&P)"),ui->textEdit,&QTextEdit::paste,QKeySequence::Paste);
    paste->setEnabled(ui->textEdit->textCursor().hasSelection());

    QAction *clear = menu->addAction(tr("清空"),ui->textEdit,&QTextEdit::clear);
    clear->setEnabled(!ui->textEdit->document()->isEmpty());

    menu->addSeparator();
    QAction *selectAll = menu->addAction(tr("全选"),ui->textEdit,&QTextEdit::selectAll,QKeySequence::SelectAll);
    selectAll->setEnabled(!ui->textEdit->document()->isEmpty());

    menu->exec(event->globalPos());

    delete menu;
}

void MainWindow::on_actionExit_triggered()
{
    if(maybeSave())
        qApp->exit();
}

void MainWindow::on_actionUndo_triggered()
{
    ui->textEdit->undo();
}

void MainWindow::on_actionCut_triggered()
{
    ui->textEdit->cut();
}

void MainWindow::on_actionCopy_triggered()
{
    ui->textEdit->copy();
}

void MainWindow::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}

void MainWindow::printPreview(QPrinter *printer)
{
    ui->textEdit->print(printer);
}

void MainWindow::doPrintPreview()
{
    QPrinter printer;
    QPrintPreviewDialog preview(&printer,this);
    connect(&preview,&QPrintPreviewDialog::paintRequested,this,&MainWindow::printPreview);
    preview.exec();
}

void MainWindow::doPrint()
{
    QPrinter printer;
    QPrintDialog dlg(&printer,this);
    if(ui->textEdit->textCursor().hasSelection())
        dlg.addEnabledOption(QAbstractPrintDialog::PrintSelection);
    if(dlg.exec() == QDialog::Accepted)
    {
        ui->textEdit->print(&printer);
    }
}

void MainWindow::on_actionDelete_triggered()
{
    ui->textEdit->textCursor().removeSelectedText();
}

void MainWindow::on_actionFind_triggered()
{
    QDialog *findDialog = new QDialog(this);
    QVBoxLayout *layoutV = new QVBoxLayout(findDialog);
    QHBoxLayout *layoutH = new QHBoxLayout(findDialog);
    QPushButton *button = new QPushButton(tr("查找下一个"));
    QLabel *label = new QLabel(tr("查找内容为："));
    findLineEdit = new QLineEdit;
    layoutH->addWidget(label);
    layoutH->addWidget(findLineEdit);
    layoutV->addLayout(layoutH);
    layoutV->addWidget(button,0,Qt::AlignRight);
    findDialog->setWindowTitle(tr("查找"));
    findDialog->show();

    connect(button,&QPushButton::clicked,this,&MainWindow::findText);
}

void MainWindow::on_actionAllpick_triggered()
{
    ui->textEdit->selectAll();
}

void MainWindow::findText()
{
    QString findText = findLineEdit->text();
    if(!ui->textEdit->find(findText,QTextDocument::FindBackward))
    {
        QMessageBox::warning(this,tr("查找"),tr("找不到%1").arg(findText));
    }

}

void MainWindow::on_actionDate_triggered()
{
    QTime *currentTime = new QTime;
    QDate *currentDate = new QDate;
    ui->textEdit->setText(currentTime->currentTime().toString() + " " +
                          currentDate->currentDate().toString("yyyy/MM/dd"));

}

void MainWindow::on_actionAbout_triggered()
{
    qApp->aboutQt();
}

void MainWindow::on_actionReplace_triggered()
{
    QDialog *replaceDialog = new QDialog(this);
    findLineEdit = new QLineEdit;
    replaceLineEdit = new QLineEdit;
    QLabel *label0 = new QLabel(tr("查找内容为："));
    QLabel *label1 = new QLabel(tr("替换为："));
    QPushButton *button = new QPushButton(tr("全部替换"));
    QHBoxLayout *layout0 = new QHBoxLayout;
    QHBoxLayout *layout1 = new QHBoxLayout;
    QVBoxLayout *layout2 = new QVBoxLayout(replaceDialog);
    layout0->addWidget(label0);
    layout0->addWidget(findLineEdit);
    layout1->addWidget(label1);
    layout1->addWidget(replaceLineEdit);
    layout2->addLayout(layout0);
    layout2->addLayout(layout1);
    layout2->addWidget(button,0,Qt::AlignRight);
    replaceDialog->show();

    connect(button,&QPushButton::clicked,this,&MainWindow::replaceText);
}

void MainWindow::replaceText()
{
    QString text = findLineEdit->text();
    QString replace = replaceLineEdit->text();
    while(ui->textEdit->find(text,QTextDocument::FindBackward))
    {
        ui->textEdit->textCursor().insertText(replace);
    }
}

void MainWindow::on_actionAuto_triggered(bool checked)
{
    if(checked == true)
    {
        ui->textEdit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    }
    else
    {
        ui->textEdit->setWordWrapMode(QTextOption::NoWrap);
    }
}

void MainWindow::on_actionFont_triggered()
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok,this);
    if(ok)
    {
        ui->textEdit->setFont(font);
    }
}

void MainWindow::showColRow()
{
    row = ui->textEdit->textCursor().blockNumber() + 1;
    col = ui->textEdit->textCursor().columnNumber() + 1;
    ui->statusBar->showMessage(tr("%1行，%2列").arg(row).arg(col));
}

void MainWindow::on_actionGoto_triggered()
{
    QDialog *gotoDialog = new QDialog(this);
    QVBoxLayout *layoutV = new QVBoxLayout;
    QHBoxLayout *layoutH = new QHBoxLayout;
    QLabel *label = new QLabel(tr("行号："),gotoDialog);
    QPushButton *button1 = new QPushButton(tr("转到"));
    QPushButton *button2 = new QPushButton(tr("取消"));
    gotoLineEdit = new QLineEdit;
    layoutV->addWidget(label);
    layoutV->addWidget(gotoLineEdit);
    layoutH->addWidget(button1);
    layoutH->addWidget(button2);
    layoutH->setContentsMargins(50,0,0,0);
    layoutV->addLayout(layoutH);
    gotoDialog->setLayout(layoutV);
    gotoDialog->setWindowTitle(tr("转到指定行"));

    connect(button1,&QPushButton::clicked,this,&MainWindow::gotoRow);
    connect(button1,&QPushButton::clicked,gotoDialog,&QDialog::close);
    connect(button2,&QPushButton::clicked,gotoDialog,&QDialog::close);

    gotoDialog->exec();


}

void MainWindow::gotoRow()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::Start);
    ui->textEdit->setTextCursor(cursor);
    int times = gotoLineEdit->text().toInt() - 1;
    for(int i = 0 ; i < times ; i++)
    {
        cursor.movePosition(QTextCursor::Down);
    }
    ui->textEdit->setTextCursor(cursor);
}

