#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

template <typename T>
 struct Node {
    Node<T>* left;
    Node<T>* right;
    Node<T>* parent;
    T value;
    int height;
    size_t size;
    explicit Node(T& _value):left(nullptr), right(nullptr),parent(nullptr), value(_value),height(1), size(1){}
    Node(T& _value, Node<T>* _parent): left(nullptr), right(nullptr), parent(_parent), value(_value),height(1), size(1){}
    ~Node(){}
};

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
        Node<T>* target = accessAt(root, index);
        return target->value;
    }
    T& operator [] (size_t index){
        if(size() <= index) throw std::out_of_range("[] index");
        Node<T>* target = accessAt(root, index);
        return target->value;
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
            root = new Node<T>(value);
            updateHeight(root);
            return true;
        }
        root = insertAtNode(root, nullptr, index, value);
//        cout << "inserted" << endl;
//        checkHeight();
        return true;
    }

    bool erase(size_t index){
//        std::cout<< "BEFORE: size: " <<  size() << " index: " << index<< std::endl;
        if(size() <= index) throw std::out_of_range("erase index");
        if(root == nullptr) return false;
        root = eraseAtNode(root, index);
//        std::cout<< "AFTER: size: " <<  size() << " index: " << index<< std::endl;
//        cout << "erased" << endl;
//        checkHeight();
        return true;
    }

