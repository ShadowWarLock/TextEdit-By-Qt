#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>

class QPrinter;
class QLineEdit;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void loadFile(const QString &filename);
    void saveFile(const QString &filename);
    void saveAs();

private slots:

    void on_actionStatus_triggered(bool checked);

    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionSaveAs_triggered();

    void doucumentWasModified();
    void on_actionExit_triggered();

    void on_actionUndo_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void printPreview(QPrinter *printer);

    void doPrint();

    void doPrintPreview();

    void on_actionDelete_triggered();

    void on_actionFind_triggered();

    void on_actionAllpick_triggered();

    void findText();

    void on_actionDate_triggered();

    void on_actionAbout_triggered();

    void on_actionReplace_triggered();

    void replaceText();

    void on_actionAuto_triggered(bool checked);

    void on_actionFont_triggered();

    void showColRow();

    void on_actionGoto_triggered();

    void gotoRow();


protected:
    void closeEvent(QCloseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    Ui::MainWindow *ui;
    bool maybeSave();
    bool ifSave;//true表示保存过
    QString curFile;//保存当前文件路径
    QLineEdit *findLineEdit;
    QLineEdit *replaceLineEdit;
    QLineEdit *gotoLineEdit;
    int col;//保存行数
    int row;//保存列数
};

#endif // MAINWINDOW_H
