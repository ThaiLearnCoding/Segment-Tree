#include<iostream>
#include <math.h>
#include <queue>
#include<vector>

using namespace std;

/**
 * Color to visualise the structure
 */
const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string CYAN = "\033[36m";


void printNChar(int n, char c)
{
    for (int i = 0; i < n - 1; i++)
        cout << c;
}

template <class T>
class Node{
private:
    T val;
    string color;
    bool flag;

public:
    Node(T val = NULL, string color = RESET, bool flag = false) : val(val), color(color), flag(flag) {}

    T getVal() { return this->val; }

    void changeResult(T val, string color)
    {
        this->val = val;

        if(color != RED)
            this->color = color;
        else
        {
            flag = true;
        }
    }

    void flagOn() { flag = true; }

    void printResult()
    {
        cout << ((flag == true)? RED : color )<< val << RESET;
        flag = false; // Reset flag
    }
};

template <class T>
class SegmentTree
{

    /***************************************************************************************
     *          Note for all the contents of this source code
     *
     *  int node : index of the node in the Segment Tree array
     *  int L, R : lower and upperbound for the range we are searching for or manipulate on
     *  int idx  : index of the node that need to manipulate on
     *  T val    : value to update
     *  int L_query / R_query :  range's bound that need to be considered
     *
     */


    // ----- Operation applied -----
private: 

    static T sum(T val1, T val2) { return val1 + val2; }
    static T mul(T val1, T val2) { return val1 * val2; }
    static T min(T val1, T val2) { return val1 < val2 ? val1 : val2; }
    static T max(T val1, T val2) { return val1 > val2 ? val1 : val2; }

        //----- Attributes needed for class -----
private : 
    vector<Node<T>> TreeArray;      // Hold value for a range according to the function itself
    T *SrcArr;                      // Base Value (leaf nodes in Tree Array)

    T (*opt)(T, T);                 // Function pointer to represent the function of operation applied
    T empty_val;                    // Representing empty elements
    T invalid_val;                  // Return when it's out of range

    int count;                      // Number of elements in SrcArr
    int capacity;                   // Maxinum number of nodes in Tree Array, been proved to be 4 * count


    //----- Store Helper functions for main methods-----
private:
    int getHeightRec()
    {
        if(count == 0)
        {
            cout << "-----------Tree is empty-----------\n";
            return -1;
        }

        /***
         *      Idea:
         * Suppose you have n number of elements, so the index of each ranges from 0 to n - 1
         * To construct the tree, you need to divide the range smaller
         *      until you reach the first element, in all case its index is 0
         * So that means the height of the tree is equal to
         *      the number of time you divide the (n - 1) by 2 until it get 0
         * 
         * Ex:  9 elements -> index range: 0-8  
         *      8->4->2->1->0 (5 times mean height is 5)
         */
        int height = 1;
        int start = count - 1;
        while(start)
        {
            start /= 2;
            ++height;
        }
        return height;
    }

    void recalulate(int node, string color)
    {
        // Find value for the parent node
        // Calculate the result 
        // Change the color for that node also
        TreeArray[node].changeResult(opt(TreeArray[node * 2 + 1].getVal(), TreeArray[node * 2 + 2].getVal()), color);
    }
    
    void createTree(int node, int L, int R) 
    {
        // Base case
        if (L == R)
        {
            TreeArray[node].changeResult(SrcArr[L], GREEN);
        }
        else
        {
            // Recursion to meet the leaf node
            int mid = (L + R) / 2;
            createTree(node * 2 + 1, L, mid);
            createTree(node * 2 + 2, mid + 1, R);

            recalulate(node, CYAN);
        }
    }

    void buildTree()
    {
        if (SrcArr == nullptr)
        {
            cout << "Have nothing to build!";
            return;
        }

        createTree(0, 0, count - 1);
    }

    void updating(int node, int L, int R, int idx, T val)
    {
        if(L == R)
        {
            SrcArr[idx] = val;
            TreeArray[node].changeResult(val, RED);
        }
        else
        {
            int mid = (L + R) / 2;

            // Just need to update one of its subtree 
            if(L <= idx && idx <= mid)
                updating(node * 2 + 1, L, mid, idx, val);
            else
                updating(node * 2 + 2, mid + 1, R, idx, val);

            recalulate(node, RED);
        }
    }

