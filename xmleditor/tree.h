#ifndef XML_TREE_H
#define XML_TREE_H
#include <QString>
#include <QVector>
#include <QFile>
#include <QTextStream>

class Node
{
public:
    QString Data;
    QString Attribute;
    QVector<QString>AttributeName;
    QVector<QString>AttributeVal;
    QString TagName;
    QString Closetag;
    QVector<Node*>Children;
    QString type;
    int repeated;
    Node* Parent;

    //Constructor
    Node(QString Attr , QString Name , QString DataIn = "")
    {
        Data = DataIn;
        Attribute = Attr;
        TagName = Name;
        Closetag = "";
        type ="";
        Parent = NULL;
        repeated = 0;


    }
};

class XML_Tree
{
private:
    Node* Head;
    Node* MostRecent;

public:
    XML_Tree();
    ~XML_Tree();
    Node* GetHead() { return Head; }
    void insertChild(Node *child);
    void DoneNode();
    void print(Node * start,QString &out, int level);
    void Fill(QString text);
    void CorrectError(Node* node);
    void CheckError(Node * node, QString &out,int lvl, QVector<QVector<int>> &high);
    void minify(Node * start ,QString &out);
    void attSeperator(Node* In);
    void XMLtoJSON(Node * node, int lvl , QString &outfile);
    void DestroyRecursive(Node *node);
};

#endif // XML_TREE_H
