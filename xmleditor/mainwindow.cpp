#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "form.h"
#include <QShortcut>
#include "string"
#include <vector>
#include "tree.h"
#include <sstream>
#include <string>
#include <map>
 #include <iostream>
#include <iterator>
#include <vector>


template <typename Iterator>
Iterator compress(const std::string &uncompressed, Iterator result) {
  int dictSize = 256;

  std::map<std::string,int> dictionary;
  for (int i = 0; i < 256; i++)
    dictionary[std::string(1, i)] = i;

  std::string w;
  for (std::string::const_iterator it = uncompressed.begin();
       it != uncompressed.end(); ++it) {
    char c = *it;
    std::string wc = w + c;
    if (dictionary.count(wc))
      w = wc;
    else {
      *result++ = dictionary[w];
      dictionary[wc] = dictSize++;
      w = std::string(1, c);
    }
  }

  if (!w.empty())
    *result++ = dictionary[w];
  return result;
}


template <typename Iterator>
std::string decompress(Iterator begin, Iterator end) {
  // Build the dictionary.
  int dictSize = 256;
  std::map<int,std::string> dictionary;
  for (int i = 0; i < 256; i++)
    dictionary[i] = std::string(1, i);

  std::string w(1, *begin++);
  std::string result = w;
  std::string entry;
  for ( ; begin != end; begin++) {
    int k = *begin;
    if (dictionary.count(k))
      entry = dictionary[k];
    else if (k == dictSize)
      entry = w + w[0];
    else
      throw "Bad compressed k";

    result += entry;

    dictionary[dictSize++] = w + entry[0];

    w = entry;
  }
  return result;
}



