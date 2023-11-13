#include <iostream>
#include <iomanip>

using namespace std;


template < typename T >
struct Array {
    bool empty() const {
        return root == nullptr;
    }

    Array():root(nullptr){}
    ~Array(){
        deleteSubTree(root);
    }

    size_t size() const {
        if(root==nullptr) return 0;
        return root->size;
    }

    const T& operator [] (size_t index) const{
        if(size() <= index) throw std::out_of_range("[] index");
        return index_to_node(index)->value;
    }
    T& operator [] (size_t index){
        if(size() <= index) throw std::out_of_range("[] index");
        return index_to_node(index)->value;
    }

    void print(){
        for(size_t i = 0; i < size(); i++){
            std::cout << "["<< i << "] "<<  operator [] (i) << std::endl;
        }
    }

    void checkHeight(){
        recHeight(root);
    }

    bool insert(size_t index, T value) {
        if(size() < index) throw std::out_of_range("insert index");
        if(root == nullptr && index == 0){
            root = new Node(value);
            updateHeight(root);
            return true;
        }
        root = insertAtNode(root, nullptr, index, value);
        cout << "inserted" << endl;
        checkHeight();
        return true;
    }

    bool erase(size_t index){
//        std::cout<< "BEFORE: size: " <<  size() << " index: " << index<< std::endl;
        if(size() <= index) throw std::out_of_range("erase index");
        if(root == nullptr) return false;
        root = eraseAtNode(root, index);
//        std::cout<< "AFTER: size: " <<  size() << " index: " << index<< std::endl;
        cout << "erased" << endl;
        checkHeight();
        return true;
    }

private:
    struct Node {
        Node* left;
        Node* right;
        Node* parent;
        T value;
        int height;
        size_t size;
        explicit Node(T& _value):left(nullptr), right(nullptr),parent(nullptr), value(_value),height(1), size(1){}
        Node(T& _value, Node* _parent): left(nullptr), right(nullptr), parent(_parent), value(_value),height(1), size(1){}
        ~Node(){}
    };

    void deleteSubTree(Node* node){
        if(node == nullptr) return;
        deleteSubTree(node->left);
        deleteSubTree(node->right);
        delete node;
    }

    Node* index_to_node(size_t index) const {
        if(!root || index > root->size ){
            throw std::out_of_range("Index");
        }
        Node* curr = root;
        size_t sum = 0;
        // never goes parent thus it should end.
        while(curr){
            size_t left_size = curr->left ? curr->left->size: 0;
            if(index < sum + left_size){
                curr = curr->left;
            }else if(index > sum + left_size){
                sum += left_size +1;
                curr = curr->right;
            }else{
                return curr;
            }
        }
        return curr;
    }



    void checkrelation(Node* node){
        if(node == nullptr) return;
        if(node->left) {
            if(node->left->parent != node){
                std::cout << "WRONG PARENT!!" << std::endl;
            }
        }
        if(node->right){
            if(node->right->parent != node){
                std::cout << "WRONG PARENT!!" << std::endl;
            }
        }
        if(node->parent){
            if(node->parent->left != node && node->parent->right != node){
                std::cout << "WRONG PARENT!!" << std::endl;
            }
        }else{
            if(node != root ){
                std::cout << "WRONG PARENT!! root" << std::endl;
            }
        }
    }

    int recHeight(Node* node){
        if(node== nullptr){
            return 0;
        }
        int left = recHeight(node->left);
        int right = recHeight(node->right);

        checkrelation(node);

        if(node->height != std::max(left, right) + 1){
            std::cout << "WRONG HEIGHT!!" << std::endl;
        }else{
//            std::cout << "CORRECT HEIGHT!!" << std::endl;
        }
//        cout << left-right << endl;

        if(std::abs(left-right) > 1){
            std::cout << "UNBALANCED!! left: " << left << " right: " << right << " hei: " << root->height - node->height<< std::endl;
        }
        return left>right?left +1: right+1;
    }


    Node* RRR(Node* root){
        Node *tmp_node = root->right;
        root->right = tmp_node->left;

        if(tmp_node->left != nullptr)
            tmp_node->left->parent = root;
        tmp_node->left = root;
        tmp_node->parent = root->parent;
        root->parent = tmp_node;

        if(tmp_node->parent != nullptr){
            if(tmp_node->parent->left == root){
                tmp_node->parent->left = tmp_node;
            }else{
                tmp_node->parent->right = tmp_node;
            }
        }

        root = tmp_node;
        updateHeight(root->left);
        updateHeight(root->right);
        updateHeight(root);
        updateHeight(root->parent);
        return root;
    }


    Node* LLR(Node* root){
        Node *tmp_node = root->left;
        root->left = tmp_node->right;
        if(tmp_node->right != nullptr)
            tmp_node->right->parent = root;

        tmp_node->right = root;
        tmp_node->parent = root->parent;
        root->parent = tmp_node;
        if(tmp_node->parent != nullptr){
            if(tmp_node->parent->left == root){
                tmp_node->parent->left = tmp_node;
            }else{
                tmp_node->parent->right = tmp_node;
            }
        }
        root = tmp_node;
        updateHeight(root->left);
        updateHeight(root->right);
        updateHeight(root);
        updateHeight(root->parent);
        return root;
    }

