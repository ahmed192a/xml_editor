#include "tree.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QStack>
#include <QDir>
#include <algorithm>


void removespaces(QString text){
    QStringList s = text.split('\n');
    for(int i = 0; i<s.count();i++){
        while(s[i][0]==' '){
            s[i] = s[i].mid(1,s[i].count()-1);
        }
    }

}



void spaces(QString &out, int level){
    for (int i =0; i<level;i++){
        out+='\t';
    }
}

XML_Tree::XML_Tree(){
    Head = nullptr;
    MostRecent = nullptr;
}

//destructor
XML_Tree::~XML_Tree()
{
    Node * node;
    for(int i = 0; i< Head->Children.count();i++){
        node = Head->Children[i];
        DestroyRecursive(node);
    }
    delete Head;

}
void XML_Tree::DestroyRecursive(Node *node)
{
    if (node)
    {
        for(int i  = 0; i< node->Children.count();i++)
            DestroyRecursive(node->Children[i]);
        delete node;
    }
}

// Insert child in the tree
void XML_Tree::insertChild(Node* child){
    if(Head ==nullptr){
        Head = new Node("","","");
        Head->type = "Parent";
        child->Parent = Head;
        Head->Children.push_back(child);
        Head->Parent = nullptr;
        MostRecent = child;
    }
    else{

        if(child->TagName == MostRecent->TagName.mid(0,MostRecent->TagName.size()-1)){
            child->repeated=true;
        }

        child->Parent = MostRecent;
        MostRecent->Children.push_back(child);
        MostRecent = child;
    }
}

void XML_Tree::DoneNode(){
    //MostRecent->Closetag = closetag;
    MostRecent = MostRecent->Parent;
}
void XML_Tree::print(Node * start ,QString &out, int level){
    if(start == nullptr){
        out+="";
        return;
    }
    Node * current = start;
    Node * child;
    if(start->type=="comment"){
        spaces(out, level);
        out+=start->Data+"\n";
        return;

    }
    spaces(out, level);
    /*if(current->repeated){
        out+="rep ";
    }*/
    if(current->Attribute=="")
        out+= "<"+current->TagName+">\n";
    else{
        out+= "<"+current->TagName+" ";
        for(int i =0;i<current->AttributeName.count();i++){
            out+=current->AttributeName[i]+"=\""+current->AttributeVal[i]+"\"";
            if(i!=current->AttributeName.count()-1){
                out+=" ";
            }
        }
        if(current->type == "closed"){
            out+="/>\n";
            return;
        }
        out+=">\n";
    }

    for(int i  = 0; i< current->Children.count();i++){
        child = current->Children[i];
        if(child==nullptr)continue;
        print(child, out, level+1);

    }
    if(current->Data!= ""){
    spaces(out, level+1);
    out+=current->Data+"\n";
    }
    spaces(out, level);
    out+="<"+current->Closetag+">\n";
}

