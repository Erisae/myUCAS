/*
Description:
假设以块链结构作串的存储结构。试编写判别给定串是否具有对称性的算法，并要求算法的时间复杂度为O(StrLength(S))。注：字符串长度不超过200，且有可能包含空格。
Input:
一个字符串 s 。
Output:
如果对称输出 "Yes",  不对称输出 “No"
*/
#include <stdio.h>//用双向循环链表
#include <stdlib.h>
typedef struct node
{
    char character;
    struct node *next;
    struct node *pre;
}list,*plist;
int judge(plist,plist);
int main()
{
    plist head=(plist)malloc(sizeof(list));
    plist p,pp;
    int c;
    int result;
    head->character='\0';
    head->next=head;
    head->pre=head;
    pp=head;
    while((c=getchar())!='\n')
    {
        p=(plist)malloc(sizeof(list));
        p->character=c;
        p->next=head;
        pp->next=p;
        p->pre=pp;
        pp=p;
    }
    head->pre=p;
    result=judge(head->next,head->pre);
    if(result==1)
    printf("Yes");
    else 
    printf("No");

    return 0;
}

int judge(plist start,plist end)
{
    if(start==end || end->next==start)
    return 1;//1说明是对称的
    else
    {
        if(start->character==end->character)
            judge(start->next,end->pre);
        else
        {
            return 0;//0说明不是对称的
        }
    }
}