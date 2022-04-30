#include <iostream>
#include <chrono>
#include <ios>
#include <ctime>
#include <fstream>
#include <vector>
#include<thread>
// unistd header -only for Linux
//#ifdef linux
//#include <unistd.h>
//#endif

using namespace std;


int THREAD_COUNT;
const unsigned int BLOCK_SIZE = 25000;
// Struct describing a DL List node


// generates_carry[i] "knows" if the block with index 0 will yield a carry for the next block
vector<bool> generates_carry = vector<bool>();


// array which will hold the thread objects that will be spawned in order to compute the additions for each block


vector<std::thread> threadPool = vector<std::thread>();

struct node {
    static int count;
    // A node contains BLOCK_SIZE data slots
    char digit[BLOCK_SIZE]{};
    char symmetric_digit[BLOCK_SIZE]{};
    int first_free_position = BLOCK_SIZE - 1;
    int index;
    node* next{};
    node* prev{};

    node() {
        this->index = count++;
    }
};

int node::count = 0;

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

// Add a node to the beginning of the DL List
void pushFront(list*& l, char digit, char symmetric_digit, int hard_position = -1) {

    // Just like for pushBack, we need to check if the list has all block slots occupied

    if(l->first != nullptr && l->first->first_free_position >=0) {
        // just add the current data into the first available slot and update the first available slot position
        l->first->digit[l->first->first_free_position] = digit;
        l->first->symmetric_digit[l->first->first_free_position] = symmetric_digit;
        l->first->first_free_position--;
        return;
    }

    // Else, a new node must be added


    node* newNode = new node;
    // Allocate space for the block in the generates_carry array
    generates_carry.push_back(false);
    // Allocate space for in the threadPool for another thread that will manage
    // the computation for this block
    threadPool.emplace_back();
    // Add the node data in the first available position for this block and update the first available slot unless
    // hard_position is not provided
    if(hard_position == -1) {
        newNode->digit[newNode->first_free_position] = digit;
        newNode->symmetric_digit[newNode->first_free_position] = symmetric_digit;
        newNode->first_free_position--;
    } else {
        // If hard position is set, then put the data in the slot indicated by hard position. The caller of the function
        // will be responsible with updating the first free position of the node
        newNode->digit[hard_position] = digit;
        newNode->symmetric_digit[hard_position] = symmetric_digit;
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

    int reversedNumber = 0;
    int numberCopy = number;
    while(numberCopy) {
        reversedNumber = reversedNumber * 10 + numberCopy % 10;
        numberCopy /= 10;
    }

    //cout << "Reversed number  = " << reversedNumber << "\n";
    // Also, we need to insert the symmetric digit of the current digit

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

        pushFront(l,char(number % 10), char(reversedNumber % 10));
        number /= 10;
        reversedNumber /= 10;
    }
}
//
//void printList(list*& l, bool reverse = false, bool debug = false, bool symmetric_only = false,  string filename = "") {
//    // Print the list on the screen
//
//
//    // If reverse is set to true, the current node becomes the last node instead of fist node
//    node* current = reverse ? l->last : l->first;
//
//
//    int nc = 0;
//    // If a file name is provided, then write the output to that file
//    ofstream fout;
//    if(!filename.empty()) {
//        fout.open(filename);
//    }
//
//    while(current != nullptr) {
//
//        if(debug) {
//            cout << "NODE #" << nc << "(addr -> " << current << ")" << " idx = " << current->index <<  "\n";
//            ++nc;
//        }
//        // For the current node, we just need to loop from first_free_position + 1 to BLOCK_SIZE - 1 and
//        // print the elements
//        for(unsigned int i = current->first_free_position+1;i < BLOCK_SIZE;++i) {
//            if(!filename.empty()) {
//                fout << int(current->digit[i]& 0xF);
//                if (debug) {
//                    // If debug is set to true, also print the symmetric digit
//                    //fout << " r[" << int(current->symmetric_digit[i] & 0xF) << "] ";
//                    cout << ' ';
//                }
//            }
//            else {
//                if(!symmetric_only) {
//                    cout << int(current->digit[i] & 0xF);
//                }
//                if (debug) {
//                    // If debug is set to true, also print the symmetric digit
//                    if(symmetric_only) {
//                        cout << int(current->symmetric_digit[i] & 0xF);
//                    }
//                    else {
//                        cout << " r[" << int(current->symmetric_digit[i] & 0xF) << "] ";
//                    }
//                   cout << ' ';
//                }
//            }
//        }
//
//        if(debug) {
//            if(!filename.empty()) {
//                fout << "\n";
//            } else {
//                cout << "\n";
//            }
//        }
//        // If reverse is set to true, the current node becomes the prev node instead of next node
//        current = reverse ? current->prev : current->next;
//    }
//
//    cout << '\n';
//}
//
//
//void printList2(list*& l) {
//
//    // If reverse is set to true, the current node becomes the last node instead of fist node
//    node* current = l->last;
//
//    while(current != nullptr) {
//
//        // For the current node, we just need to loop from first_free_position + 1 to BLOCK_SIZE - 1 and
//        // print the elements
//        for(int i = BLOCK_SIZE - 1;i > current->first_free_position;i--) {
//            cout << int(current->symmetric_digit[i] & 0xF);
//        }
//
//        // If reverse is set to true, the current node becomes the prev node instead of next node
//        current = current->prev;
//    }
//
//    cout << '\n';
//}