void XML_Tree::Fill(QString text){
    int st = 0;
    int ct = 0;
    int sp = 0;

    QStringList lines = text.split("\n");
    QString sub, subb;
    Node * newnode = nullptr;
    text.erase(std::remove(text.begin(), text.end(), '\t'), text.end());
    text.erase(std::remove(text.begin(), text.end(), '\n'), text.end());
    for(int i = 0; i<text.count();i++){
        if(text[i]=='<' ){
            if(text[i+1] == '!' || text[i+1] == '?'){
                newnode = new Node("","","");
                newnode->type = "comment";
                insertChild(newnode);

                st=i;
            }
            else if(text[i+1] == '/'){
                if(MostRecent->Children.count()==0){

                    sub = text.mid(ct+1, i-ct-1);

                    newnode->Data = sub;
                }
                st = i+1;
                sp =1;
            }
            else{

                if(subb!= "" &&MostRecent != nullptr ) MostRecent->Data =subb;
                if(newnode != nullptr && newnode->Data != ""){
                    newnode->Closetag = "";
                    DoneNode();
                    newnode = nullptr;
                }
                newnode =new Node("", "", "");
                 //insertChild(newnode);
                st = i+1;
            }
            subb="";
        }
        else if(text[i]=='>'){

            ct = i;
            if(text[i-1]=='-' || text[i-1]=='?'){
                sub = text.mid(st,i-st+1);
                newnode->Data = sub;
                DoneNode();
                newnode = MostRecent;
            }
            else if(sp == 1){
                sub = text.mid(st, i-st);
                if(MostRecent->TagName!= sub.mid(1,sub.size()-1) && sub.mid(1,sub.size()-1) == MostRecent->Parent->TagName){
                    MostRecent->Closetag ="";
                    DoneNode();
                    MostRecent->Closetag = sub;
                    DoneNode();
                }
                else{
                    MostRecent->Closetag = sub;
                    DoneNode();
                }
                newnode =nullptr;
                sp=0;
            }
            else if(text[i-1]=='/'){
                sub = text.mid(st, i-1);
                if(sub.indexOf(' ',0)!= -1){
                    newnode->TagName = sub.mid(0,sub.indexOf(' ',0));
                    newnode->Attribute = sub.mid(sub.indexOf(' ',0)+1,sub.size()-(sub.indexOf(' ',0)+1));
                    attSeperator(newnode);
                    insertChild(newnode);
                }
                newnode->type = "closed";
                DoneNode();
                newnode->Closetag =sub;
                newnode =nullptr;
            }
            else{
                sub = text.mid(st,i-st);
                if(sub.indexOf(' ',0)!= -1){
                    newnode->TagName = sub.mid(0,sub.indexOf(' ',0));
                    newnode->Attribute = sub.mid(sub.indexOf(' ',0)+1,sub.size()-(sub.indexOf(' ',0)+1));
                    attSeperator(newnode);
                    //insertChild(newnode);
                }
                else
                newnode->TagName = sub;

                insertChild(newnode);
            }
            subb = "";
        }else{
            if(!(subb=="" && text[i]==' '))
                subb+=text[i];
        }

    }


}


void XML_Tree::XMLtoJSON(Node *node, int lvl, QString &outfile){
    spaces(outfile, lvl);
    if(!node->repeated)
    outfile+= "\""+node->TagName+"\" : ";
    if(node->Attribute!="" &&node->Children.count() ==0){
        //Attributes
        outfile+="{\n";
        for(int i = 0; i<node->AttributeName.count();i++){
            spaces(outfile, lvl+1);
            outfile+="\"@"+node->AttributeName[i]+"\" : \""+node->AttributeVal[i]+"\"";
                outfile+=",\n";
        }
        if(node->type!="closed"){
            spaces(outfile, lvl+1);
            outfile+="\"#text\" : \""+node->Data+"\"\n";
        }
        spaces(outfile, lvl);
        outfile+="}";
    }else{
        if(node->Children.count()==0){
            //if(node->Attribute=="")
                outfile+= "\""+node->Data+"\"";

        }
        else{
            if(node->Children[0]->repeated){
                outfile+= "[\n";
                if(node->Attribute!=""){
                    //outfile+="{\n";
                    for(int i = 0; i<node->AttributeName.count();i++){
                        spaces(outfile, lvl+1);
                        outfile+="\"@"+node->AttributeName[i]+"\" : \""+node->AttributeVal[i]+"\"";
                        outfile+=",\n";
                    }

                    //spaces(outfile, lvl);
                    //outfile+="}";
                }
                for(int i = 0; i< node->Children.count();i++){
                    if(node->Children[i]->type =="comment") continue;
                    XMLtoJSON(node->Children[i], lvl+1, outfile);
                    if(i!=node->Children.count()-1)
                        outfile+= ",\n";
                    else
                        outfile+= "\n";
                }
                spaces(outfile, lvl);
                outfile+= "]";
            }
            else{
                outfile+= "{\n";
                if(node->Attribute!=""){
                    //outfile+="{\n";
                    for(int i = 0; i<node->AttributeName.count();i++){
                        spaces(outfile, lvl+1);
                        outfile+="\"@"+node->AttributeName[i]+"\" : \""+node->AttributeVal[i]+"\"";
                        outfile+=",\n";
                    }
                }
                for(int i = 0; i< node->Children.count();i++){
                    if(node->Children[i]->type =="comment") continue;
                    XMLtoJSON(node->Children[i], lvl+1, outfile);
                    if(i!=node->Children.count()-1)
                        outfile+= ",\n";
                    else
                        outfile+= "\n";
                }
                spaces(outfile, lvl);
                outfile+= "}";

            }
        }
    }
}

