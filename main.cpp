#include <iostream>
#include <string>
#include <fstream>
#include <sstream> 
#include <iomanip>
#include <queue>
#include <vector>
#include <algorithm>
#include "ArgumentManager.h"

using namespace std;

class TreeNode
{
    private:
        int degree;
        vector<int> vals;
        vector<TreeNode*> children;
        bool isLeaf;

    public:
        TreeNode(int deg, bool leaf)
        {
            degree = deg;
            isLeaf = leaf;
        }

        bool isOverflow()
        {
            int n = vals.size();
            return n > degree - 1;
        }

        //inserts val into correct place in vals
        void insertValSort(int val)
        {
            vals.push_back(val);
            sort(vals.begin(), vals.end());
            return;
        }

        bool hasVal(int val)
        {
            for (int i = 0; i < vals.size(); i++)
            {
                if(vals.at(i) == val)
                {
                    return true;
                }
            }

            return false;
        }

        //splits child node into 2 children nodes
        void splitFullChild(int indexOfChild, TreeNode* childToSplit)
        {
            //allocate new node to split into that will take the last half of the node split
            TreeNode* newNode = new TreeNode(childToSplit->degree, childToSplit->isLeaf);

            //get mid info
            int midPos = (childToSplit->vals.size() - 1) / 2;
            int mid = childToSplit->vals.at(midPos);

            //transfer last half of nodes to new node, remove last half of nodes from old node
            int numToRemove = 1;
            for(int i = midPos + 1; i < childToSplit->vals.size(); i++)
            {
                newNode->vals.push_back(childToSplit->vals.at(i));
                numToRemove++;
            }
            for(int i = 0; i < numToRemove; i++)
            {
                childToSplit->vals.pop_back();
            }

            //transfer last half of children to new node, remove last half of children from old node
            if(!childToSplit->isLeaf)
            {
                numToRemove = 0;
                for(int i = midPos + 1; i < childToSplit->children.size(); i++)
                {
                    newNode->children.push_back(childToSplit->children.at(i));
                    numToRemove++;
                }

                for(int i = 0; i < numToRemove; i++)
                {
                    childToSplit->children.pop_back();
                }
            }

            //set new node as child of current node
            children.insert(children.begin() + indexOfChild + 1, newNode);

            //add the mid value of split node to vals of current node
            insertValSort(mid);

            return;
        }

    friend class BTree;
};

class BTree
{
    private:
        TreeNode* root;
        int treeDegree;
        int height;

    public:
        BTree(int deg)
        {
            root == nullptr;
            treeDegree = deg;
            height = 0;
        }

        //adds val to correct leaf, if overflows return true, else false
        bool splitAfterInsert(TreeNode* curr, int val)
        {
            //makes sure no duplicate values
            if(!(curr->hasVal(val)))
            {
                //add value to leaf
                if(curr->isLeaf)
                {
                    curr->insertValSort(val);
                }
                else
                {
                    //gets index of child to move to
                    int childIndex = 0;
                    for(int i = 0; i < curr->vals.size(); i++)
                    {
                        if(val > curr->vals.at(i))
                        {
                            childIndex = i + 1;
                        }
                    }

                    //if overflow in the child node, split it
                    bool overflowTriggered = splitAfterInsert(curr->children[childIndex], val);
                    if(overflowTriggered)
                    {
                        curr->splitFullChild(childIndex, curr->children[childIndex]);
                    }
                }
            }

            return curr->isOverflow();
        }

        void insert(int val)
        {
            //if no root
            if (height == 0)
            {
                //allocate new node, set root to new node
                TreeNode* temp = new TreeNode(treeDegree, true);
                temp->insertValSort(val);

                root = temp;
                height++;
            }
            else
            {
                bool rootOverflow = splitAfterInsert(root, val);

                //if root overflows, create new root, split old root as children between new root
                if(rootOverflow)
                {
                    TreeNode* newRoot = new TreeNode(treeDegree, false);

                    newRoot->children.push_back(root);                    
                    newRoot->splitFullChild(0, root);
                   
                    root = newRoot;
                    height++;
                }
            }

            return;
        }

        void printHeight(ofstream& outfile)
        {
            outfile << "Height=" << height << endl;
            return;
        }

        void printLevel(int level, ofstream& outfile)
        {
            //if invalid level
            if(level > height || level < 1 || root == nullptr)
            {
                outfile << "Empty";
            }
            else
            {
                //creates queue for breadth-first search
                queue<TreeNode*> q;
                q.push(root);

                //each iteration goes down 1 level
                int currLevel = 1;
                while(currLevel < level)
                {
                    int nodesInLevel = q.size();

                    //each iteration puts all children of current node in queue
                    while(nodesInLevel > 0)
                    {
                        for (int i = 0; i < q.front()->vals.size() + 1; i++)
                        {
                            q.push(q.front()->children.at(i));
                        }
                        q.pop();

                        nodesInLevel--;
                    }

                    currLevel++;
                }

                //empties queue for specified level
                while(!q.empty())
                {
                    for(int i = 0; i < q.front()->vals.size(); i++)
                    {
                        outfile << q.front()->vals.at(i) << " ";
                    }

                    q.pop();
                }
            }

            outfile << endl;
            return;
        }

        void printTree()
        {
            //creates queue for breadth-first traversal
            queue<TreeNode*> q;
            q.push(root);

            //iterates through each level
            int currLevel = 1;
            while(currLevel <= height)
            {
                int nodesInLevel = q.size();

                //prints out "[<vals in node>](<# of children of node>)", same line as other same level nodes
                while(nodesInLevel > 0)
                {
                    cout << "[";
                    for(int i = 0; i < q.front()->vals.size(); i++)
                    {
                        cout << q.front()->vals.at(i);
                        if(i != q.front()->vals.size() - 1)
                        {
                            cout << " ";
                        }
                    }
                    cout << "](" << q.front()->children.size() << ") ";

                    //adds node's children to queue if node isn't leaf
                    if(currLevel != height)
                    {
                        for (int i = 0; i < q.front()->vals.size() + 1; i++)
                        {
                            q.push(q.front()->children.at(i));
                        }
                    }
                    q.pop();

                    nodesInLevel--;
                }

                cout << endl;
                currLevel++;
            }

            return;
        }
};

int main (int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Usage: program \"input=<file>;output=<file>\"" << endl;
        return -1;
    }

    ArgumentManager am(argc, argv);
	string inFilename = am.get("input");
	string outFilename = am.get("output");
    string comFilename = am.get("command");

    ifstream input;
    input.open(inFilename);
    ofstream output;
    output.open(outFilename);
    ifstream command;
    command.open(comFilename);

    
    //gets degree of tree
    int degreeOfTree = -1;
    string fileString;
    command >> fileString;

    fileString = fileString.substr(7);
    stringstream ss;
    ss << fileString;
    ss >> degreeOfTree;

    //gets levels to print in output
    queue<int> levelQueue;
    int fileInt;
    while (command >> fileString >> fileInt)
    {
        levelQueue.push(fileInt);
    }

    //initialize tree
    BTree MyTree(degreeOfTree);

    //insert vals into tree
    while (input >> fileInt)
    {
        MyTree.insert(fileInt);
    }

    //output height and specified levels to file
    MyTree.printHeight(output);

    while(!levelQueue.empty())
    {
        MyTree.printLevel(levelQueue.front(), output);
        levelQueue.pop();
    }

    input.close();
    output.close();
    command.close();
}