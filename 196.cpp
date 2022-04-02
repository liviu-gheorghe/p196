#include <iostream>
#include <chrono>
#include <ios>
#include <iomanip>
#include <vector>

using namespace std;

const unsigned int BLOCK_SIZE = 10000;

// Struct describing the data of a DL List node
struct node_data {
    int digit{};
    int old_digit{};
    int visited = 0;
};

// Struct describing a DL List node
struct node {

    // A node contains BLOCK_SIZE data slots
    node_data data[BLOCK_SIZE];
    int first_free_position = BLOCK_SIZE - 1;
    node* next{};
    node* prev{};
};


// Struct describing a DL List
struct list {
    node* first;
    node* last;
};

// Add a digit to the list
// If the current block is full, add a node to the end of the DL List
// If the block is not full, add the digit in the first free position and increment it



// The data will be inserted into the slots from end to beginning, so the first available slot will be BLOCK_SIZE -1
// When firstFreePosition is -1, no more data can be added into the first slot

// We only need to push back in a specific node only for the initial node creation (when we're pushing a specific number
// there). After that, we only add digits to the beginning


// The creation of a node is done when a push occurs, with a certain node data.
// In the node, each node_data will be added from left to right in the data vector


void pushBack(list*& l, node_data data, int hard_position = -1) {

    //Check if the last node of the list has all block slots occupied
    if(l->last != nullptr && l->last->first_free_position >=0) {
        // just add the current data into the first available slot and update the first available slot position
        l->last->data[l->last->first_free_position--] = data;
        return;
    }

    // Else, a new node must be added

    node* newNode = new node;
    // Add the node data in the first available position for this block and update the first available slot unless
    // hard_position is not provided
    if(hard_position == -1) {
        newNode->data[newNode->first_free_position--] = data;
    } else {
        // If hard position is set, then put the data in the slot indicated by hard position. The caller of the function
        // will be responsible with updating the first free position of the node
        newNode->data[hard_position] = data;
    }
    newNode->next = nullptr;


    // if the last node of the list is null, then the list is empty
    // This means we need to assign the newNode pointer both to head and tail
    if(l->last == nullptr) {
        l->first = newNode;
        l->last = newNode;
        return;
    }

    // if we have a non-empty list, then do the following
    // The prev node of newNode becomes the current last node
    newNode->prev = l->last;
    // Next node of newNode will be null
    newNode->next = nullptr;
    // The next node of the current last node becomes newNode
    l->last->next = newNode;
    // The last node becomes newNode
    l->last = newNode;
}

// Add a node to the beginning of the DL List
void pushFront(list*& l, node_data data, int hard_position = -1) {



    // Just like for pushBack, we need to check if the list has all block slots occupied

    if(l->first != nullptr && l->first->first_free_position >=0) {
        // just add the current data into the first available slot and update the first available slot position
        l->first->data[l->first->first_free_position--] = data;
        return;
    }

    // Else, a new node must be added


    node* newNode = new node;
    // Add the node data in the first available position for this block and update the first available slot unless
    // hard_position is not provided
    if(hard_position == -1) {
        newNode->data[newNode->first_free_position--] = data;
    } else {
        // If hard position is set, then put the data in the slot indicated by hard position. The caller of the function
        // will be responsible with updating the first free position of the node
        newNode->data[hard_position] = data;
    }
    newNode->next = nullptr;

        // if the first node of the list is null, then the list is empty
        // This means we need to assign the newNode pointer both to head and tail
        if(l->first == nullptr) {
            l->first = newNode;
            l->last = newNode;
            return;
        }

        // if we have a non-empty list, then do the following
        // The next node of newNode becomes the current first node
        newNode->next = l->first;
        // Prev node of new node will be nullptr
        newNode->prev = nullptr;
        // The prev node of the current first node becomes newNode
        l->first->prev = newNode;
        // The first node becomes newNode
        l->first = newNode;

}

// pushes a number to a list
void pushNumberToList(list*& l, int number) {

    //When pushing a number to the list, we need to insert the digits in their order
    // Because pushing with blocks puts the current digit on the leftmost position, we
    // can insert the digits of the number in reverse order

    while(number) {
        // We need to push front because the order of the blocks matter



        // ex we have the number 12345 with a block size of 3

        /**

         Insert 5: ()   ,    (5)
         Insert 4  ()   ,   (45)
         Insert 3  ()   ,  (345)
         Insert 2  (2)  ,  (345)
         Insert 1  (12) ,  (345)

         */

        pushFront(l,{number % 10, number % 10, 0});
        number /= 10;
    }
}

