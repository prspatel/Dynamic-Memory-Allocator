/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "debug.h"
#include "sfmm.h"
void colasce(void *c);
void *sf_malloc(size_t size) {
    if(size<=0){
            return NULL;
    }
    if(sf_free_list_head.body.links.next == NULL){

        void *p = sf_mem_grow();

        if(p == NULL){
            sf_errno=ENOMEM;
            return NULL;
        }

        sf_prologue t;
        sf_prologue *t1 = (sf_prologue *)sf_mem_start();
        *t1=t;
        (*t1).block.header.requested_size = 0;
        (*t1).block.header.block_size = 33;
        (*t1).footer.requested_size = 0;
        (*t1).footer.block_size = 33;

        t1++;

        sf_epilogue k;
        sf_epilogue *k1 = (sf_epilogue *)sf_mem_end();
        k1--;
        k.header.requested_size = 0;
        k.header.block_size = 1;
        *k1=k;

        sf_block pointer;
        sf_block *timepass=&pointer;

        ( *timepass).header.requested_size = 0;
        ( *timepass).header.block_size = 4048;
        ( *timepass).body.links.next = &sf_free_list_head;
        ( *timepass).body.links.prev = &sf_free_list_head;
        sf_block *point = (sf_block*)t1;
        sf_free_list_head.body.links.next = point;
        sf_free_list_head.body.links.prev = point;

        sf_header fo;
        fo.requested_size=0;
        fo.block_size=4048;
        sf_header *fo1 = (sf_header *)k1;
        fo1--;

        t1++;
        *point=pointer;
        *fo1=fo;

        //sf_show_heap();

    }
    int headerplussize = size+8;

    int quotient = headerplussize/16;
    int remainder = headerplussize % 16;
    int finalSize=0;
    if(headerplussize<32){
        finalSize=32;
    }
    else{
        if(remainder!=0){
            finalSize =quotient * 16+16;
        }
        else{
            finalSize = headerplussize;
        }
    }
    int index = (finalSize-32)/16;
    if(index>10 || sf_quick_lists[index].first==NULL){
        while(1){

            sf_block *head = sf_free_list_head.body.links.next;

            while(head!=&sf_free_list_head){

                if((*head).header.block_size>=finalSize){
                    if((*head).header.block_size-finalSize>=32){
                        //split
                        sf_block* nextC = head->body.links.next;
                        sf_block* prevC = head->body.links.prev;

                        (*((*head).body.links.prev)).body.links.next =nextC;
                        (*((*head).body.links.next)).body.links.prev =prevC;
                        sf_block newBlock;
                        unsigned int bs = (*head).header.block_size;
                        bs>>=2;
                        bs<<=2;
                        newBlock.header.requested_size =(*head).header.requested_size;
                        newBlock.header.block_size= bs-finalSize;
                        newBlock.header.block_size+=2; //previous is allocated
                        newBlock.body.links.next = sf_free_list_head.body.links.next;

                        newBlock.body.links.prev = &sf_free_list_head;

                        void *newHeader = head;
                        newHeader+=finalSize;

                        (*(sf_free_list_head.body.links.next)).body.links.prev = (sf_block*)newHeader;
                        sf_free_list_head.body.links.next = (sf_block*)newHeader;
                        *((sf_block*)newHeader)= newBlock;
                        newHeader+=newBlock.header.block_size;
                        newHeader-=2;
                        sf_header *footer = (sf_header*)newHeader;
                        footer--;
                        (*footer).block_size=newBlock.header.block_size-2;
                        (*footer).block_size+=2; //previous allocated

                        (*head).header.block_size = finalSize+1 +((*head).header.block_size % 16); //making it allocated
                        (*head).header.requested_size= size;

                        return (void*)head+8;

                    }
                    else{

                        sf_block* nextC = head->body.links.next;
                        sf_block* prevC = head->body.links.prev;

                        (*((*head).body.links.prev)).body.links.next =nextC;
                        (*((*head).body.links.next)).body.links.prev =prevC;
                        unsigned int bs = (*head).header.block_size;
                        bs>>=2;
                        bs<<=2;
                        (*head).header.requested_size = size;
                        (*head).header.block_size++;//alocated
                        void *k = head;
                        k+=bs;
                        void *ep = sf_mem_end();
                        sf_epilogue *epP = (sf_epilogue*)ep;
                        epP--;
                        if(k==(void*)epP){
                            (*epP).header.block_size+=2;
                        }
                        else{
                            sf_block *next = (sf_block*)k;
                            (*next).header.block_size+=2;
                        }
                        return (void*)head+8;
                    }
                }
                head = (*head).body.links.next;
            }
            //debug("Hello");
            void *newMem = sf_mem_grow();

             if(newMem == NULL){
                sf_errno=ENOMEM;
                return NULL;
            }

            sf_epilogue k;
            sf_epilogue *k1 = (sf_epilogue *)sf_mem_end();
            k1--;
            k.header.requested_size = 0;
            k.header.block_size = 1;
            *k1=k;
            k1--;
            sf_header footer;
            footer.requested_size= 0;
            footer.block_size = 4096;
            *((sf_header*) k1)=footer;

            sf_epilogue *epi = (sf_epilogue*)newMem;
            epi--;//store the block at epi
            int epiS = (*epi).header.block_size;


            void *k2 = epi;

            sf_block newBheader;
            newBheader.header.block_size = 4096;
            newBheader.header.requested_size = 0 ;
            *((sf_block*)k2) = newBheader;

            if(epiS==1){
                //colasce(k2);
                //sf_show_heap();
                //collate coz previous is  a free pointer.
                sf_header *prevFoot = (sf_header*)k2;
                prevFoot--;
                //debug("Hello");
                unsigned int prevBS = (*prevFoot).block_size;
                prevBS>>=2;
                prevBS<<=2;
                //debug("%d", prevBS);
                void *prev = prevFoot;
                prev-=(prevBS-8);//previous block header
                sf_block *prevBlock = (sf_block *)prev;
                //debug("%d", (*prevBlock).header.block_size);

                sf_block *kutriya =   (*prevBlock).body.links.next;
                int n_size = newBheader.header.block_size;

                (*((*prevBlock).body.links.prev)).body.links.next =kutriya;
                (*prevBlock).header.block_size += n_size;

                kutriya = sf_free_list_head.body.links.next;
                (*prevBlock).body.links.next =kutriya ;

                (*(sf_free_list_head.body.links.next)).body.links.prev = prevBlock;
                sf_free_list_head.body.links.next = prevBlock;
                (*prevBlock).body.links.prev = &sf_free_list_head;
                (*((sf_header*) k1)).block_size = (*prevBlock).header.block_size;
            }
            else{
                newBheader.body.links.next = sf_free_list_head.body.links.next;
                (*(sf_free_list_head.body.links.next)).body.links.prev = (sf_block*)k2;
                newBheader.body.links.prev = &sf_free_list_head;
                sf_free_list_head.body.links.next = (sf_block*)k2;
            }
            //debug("mem");
            //sf_show_heap();

        }

    }
    else{
        sf_block *exactPointer = sf_quick_lists[index].first;
        if(sf_quick_lists[index].length==1){
            sf_quick_lists[index].length=0;
            sf_quick_lists[index].first=NULL;
        }
        else{
            sf_quick_lists[index].first = (*sf_quick_lists[index].first).body.links.next;
            (*exactPointer).header.requested_size = size;
            sf_quick_lists[index].length--;
        }

        return (void *)exactPointer+8;
    }

    return NULL;

}