void get_path(QString in, QStringList files, QString &out){
    QString filename="";
    QStringList sp;
    for(int i  = 0; i < files.count();i++){
        sp = files[i].split('/');
        if(sp[sp.count()-1] == in){
            filename = files[i];
            break;
        }
    }
    out = filename;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model.setRootPath(dir.currentPath());
    ui->treeView->setModel(&model);
    for(int i = 1; i<model.columnCount();i++){
        ui->treeView->hideColumn(i);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionopen_file_triggered()
{
    QStringList name;
    QString filename = QFileDialog::getOpenFileName(this,"Open file name", "C:\\","*.txt *.xml *.json");
    for(int i =0; i<files.count();i++){
        if(filename == files[i]){
            name = filename.split('/');
            for(int j = 0; i<ui->tabWidget->count();i++){
                if(ui->tabWidget->tabText(j) ==  name[name.count()-1]){
                    ui->tabWidget->setCurrentIndex(j);
                }
            }
            //QMessageBox::warning(this,"Warning","File if opned" );
            return;
        }
    }
    QFile file(filename);
    currentfile = filename;

    if(!file.open(QIODevice::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning","Can't open file : "+ file.errorString() );
        return;
    }
    files.append(filename);
    QTextStream in(&file);
    QString text = in.readAll();
    name = filename.split('/');

    //QTextEdit* plainTextEdit = new QTextEdit();
    //Form* form = new Form();
    QTextEdit* plainTextEdit = new QTextEdit();
    plainTextEdit->setTabStopDistance(30);
    ui->tabWidget->addTab(plainTextEdit, name[name.count()-1]);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
    plainTextEdit = (QTextEdit*) ui->tabWidget->widget(ui->tabWidget->currentIndex()); // for the first tab
    plainTextEdit->setText(text);
    /*ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
    ui->textEdit->setText(text);*/
    file.close();
}

void MainWindow::on_actionsave_as_triggered()
{
    int ind = ui->tabWidget->currentIndex();
    QString currentTabText = ui->tabWidget->tabText(ind);
    QString filename="";
    get_path(currentTabText,files, filename);
    filename[filename.count()-currentTabText.count()] = '\0';
    filename = QFileDialog::getSaveFileName(this,"Save as", filename,"*.txt *.xml *.json");
    QFile file(filename);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning","Can't save file : "+ file.errorString() );
        return;
    }
    currentfile = filename;
    QTextStream out(&file);
    QTextEdit* plainTextEdit = (QTextEdit*) ui->tabWidget->widget(ui->tabWidget->currentIndex()); // for the first tab
    QString text  =  plainTextEdit->toPlainText();
    out<<text;
    file.close();
}


void MainWindow::on_actionexit_triggered()
{
    QApplication::quit();

}


void MainWindow::on_actioncopy_triggered()
{
    ui->textEdit->copy();
}


void MainWindow::on_actionpaste_triggered()
{
    ui->textEdit->paste();
}


void MainWindow::on_actioncut_triggered()
{
    ui->textEdit->cut();
}


void MainWindow::on_actionredo_triggered()
{
    ui->textEdit->redo();
}



void MainWindow::on_actionundo_triggered()
{
    ui->textEdit->undo();
}


void MainWindow::on_actionsave_triggered()
{
    QString filename = "";
    int ind = ui->tabWidget->currentIndex();
    QString currentTabText = ui->tabWidget->tabText(ind);
    get_path(currentTabText, files, filename);
    QStringList sp;
    /*
    for(int i  = 0; i < files.count();i++){
        sp = files[i].split('/');
        if(sp[sp.count()-1] == currentTabText){
            filename = files[i];
            break;
        }
    }
    */
    //filename = currentfile;
    QFile file(filename);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning","Can't save file : "+ file.errorString() );
        return;
    }
    currentfile = filename;
    QTextStream out(&file);
    QTextEdit* plainTextEdit = (QTextEdit*) ui->tabWidget->widget(ui->tabWidget->currentIndex());
    QString text = plainTextEdit->toPlainText();
    out<<text;
    file.close();
}




void MainWindow::on_treeView_activated(const QModelIndex &index)
{
    QStringList name;
    QString filename = model.filePath(index);
    for(int i =0; i<files.count();i++){
        if(filename == files[i]){
           // QList child = ;
            //QString currentTabText = ui->tabWidget->tabText(ind);
            name = filename.split('/');
            for(int j = 0; j<ui->tabWidget->count();j++){
                if(ui->tabWidget->tabText(j) ==  name[name.count()-1]){
                    ui->tabWidget->setCurrentIndex(j);
                }
            }
            //QMessageBox::warning(this,"Warning","File if opned" );
            return;
        }
    }


    QFile file(filename);
    currentfile = filename;
    if(!file.open(QIODevice::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning","Can't open file : "+ file.errorString() );
    }
    files.append(filename);
    QTextStream in(&file);
    QString text = in.readAll();
    name = filename.split('/');
    QTextEdit* plainTextEdit = new QTextEdit();
    plainTextEdit->setTabStopDistance(30);
    ui->tabWidget->addTab(plainTextEdit, name[name.count()-1]);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
    plainTextEdit = (QTextEdit*) ui->tabWidget->widget(ui->tabWidget->currentIndex()); // for the first tab
    plainTextEdit->setText(text);
    //ui->textEdit->setText(text);
    file.close();

}




void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    int ind = ui->tabWidget->currentIndex();
    QString currentTabText = ui->tabWidget->tabText(ind);
    QStringList sp;
    for(int i  = 0; i < files.count();i++){
        sp = files[i].split('/');
        if(sp[sp.count()-1] == currentTabText){
            // if we want to save file before closing the tab uncomment the next line
            //on_actionsave_triggered();
            files.removeAt(i);
            break;
        }
    }
    ui->tabWidget->removeTab(index);
}



void MainWindow::on_actionnew_file_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this,"Save as");
    QFile file(filename);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning","Can't save file : "+ file.errorString() );
        return;
    }
    files.append(filename);
    QStringList name = filename.split('/');
    QTextEdit* plainTextEdit = new QTextEdit();
    plainTextEdit->setTabStopDistance(30);
    ui->tabWidget->addTab(plainTextEdit, name[name.count()-1]);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
    file.close();
}