void printList(list*& l, bool reverse = false) {
    // Print the list on the screen


    // If reverse is set to true, the current node becomes the last node instead of fist node
    node* current = reverse ? l->last : l->first;


    int nc = 0;

    while(current != nullptr) {

        //cout << "NODE #" << nc << "\n";
        nc++;
        // For the current node, we just need to loop from first_free_position + 1 to BLOCK_SIZE - 1 and
        // print the elements
        for(unsigned int i = current->first_free_position+1;i < BLOCK_SIZE;i++) {
            cout << current->data[i].digit;
        }

        cout << "\n";
        // If reverse is set to true, the current node becomes the prev node instead of next node
        current = reverse ? current->prev : current->next;
    }

    cout << '\n';
}

int main() {

    //cout << "Introdu numarul de iteratii:";
    cout << "Start computation...\n";
    unsigned int n = 100000;
    //cin>>n;

    // Declare a list

    list* l = new list;
    l->first = nullptr;
    l->last = nullptr;

    // Add 196 to the list
    pushNumberToList(l,196);


    //printList(l);
    //exit(0);


    // Start the timer in order to compute the CPU time of the iteration

    auto start = chrono::high_resolution_clock::now();
    ios_base::sync_with_stdio(false);

    // Variable holding the current interation
    int step = 1;

    // Declare 2 iterators that will be used in the below loop
    node* i;
    node* j;


    int i_node_iterator;
    int j_node_iterator;

    // Create a carry variable which will hold the "left to add" value for the next digit (initially 0)
    int carry = 0;
    // Create a sum variable to hold the current sum
    int sum = 0;


    // While the current iteration count is less than the number of required iterations
    while(step <= n) {


//        if(step % 1000) {
//            cout << step << "\n";
//        }
        // Reverse and add the number stored in the list

        // Each node of the linked list stores a digit of the number.
        // As we need to reverse and add the number, this means that we need to construct
        // a new number by adding each digit with the "opposite" one - the one equally distanced
        // by the end as the current digit


        // So we will initialize the first pointer as the beginning  of the list (pointing to the first digit)
        // and the second pointer to the end of the list (pointing to the last digit)

        /**
         We know that a base-10 numbers sum (each number having k digits) can be a number with at most k+1 digits.
         As we start with 196 (which has three digits), we can make the following observations:
            - After the first iteration, a number consisting of maximum 4 digits will be created
            - After the second iteration, a number consisting of maximum 5 digits will be created
            - ...
            - After the n'th iteration, a number consisting of maximum n+3 digits will be created

         So if that's the case (the resulted sum has one more 2 digits),
         we will allocate memory for the new digit and "pushFront" the node


         We will make this addition "in-place", so the digits of the same list will be altered as we advance through
         the list
        */



        // Initialize the two pointers

        i = l->first;
        j = l->last;
        // Init the carry with 0
        carry = 0;

        // Start from first_free_position + 1 for i node
        i_node_iterator = i->first_free_position + 1;
        //Start from BLOCK_SIZE - 1 for j node
        j_node_iterator = BLOCK_SIZE - 1;

        while(i != nullptr && j != nullptr) {



            // For each node, loop through the populated data slots and make the necessary computation


            while(i_node_iterator < BLOCK_SIZE && j_node_iterator > j->first_free_position) {
                // Update the current sum
                sum = i->data[i_node_iterator].old_digit + j->data[j_node_iterator].old_digit + carry;
                i->data[i_node_iterator].visited++;
                j->data[j_node_iterator].visited++;

                // Put the sum in the current digit (we begin adding "from the back"). So the current digit is the one
                // referenced by j

                // Modulo 10 to handle overflow
                j->data[j_node_iterator].digit = sum % 10;

                // Update the carry
                carry = sum / 10;


                // If we got through a certain node already two times, then copy the new digit in the old digit and reset
                // the counter

                if(i->data[i_node_iterator].visited == 2) {
                    i->data[i_node_iterator].old_digit = i->data[i_node_iterator].digit;
                    i->data[i_node_iterator].visited = 0;
                }


                if(j->data[j_node_iterator].visited == 2) {
                    j->data[j_node_iterator].old_digit = j->data[j_node_iterator].digit;
                    j->data[j_node_iterator].visited = 0;
                }

                // Update the iterators

                i_node_iterator++;
                j_node_iterator--;

            }

            // If one of the iterators hasn't finished going through all the slots, do not go to the next node.
            // Instead, keep the value of [i/j]_node_iterator untouched and increment go to the next / prev node
            // with the other list iterator


            // If i_node_iterator reached BLOCK_SIZE

            if(i_node_iterator >= BLOCK_SIZE) {
                // Then it means that we need to advance pointer i and re-initialize i_node_iterator
                // Update the first pointer
                if(i != nullptr) {
                    i = i->next;
                    // Check if the advanced pointer is not null
                    if(i != nullptr) {
                        // Then update the i_node_iterator value
                        i_node_iterator = i->first_free_position + 1;
                    }
                }
            }

            // Same logic for j_node_iterator
            // If i_node_iterator reached first_free_position
            if(j_node_iterator <= j->first_free_position) {
                // Then it means that we need to advance pointer j and re-initialize j_node_iterator
                if(j != nullptr) {

                    /** If j points to the head of the list (the previous node is null) and the carry is 1,
                     then we have two options


                     - Either push the data into the first slot available of the first node if we still have space
                     - Either create a new node and "pushFront" it. The value of this node will be obviously 1

                     **/

                    // This logic is already implemented in the pushFront method, so we just call it with our data
                    if(j->prev == nullptr && carry == 1) {
                        pushFront(l,{1,1,0});
                        // Now make j null in order to exit the loop
                        j = nullptr;
                    }


                    // Check if the advanced pointer is not null
                    if(j != nullptr) {
                        // Update the second pointer
                        j  = j->prev;
                        // Then re-initialize the i_node_iterator value
                        j_node_iterator = BLOCK_SIZE - 1;
                    }
                }
            }
        }

        // Update the counter
        step++;
    }


    // Compute the execution time of the iteration
    auto end = chrono::high_resolution_clock::now();
    double time_taken =  chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    time_taken *= 1e-6;

    //printList(l);


    // Print the execution time
    cout << "EXECUTION TIME : " << fixed
         << time_taken << setprecision(9);
    cout << " ms" << endl;


    return 0;
}


  //g++ -faggressive-loop-optimizations  -fbranch-probabilities -fbranch-target-load-optimize -fbranch-target-load-optimize2 -fbtr-bb-exclusive           -fcaller-saves -fcombine-stack-adjustments  -fconserve-stack           -fcompare-elim  -fcprop-registers  -fcrossjumping           -fcse-follow-jumps  -fcse-skip-blocks  -fcx-fortran-rules           -fcx-limited-range -fdata-sections  -fdce  -fdelayed-branch           -fdelete-null-pointer-checks  -fdevirtualize           -fdevirtualize-speculatively -fdevirtualize-at-ltrans  -fdse           -fearly-inlining  -fipa-sra  -fexpensive-optimizations           -ffat-lto-objects -ffast-math  -ffinite-math-only           -ffloat-store -fno-ira-share-save-slots -fno-ira-share-spill-slots           -fisolate-erroneous-paths-dereference           -fisolate-erroneous-paths-attribute -fivopts           -fkeep-inline-functions  -fkeep-static-functions          -fkeep-static-consts  -flimit-function-alignment           -flive-range-shrinkage -floop-block  -floop-interchange           -floop-strip-mine -floop-unroll-and-jam  -floop-nest-optimize           -floop-parallelize-all -flra-remat  -flto       -fmerge-all-constants -fmerge-constants  -fmodulo-sched           -fmodulo-sched-allow-regmoves -fmove-loop-invariants -fno-branch-count-reg -fno-defer-pop           -fno-fp-int-builtin-inexact  -fno-function-cse           -fno-guess-branch-probability  -fno-inline  -fno-math-errno           -fno-peephole -fno-peephole2  -fno-printf-return-value -fno-sched-interblock -fno-sched-spec  -fno-signed-zeros           -fno-toplevel-reorder  -fno-trapping-math           -fno-zero-initialized-in-bss -fomit-frame-pointer           -foptimize-sibling-calls -fpartial-inlining  -fpeel-loops           -fpredictive-commoning -fprefetch-loop-arrays           -fprofile-correction -fprofile-use -fno-sched-interblock -fno-sched-spec  -fno-signed-zeros           -fno-toplevel-reorder  -fno-trapping-math           -fno-zero-initialized-in-bss -fomit-frame-pointer -foptimize-sibling-calls -fpartial-inlining  -fpeel-loops  -fpredictive-commoning -fprefetch-loop-arrays  -fprofile-correction -fprofile-use -freorder-blocks-and-partition  -freorder-functions           -funsafe-math-optimizations   -ftree-sra -Ofascleart  -Og 196.cpp -o 196