void sf_free(void *pp) {
    if(pp==NULL){
        abort();
    }
    pp-=8;

    if(pp<sf_mem_start()||pp>sf_mem_end()){
        abort();
    }
    sf_block *freeB = (sf_block*)pp;

 //debug("%d",(*freeB).header.block_size);
    int bs  = (*freeB).header.block_size;

    if(bs%16==0||bs%16==2){
        abort();
    }
    bs>>=2;
    bs<<=2;

    if(bs%16!=0 || bs<32){

        abort();
    }
    int req_Size =(*freeB).header.requested_size;

    if(req_Size+8>bs){
        abort();
    }
    int b_size = (*freeB).header.block_size;
    if(b_size%16==1){
        //previous is free
        void*k = sf_mem_start();
        k+=(sizeof(sf_prologue));


        if( pp != k){
           sf_header* prevF = (sf_header*)(pp-8);
            if((*prevF).block_size%16==1||(*prevF).block_size%16==3){
                //debug("HERE");
                abort();
                }
            int size = (*prevF).block_size;
            size=size>>2;
            size=size<<2;
            sf_block* prevHeader = (sf_block*)(pp - size);
            if((*prevHeader).header.block_size%16==0||(*prevHeader).header.block_size%16==3){
                //debug("HERE");
                abort();
            }
        }

    }

    bs=bs-32;



    (*freeB).header.requested_size=0;
    //(*freeB).header.block_size; //made it free
    (*freeB).body.links.next = NULL;
    (*freeB).body.links.prev = NULL;

    sf_header footer;
    footer.requested_size=0;
    footer.block_size = (*freeB).header.block_size;

    void *foot = (void *)freeB;
    foot+=(bs+32-8);
    (*((sf_header*)foot)) = footer;

    if(bs/16>=10){
        //add this to free list
        colasce(freeB);
    }
    else{

        if(sf_quick_lists[bs/16].length==0){
            sf_quick_lists[bs/16].first = freeB;
            sf_quick_lists[bs/16].length++;
        }
        else if(sf_quick_lists[bs/16].length<5){
            (*freeB).body.links.next = sf_quick_lists[bs/16].first;
            sf_quick_lists[bs/16].first = freeB;
            sf_quick_lists[bs/16].length++;
        }
        else{

            int i=0;
            while(i<5){

                sf_block *firstP = sf_quick_lists[bs/16].first;
                sf_quick_lists[bs/16].first = (*(sf_quick_lists[bs/16].first)).body.links.next;
                colasce(firstP);
                //firstP = (*firstP).body.links.next;
                sf_quick_lists[bs/16].length--;

                i++;
            }
            sf_quick_lists[bs/16].first = freeB;
            sf_quick_lists[bs/16].length=1;
        }
    }
}

