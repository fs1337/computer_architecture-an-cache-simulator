/*
Cache Simulator
Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
s = log2(#sets)   b = log2(block size)  t=32-s-b
*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>

using namespace std;
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss

struct config{
       int L1blocksize;
       int L1setsize;
       int L1size;
       int L2blocksize;
       int L2setsize;
       int L2size;
       };

/* you can define the cache class here, or design your own data structure for L1 and L2 cache
class cache {
      
      }
*/

struct Block
{
    bitset<32> tagBlock;
    bitset<1> validBit;
};

unsigned long test = 0;

class cache{
public:
    cache(int indexValue, int setValue)
    {
        totalIndexNumber = pow(2,indexValue);
        Block block1;
        block1.tagBlock = bitset<32> (0);
        block1.validBit = bitset<1> (0);
        
        setSet(setValue);
        setIndex(indexValue);
        tag.resize(setValue*totalIndexNumber);
        tag[0] = block1;
    }
    
    void setCounterSetAssociative(int index)
    {
        counter[index] =  counter[index] + 1;
       
        if(counter[index] >= set)
        {
            counter[index] = 0;
        }
    }
    
    int getCounterSetAssociative(int index)
    {
        return counter[index];
    }
    
    void setCounterFullyAssociative()
    {
        counter[0] = counter[0] + 1;
        if(counter[0] >= getTotalIndexNumber())
        {
            counter[0] = 0;
        }
    }
    
    int getCounterFullyAssociative()
    {
        return counter[0];
    }

    void initialize(int offsetBits, int tagBits)
    {
        setOffsetBits(offsetBits);
        setTagBits(tagBits);
        //not fully-associative by default
        setFullyAssociative(0);
    }
    
    //initialize counter
    void initialize_counter()
    {
        int counterValue = 0;
        if(getFullyAssociative() == 1)
        {
            counter.resize(1);
            counter[0] = counterValue;
        }
        else
        {
            counter.resize(getTotalIndexNumber());
            counter[0] = counterValue;
        }
    }
    
    void test()
    {
        int i = 0;
        /*for(i = 0; i < pow(2,index)*set; i ++)
        {
            //printf("index %d\n", i);
            //printf("tag %lx validBits%lx\n", tag[i].tagBlock.to_ulong(), tag[i].validBit.to_ulong());
        }*/
        printf("offsetBits %d set %d index %d tagBits %d totalIndexNumber %d\n", getOffsetBits(), getSet(), getIndex(), getTagBits(), getTotalIndexNumber());
    }
    
    int getOffsetBits()
    {
        return offsetBits;
    }
    void setOffsetBits(int offsetBitsValue)
    {
        offsetBits = offsetBitsValue;
    }
    
    void setIndex(int indexValue)
    {
        index = indexValue;
    }
    
    int getIndex()
    {
        return index;
    }
    
    int getTagBits()
    {
        return tagbits;
    }
    void setTagBits(int tagBitsValue)
    {
        tagbits = tagBitsValue;
    }
    
    int getSet()
    {
        return set;
    }
    void setSet(int setValue)
    {
        set = setValue;
    }
    
    int getTotalIndexNumber()
    {
        return totalIndexNumber;
    }
    
    void setFullyAssociative(int flag)
    {
        fullyAssociative = flag;
    }
    
    int getFullyAssociative()
    {
        return fullyAssociative;
    }
    
private:
    //cache related
    int index;
    int set;
    int tagbits;
    int offsetBits;
    int fullyAssociative;
    
    //RR counter
    int totalIndexNumber;
    
public:
    vector<Block >tag;
    vector<int > counter;//RR counter
};

void initializeCacheParameter(int blockSize, int setSize, int size, int &index,int &offsetBits, int &set, int &tagBits);
int checkMissOrHit(int cacheLayer, cache *cacheTemp, bitset<32> addr);
void updateTagAndValueBits(cache *cacheTemp, bitset<32> addr);
void updateL1L2AccessState(int accessState, int &L1AcceState, int &L2AcceState, int accessType);