int main() {

    cout << "Start computation...\n";
    unsigned int n = 100000;

    THREAD_COUNT = n / BLOCK_SIZE;

    cout << "TREAD COUNT : " << THREAD_COUNT << "\n";
    //cin>>n;

    // Declare a list

    list* l = new list;
    l->first = nullptr;
    l->last = nullptr;

    // Add 196 to the list
    pushNumberToList(l,196);

    // Measure time using c clock

    double time_spent = 0.0;
    clock_t begin_clock = clock();

    // Measure time using chrono c++ lib
    // Start the timer in order to compute the CPU time of the iteration

    auto start = chrono::high_resolution_clock::now();
    //ios_base::sync_with_stdio(false);

    // Variable holding the current interation
    int step = 1;

    // Declare 2 iterators that will be used in the below loop
    node* i;
    node* j;

    register int i_node_iterator;
    register int j_node_iterator;

    // Create a carry variable which will hold the "left to add" value for the next digit (initially 0)
    int carry = 0;
    // Create a sum variable to hold the current sum
    int sum = 0;

    int carry_propagated = 0;

    int block_id;


    bool second_half;
    bool current_generates_carry;
    // While the current iteration count is less than the number of required iterations
    bool first_block_carry_generated;
    while(step <= n) {

        first_block_carry_generated = false;
        carry_propagated = 0;

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

        // Pt ca am salvat pt fiecare bloc si simetricul, nu mai e nevoie sa "calculam coordonatele" ferestrei curente
        // de procesare, si putem pur si simplu sa calculam cate un bloc in paralel

        // Actualizam generates_carry pt fiecare block

        // Nu conteaza de unde incepem (primul sau ultimul nod)


//        if(step == 501) {
//
//            printList(l, false, true);
//            cout << "\n\n";
//
//            printList2(l);
//        }

        i = l->first;
        while(i != nullptr) {

            current_generates_carry = false;
            i_node_iterator = i->first_free_position +1;
            while(true) {
                // If the current (digit + symmetric digit is >= 10), then the block yields a carry
                if(i->digit[i_node_iterator] + i->symmetric_digit[i_node_iterator] >= 10) {
                    current_generates_carry = true;
                    break;
                }
                else if(i->digit[i_node_iterator] + i->symmetric_digit[i_node_iterator] < 9) {
                    break;
                } else {
                    // The current sum is 9
                    // If we are at the end of the block (very unlikely if the block size is big), then
                    // just break - no carry
                    // Else increment the i_node_iterator
                    if(i_node_iterator >= BLOCK_SIZE - 1) {
                        break;
                    }
                    else {
                        i_node_iterator++;
                    }
                }
            }
            generates_carry[i->index] = current_generates_carry;
            //cout << "Blocul " << i->index << (current_generates_carry ? " genereaza carry\n": " nu genereaza carry\n");
            i = i->next;
        }


        i = l->first;

        while(i != nullptr) {


            block_id = i->index;

            threadPool[block_id] = std::thread([i, block_id, &first_block_carry_generated]() {

                int carry = 0;
                int sum;
                if(block_id > 0) {
                    // The initial carry for this block is 1 if the previous block generated a carry
                    carry = generates_carry[block_id - 1];
                }

                for(int current_digit_idx = BLOCK_SIZE - 1; current_digit_idx > i->first_free_position; current_digit_idx--) {
                    sum = (i->digit[current_digit_idx]  & 0xF) + (i->symmetric_digit[current_digit_idx]) + carry;
                    i->digit[current_digit_idx] = i->digit[current_digit_idx] | ((sum % 10) << 4);
                    carry = sum / 10;
                }

                if(i->prev == nullptr && carry == 1) {
                    first_block_carry_generated = true;
                }

            });

            i = i->next;

        }


        for(block_id = 0; block_id < node::count; block_id++) {
            threadPool[block_id].join();
        }


        if(first_block_carry_generated) {
            pushFront(l, (1 << 4) | 1, 0);
        }

        i = l->first;
        j = l->last;
        j_node_iterator = BLOCK_SIZE - 1;

        second_half = false;


        // Aggregation step, shift all the values from the upper nybble to the lower nybble and update the
        // symmetric digit for each digit


        // Also, find a faster way to check if we're in the second half (cache the prev second half and
        // update the middle position when a new digit is inserted in the list)

        while(i != nullptr) {
            for(i_node_iterator = i->first_free_position + 1; i_node_iterator < BLOCK_SIZE; i_node_iterator++) {


                second_half = second_half | ((i==j && i_node_iterator >= j_node_iterator) || (i->index < j->index));

                i->digit[i_node_iterator] = (i->digit[i_node_iterator] >> 4) & 0xF;
                i->symmetric_digit[i_node_iterator] = (j->digit[j_node_iterator] >> (4 * !(second_half))) & 0xF;
                if(j_node_iterator == 0) {
                    j_node_iterator = BLOCK_SIZE - 1;
                    j = j->prev;
                }
                else {
                    j_node_iterator--;
                }
            }
            i = i->next;
        }

        // Update the counter
        ++step;
    }


    // Compute the execution time of the iteration
    auto end = chrono::high_resolution_clock::now();
    double time_taken =  chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    time_taken *= 1e-6;



    clock_t end_clock = clock();

    // calculate elapsed time by finding difference (end - begin) and
    // dividing the difference by CLOCKS_PER_SEC to convert to seconds
    time_spent += (double)(end_clock - begin_clock) / CLOCKS_PER_SEC ;
    time_spent *= 1000;

    printf("The elapsed time computed using C clock is %f ms\n", time_spent);

    // Print the execution time
    cout << "EXECUTION TIME (chrono): " << time_taken;
    cout << " ms" << endl;


    // ndig = 41490 for 100k iterations


//    if(!carry_propagated) {
//        cout << "The number has reached a palindrome after " << step << " iterations\n";
//    }
//
//    printList(l, false, true);
//    cout << "\n\n";
//
//    printList2(l);
    return 0;
}