void *sf_realloc(void *pp, size_t rsize) {
    if(pp==NULL){
        abort();
    }
    pp-=8;

    if(pp<sf_mem_start()||pp>sf_mem_end()){
        abort();
    }
    sf_block *freeB = (sf_block*)pp;

 //debug("%d",(*freeB).header.block_size);
    int bs  = (*freeB).header.block_size;

    if(bs%16==0||bs%16==2){
        abort();
    }
    bs>>=2;
    bs<<=2;

    if(bs%16!=0 || bs<32){

        abort();
    }
    int req_Size =(*freeB).header.requested_size;

    if(req_Size+8>bs){
        abort();
    }
    int b_size = (*freeB).header.block_size;
    if(b_size%16==1){
        //previous is free
        void*k = sf_mem_start();
        k+=(sizeof(sf_prologue));


        if( pp != k){
           sf_header* prevF = (sf_header*)(pp-8);
            if((*prevF).block_size%16==1||(*prevF).block_size%16==3){
                //debug("HERE");
                abort();
                }
            int size = (*prevF).block_size;
            size=size>>2;
            size=size<<2;
            sf_block* prevHeader = (sf_block*)(pp - size);
            if((*prevHeader).header.block_size%16==0||(*prevHeader).header.block_size%16==3){
                //debug("HERE");
                abort();
            }
        }

    }

    if(rsize<(*freeB).header.requested_size){
        //small requested
        if((*freeB).header.requested_size-rsize<32){
            (*freeB).header.requested_size=rsize;
            return (void*)freeB+8;
        }
        else{

            // int headerplussize = rsize+8;

            // int quotient = headerplussize/16;
            // int remainder = headerplussize % 16;
            // int finalSize=0;
            // if(remainder!=0){
            //         finalSize =quotient * 16+16;
            // }
            // else{
            //     finalSize = headerplussize;
            // }
            // sf_block newHeader;
            // newHeader.header.requested_size = rsize;
            // newHeader.header.block_size = finalSize;
        }
    }
    else{
        void *p = sf_malloc(rsize);
        if(p==NULL){
            return NULL;
        }
        pp+=8;
        memcpy(pp, p, (*freeB).header.requested_size);
        sf_free(pp);
        return p;

    }
    return NULL;
}