void MainWindow::on_actiontojson_triggered()
{
    QString filename = "";
    int ind = ui->tabWidget->currentIndex();
    QString currentTabText = ui->tabWidget->tabText(ind);

    QString newtext;
    QTextEdit* plainTextEdit = (QTextEdit*) ui->tabWidget->widget(ind);
    QString text  =  plainTextEdit->toPlainText();
    get_path(currentTabText, files, filename);
    filename.replace(filename.indexOf('.',0)+1,filename.count()-filename.indexOf('.',0)-1,"json");
    QFile file(filename);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,"Warning","Can't create file : "+ file.errorString() );
        return;
    }
    files.append(filename);
    QStringList name = filename.split('/');
    plainTextEdit = new QTextEdit();
    plainTextEdit->setTabStopDistance(30);
    ui->tabWidget->addTab(plainTextEdit, name[name.count()-1]);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
    XML_Tree tt;
    tt.Fill(text);
    //tt.print(tt.GetHead(),newtext, 0);
    Node * node = tt.GetHead();
    newtext+="{\n";
    for(int i = 0; i < node->Children.count();i++){
        if(node->Children[i]->type =="comment") continue;
        tt.XMLtoJSON(node->Children[i],1,newtext);
        if(i!=node->Children.count()-1){
            newtext+=",\n";
        }
    }
    newtext+="\n}";
    plainTextEdit = (QTextEdit*) ui->tabWidget->widget(ui->tabWidget->currentIndex()); // for the first tab
    plainTextEdit->setText(newtext);

    file.close();

}


void MainWindow::on_actionopen_folder_triggered(){

}

void MainWindow::on_actionopen_folder_2_triggered(){

}
void MainWindow::on_actionto_json_triggered(){

}

void MainWindow::on_actionCheck_Error_triggered()
{
    //QString filename = QFileDialog::getSaveFileName(this,"Save as");
    QTextEdit* plainTextEdit = (QTextEdit*) ui->tabWidget->widget(ui->tabWidget->currentIndex());
    QString text  =  plainTextEdit->toPlainText();
    QString out;
    XML_Tree tt;
    tt.Fill(text);
    QVector<QVector<int>>ind;
    Node * node = tt.GetHead();
    for(int i=0;i<node->Children.count();i++){
        tt.CheckError(node->Children[i],out,0,ind);
    }
    while(out[out.count()-1]=='\n'){
        out = out.mid(0,out.count()-1);
    }
    //tt.print(tt.GetHead(),out,0);
    plainTextEdit->setText(out);
    int begin = 0;
    int end = 0;
    QTextCharFormat fmt;
    fmt.setBackground(Qt::red);
    QTextCursor cursor(plainTextEdit->document());
    for(int i =0;i<ind.count();i++){
        begin = ind[i][0];
        end   = begin+ind[i][1];
        cursor.setPosition(begin, QTextCursor::MoveAnchor);
        cursor.setPosition(end, QTextCursor::KeepAnchor);
        cursor.setCharFormat(fmt);
    }
    QMessageBox::warning(this, "Error", "Found "+ QString::number(ind.count()) +" Errors.");
            //warning(this,"Warning","Can't save file : "+ file.errorString() );
}


void MainWindow::on_actionFormat_triggered()
{

        QTextEdit* plainTextEdit = (QTextEdit*) ui->tabWidget->widget(ui->tabWidget->currentIndex());
        QString text  =  plainTextEdit->toPlainText();
        QString final_text;
        QString out;

        XML_Tree tt;
        tt.Fill(text);
        Node * node = tt.GetHead();
        for(int i = 0; i <node->Children.count(); i++){
            tt.print(node->Children[i], final_text,0);
        }
        while(final_text[final_text.count()-1]=='\n'){
            final_text = final_text.mid(0,final_text.count()-1);
        }

         plainTextEdit = (QTextEdit*) ui->tabWidget->widget(ui->tabWidget->currentIndex()); // for the first tab
         plainTextEdit->setText(final_text);

}