    T queryRecursion(int node, int L, int R, int L_query, int R_query)
    {
        // Out of range case
        if(R_query < L || R < L_query)
            return invalid_val;

        /**
         * If the node is not invalid, it is processed
         * -> Turn on the flag to track the path
         */
        TreeArray[node].flagOn();


        /**
         * Node range is within the query range
         * 
         * Query range: ########### L--node range---R ##########------Other nodes range------#############
         * 
         */
        if (L_query <= L && R <= R_query)
            return TreeArray[node].getVal();

        // Split the range of nodes to get smaller fit-in range
        int mid = (L + R) / 2;
        T sub1 = queryRecursion(node * 2 + 1, L, mid, L_query, R_query);
        T sub2 = queryRecursion(node * 2 + 2, mid + 1, R, L_query, R_query);
        return opt(sub1, sub2); 
    }


//----- Main methods -----
public:
    SegmentTree(T *SrcArr = 0, int count = 0, T empty_val = NULL, string operation = "sum") 
    {
        this->SrcArr = SrcArr;
        this->count = count;
        this->capacity = 4 * count;
        this->empty_val = empty_val;
        this->TreeArray.resize(capacity, empty_val);

        // Choose operation
        if (operation == "mul")
        {
            opt = &SegmentTree::mul;
            invalid_val = 1;
        }
        else if (operation == "min")
        {
            opt = &SegmentTree::min;
            invalid_val = INT_MAX;
        }
        else if (operation == "max")
        {
            opt = &SegmentTree::max;
            invalid_val = INT_MIN;
        }
        else // Default: sum
        {
            opt = &SegmentTree::sum;
            invalid_val = 0;
        }
        
        buildTree();
    }

    ~SegmentTree() {
        TreeArray.clear();
        delete SrcArr;
    }

    // Update the value of the node that has index idx in SrcArray
    void updateValue(int idx, T val)
    {
        updating(0, 0, count - 1, idx, val);
    }

    T query(int L_query, int R_query)
    {
        return queryRecursion(0, 0, count - 1, L_query, R_query);
    }

    int getHeight()
    {
        return this->getHeightRec();
    }
    
    void printTreeStructure()
    {
        int height = this->getHeight();
        if (this->count == 0)
        {
            cout << "Tree is empty\n";
            return;
        }
        queue<int> q; // Store idx of nodes to be printed
        q.push(0);
        int start;

        int count = 0;
        int maxNode = 1;
        int level = 0;
        int space = pow(2, height);
        printNChar(space/2, ' ');

        while (!q.empty())
        {
            start = q.front();
            q.pop();
            if (TreeArray[start].getVal() == empty_val || start == empty_val)
            {
                cout << " ";
                q.push(empty_val);
                q.push(empty_val);
            }
            else
            {
                TreeArray[start].printResult();
                q.push(start * 2 + 1);
                q.push(start * 2 + 2);
            }
            printNChar(space, ' ');
            count++;
            if (count == maxNode)
            {
                cout << endl;
                count = 0;
                maxNode *= 2;
                level++;
                space /= 2;
                printNChar(space/2, ' ');
            }
            if (level == height)
                return;
        }
    }
};

template<class T>
void updateValue_main(SegmentTree<T>& tree, int& count)
{
    printNChar(pow(2, tree.getHeight()) * 1.5, '#');
    int idx = rand() % count;
    int val = rand() % (5 * count);
    cout << "\nUpdating at index " << idx << " by value " << val << endl;
    tree.updateValue(idx, val);
    tree.printTreeStructure();
}

template <class T>
void queryRange_main(SegmentTree<T> &tree, int &count) 
{
    printNChar(pow(2, tree.getHeight()) * 1.5, '#');
    int L_query = rand() % count;
    int R_query = L_query + rand() % (count - L_query);
    cout << "\nResult of the query with the range from " << L_query << " to " << R_query << " is "
         << tree.query(L_query, R_query)
         << endl;
    tree.printTreeStructure();
}


void RandonTestcase(int count, string operation)
{
    srand(time(0));

    double arr[count];
    cout << "Value of numbers for array: \n";
    for (int idx = 0; idx < count; ++idx)
    {
        arr[idx] = rand() % (5 * count);
        cout << idx << " : " << arr[idx] << endl;
    }


    /**
     * Print out Orginal Structure
     */
    SegmentTree<double> tree(arr, count, -1, operation); // Change the empty_val -1 to different usage ????
    printNChar(pow(2, tree.getHeight())*1.5 , '#');
    cout << "\nOriginal structure of " << operation << " Segment Tree : " << endl;
    tree.printTreeStructure();

    /**
     * Updating method
     */
    updateValue_main(tree, count);

    /**
     * Querying a specific range
     */
    queryRange_main(tree, count);
}

int main()
{
    int count;
    cout << "Type in count: ";
    cin >> count;

    string operation;
    cout << "Choose the operation (sum/mul/min/max) \nBy typing the name of the operation here: ";
    cin >> operation;

    RandonTestcase(count, operation);
}