#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, QString arg) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    new SubSyntaxHighligther(ui->textEdit->document());
    createActions();
    createStatusBar();

    readSettings();

    connect(ui->textEdit->document(), &QTextDocument::contentsChanged,
            this, &MainWindow::documentWasModified);

#ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &MainWindow::commitData);
#endif

    setCurrentFile(QString());
    setUnifiedTitleAndToolBarOnMac(true);

    if(!arg.isEmpty()) {
        loadFile(arg);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        ui->textEdit->clear();
        setCurrentFile(QString());
    }
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this,"Load subtitle file",currentDir,
                                                        "SUB files (*.srt *.sub);;Text files (*.txt)");
        if (!fileName.isEmpty()) {
            loadFile(fileName);
        }
    }
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QString file = QFileDialog::getSaveFileName(this,"Save as subtitle file",currentDir,
                                 "SUB files (*.srt *.sub);;Text files (*.txt)");
    if (file.isEmpty())
        return false;
    return saveFile(file);
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Edit Subtitle"),
            tr("<html><head/><body>"
               "<p>Programmed and designed by:</p>"
               "<p><b>Frank Rodríguez Siret</b></p>"
               "<p>Email: <a href=\"mailto:frank.siret@gmail.com\">frank.siret@gmail.com</a></p>"
               "</body></html>"));
}

void MainWindow::documentWasModified()
{
    setWindowModified(ui->textEdit->document()->isModified());
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Save &As..."), this, &MainWindow::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));

#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
    QAction *cutAct = new QAction(cutIcon, tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, ui->textEdit, &QTextEdit::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png"));
    QAction *copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, ui->textEdit, &QTextEdit::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png"));
    QAction *pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, ui->textEdit, &QTextEdit::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);

    menuBar()->addSeparator();
#endif // !QT_NO_CLIPBOARD

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the Edit Subtitle's About box"));

//    QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
//    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));

    #ifndef QT_NO_CLIPBOARD
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(ui->textEdit, &QTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(ui->textEdit, &QTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
    #endif // !QT_NO_CLIPBOARD
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    settings = new QSettings(QCoreApplication::applicationDirPath() + "\\config.ini", QSettings::IniFormat);
    currentDir = settings->value("currentDir", qApp->applicationDirPath()).toString();
}

void MainWindow::writeSettings()
{
    settings->setValue("currentDir", currentDir);
}

bool MainWindow::maybeSave()
{
    if (!ui->textEdit->document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    currentDir = QDir(fileName).absolutePath();

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    QString t = in.readAll();
    ui->textEdit->setPlainText(t);

    QRegExp r = QRegExp("(?:[ ]*)([0-9]+)(?:[ ]*\n[ ]*)([0-9]+)(?::)([0-9]+)(?::)([0-9]+)(?:,)([0-9]+)"
                        "(?:[ ]*-->[ ]*)([0-9]+)(?::)([0-9]+)(?::)([0-9]+)(?:,)([0-9]+)(?:[ ]*)");

    QStringList text = t.split(r);
    int i=1;
    int index = t.indexOf(r);
    while (index >= 0) {
        int length = r.matchedLength();
        DB << r.capturedTexts();
        DB << text[i++];
        index = t.indexOf(r, index + length);
    }


#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << ui->textEdit->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    ui->textEdit->document()->setModified(false);
    setWindowModified(false);

    QString title = curFile;
    if (curFile.isEmpty())
        title = "untitled.srt";
    setWindowFilePath(title);
}

#ifndef QT_NO_SESSIONMANAGER
void MainWindow::commitData(QSessionManager &manager)
{
    if (manager.allowsInteraction()) {
        if (!maybeSave())
            manager.cancel();
    } else {
        // Non-interactive: save without asking
        if (ui->textEdit->document()->isModified())
            save();
    }
}
#endif

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_apply_clicked()
{

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    QString t = ui->textEdit->document()->toPlainText();
    QRegExp r = QRegExp("[0-9]+:[0-9]+:[0-9]+,[0-9]+");
    QTime time = ui->time->time();

    int index = t.indexOf(r);
    while (index >= 0) {
        int length = r.matchedLength();

        QString p = t.section("",index + 1,index + length);

        int chh  = p.section(":",0,0).toInt();
        int cmm  = p.section(":",1,1).toInt();
        int css  = p.section(":",2).section(",",0,0).toInt();
        int czzz = p.section(",",1).toInt();

        int thh  = time.hour();
        int tmm  = time.minute();
        int tss  = time.second();
        int tzzz = time.msec();

        int s = ui->up->isChecked() ? 1 : -1;

        int tmp = czzz + s * tzzz;
        int nzzz = (tmp + 1000) % 1000;

        if(tmp < 0 || tmp >= 1000) tmp = 1;
        else tmp = 0;

        tmp = css + s * (tss + tmp);
        int nss  = (tmp + 60) % 60;

        if(tmp < 0 || tmp >= 60) tmp = 1;
        else tmp = 0;

        tmp = cmm + s * (tmm + tmp);
        int nmm  = (tmp + 60) % 60;

        if(tmp < 0 || tmp >= 60) tmp = 1;
        else tmp = 0;

        tmp = chh + s * (thh + tmp);
        int nhh  = tmp;

        if(nhh < 0)
            break;

        QTime tn = QTime(nhh, nmm, nss, nzzz);

        t.replace(index, length, tn.toString("hh:mm:ss,zzz"));

        index = t.indexOf(r, index + length);
    }

    ui->textEdit->document()->setPlainText(t);

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

}
