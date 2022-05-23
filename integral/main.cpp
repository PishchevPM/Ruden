#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <algorithm>
#include <iostream>
#include <vector>

int main (int argc, char* argv[])
{
    int calcs = atoi (argv[1]);
    int matrix_size = atoi (argv[2]);

    int     semid;
    char    pathname[] = "novkeyf";
    key_t   key;
    struct sembuf mybuf;

    float sign = 1;

    void* shmem = mmap(NULL, 3 * sizeof (int) + matrix_size * matrix_size * sizeof (float), PROT_WRITE | PROT_READ,
        MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if(shmem == MAP_FAILED)
	{
		printf("Mapping failed!\n");
		exit(-1);
	}

    float* matrix = (float*) shmem;
    int* cursubmatr = (int*)(matrix + (matrix_size * matrix_size));
    int* curstr = cursubmatr + 1;
    int* curend = cursubmatr + 2;

    (*cursubmatr) = 0;
    (*curstr) = 1;
    (*curend) = 0;

    for (int i = 0; i < matrix_size*matrix_size; i++)
    {
        std::cin >> matrix[i];
    }

    if((key = ftok(pathname,0)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if((semid = semget(key, 2, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can\'t create semaphore set\n");
        exit(-1);
    }

    mybuf.sem_num = 0;
    mybuf.sem_op  = 1;
    mybuf.sem_flg = 0;

    if(semop(semid, &mybuf, 1) < 0)
    {
        std::cerr << "Can\'t add 1 to semaphore" << std::endl;
        exit(-1);
    } 

    int ppid;
    for (int i = 0; i < calcs; i++)
    {
        ppid = fork ();
        if (ppid < 0)
        {
            std::cerr << "fork() error" << std::endl;
            exit(-1);
        }
        else if (ppid == 0) //child
            break;
    }

    while (true)
    {

        if (*curstr >= matrix_size)
        {
            mybuf.sem_op  = -1;
            if(semop(semid, &mybuf, 1) < 0)
            {
                std::cerr << "Can\'t wait for condition" << std::endl;
                exit(-1);
            }  
            (*curend)++;
            if ((*curend) == calcs)
            {
                (*cursubmatr)++;
                (*curstr) = (*cursubmatr) + 1;
                (*curend) = 0;

                mybuf.sem_op  = 1;
                if(semop(semid, &mybuf, 1) < 0)
                {
                    std::cerr << "Can\'t wait for condition" << std::endl;
                    exit(-1);
                }  
                if (*cursubmatr == matrix_size - 1)
                {                        
                    float answer = sign;
                    for (int i = 0; i < matrix_size; i++)  
                        answer *= matrix [i * matrix_size + i];

                    std::cout << answer << std::endl;

                    for (int i = 0; i < calcs - 1; i++)
                    {
                        mybuf.sem_op  = 1;
                        mybuf.sem_num = 1;
                        if(semop(semid, &mybuf, 1) < 0)
                        {
                            std::cerr << "Can\'t add 1 to semaphore" << std::endl;
                            exit(-1);
                        }
                        mybuf.sem_num = 0;
                    }

                    break;

                }

                int i;
                for (i = (*cursubmatr); i < matrix_size; i++)
                {
                    if (matrix [i*matrix_size + (*cursubmatr)] > 10*__FLT_EPSILON__ ||
                        matrix [i*matrix_size + (*cursubmatr)] < -10*__FLT_EPSILON__)
                        break;
                }
                if (i == matrix_size)
                {
                    std::cout << 0 << std::endl;
                    for (int i = 0; i < calcs - 1; i++)
                    {
                        mybuf.sem_op  = 1;
                        mybuf.sem_num = 1;
                        if(semop(semid, &mybuf, 1) < 0)
                        {
                            std::cerr << "Can\'t add 1 to semaphore" << std::endl;
                            exit(-1);
                        }
                        mybuf.sem_num = 0;
                    }
                    break;
                }
                else if (i != (*cursubmatr))
                {
                    for (int k = (*cursubmatr); k < matrix_size; k++)
                        std::swap (matrix [i * matrix_size + k], 
                                   matrix [(*cursubmatr) * matrix_size + k]);
                    sign *= -1;
                }

                for (int i = 0; i < calcs; i++)
                {
                    mybuf.sem_op  = 1;
                    mybuf.sem_num = 1;
                    if(semop(semid, &mybuf, 1) < 0)
                    {
                        std::cerr << "Can\'t add 1 to semaphore" << std::endl;
                        exit(-1);
                    }
                    mybuf.sem_num = 0;
                }
                
            }
            else
            {
                mybuf.sem_op  = 1;
                if(semop(semid, &mybuf, 1) < 0)
                {
                    std::cerr << "Can\'t wait for condition" << std::endl;
                    exit(-1);
                }  
            }

            mybuf.sem_op  = -1;
            mybuf.sem_num = 1;
            if(semop(semid, &mybuf, 1) < 0)
            {
                std::cerr << "Can\'t wait for condition" << std::endl;
                exit(-1);
            }  
            mybuf.sem_num = 0;

            if (*cursubmatr == matrix_size - 1)
                break;
        }

        mybuf.sem_op  = -1;
        if(semop(semid, &mybuf, 1) < 0)
        {
            std::cerr << "Can\'t wait for condition" << std::endl;
            exit(-1);
        }  

        bool apr = (*curstr == matrix_size);
        int tecobrstr = (*curstr);
        (*curstr)++;
        if (apr) (*curstr)--;
        mybuf.sem_op  = 1;
        
        if(semop(semid, &mybuf, 1) < 0)
        {
            std::cerr << "Can\'t add 1 to semaphore" << std::endl;
            exit(-1);
        }  
        if (apr) continue;

        float tempr = matrix [tecobrstr*matrix_size + (*cursubmatr)];
        for (int i = (*cursubmatr); i < matrix_size; i++)
        {
            matrix [tecobrstr*matrix_size + i] -= matrix [(*cursubmatr)*matrix_size + i] *
                                                  tempr / matrix [(*cursubmatr)*matrix_size + (*cursubmatr)];
        };
    }

    munmap (shmem, 3 * sizeof (int) + matrix_size * matrix_size * sizeof (float));
}