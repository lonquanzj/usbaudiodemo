#include "MailBox.h"



MailBox::MailBox(const wxString& /*i_name*/)
{
    Queue.reserve(32);
}


MailBox::~MailBox()
{

}


int MailBox::NrOfMessagesInQueue()
{
    return (int) Queue.size();
}


// pthreads implementation
PThreadMailBox::PThreadMailBox(const wxString& i_name)
 : MailBox(i_name)
{
    pthread_mutex_init(&pt_ConditionMutex, NULL);
    pthread_cond_init(&pt_Condition, NULL);
}


PThreadMailBox::~PThreadMailBox()
{
    pthread_cond_destroy(&pt_Condition);
    pthread_mutex_destroy(&pt_ConditionMutex);
}


void PThreadMailBox::PutMsg(Message *Msg)
{
    //wxLogDebug("put msg %lu, %lu", Queue.size(), Queue.capacity());
    pthread_mutex_lock(&pt_ConditionMutex);
    Queue.push_back(Msg);
    pthread_cond_signal(&pt_Condition);
    pthread_mutex_unlock(&pt_ConditionMutex);
    //wxLogDebug("put msg end %lx", this);
}


Message *PThreadMailBox::WaitForMessage()
{
    pthread_mutex_lock(&pt_ConditionMutex);

    if (Queue.empty())
    {
        pthread_cond_wait(&pt_Condition, &pt_ConditionMutex);
        pthread_mutex_unlock(&pt_ConditionMutex);

        pthread_mutex_lock(&pt_ConditionMutex);
        Message *msg = (Message *) Queue.front();
        Queue.erase(Queue.begin()); //Queue.pop_front();
        pthread_mutex_unlock(&pt_ConditionMutex);
        return msg;
    }
    else
    {
        Message *msg = (Message *) Queue.front();
        Queue.erase(Queue.begin()); //Queue.pop_front();
        pthread_mutex_unlock(&pt_ConditionMutex);
        return msg;
    }

    return NULL;
}
