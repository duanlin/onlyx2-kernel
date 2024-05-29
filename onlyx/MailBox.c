#include <string.h>
#include <malloc.h>

#include "MailBox.h"

#include "SysSched.h"


int initMailBox(MailBox* mailbox)
{
	memset(mailbox, 0, sizeof(MailBox));
	
	if(initMutex(&mailbox->mutex))
		return -1;
	if(initSemaphore(&mailbox->queue))
		return -2;

	return 0;
}

int sendMail(MailBox* mailbox, uint32_t info, void* payload, size_t size)
{
	SignedMail* mail;
	SignedMail* next;

	mail = malloc(sizeof(SignedMail));
	if(!mail)
		return -1;
	memset(mail, 0, sizeof(SignedMail));

	mail->taskid = gCurrentTask;
	mail->info = info;
	mail->payload = malloc(size);
	if(!mail->payload)
	{
		free(mail);

		return -2;
	}
	memcpy(mail->payload, payload, size);

	if(lockMutex(&mailbox->mutex))
		return -3;

	if(!mailbox->mailChain)
		mailbox->mailChain = mail;
	else for(next = mailbox->mailChain; next != NULL; next = next->nextMail)
	{
		if(!next->nextMail)
		{
			next->nextMail = mail;

			break;
		}
	}
	if(giveSemaphore(&mailbox->queue))
		return -4;

	if(giveMutex(&mailbox->mutex))
		return -5;

	return 0;
}

int pollMail(MailBox* mailbox, uint32_t info, void* payload, size_t size)
{
	SignedMail* mail;
	SignedMail* next;

	mail = malloc(sizeof(SignedMail));
	if(!mail)
		return -1;
	memset(mail, 0, sizeof(SignedMail));

	mail->taskid = gCurrentTask;
	mail->info = info;
	mail->payload = payload;
	mail->waiting = true;

	if(lockMutex(&mailbox->mutex))
		return -2;

	if(!mailbox->mailChain)
		mailbox->mailChain = mail;
	else for(next = mailbox->mailChain; next != NULL; next = next->nextMail)
	{
		if(!next->nextMail)
		{
			next->nextMail = mail;

			break;
		}
	}
	if(giveSemaphore(&mailbox->queue))
		return -3;

	if(giveMutex(&mailbox->mutex))
		return -4;

	if(takeSemaphore(&mail->echo))
		return -5;

	free(mail);

	return 0;
}

int takeMail(MailBox* mailbox, MailHandler handler)
{
	SignedMail* mail;

	if(lockMutex(&mailbox->mutex))
		return -1;

	if(!mailbox->queue.count)
	{
		if(giveMutex(&mailbox->mutex))
			return -2;

		return -3;
	}

	if(mailbox->mailChain)
	{
		mail = mailbox->mailChain;
		mailbox->mailChain = mail->nextMail;

		if(mail->waiting)
		{
			if(giveSemaphore(&mail->echo))
				return -4;
		}
		else
		{
			free(mail->payload);
			free(mail);
		}
	}

	if(giveMutex(&mailbox->mutex))
		return -5;

	return 0;
}

int waitMail(MailBox* mailbox, MailHandler handler)
{
	SignedMail* mail;

	if(takeSemaphore(&mailbox->queue))
		return -1;

	if(lockMutex(&mailbox->mutex))
		return -2;

	if(mailbox->mailChain)
	{
		mail = mailbox->mailChain;
		mailbox->mailChain = mail->nextMail;

		if(mail->waiting)
		{
			if(giveSemaphore(&mail->echo))
				return -3;
		}
		else
		{
			free(mail->payload);
			free(mail);
		}
	}

	if(giveMutex(&mailbox->mutex))
		return -4;

	return 0;
}