private:

    void deleteSubTree(Node<T>* node){
        if(node == nullptr) return;
        deleteSubTree(node->left);
        deleteSubTree(node->right);
        delete node;
    }

    Node<T>* accessAt(Node<T>* node, size_t index) const {
        if(node == nullptr) throw std::out_of_range("[] index");
        size_t left_size = node->left ? node->left->size: 0;
        if(index < left_size){
            return accessAt(node->left, index);
        }else if(index > left_size){
            return accessAt(node->right, index - left_size -1);
        }else{
            return node;
        }
    }


    void checkrelation(Node<T>* node){
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

    int recHeight(Node<T>* node){
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


    Node<T>* RRR(Node<T>* root){
        Node<T>*tmp_node = root->right;
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


    Node<T>* LLR(Node<T>* root){
        Node<T> *tmp_node = root->left;
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

    Node<T>* LRR(Node<T>* root){
        root->left = RRR(root->left);
        return LLR(root);
    }

    Node<T>* RLR(Node<T>* root){
        root->right = LLR(root->right);
        return RRR(root);
    }

    Node<T>* Balance(Node<T>* c_root){
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



    Node<T>* insertAtNode(Node<T>* node, Node<T>* parent, size_t index, T value){
        if(node == nullptr){
            node = new Node<T>(value, parent);
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

    Node<T>* eraseAtNode(Node<T>* node, size_t index){
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
                Node<T>* temp = node->left ? node->left : node->right;
                if(temp == nullptr){
                    temp = node;
                    node = nullptr;
                    delete temp;
                }else{
                    // node->value = std::move(temp->value);
                    // node->left = temp->left;
                    // node->right = temp->right;
                    // but not parent;
                    temp->parent = node->parent;
                    delete node;
                    node = temp;
                }
                
            }else{
                Node<T>* temp = node->right;
                while(temp->left != nullptr){
                    temp = temp->left;
                }
                // Node<T> temp_pointer = *node;

                Node<T>* node_left = node->left;
                Node<T>* node_right = node->right;
                Node<T>* node_parent = node->parent;

                Node<T>* temp_left = temp->left;
                Node<T>* temp_right = temp->right;
                Node<T>* temp_parent = temp->parent;


                if(node_left != nullptr){
                    node_left->parent = temp;
                }
                if(node_right != nullptr){
                    node_right->parent = temp;
                }
                if(node_parent != nullptr){
                    if(node_parent->left == node){
                        node_parent->left = temp;
                    }else{
                        node_parent->right = temp;
                    }
                }
                if(temp_left != nullptr){
                    temp_left->parent = node;
                }
                if(temp_right != nullptr){
                    temp_right->parent = node;
                }
                if(temp_parent != nullptr){
                    if(temp_parent->left == temp){
                        temp_parent->left = node;
                    }else{
                        temp_parent->right = node;
                    }
                }
                std::swap(node->parent, temp->parent);
                std::swap(node->left, temp->left);
                std::swap(node->right, temp->right);
                std::swap(node->height, temp->height);
                std::swap(node->size, temp->size);

                node = temp;

                // if(node->left != nullptr){
                //     node->left->parent = temp;
                // }
                // if(node->right != nullptr){
                //     node->right->parent = temp;
                // }
                // if(node->parent != nullptr){
                //     if(node->parent->left == node){
                //         node->parent->left = temp;
                //     }else{
                //         node->parent->right = temp;
                //     }
                // }

                // if(temp->left != nullptr){
                //     temp->left->parent = node;
                // }
                // if(temp->right != nullptr){
                //     temp->right->parent = node;
                // }
                // if(temp->parent != nullptr){
                //     if(temp->parent->left == temp){
                //         temp->parent->left = node;
                //     }else{
                //         temp->parent->right = node;
                //     }
                // }

                // std::swap(node->parent, temp->parent);
                // std::swap(node->left, temp->left);
                // std::swap(node->right, temp->right);
                // std::swap(node->height, temp->height);
                // std::swap(node->size, temp->size);

                // node = temp;

                
                // node->value = temp->value;
                node->right = eraseAtNode(node->right, 0);
            }
        }
        if(node == nullptr) return node;
        updateHeight(node);
        node = Balance(node);
        return node;
    }

    void updateHeight(Node<T>* c_root){
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

    Node<T>* root;
};




int main() {
    {
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
        // arr.print();

        Array<int> arr2;
        std::srand(std::time(nullptr)); // Initialize random seed

        int numberOfOperations = 5000; // Total number of operations

        for (int i = 0; i < numberOfOperations; i++) {
            int operation = std::rand() % 2; // Randomly decide between insert (0) and erase (1)
            int value = std::rand() % 1000; // Random value between 0 and 999
            int index = arr2.size() > 0 ? std::rand() % arr2.size() : 0; // Random index within the current size of arr2

            if (operation == 0) {
                // Insert operation
                arr2.insert(index, value);
            } else {
                // Erase operation, making sure arr2 is not empty
                if (arr2.size() > 0) {
                    arr2.erase(index); // Using index directly as it's already ensured to be within bounds
                }
            }
        }
        // arr2.print();
    }

    {

        Array<int> arr;
        std::vector<int> vec;
        std::srand(std::time(nullptr)); // Initialize random seed

        int numberOfOperations = 1000; // Total number of operations

        for (int i = 0; i < numberOfOperations; i++) {
            int operation = std::rand() % 2; // Insert (0) or Erase (1)
            int value = std::rand() % 1000; // Random value between 0 and 999

            if (operation == 0) {
                // Insert operation
                int index = arr.size() > 0 ? std::rand() % arr.size() : 0;
                arr.insert(index, value);
                vec.insert(vec.begin() + index, value);
            } else {
                // Erase operation, making sure both structures are not empty
                if (arr.size() > 0 && vec.size() > 0) {
                    int index = std::rand() % arr.size();
                    arr.erase(index);
                    vec.erase(vec.begin() + index);
                }
            }
        }
        bool correct = true;
        // Compare and print results
        if(arr.size() != vec.size()){
            std::cout << "WRONG!!size" << std::endl;
            correct = false;
        }
        for(size_t i = 0; i < arr.size(); i++){
            if(arr[i] != vec[i]){
                std::cout << "WRONG!!content" << std::endl;
                correct = false;
            }
        }
        // Optionally, print the structures
        // [Your code to print arr and vec]
        if(correct){
            std::cout << "CORRECT!!" << std::endl;
        }
    }

}