void XML_Tree::CorrectError(Node * node){
    Node* current  = node;
    for(int i =0;i<current->Children.count();i++){
        CorrectError(current->Children[i]);
    }
    if(current->type != ""){
        return;
    }

    if(current->Closetag=="" ||current->TagName!=current->Closetag.mid(1,current->Closetag.size()-1)){
        //visualize error
        current->Closetag="/"+current->TagName;
    }
}

void XML_Tree::CheckError(Node * node, QString &out,int lvl, QVector<QVector<int>> &high){
    Node* current  = node;
    QVector<int> * v;

    if(current->type=="comment"){
        spaces(out, lvl);
        out+=current->Data+"\n";
        return;

    }
    spaces(out, lvl);
    if(current->Attribute=="")
        out+= "<"+current->TagName+">\n";
    else{
        out+= "<"+current->TagName+" ";
        for(int i =0;i<current->AttributeName.count();i++){
            out+=current->AttributeName[i]+"=\""+current->AttributeVal[i]+"\"";
            if(i!=current->AttributeName.count()-1){
                out+=" ";
            }
        }
        if(current->type == "closed"){
            out+="/>\n";
            return;
        }
        out+=">\n";
    }
    for(int i =0;i<current->Children.count();i++){
        //spaces(out, lvl+i+1);
        //out+="<"+current->TagName+">\n";
        CheckError(current->Children[i], out, lvl+1, high);
    }
    if(current->Data!=""){
        spaces(out, lvl+1);
        out+=current->Data+"\n";
    }
    spaces(out, lvl);
    if(current->Closetag==""){
        v=new QVector<int>;
        v->push_back(out.size());
        v->push_back(1);
        high.push_back(*v);
        out+="\t\n";

        //visualize error
        //current->Closetag="/"+current->TagName;

    }
    else if(current->TagName!=current->Closetag.mid(1,current->Closetag.size()-1)){
        v=new QVector<int>;
        v->push_back(out.size());
        v->push_back(current->Closetag.size()+2);
        high.push_back(*v);
        out+= "<"+current->Closetag+">\n";
    }
    else{
        out+= "<"+current->Closetag+">\n";
    }
}
void XML_Tree::minify(Node * start ,QString &out){
    if(start == nullptr){
        out="";
        return;
    }
    Node * current = start;
    Node * child;
    /*if(current->repeated){
        out+="rep ";
    }*/
    if(current->type=="comment"){
        out+=start->Data;
        return;
    }
    if(current->Attribute=="")
        out+= "<"+current->TagName+">";
    else{
        out+= "<"+current->TagName+" ";
        for(int i =0;i<current->AttributeName.count();i++){
            out+=current->AttributeName[i]+"=\""+current->AttributeVal[i]+"\"";
            if(i!=current->AttributeName.count()-1){
                out+=" ";
            }
        }
        if(current->type == "closed"){
            out+="/>\n";
            return;
        }
        out+=">";
    }

    for(int i  = 0; i< current->Children.count();i++){
        child = current->Children[i];
        if(child==nullptr)continue;
        minify(child, out);

    }
    if(current->Data!= ""){
    out+=current->Data;
    }
    out+="<"+current->Closetag+">";
}


void XML_Tree::attSeperator(Node * In)
{
    if (In->Attribute.length() > 0) {
        int starting = 0;
        bool checkForQuot = false;

        for (int i = 0; i < In->Attribute.length(); i++) {
            if (In->Attribute[i] == '=') {
                In->AttributeName.push_back(In->Attribute.mid(starting, i - starting));

            }
            else if (In->Attribute[i] == '"' && checkForQuot == false) {
                checkForQuot = true;
                starting = i + 1;
            }
            else if (In->Attribute[i] == '"' && checkForQuot == true) {
                In->AttributeVal.push_back(In->Attribute.mid(starting, i - starting));
                checkForQuot = false;
            }
            else if (In->Attribute[i] == ' ' && checkForQuot == false) {
                starting = i + 1;
            }
        }

    }
}




