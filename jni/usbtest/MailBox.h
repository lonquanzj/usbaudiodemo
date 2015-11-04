#ifndef MAILBOX_H
#define MAILBOX_H

class MailBox;

#include <pthread.h>
#include <vector>

using namespace std;
class wxString;

struct Message
{
    MailBox *ReplyMailBox;
    int Type;
    bool DeleteOnReceive;
};

class MailBox
{
public:

	MailBox(const wxString& i_name);
	virtual ~MailBox();

    virtual void PutMsg(Message *Msg) = 0;
    virtual Message *WaitForMessage() = 0;
    
    int NrOfMessagesInQueue();

protected:
    vector<Message *> Queue;

private:
};


class PThreadMailBox : public MailBox
{
public:

	PThreadMailBox(const wxString& i_name);
	~PThreadMailBox();

    void PutMsg(Message *Msg);
    Message *WaitForMessage();

private:

    pthread_mutex_t pt_ConditionMutex;
    pthread_cond_t pt_Condition;
};

#endif