    Node* LRR(Node* root){
        root->left = RRR(root->left);
        return LLR(root);
    }

    Node* RLR(Node* root){
        root->right = LLR(root->right);
        return RRR(root);
    }

    Node* Balance(Node* c_root){
        int firstheight = 0;
        int secondheight = 0;

        if(c_root->left != nullptr) firstheight = c_root->left->height;
        if(c_root->right != nullptr) secondheight = c_root->right->height;

        int balance = secondheight - firstheight;
        if(balance > 1){
            // right is bigger
            int rightRightHeight = 0;
            int rightLeftHeight = 0;
            if(c_root->right->right != nullptr) rightRightHeight = c_root->right->right->height;
            if(c_root->right->left != nullptr) rightLeftHeight = c_root->right->left->height;
            if(rightRightHeight >= rightLeftHeight){
                c_root = RRR(c_root);
            }else {
                c_root = RLR(c_root);
            }
        }else if (balance < -1){
            // left is bigger
            int leftRightHeight = 0;
            int leftLeftHeight = 0;
            if(c_root->left->right != nullptr) leftRightHeight = c_root->left->right->height;
            if(c_root->left->left != nullptr) leftLeftHeight = c_root->left->left->height;
            if(leftLeftHeight >= leftRightHeight){
                c_root = LLR(c_root);
            }else {
                c_root = LRR(c_root);
            }
        }
        return c_root;
    }



    Node* insertAtNode(Node* node, Node* parent, size_t index, T value){
        if(node == nullptr){
            node = new Node(value, parent);
            return node;
        }
        size_t left_size = node->left ? node->left->size: 0;
        if(index <= left_size){
            node->left = insertAtNode(node->left, node, index, value);
        }else{
            node->right = insertAtNode(node->right, node, index - left_size -1, value);
        }

        updateHeight(node);
        node = Balance(node);
        return node;
    }

    Node* eraseAtNode(Node* node, size_t index){
        if(node == nullptr) {
            return nullptr;
        }

        size_t left_size = node->left ? node->left->size: 0;
        if(index < left_size){
            node->left = eraseAtNode(node->left, index);
        }else if(index > left_size){
            node->right = eraseAtNode(node->right, index - left_size -1);
        }else{
            if(node->left == nullptr || node->right == nullptr){
                Node* temp = node->left ? node->left : node->right;
                if(temp == nullptr){
                    temp = node;
                    node = nullptr;
                }else{
                    node->value = temp->value;
                    node->left = temp->left;
                    node->right = temp->right;
                    // but not parent;
                }
                delete temp;
            }else{
                Node* temp = node->right;
                while(temp->left != nullptr){
                    temp = temp->left;
                }
                node->value = temp->value;
                node->right = eraseAtNode(node->right, 0);
            }
        }
        if(node == nullptr) return node;
        updateHeight(node);
        node = Balance(node);
        return node;
    }

    void updateHeight(Node* c_root){
        if(c_root == nullptr) return;

        int val = 1;
        int size = 1;
        if(c_root->left != nullptr){
            val = c_root->left->height + 1;
            size += c_root->left->size;
        }
        if(c_root->right != nullptr){
            val = std::max(val, c_root->right->height + 1);
            size += c_root->right->size;
        }
        c_root->size = size;
        c_root->height = val;
    }



    Node* root;
};


int main() {
    Array<int>arr;
    arr.insert(0,1);
    arr.insert(1, 3);
    arr.insert(2, 5);
    arr.insert(3, 7);
    arr.insert(4, 9);
    for(int i = 0; i < 1000; i++){
        arr.insert(i%745+i%4, i);
    }

    for (int i = 0; i < 100; i++) {
        if (i % 3 > 0) {
            arr.erase(i % 15);
        }else{
            arr.insert(i % 11, i);
        }

    }
    for(int i = 0; i < 100; i++){
        arr.erase(i%13);
    }
    for(int i = 0; i < 1000; i++){
        arr.insert(i%745+i%4, i);
    }
    for(int i = 0; i < 100; i++){
        arr.erase(i%13);
    }
    for(int i = 0; i < 1000; i++){
        arr.insert(i%745+i%4, i);
    }
    for(int i = 0; i < 100; i++){
        arr.erase(i%13);
    }

    for(int i = 0; i < 1000; i++){
        arr.insert(i%745+i%4, i);
        arr.size();
    }
    for(int i = 0; i < 100; i++){
        arr.erase(i%13);
    }

    for (int i = 0; i < 1000; i++) {
        if (i % 3 > 0) {
            arr.erase(i % 15);
        }else{
            arr.insert(i % 11, i);
        }

    }



    arr.print();

//         Inserting in a pattern that is likely to unbalance the tre

//        // Deleting in a pattern that is likely to unbalance the tree
//





//    for(size_t i = 0; i < arr.size(); i++){
//        cout << arr[i] << " ";
//    }cout << endl;

}