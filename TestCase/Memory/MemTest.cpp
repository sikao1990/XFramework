#include <stdio.h>
#include <stdlib.h>
#include "XStaticAlloc.h"
#include <vector>

/************************************************************************/
/*@File Name         : MemTest.cpp
/*@Created Date      : 2022/07/20
/*@Author            : zhuliwu
/*@email             : sikao1990@outlook.com
/*@Description       : 内存管理白盒测试文件
/*@Version           : 1.0
/************************************************************************/

#define TEST_LEN    (134)
/*
[8][4][4][4][4]|[-2-][-96-][-12-]
*/


//for Test11
//#define TEST_LEN      (32*256)

//与实际项目中结构提大小一致
#define TEST_BLOCK     204


//格式化和读取测试
void Test1(XStaticAlloc& allotor1, XStaticAlloc& allotor2,char*pStart,int nLen)
{
    allotor1.Init(pStart, nLen);
    allotor1.debug_print_init(true);
    allotor1.debug_for_UseInfo("Allotor1");

    allotor2.Init(pStart, nLen);
    allotor2.debug_print_init(true);
    allotor2.debug_for_UseInfo("Allotor2");
}

//申请和释放配对测试
void Test2(XStaticAlloc& allotor1, XStaticAlloc& allotor2, char*pStart, int nLen)
{
    allotor1.debug_for_UseInfo("Alloc begin");

    int* p = (int*)allotor1.Alloc(sizeof(int));
    allotor1.debug_for_UseInfo("Alloc p 4");

    allotor1.Free(p);
    allotor1.debug_for_UseInfo("Free p 4");
}
//挤压申请测试
void Test3(XStaticAlloc& allotor1, XStaticAlloc& allotor2, char*pStart, int nLen)
{
    allotor1.debug_for_UseInfo("Alloc begin");

    int* p = (int*)allotor1.Alloc(sizeof(int));
    allotor1.debug_for_UseInfo("Alloc p 4");

    char* p2 = (char*)allotor1.Alloc(6);
    allotor1.debug_for_UseInfo("Alloc p2 6");

    int* p3 = (int*)allotor1.Alloc(sizeof(int));
    allotor1.debug_for_UseInfo("Alloc p3 4");

    //越界测试
    char* p4 = (char*)allotor1.Alloc(64);
    if (NULL != p4)
        allotor1.debug_for_UseInfo("Alloc p4 64");
    else
        puts("Alloc 64 failed!");
}
//跨标记测试
void Test4(XStaticAlloc& allotor1, XStaticAlloc& allotor2, char*pStart, int nLen)
{
    //allotor1.debug_for_UseInfo("Alloc begin");

    int* p = (int*)allotor1.Alloc(sizeof(int));
    //allotor1.debug_for_UseInfo("Alloc p 4");

    char* p2 = (char*)allotor1.Alloc(6);
    //allotor1.debug_for_UseInfo("Alloc p2 6");

    int* p3 = (int*)allotor1.Alloc(sizeof(int));
    //allotor1.debug_for_UseInfo("Alloc p3 4");

    short* ps1 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps1 2");//-
    short* ps2 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps2 2");
    short* ps3 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps3 2");
    short* ps4 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps4 2");
    short* ps5 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps5 2");
    short* ps6 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps6 2");
    short* ps7 = (short*)allotor1.Alloc(sizeof(short));
    allotor1.debug_for_UseInfo("Alloc ps7 2");//-
    short* ps8 = (short*)allotor1.Alloc(sizeof(short));//代码符合预期
    if (NULL != ps8)
        allotor1.debug_for_UseInfo("Alloc ps8 2");
    else
        puts("alloc ps8 failed!");//代码符合预期
    short* ps9 = (short*)allotor1.Alloc(sizeof(short));
    if( NULL != ps9)
        allotor1.debug_for_UseInfo("Alloc ps1-ps9 18");
    else
        puts("alloc ps9 failed!");

    allotor1.Free(ps6);
    allotor1.debug_for_UseInfo("Alloc ps6 2");
}
//跳跃式释放和回收测试
void Test5(XStaticAlloc& allotor1, XStaticAlloc& allotor2, char*pStart, int nLen)
{
    //allotor1.debug_for_UseInfo("Alloc begin");

    int* p = (int*)allotor1.Alloc(sizeof(int));
    //allotor1.debug_for_UseInfo("Alloc p 4");

    char* p2 = (char*)allotor1.Alloc(6);
    //allotor1.debug_for_UseInfo("Alloc p2 6");

    int* p3 = (int*)allotor1.Alloc(sizeof(int));
    //allotor1.debug_for_UseInfo("Alloc p3 4");

    short* ps1 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps1 2");//-
    short* ps2 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps2 2");
    short* ps3 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps3 2");
    short* ps4 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps4 2");
    short* ps5 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps5 2");
    short* ps6 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps6 2");
    short* ps7 = (short*)allotor1.Alloc(sizeof(short));
    allotor1.debug_for_UseInfo("Alloc ps7 2");//-

    allotor1.Free(p3);
    allotor1.debug_for_UseInfo("Free p3 4");//-

    allotor1.Free(ps2);
    allotor1.debug_for_UseInfo("Free ps2 2");//-

    char* pc6 = (char*)allotor1.Alloc(6);
    allotor1.debug_for_UseInfo("Alloc pc6 2");//-
}
//共享测试
void Test6(XStaticAlloc& allotor1, XStaticAlloc& allotor2, char*pStart, int nLen)
{
    int* p = (int*)allotor1.Alloc(sizeof(int));
    //allotor1.debug_for_UseInfo("Alloc p 4");

    char* p2 = (char*)allotor1.Alloc(6);
    //allotor1.debug_for_UseInfo("Alloc p2 6");

    int* p3 = (int*)allotor1.Alloc(sizeof(int));
    //allotor1.debug_for_UseInfo("Alloc p3 4");

    short* ps1 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps1 2");//-
    short* ps2 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps2 2");
    short* ps3 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps3 2");
    short* ps4 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps4 2");
    short* ps5 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps5 2");
    short* ps6 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps6 2");
    short* ps7 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps7 2");//-

    allotor1.Free(p3);
    //allotor1.debug_for_UseInfo("Free p3 4");//-

    allotor1.Free(ps2);
    //allotor1.debug_for_UseInfo("Free ps2 2");//-

    char* pc6 = (char*)allotor1.Alloc(6);
    allotor1.debug_for_UseInfo("Alloc pc6 2");//-

    //设置数据
    char* pp[] = {
        (char*)p,
        (char*)p2,
        //p3,
        (char*)ps1,
        //ps2,
        (char*)ps3,
        (char*)ps4,
        (char*)ps5,
        (char*)ps6,
        (char*)ps7,
        (char*)pc6,
    };
    for (int i = 0; i < sizeof(pp) / sizeof(void*); i++)
    {
        pp[i][0] = 'A' + i;
        printf("[%p]=[%c]", pp[i], pp[i][0]);
    }
    puts("");

    allotor2.Refresh();
    allotor2.debug_for_UseInfo("Refresh allocr2");

    //读取数据
    const std::map<unsigned char*, int>& allDataSet = allotor2.GetMemoryUseInfo();
    printf(" total info count:%d\n",allDataSet.size());
    for (auto it = allDataSet.begin(); it != allDataSet.end(); it++)
    {
        printf("[%p]=[%c]",it->first,it->first[0]);
    }
    puts("");
}
//偏移标记修正测试1
void Test7(XStaticAlloc& allotor1, XStaticAlloc& allotor2, char*pStart, int nLen)
{
    int* p = (int*)allotor1.Alloc(sizeof(int));
    //allotor1.debug_for_UseInfo("Alloc p 4");

    char* p2 = (char*)allotor1.Alloc(6);
    //allotor1.debug_for_UseInfo("Alloc p2 6");

    int* p3 = (int*)allotor1.Alloc(sizeof(int));
    //allotor1.debug_for_UseInfo("Alloc p3 4");

    short* ps1 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps1 2");//-
    short* ps2 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps2 2");
    short* ps3 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps3 2");
    short* ps4 = (short*)allotor1.Alloc(sizeof(short));
    allotor1.debug_for_UseInfo("Alloc ps4 2");

    allotor2.Refresh();
    allotor2.Free(ps3);
    allotor2.debug_for_UseInfo("Free ps3 2");

    allotor1.Refresh();
    allotor1.debug_for_UseInfo("Free ps3 2");
}
//偏移标记修正测试2
void Test8(XStaticAlloc& allotor1, XStaticAlloc& allotor2, char*pStart, int nLen)
{
    int* p = (int*)allotor1.Alloc(sizeof(int));
    //allotor1.debug_for_UseInfo("Alloc p 4");

    char* p2 = (char*)allotor1.Alloc(6);
    //allotor1.debug_for_UseInfo("Alloc p2 6");

    int* p3 = (int*)allotor1.Alloc(sizeof(int));
    //allotor1.debug_for_UseInfo("Alloc p3 4");

    short* ps1 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps1 2");//-
    short* ps2 = (short*)allotor1.Alloc(sizeof(short));
    allotor1.debug_for_UseInfo("Alloc ps2 2");

    allotor2.Refresh();
    allotor2.Free(ps2);
    allotor2.debug_for_UseInfo("Free ps2 2");

    allotor2.Free(p3);
    allotor2.debug_for_UseInfo("Free p3 4");

    allotor1.Refresh();
    allotor1.debug_for_UseInfo("Free p3 4");

}
//碎片合并测试
void Test9(XStaticAlloc& allotor1, XStaticAlloc& allotor2, char*pStart, int nLen)
{
    int* p = (int*)allotor1.Alloc(sizeof(int));
    //allotor1.debug_for_UseInfo("Alloc p 4");

    char* p2 = (char*)allotor1.Alloc(6);
    //allotor1.debug_for_UseInfo("Alloc p2 6");

    int* p3 = (int*)allotor1.Alloc(sizeof(int));
    //allotor1.debug_for_UseInfo("Alloc p3 4");

    short* ps1 = (short*)allotor1.Alloc(sizeof(short));
    //allotor1.debug_for_UseInfo("Alloc ps1 2");//-
    short* ps2 = (short*)allotor1.Alloc(sizeof(short));
    allotor1.debug_for_UseInfo("Alloc ps2 2");

    allotor2.Refresh();
    allotor2.Free(p);
    allotor2.debug_for_UseInfo("Free p 4");

    allotor2.Free(p3);
    allotor2.debug_for_UseInfo("Free p3 4");

    allotor2.Free(ps2);
    allotor2.debug_for_UseInfo("Free ps2 2");
}

