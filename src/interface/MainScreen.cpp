#include "MainScreen.h"
#include <QMenuBar>
#include <QMenu>
#include <QIcon>
#include <QTabWidget>
#include <QLabel>
#include <QShortcut>
#include <QFileDialog>
#include <QStatusBar>
#include <QMessageBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include "../manager/Manager.h"
#include <boost/filesystem/path.hpp>

MainScreen::~MainScreen()
{
   delete navbar;
   delete status;
   delete tabs;
}

MainScreen::MainScreen(QMainWindow* parent) :QMainWindow(parent), manager(nullptr)
{
	//setFixedSize(1920, 1020);
   setMinimumSize(800,600);
   setWindowTitle(QString("AudacesCAD"));
	showMaximized();

	navbar = menuBar();	

   status = statusBar();
   status->show();

   tabs = new QTabWidget(this);
   setCentralWidget(tabs);
   tabs->setTabsClosable(true);
   tabs->setMovable(true);
   tabs->setVisible(false);

   connect(tabs,&QTabWidget::tabCloseRequested,this,&MainScreen::closeTabDialog);
   connect(tabs,&QTabWidget::tabBarClicked,this,&MainScreen::tabChangedSignal);

	QMenu* file = new QMenu("File");
	QAction* line = new QAction("Line");
	QAction* bezier = new QAction("Bezier");
	QAction* arc = new QAction("Arc");	
	
   QAction* newFile = new QAction("New");
	QAction* open = new QAction("Open");
	QAction* save = new QAction("Save");
	QAction* clear = new QAction("Clear");
	QAction* close = new QAction("Close");

   file->addAction(newFile);
	file->addAction(open);
	file->addAction(save);
	file->addAction(clear);
	file->addAction(close);

   QIcon* iconNew = new QIcon("C:/Users/lucas.picollo/Documents/Projects/QT- Setup/icons/new.png");
	QIcon* iconSave = new QIcon("C:/Users/lucas.picollo/Documents/Projects/QT- Setup/icons/save.png");
	QIcon* iconClear = new QIcon("C:/Users/lucas.picollo/Documents/Projects/QT- Setup/icons/clean.png");
	QIcon* iconClose = new QIcon("C:/Users/lucas.picollo/Documents/Projects/QT- Setup/icons/close.png");
	QIcon* iconOpen = new QIcon("C:/Users/lucas.picollo/Documents/Projects/QT- Setup/icons/open.png");

   newFile->setIcon(*iconNew);
	open->setIcon(*iconOpen);
	save->setIcon(*iconSave);
	clear->setIcon(*iconClear);
	close->setIcon(*iconClose);

	navbar->addMenu(file);
	navbar->addAction(line);
	navbar->addAction(bezier);
	navbar->addAction(arc);

	connect(line, &QAction::triggered, this, &MainScreen::lineCommandSignal);
	connect(bezier, &QAction::triggered, this, &MainScreen::bezierCommandSignal);
	connect(arc, &QAction::triggered, this, &MainScreen::archCommandSignal);

   connect(newFile,&QAction::triggered,this,&MainScreen::newFile);
	connect(open, &QAction::triggered, this,  &MainScreen::loadFile);
	connect(save, &QAction::triggered, this,  &MainScreen::saveFile);
	connect(clear, &QAction::triggered, this, &MainScreen::clearTab);
	connect(close, &QAction::triggered, this, &MainScreen::closeFile);
}

////////////////////////////////////////////////////////////////////////////////

void MainScreen::start()
{
	show();
}

////////////////////////////////////////////////////////////////////////////////

void MainScreen::newFile()
{
   manager->newFile();
}

void MainScreen::loadFile()
{
   manager->openFile();
}

void MainScreen::saveFile()
{
   manager->saveFile();
}

void MainScreen::discardFile(int tabIndex)
{
   manager->discardFile(tabIndex);
}

void MainScreen::closeFile()
{
   manager->closeFile();
}


////////////////////////////////////////////////////////////////////////////////

std::string MainScreen::getFileName(std::string pathFile)
{
   boost::filesystem::path path(pathFile);

   std::string arq = path.stem().string();

   if (arq == ".")
      arq = "";
  
   return arq;
}

void MainScreen::addTab(View* view, std::string name)
{
   tabs->addTab(view,QString::fromStdString(name));
   tabs->show();
}

void MainScreen::closeTabDialog()
{
   View* view = dynamic_cast<View*>(tabs->widget(tabs->tabPosition()));

//   if ( !view->getFile().getSaved())
  // {      
      QMessageBox warning;
      warning.setText("The document has been modified.");
      warning.setInformativeText("Do you want to save your changes?");
      warning.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
      warning.setDefaultButton(QMessageBox::Save);
      warning.setIcon(QMessageBox::Question);

      const int option = warning.exec();

      switch (option)
      {
      case QMessageBox::Save: saveFile(); break;
      case QMessageBox::Discard: discardFile(tabs->tabPosition()); break;
      }
   //}     

      if (tabs->count() == 0)
         tabs->setVisible(false);
}

void MainScreen::tabChangedSignal()
{
   manager->setCurrentFileByTab(dynamic_cast<View*>(tabs->currentWidget()));
}

void MainScreen::clearTab()
{
   manager->clearAllItems();
}

////////////////////////////////////////////////////////////////////////////////

void MainScreen::errorMessage()
{
   QMessageBox::warning(this,tr("Fatal error"),tr("An error occurred while processing your request."),QMessageBox::Ok);
}

void MainScreen::successMessage()
{
   QMessageBox::information(this,tr("Good news"),tr("Successful request."),QMessageBox::Ok);
}

STRUCTNEWFILE MainScreen::newFileDialog()
{
   QDialog dialog(this);
   dialog.setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowMaximizeButtonHint & ~Qt::WindowMinimizeButtonHint);
   dialog.setWindowTitle(QString("Create new file"));
   QFormLayout form(&dialog);

   QLineEdit* lName = new QLineEdit(&dialog);
   form.addRow(new QLabel("File name:"),lName);

   QLineEdit* wEdit = new QLineEdit(&dialog);
   wEdit->setText(QString("800"));
   form.addRow(new QLabel("Width"),wEdit);

   QLineEdit* hEdit = new QLineEdit(&dialog);
   hEdit->setText(QString("600"));
   form.addRow(new QLabel("Height"),hEdit);   

   QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,Qt::Horizontal,&dialog);
   form.addRow(&buttonBox);

   connect(&buttonBox,SIGNAL(accepted()),&dialog,SLOT(accept()));
   connect(&buttonBox,SIGNAL(rejected()),&dialog,SLOT(reject()));
   
   int exec = dialog.exec();

   std::string name = lName->text().toStdString();
   int height = hEdit->text().toInt();
   int width = wEdit->text().toInt();

   return STRUCTNEWFILE(exec,name,width,height);
}

////////////////////////////////////////////////////////////////////////////////

void MainScreen::setStatusMessage(Instruction in)
{
   switch (in)
   {
   case 0: status->showMessage("Waiting for a click on the screen."); break;
   case 1: status->showMessage("Keep pressed and move the cursor. (Release to finish)"); break;
   case 2: status->showMessage("Click to set the draw's length."); break;
   }
}

void MainScreen::lineCommandSignal()
{
   manager->startLineCommand();
}

void MainScreen::bezierCommandSignal()
{
   manager->startBezierCommand();
}

void MainScreen::archCommandSignal()
{
   manager->startArchCommand();
}