void MainWindow::on_actionminify_triggered()
{
    QTextEdit* plainTextEdit = (QTextEdit*) ui->tabWidget->widget(ui->tabWidget->currentIndex());
    QString text  =  plainTextEdit->toPlainText();

    QString final_text;
    QStringList data = text.split('\n');
    XML_Tree tt;
    tt.Fill(text);
    Node * node = tt.GetHead();
    for(int i =0; i<node->Children.count(); i++){
        tt.minify(node->Children[i],final_text);
    }
     plainTextEdit = (QTextEdit*) ui->tabWidget->widget(ui->tabWidget->currentIndex()); // for the first tab
     plainTextEdit->setText(final_text);
}


void MainWindow::on_actioncorrect_errors_triggered()
{
    QTextEdit* plainTextEdit = (QTextEdit*) ui->tabWidget->widget(ui->tabWidget->currentIndex());
    QString text  =  plainTextEdit->toPlainText();
    QString out;
    XML_Tree tt;
    tt.Fill(text);
    tt.CorrectError(tt.GetHead());

    Node* node = tt.GetHead();
    for(int i = 0; i <node->Children.count(); i++){
        tt.print(node->Children[i], out,0);
    }
    while(out[out.count()-1]=='\n'){
        out = out.mid(0,out.count()-1);
    }
    plainTextEdit->setText(out);
    QMessageBox::information(this, "Correct", "All Errors are handled.");

}


void MainWindow::on_actioncompress_triggered()
{
    QTextEdit* plainTextEdit = (QTextEdit*) ui->tabWidget->widget(ui->tabWidget->currentIndex());
    QString in  =  plainTextEdit->toPlainText();
    std::vector<int> compressed;
    QString text;
    XML_Tree tt;
    tt.Fill(in);
    Node * node = tt.GetHead();
    for(int i =0; i<node->Children.count(); i++){
        tt.minify(node->Children[i],text);
    }
    compress(text.toLocal8Bit().constData(), std::back_inserter(compressed));

    QString filename = "";
    int ind = ui->tabWidget->currentIndex();
    QString currentTabText = ui->tabWidget->tabText(ind);
    QStringList sp;
    plainTextEdit = (QTextEdit*) ui->tabWidget->widget(ui->tabWidget->currentIndex());
    for(int i  = 0; i < files.count();i++){
        sp = files[i].split('/');
        if(sp[sp.count()-1] == currentTabText){
            filename = files[i];
            break;
        }
    }

    filename =filename.mid(0, filename.lastIndexOf('.'));
    QFile fo(filename+".compressed"); // this is your compressed output file
    if (fo.open(QFile::ReadWrite))
    {
        QDataStream out(&fo);
        out<<(qint32)compressed.size();
        //out<<(qint16)min;
        for(size_t i = 0; i<compressed.size();i++){
            out<<(qint16)(compressed[i]);
        }
        fo.close();
    }

}


void MainWindow::on_actionunCompress_triggered()
{

    QString filename = QFileDialog::getOpenFileName(this,"open", "C:\\","*.compressed");
    QFile fi(filename);
    std::vector<int> compressed;
    qint32 size;
    qint16 v;
    std::string decompressed;
    //qint16 min;
    if (fi.open(QFile::ReadWrite))
    {
        QDataStream in(&fi);
        in>>size;
        //in>>min;
        for(int i =0; i<size; i++){
            in>>v;
            compressed.push_back((int)v);
        }
         decompressed = decompress(compressed.begin(), compressed.end());

        fi.close();
        filename =filename.mid(0, filename.lastIndexOf('.'));
        QFile fo(filename+"1.xml");
        if (fo.open(QFile::ReadWrite))
        {
            QTextStream out(&fo);
            QString qstr = QString::fromStdString(decompressed);
            out<<qstr;

            fo.close();
        }
    }

}