// What is the probability of having a carry from a N digit number add-and reverse computation?
/**
 Let the number be of the following form
        N = d1 d2 d3 d4 d5 d6 ... dn

        So the computation that must be made has the following form:


        d(1)   d(2)    d(3)    d(4)    d(5)    d(6)  .... d(n)
        d(n)  d(n-1)  d(n-2)  d(n-3)  d(n-4)  d(n-5) .... d(1)

         First of all, if (d(1) + d(n)) > 9 then we have a carry


         All cases: d1 goes from 1 to 9, d2 goes from 1 to 9 => 81 total cases (the first digit of a number cannot be 0)

        Compatible cases:

         - d1 == 1 => d2 == 9 :  1 case
         - d2 == 2 => d2 == {8,9}: 2 cases
         - d2 == 3 => d2 == {7,8,9}: 3 cases
         .....
         - d2 = 9 => d2 == {1,2,3,4,5,6,7,8,9}: 9 cases

         Total compatible cases: 9 * 8 / 2 = 36
 */


//g++ -faggressive-loop-optimizations  -fbranch-probabilities -fbranch-target-load-optimize -fbranch-target-load-optimize2 -fbtr-bb-exclusive           -fcaller-saves -fcombine-stack-adjustments  -fconserve-stack           -fcompare-elim  -fcprop-registers  -fcrossjumping           -fcse-follow-jumps  -fcse-skip-blocks  -fcx-fortran-rules           -fcx-limited-range -fdata-sections  -fdce  -fdelayed-branch           -fdelete-null-pointer-checks  -fdevirtualize           -fdevirtualize-speculatively -fdevirtualize-at-ltrans  -fdse           -fearly-inlining  -fipa-sra  -fexpensive-optimizations           -ffat-lto-objects -ffast-math  -ffinite-math-only           -ffloat-store -fno-ira-share-save-slots -fno-ira-share-spill-slots           -fisolate-erroneous-paths-dereference           -fisolate-erroneous-paths-attribute -fivopts           -fkeep-inline-functions  -fkeep-static-functions          -fkeep-static-consts  -flimit-function-alignment           -flive-range-shrinkage -floop-block  -floop-interchange           -floop-strip-mine -floop-unroll-and-jam  -floop-nest-optimize           -floop-parallelize-all -flra-remat  -flto       -fmerge-all-constants -fmerge-constants  -fmodulo-sched           -fmodulo-sched-allow-regmoves -fmove-loop-invariants -fno-branch-count-reg -fno-defer-pop           -fno-fp-int-builtin-inexact  -fno-function-cse           -fno-guess-branch-probability  -fno-inline  -fno-math-errno           -fno-peephole -fno-peephole2  -fno-printf-return-value -fno-sched-interblock -fno-sched-spec  -fno-signed-zeros           -fno-toplevel-reorder  -fno-trapping-math           -fno-zero-initialized-in-bss -fomit-frame-pointer           -foptimize-sibling-calls -fpartial-inlining  -fpeel-loops           -fpredictive-commoning -fprefetch-loop-arrays           -fprofile-correction -fprofile-use -fno-sched-interblock -fno-sched-spec  -fno-signed-zeros           -fno-toplevel-reorder  -fno-trapping-math           -fno-zero-initialized-in-bss -fomit-frame-pointer -foptimize-sibling-calls -fpartial-inlining  -fpeel-loops  -fpredictive-commoning -fprefetch-loop-arrays  -fprofile-correction -fprofile-use -freorder-blocks-and-partition  -freorder-functions           -funsafe-math-optimizations   -ftree-sra -Ofascleart  -Og 196.cpp -o 196