void colasce(void *c){

    sf_block *sBlock = (sf_block*)c;
    //(*sBlock).header.block_size--;
    int k = (*sBlock).header.block_size%16;
    bool previous =false;
    bool next = false;
    void *prol = sf_mem_start();
    prol+=sizeof(sf_prologue);
    if(prol == c){
        previous=true;
    }
    else if(k==2||k==3){
        previous = true;
    }
    unsigned int size = (*sBlock).header.block_size;
    size = size>>2;
    size = size<<2;
    c+=size;
    sf_epilogue *epi =(sf_epilogue*) sf_mem_end();
    epi--;

    sf_block *nextBlock;
    if(c!=(void*)epi){//next is not epi
        nextBlock = (sf_block*)c;
        if((*nextBlock).header.block_size%16==1 || (*nextBlock).header.block_size%16==3){
            next=true;
            (*nextBlock).header.block_size-=2;//previous is free
        }
    }
    else{
        next=true;
        (*epi).header.block_size-=2;
    }

    //bool changelinks =true;
    c-=size;
    sf_block *finalStart;
    //cases
    if(previous==false && next==false){//collate top and bottom
        sf_header *prevBfooter =  (sf_header*)c;
        prevBfooter--;
        unsigned int prevBsize = (*prevBfooter).block_size;
        prevBsize = prevBsize>>2;
        prevBsize = prevBsize<<2;

        sf_block *prevB = (sf_block*)(c-prevBsize);

        unsigned int nextBsize = (*nextBlock).header.block_size;
        nextBsize = nextBsize>>2;
        nextBsize = nextBsize<<2;

        (*prevB).header.block_size+=(size+nextBsize);
//hello
        void *k = prevB;
        k+=((*prevB).header.block_size-8);
        sf_header *footer = (sf_header*)k;
        (*footer).block_size = (*prevB).header.block_size;

        sf_block *nextC = nextBlock->body.links.next;
        sf_block *prevC = prevB->body.links.prev;

        (*((*prevB).body.links.prev)).body.links.next = nextC;
        (*((*nextBlock).body.links.next)).body.links.prev = prevC;
        finalStart = prevB;

    }
    else if(previous==false && next==true){//collate top
        sf_header *prevBfooter =  (sf_header*)c;
        prevBfooter--;
        unsigned int prevBsize = (*prevBfooter).block_size;
        prevBsize >>=2;
        prevBsize <<=2;

        sf_block *prevB = (sf_block*)(c-prevBsize);

        (*prevB).header.block_size += size;

        sf_header *newf = (sf_header*)(c+size);
        newf--;
        sf_header footer ;
        footer.requested_size =0;
        footer.block_size = (*prevB).header.block_size;
        *newf = footer;
        sf_block *nextC = prevB->body.links.next;
        sf_block *prevC = prevB->body.links.prev;
        (*((*prevB).body.links.prev)).body.links.next = nextC;
        (*((*prevB).body.links.next)).body.links.prev = prevC;
        finalStart = prevB;



    }
    else if(previous==true && next==false){//collate bottom

        unsigned int nextBsize = (*nextBlock).header.block_size;
        nextBsize = nextBsize>>2;
        nextBsize = nextBsize<<2;
        (*sBlock).header.block_size += nextBsize;
        void *k = nextBlock;
        k+=(nextBsize-8);
        sf_header *footer = (sf_header*)k;
        (*sBlock).header.block_size--;
        (*footer).block_size = (*sBlock).header.block_size;

        sf_block* nextC =  nextBlock->body.links.next;
        sf_block* prevC =  nextBlock->body.links.prev;
        (*((*nextBlock).body.links.prev)).body.links.next = nextC; //skipping
        (*((*nextBlock).body.links.next)).body.links.prev = prevC;
        finalStart = sBlock;
    }
    else{
        int p=0;
        if((*sBlock).header.block_size%16>=2)
        {
            p=2;
        }

       (*sBlock).header.block_size >>=1;
       (*sBlock).header.block_size <<=1;

       void  *k = sBlock;
       k+= ((*sBlock).header.block_size-8-p); //coz previous is allocated
       sf_header footer;
       footer.block_size = (*sBlock).header.block_size;
       footer.requested_size=0;
       (*((sf_header*)k)) = footer;
       finalStart = sBlock;

    }
        finalStart->body.links.next = sf_free_list_head.body.links.next;
        finalStart->body.links.prev = &sf_free_list_head;
        (*(sf_free_list_head.body.links.next)).body.links.prev = finalStart;
        sf_free_list_head.body.links.next = finalStart;
}