int main(int argc, char* argv[]){
    
    config cacheconfig;
    ifstream cache_params;
    string dummyLine;
    cache_params.open(argv[1]);
    while(!cache_params.eof())  // read config file
    {
      cache_params>>dummyLine;
      cache_params>>cacheconfig.L1blocksize;
      cache_params>>cacheconfig.L1setsize;              
      cache_params>>cacheconfig.L1size;
      cache_params>>dummyLine;              
      cache_params>>cacheconfig.L2blocksize;           
      cache_params>>cacheconfig.L2setsize;        
      cache_params>>cacheconfig.L2size;
    }
  
   // Implement by you: 
   // initialize the hirearch cache system with those configs
   // probably you may define a Cache class for L1 and L2, or any data structure you like
    
    int index, offsetBits, set, tagBits;
    //initialize L1 cache
    initializeCacheParameter(cacheconfig.L1blocksize, cacheconfig.L1setsize, cacheconfig.L1size, index, offsetBits, set, tagBits);
    cache *cacheL1 = new cache(index,set);
    cacheL1->initialize(offsetBits, tagBits);
    if(cacheconfig.L1setsize == 0)
    {
        cacheL1->setFullyAssociative(1);
    }
    cacheL1->initialize_counter();
    
    //initialize L2 cache
    initializeCacheParameter(cacheconfig.L2blocksize, cacheconfig.L2setsize, cacheconfig.L2size, index, offsetBits, set, tagBits);
    cache *cacheL2 = new cache(index, set);
    cacheL2->initialize(offsetBits, tagBits);
    if(cacheconfig.L2setsize == 0)
    {
        cacheL2->setFullyAssociative(1);
    }
    cacheL2->initialize_counter();
    
    //test
    //cacheL1->test();
    //cacheL2->test();
    
    int L1AcceState =0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
    int L2AcceState =0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;
   
    ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string(argv[2]) + ".out";
    
    traces.open(argv[2]);
    tracesout.open(outname.c_str());
    
    string line;
    string accesstype;  // the Read/Write access type from the memory trace;
    string xaddr;       // the address from the memory trace store in hex;
    unsigned int addr;  // the address from the memory trace store in unsigned int;        
    bitset<32> accessaddr; // the address from the memory trace store in the bitset;
    
    if (traces.is_open()&&tracesout.is_open()){
        while (getline (traces,line)){   // read mem access file and access Cache
            istringstream iss(line); 
            if (!(iss >> accesstype >> xaddr)) {break;}
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32> (addr);
           
            test ++;
            int accessState = 0;
           // access the L1 and L2 Cache according to the trace;
           // read
            if (accesstype.compare("R")==0)
            {
                //test
                //check
                bitset<32> addrTag(0);
                unsigned int indexCache;
                
                indexCache = int(accessaddr.to_ulong());
                indexCache = indexCache << cacheL2->getTagBits();//shift left logic, shift tagBits to left
                indexCache = indexCache >> (cacheL2->getTagBits() + cacheL2->getOffsetBits());//shift tagBits+offsetbits to right
                addrTag = accessaddr >> (cacheL2->getIndex() + cacheL2->getOffsetBits());
                
                // Implement by you:
                // read access to the L1 Cache,
                accessState = checkMissOrHit(1, cacheL1, accessaddr);
                // and then L2 (if required),
                if(accessState == 0)//L1 read miss
                {
                    // read access to the L2 Cache
                    accessState = checkMissOrHit(2, cacheL2, accessaddr);
                    if(accessState == 0)//L1 read miss and L2 read miss
                    {
                        //get data from Main memory
                        //update Tag and value bits, RM in L2
                        updateTagAndValueBits(cacheL2, accessaddr);
                        //printf("getCounter L2 %d\n", cacheL2->getSet());
                        //update Tag and value bits, RM in L1
                        updateTagAndValueBits(cacheL1, accessaddr);
                    }
                    else //L2 read hit
                    {
                        //L2 forward data to L1
                        //update Tag and value bits in L1
                        updateTagAndValueBits(cacheL1, accessaddr);
                    }
                }
                 // update the L1 and L2 access state variable;
                updateL1L2AccessState(accessState, L1AcceState, L2AcceState, 0);
             }
             else 
             {    
                 // Implement by you:
                 // write access to the L1 Cache,
                 accessState = checkMissOrHit(1, cacheL1, accessaddr);
                 // and then L2 (if required),need to write access to L2
                 if(accessState == 0)
                 {
                     accessState = checkMissOrHit(2, cacheL2, accessaddr);
                 }
                // update the L1 and L2 access state variable;
                 updateL1L2AccessState(accessState, L1AcceState, L2AcceState, 1);
            }

            tracesout<< L1AcceState<< " "<< L2AcceState <<endl;
        }
        traces.close();
        tracesout.close();
    }
    else cout<< "Unable to open trace or traceout file ";
    
    delete cacheL1;
    delete cacheL2;
   
    return 0;
}

void initializeCacheParameter(int blockSize, int setSize, int size, int &index,int &offsetBits, int &set, int &tagBits)
{
    //fully associative
    if(setSize == 0)
    {
        set = 1;
        offsetBits = log2(blockSize);
        tagBits = 32 - offsetBits;
        index = (log2(size) + 10) - log2(blockSize);
    }
    else
    {
        offsetBits = log2(blockSize);//8B
        set = log2(setSize);//1
        index = log2(size) + 10 - offsetBits - set;
        tagBits = 32 - index - offsetBits;
        set = setSize;
    }
}

