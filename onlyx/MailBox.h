#ifndef MAILBOX_H
#define MAILBOX_H


#include <stdbool.h>
#include <stdint.h>

#include "SysLock.h"
#include "SysTask.h"


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct SignedMail
{
	SysTask* taskid;
	
	uint32_t info;
	size_t size;
	void* payload;
	
	bool waiting;
	SysSem echo;

	struct SignedMail* nextMail;
	
} SignedMail;

typedef struct
{
	SysSem mutex;
	SysSem queue;
	
	SignedMail* mailChain;
	
} MailBox;


typedef int (*MailHandler)(SysTask* taskid, uint32_t info, void* payload, size_t size);


int initMailBox(MailBox* mailbox);

// None block mail send, payload memory release by receiver
int sendMail(MailBox* mailbox, uint32_t info, void* payload, size_t size);
// Blocked mail send, while reciever echo an ack
int pollMail(MailBox* mailbox, uint32_t info, void* payload, size_t size);

// None block mail take, return if there is no mail
int takeMail(MailBox* mailbox, MailHandler handler);
// Blocked mail wait, while a mail has been handled
int waitMail(MailBox* mailbox, MailHandler handler);


#ifdef __cplusplus
}
#endif


#endif
