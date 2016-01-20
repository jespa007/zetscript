#pragma once

#include <QMainWindow>
#include <QGroupBox>
#include "CGraphViewer.h"

 class QAction;
 class QMenu;
 class QPlainTextEdit;
 class QVBoxLayout;

 class CMainWindow : public QMainWindow
 {
     Q_OBJECT

 public:
     CMainWindow();

 protected:
     void closeEvent(QCloseEvent *event);

 private slots:
     void newFile();
     void open();
     bool save();
     bool saveAs();
     void about();
     void documentWasModified();

 private:
     void createActions();
     void createMenus();
     void createToolBars();
     void createStatusBar();
     void readSettings();
     void writeSettings();
     bool maybeSave();
     void loadFile(const QString &fileName);
     bool saveFile(const QString &fileName);
     void setCurrentFile(const QString &fileName);
     QString strippedName(const QString &fullFileName);

     QPlainTextEdit *textEdit;
     QString curFile;

     QMenu *fileMenu;
     QMenu *editMenu;
     QMenu *helpMenu;
     QToolBar *fileToolBar;
     QToolBar *editToolBar;
     QAction *newAct;
     QAction *openAct;
     QAction *saveAct;
     QAction *saveAsAct;
     QAction *exitAct;
     QAction *cutAct;
     QAction *copyAct;
     QAction *pasteAct;
     QAction *aboutAct;
     QAction *aboutQtAct;
     //----- new
     CGraphViewer *sampleData;
     CGraphViewer *frecuencyData;
     QGroupBox  *nordGroupBox;
 };