int checkMissOrHit(int cacheLayer, cache *cacheTemp, bitset<32> accessaddr)
{
    bitset<32> cacheTag(0);
    bitset<32> addrTag(0);
    int accessState = 0;
    int i = 0;
    int validBit = 0;
    unsigned int indexCache = 0;
    int index = 0;
    
    //get total index number
    index = cacheTemp->getTotalIndexNumber();
    //for fully associative
    addrTag = accessaddr >> cacheTemp->getOffsetBits();
    if(cacheTemp->getFullyAssociative() == 1)
    {
        for(i = 0; i < index - 1; i ++)
        {
            cacheTag = cacheTemp->tag[i].tagBlock;
            validBit = cacheTemp->tag[i].validBit.test(0);
            
            if((cacheTag == addrTag) && validBit)//find one hit in cache
            {
                accessState = cacheLayer; //write hit
                break;
            }
        }
        return accessState;
    }
    
    //for set-associative
    //get index from accessAddr
    indexCache = int(accessaddr.to_ulong());
    indexCache = indexCache << cacheTemp->getTagBits();//shift left logic, shift tagBits to left
    indexCache = indexCache >> (cacheTemp->getTagBits() + cacheTemp->getOffsetBits());//shift tagBits+offsetbits to right
    //get tagValue from accessAddr
    addrTag = accessaddr >> (cacheTemp->getIndex() + cacheTemp->getOffsetBits());//get tagValue from address
    accessState = 0;
    
    //if there is more than one way in cache, check every way until find one matched block
    for(i = 0; i < cacheTemp->getSet(); i ++)
    {
        cacheTag = cacheTemp->tag[indexCache + i*index].tagBlock;           //get tagValue in the cache index
        validBit = cacheTemp->tag[indexCache + i*index].validBit.test(0);   //get the value if validbit
        
        if((cacheTag == addrTag) && validBit)//find one hit in L1 cache
        {
            accessState = cacheLayer; //write hit
            break;
        }
    }
    
    return accessState;
}

void updateTagAndValueBits(cache *cacheTemp, bitset<32> accessaddr)
{
    int i = 0;
    int flag = 0;////check whether there is a empty space or not
    int counter = 0;
    bitset<32> cacheTag(0);
    bitset<32> addrTag(0);
    int index = 0;
    unsigned int indexCache = 0;
    int validBit = 0;
    
    //get total index number
    index = cacheTemp->getTotalIndexNumber();
    //fully-associative
    addrTag = accessaddr >> cacheTemp->getOffsetBits();
    if(cacheTemp->getFullyAssociative() == 1)
    {
        for(i = 0; i < index - 1; i ++)
        {
            validBit = cacheTemp->tag[i].validBit.test(0);
            if(validBit == 0)//not valid, update Tag value
            {
                flag = 1;
                cacheTemp->tag[i].tagBlock = addrTag;
                cacheTemp->tag[i].validBit.set();
                break;
            }
        }
        
        //no empty space, evict data for new data
        if(flag == 0)
        {
            counter = cacheTemp->getCounterFullyAssociative();
            cacheTemp->tag[counter].tagBlock = addrTag;
            cacheTemp->setCounterFullyAssociative();
        }
        
        return;
    }
    
    //set-associative
    //get index from accessAddr
    indexCache = int(accessaddr.to_ulong());
    indexCache = indexCache << cacheTemp->getTagBits();//shift left logic, shift tagBits to left
    indexCache = indexCache >> (cacheTemp->getTagBits() + cacheTemp->getOffsetBits());//shift tagBits+offsetbits to right
    //get tagValue from accessAddr
    addrTag = accessaddr >> (cacheTemp->getIndex() + cacheTemp->getOffsetBits());//get tagValue from address
    
    for(i = 0; i < cacheTemp->getSet(); i ++)
    {
        validBit = cacheTemp->tag[indexCache + i*index].validBit.test(0);
        if(validBit == 0)//not valid, update Tag value
        {
            flag = 1;
            cacheTemp->tag[indexCache + i*index].tagBlock = addrTag;
            cacheTemp->tag[indexCache + i*index].validBit.set();
            break;
        }
    }
    //no empty space, evict data for new data
    if(flag == 0)
    {
        counter = cacheTemp->getCounterSetAssociative(indexCache);
        cacheTemp->tag[indexCache + counter*index].tagBlock = addrTag;
        cacheTemp->setCounterSetAssociative(indexCache);
    }
}

void updateL1L2AccessState(int accessState, int &L1AcceState, int &L2AcceState, int accessType)
{
    switch(accessState)
    {
        case 1:
        {
            if(accessType == 0)//read
            {
                L1AcceState = RH;
                L2AcceState = NA;
                
            }
            else//write
            {
                L1AcceState = WH;
                L2AcceState = NA;
            }
            
        }
        break;
        case 2:
        {
            if(accessType == 0)//read
            {
                L1AcceState = RM;
                L2AcceState = RH;
            }
            else//write
            {
                L1AcceState = WM;
                L2AcceState = WH;
            }
        }
        break;
        case 0:
        default:
        {
            if(accessType == 0)//read
            {
                L1AcceState = RM;
                L2AcceState = RM;
                
            }
            else//write
            {
                L1AcceState = WM;
                L2AcceState = WM;
            }
        }
        break;
    }
}