//数据的一致性测试
void Test10(XStaticAlloc& allotor1, XStaticAlloc& allotor2, char*pStart, int nLen)
{
    char* p1 = (char*)allotor1.Alloc(10);
    strcpy(p1, "hello");
    int n = p1 - allotor1.GetManagementMemoryStartAddress();
    printf("nOffset:%d address:%p val:%s\n",n,p1,p1);
    allotor1.debug_for_UseInfo("alloctor1");

    allotor2.Refresh();
    const std::map<unsigned char*, int>& infos = allotor2.GetMemoryUseInfo();
    char* pBegin = (char*)allotor2.GetManagementMemoryStartAddress();
    char* p2 = pBegin + n;
    printf("alloc2 :%p val:%s val:%s\n",infos.begin()->first, infos.begin()->first,p2);
    allotor1.debug_for_UseInfo("alloctor2");
}

//容量测试
void Test11(XStaticAlloc& allotor1, XStaticAlloc& allotor2, char*pStart, int nLen)
{
    std::vector<char*> ArrayTest;
    for (int i = 0; i < 30; i++)
    {
        char* p = (char*)allotor1.Alloc(TEST_BLOCK);
        if (NULL != p)
        {
            ArrayTest.push_back(p);
        }
        else
        {
            printf("total Alloc:%d ,the next alloc failed!\n", ArrayTest.size());
            break;
        }
    }
    allotor1.debug_for_UseInfo("end");
}

