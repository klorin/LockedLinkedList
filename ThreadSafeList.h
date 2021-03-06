#ifndef THREAD_SAFE_LIST_H_
#define THREAD_SAFE_LIST_H_

#include <pthread.h>
#include <iostream>
#include <iomanip> // std::setw
#include "Mutex.h"

using namespace std;

/*
class Mutex {
private:
    pthread_mutex_t mutex;
    bool _isLocked = false;

public:
    Mutex() {
        if (pthread_mutex_init(&mutex, NULL)) perror("Mutex::ctor");
    };

    ~Mutex() {
        if (pthread_mutex_destroy(&mutex)) perror("Mutex::ctor");
    }; //NOTE: destroys lock

    void lock() {
        if (pthread_mutex_lock(&mutex)) perror("Mutex::lock");
        _isLocked = true;
    };

    void unlock() {
        _isLocked = false;
        if (pthread_mutex_unlock(&mutex)) perror("Mutex::unlock");
    };

    bool isLocked() const {
        return _isLocked;
    };
};
*/

template<typename T>
class List {
public:
    /**
     * Constructor
     */
    List() : head(nullptr), size() {}

    /**
     * Destructor
     */
    ~List() {
        try{
        while (head != nullptr) {
            Node *temp = head;
            head = head->next;
            delete temp;
        }
        }
        catch (std::exception &e){
            std::cerr << "~List() failed";
            exit(-1);
        }
    }

    class Node {
    public:
        T data;
        Node *next = nullptr;
    private:
        Mutex accessLock = Mutex();
    public:
        Node(const T& given_data): data(given_data) {}
        void lockNode(){accessLock.lock();}
        void unlockNode() { accessLock.unlock();}

    };

    class ThreadSafeCounter{
        Mutex lock = Mutex();
        unsigned int count;

    public:
        ThreadSafeCounter(): count(0) {} //ctor
        void incrementCounter(){
            lock.lock();
            count++;
            lock.unlock();
        }
        void decrementCounter(){
            lock.lock();
            count--;
            lock.unlock();
        }
        unsigned int getCounter() {
			lock.lock();
			unsigned int result = count;
			lock.unlock();
			return result;
		}
    };


    /**
     * Insert new node to list while keeping the list ordered in an ascending order
     * If there is already a node has the same data as @param data then return false (without adding it again)
     * @param data the new data to be added to the list
     * @return true if a new node was added and false otherwise
     */
    bool insertFindTarget(Node *&temp, Node *&prev, const T &data) {
        bool flag = true;
        if (head->data > data) head->lockNode();
        while (temp && temp->data <= data) {
            if (prev) prev->unlockNode();
            if (flag) temp->lockNode();
            if (temp->next)temp->next->lockNode();
            //already exists
            if (temp->data == data) {
                temp->unlockNode();
                if (temp->next) temp->next->unlockNode();
                return false;
            }
            prev = temp;
            temp = temp->next;
            flag = false;
        }
        return true;
    }

    void regularInsert(Node *&prev, Node *&temp, Node *newNode){
        {
            try {
                prev->next = newNode;
                newNode->next = temp;
                size.incrementCounter();
            }
            catch (std::exception &e){
                std::cerr << "bool insert(const T& data) failed";
                exit(-1);
            }
            __insert_test_hook();
            newNode->unlockNode();
            prev->unlockNode();
            if (temp) temp->unlockNode();
        }
    }

    void beginningInsert(Node *&temp, Node *&newNode){
        try {
            head = newNode;
            newNode->next = temp;
            size.incrementCounter();
        }
        catch (std::exception &e){
            std::cerr << "bool insert(const T& data) failed";
            exit(-1);
        }
        __insert_test_hook();
        head->unlockNode();
        head->next->unlockNode();
    }


    void headInsert(const T &data){
        Node *newNode = new Node(data);
        newNode->lockNode();
        try {
            head = newNode;
            size.incrementCounter();
        }
        catch (std::exception &e){
            std::cerr << "bool insert(const T& data) failed";
            exit(-1);
        }
        __insert_test_hook();
        newNode->unlockNode();
    }

    bool insert(const T &data) {
        // in case list is empty
        if (!head) {
            headInsert(data);
            return true;
        }

        Node *temp = head;
        Node *prev = nullptr;
        if (!insertFindTarget(temp, prev, data)) return  false;
        Node *newNode = new Node(data);
        newNode->lockNode();
        //beginning insert
        if (temp == head) beginningInsert(temp, newNode);
        //middle & end insert
        else regularInsert(prev, temp, newNode);
        return true;
    }

    /**
     * Remove the node that its data equals to @param value
     * @param value the data to lookup a node that has the same data to be removed
     * @return true if a matched node was found and removed and false otherwise
     */
    bool remove(const T &value) {
        //init + sanitize inputs
        if (getSize() == 0) return false;
        
        head->lockNode();
        if (head->next) head->next->lockNode();
        Node *current = head;
        head->unlockNode();
        if (head->next) head->next->unlockNode();
        
        Node *prev = nullptr;

        // head removal
        if (head && head->data == value){
            removeHead(current);

            __remove_test_hook();
            return true;
        }

        findTarget(current, prev, value);

        if (current->data == value) { //target found
            removeTarget(current, prev);

            __remove_test_hook();
            prev->unlockNode();
            return true;
        } else { //target unfound
            remove_releaseLocks(current, prev);
            __remove_test_hook();

            return false;
        }
    }

    void removeHead(Node *current) {
        try {
            current->lockNode();
            head = head->next;
            current->unlockNode();
            delete current;
            size.decrementCounter();
        }
        catch (std::exception &e) {
            std::cerr << "bool remove(const T &value) failed";
            exit(-1);
        }
    }

    void findTarget(Node *&current, Node *&prev, const T &value) {
        while (current->next != nullptr && current->data != value) {
            if (!prev) current->lockNode();
            if (prev) prev->unlockNode();
            prev = current;
            current = current->next;
            current->lockNode();
        }
    }

    void removeTarget(Node *&current, Node *&prev) {
        try {
            prev->next = current->next;
            current->unlockNode();
            delete current;
            size.decrementCounter();
        }
        catch (std::exception &e) {
            std::cerr << "bool remove(const T &value) failed";
            exit(-1);
        }
    }

    void remove_releaseLocks(Node *&current, Node *&prev) {
        if (prev) {
            prev->unlockNode();
            current->unlockNode();
        }
    }

    /**
     * Returns the current size of the list
     * @return current size of the list
     */
    unsigned int getSize() {
        unsigned int result;
        try {
            result = size.getCounter();
        }
        catch (std::exception &e) {
            std::cerr << "unsigned int getSize() failed";
            exit(-1);
        }
        return result;
    }


    // Don't remove
    void print() {
        Node *temp = head;
        if (temp == NULL) {
            cout << "";
        } else if (temp->next == NULL) {
            cout << temp->data;
        } else {
            while (temp != NULL) {
                cout << right << setw(3) << temp->data;
                temp = temp->next;
                cout << " ";
            }
        }
        cout << endl;
    }

    // Don't remove
    virtual void __insert_test_hook() {}

    // Don't remove
    virtual void __remove_test_hook() {}

private:
    Node *head;
    ThreadSafeCounter size;
};


#endif //THREAD_SAFE_LIST_H_