int main()
{
    char* pStart = (char*)malloc(TEST_LEN);
    XStaticAlloc allotor1;
    XStaticAlloc allotor2;

    //格式化和读取测试
    Test1(allotor1,allotor2,pStart, TEST_LEN);

    //申请和释放配对测试
#if 0
    Test2(allotor1, allotor2, pStart, TEST_LEN);
#endif
    
    //挤压申请测试
#if 1
    Test3(allotor1, allotor2, pStart, TEST_LEN);
#endif

    //跨标记测试
#if 0
    Test4(allotor1, allotor2, pStart, TEST_LEN);
#endif

    //跳跃式释放和回收测试
#if 0
    Test5(allotor1, allotor2, pStart, TEST_LEN);
#endif

    //共享测试
#if 0
    Test6(allotor1, allotor2, pStart, TEST_LEN);
#endif

    //偏移标记修正测试1
#if 0
    Test7(allotor1, allotor2, pStart, TEST_LEN);
#endif

    //偏移标记修正测试2
#if 0
    Test8(allotor1, allotor2, pStart, TEST_LEN);
#endif

    //碎片合并测试
#if 0
    Test9(allotor1, allotor2, pStart, TEST_LEN);
#endif

    //数据一致性测试
#if 0
    Test10(allotor1, allotor2, pStart, TEST_LEN);
#endif

    //容量测试
#if 0
    Test11(allotor1, allotor2, pStart, TEST_LEN);
#endif
    free(pStart);
    system("pause");
    return 0;
}