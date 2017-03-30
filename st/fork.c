/*
 * fork.c
 * Experimental fork() on Windows.  Requires NT 6 subsystem or
 * newer.
 *
 * Copyright (c) 2012 William Pitcock <nenolod@dereferenced.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * This software is provided 'as is' and without any warranty, express or
 * implied.  In no event shall the authors be liable for any damages arising
 * from the use of this software.
 */

//#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winnt.h>
#include <ntdef.h>
#include <stdio.h>
#include <errno.h>
#include <process.h>
#include "winpoll.h"
#include <string.h>
#include <io.h>

#include <Powrprof.h>
#pragma comment(lib, "powrprof.lib")

#ifdef __MINGW32__
typedef struct _CLIENT_ID {
	PVOID UniqueProcess;
	PVOID UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _SECTION_IMAGE_INFORMATION {
	PVOID EntryPoint;
	ULONG StackZeroBits;
	ULONG StackReserved;
	ULONG StackCommit;
	ULONG ImageSubsystem;
	WORD SubSystemVersionLow;
	WORD SubSystemVersionHigh;
	ULONG Unknown1;
	ULONG ImageCharacteristics;
	ULONG ImageMachineType;
	ULONG Unknown2[3];
} SECTION_IMAGE_INFORMATION, *PSECTION_IMAGE_INFORMATION;

typedef struct _RTL_USER_PROCESS_INFORMATION {
	ULONG Size;
	HANDLE Process;
	HANDLE Thread;
	CLIENT_ID ClientId;
	SECTION_IMAGE_INFORMATION ImageInformation;
} RTL_USER_PROCESS_INFORMATION, *PRTL_USER_PROCESS_INFORMATION;

#define RTL_CLONE_PROCESS_FLAGS_CREATE_SUSPENDED	0x00000001
#define RTL_CLONE_PROCESS_FLAGS_INHERIT_HANDLES		0x00000002
#define RTL_CLONE_PROCESS_FLAGS_NO_SYNCHRONIZE		0x00000004

#define RTL_CLONE_PARENT				0
#define RTL_CLONE_CHILD					297

#endif

typedef NTSTATUS (*RtlCloneUserProcess_f)(ULONG ProcessFlags,
	PSECURITY_DESCRIPTOR ProcessSecurityDescriptor /* optional */,
	PSECURITY_DESCRIPTOR ThreadSecurityDescriptor /* optional */,
	HANDLE DebugPort /* optional */,
	PRTL_USER_PROCESS_INFORMATION ProcessInformation);

pid_t fork(void)
{
	HMODULE mod;
	RtlCloneUserProcess_f clone_p;
	RTL_USER_PROCESS_INFORMATION process_info;
	NTSTATUS result;

	mod = GetModuleHandle("ntdll.dll");
	if (!mod)
		return -ENOSYS;

	clone_p = GetProcAddress(mod, "RtlCloneUserProcess");
	if (clone_p == NULL)
		return -ENOSYS;

	/* lets do this */
	result = clone_p(RTL_CLONE_PROCESS_FLAGS_CREATE_SUSPENDED | RTL_CLONE_PROCESS_FLAGS_INHERIT_HANDLES, NULL, NULL, NULL, &process_info);

	if (result == RTL_CLONE_PARENT)
	{
		HANDLE me = GetCurrentProcess();
		pid_t child_pid;

		child_pid = GetProcessId(process_info.Process);

		ResumeThread(process_info.Thread);
		CloseHandle(process_info.Process);
		CloseHandle(process_info.Thread);

		return child_pid;
	}
	else if (result == RTL_CLONE_CHILD)
	{
		/* fix stdio */
		AllocConsole();
		return 0;
	}
	else
		return -1;

	/* NOTREACHED */
	return -1;
}

int kill(pid_t dPID,int signo)
{
    HANDLE hProcessHandle = OpenProcess(PROCESS_TERMINATE, FALSE, dPID);
    return TerminateProcess( hProcessHandle, signo);
}


static const char *  inet_ntop4(const u_char *src, char *dst, size_t size)
{
     static const char fmt[] = "%u.%u.%u.%u";
     char tmp[sizeof("255.255.255.255")];
     int l;
     l = _snprintf(tmp, size, fmt, src[0], src[1], src[2], src[3]);
     if (l <= 0 || l >= size)
     {
      return (NULL);
     }
     strncpy(dst, tmp, size);
     return (dst);
}

#ifndef INADDRSZ
#define INADDRSZ	4		/* size of an IPv4 address in bytes */
#endif /* ! INADDRSZ */
#ifndef IN6ADDRSZ
#define IN6ADDRSZ	16		/* size of an IPv6 address in bytes */
#endif /* ! IN6ADDRSZ */
#ifndef INT16SZ
#define INT16SZ	2		/* size of a 16 bit integer in bytes */
#endif /* ! INT16SZ */
#ifndef INT32SZ
#define INT32SZ	4		/* size of a 32 bit integer in bytes */
#endif /* ! INT32SZ */
#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK	0x7f000001	/* loopback address */
#endif /* ! INADDR_LOOPBACK */

size_t strlcpy(char * __restrict dst, const char * __restrict src, size_t siz)
    {
        char *d = dst;
        const char *s = src;
        size_t n = siz;
    
        /* Copy as many bytes as will fit */
        if (n != 0) {
            while (--n != 0) {
                if ((*d++ = *s++) == '\0')
                    break;
            }
        }
    
        /* Not enough room in dst, add NUL and traverse rest of src */
        if (n == 0) {
            if (siz != 0)
                *d = '\0';        /* NUL-terminate dst */
            while (*s++)
                ;
        }
    
        return(s - src - 1);    /* count does not include NUL */
    }
    
static const char *  inet_ntop6(const u_char *src, char *dst, size_t size)
{
    char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"];
    char *tp, *ep;
    struct
    {
     int base, len;
    } best, cur;
    u_int words[IN6ADDRSZ / INT16SZ];
    int i;
    int advance;
    memset(words, '\0', sizeof(words));
    for (i = 0; i < IN6ADDRSZ; i++)
        words[i / 2] |= (src[i] << ((1 - (i % 2)) << 3));
    best.base = -1;
    cur.base = -1;
    for (i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
        if (words[i] == 0) {
            if (cur.base == -1)
                cur.base = i, cur.len = 1;
            else
            cur.len++;
        }
        else {
            if (cur.base != -1) {
                if (best.base == -1 || cur.len > best.len)
                best = cur;
                cur.base = -1;
            }
        }
    }
    if (cur.base != -1) {
        if (best.base == -1 || cur.len > best.len)
            best = cur;
    }
    if (best.base != -1 && best.len < 2)
        best.base = -1;
    tp = tmp;
    ep = tmp + sizeof(tmp);
    for (i = 0; i < (IN6ADDRSZ / INT16SZ) && tp < ep; i++) {
    /** Are we inside the best run of 0x00's? */
    if (best.base != -1 && i >= best.base && i < (best.base + best.len)) {
        if (i == best.base) {
            if (tp + 1 >= ep)
                return (NULL);
            *tp++ = ':';
        }
        continue;
        }
        /** Are we following an initial run of 0x00s or any real hex? */
        if (i != 0) {
            if (tp + 1 >= ep)
                return (NULL);
            *tp++ = ':';
        }
        /** Is this address an encapsulated IPv4? */
        if (i == 6 && best.base == 0 &&
        (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
            if (!inet_ntop4(src+12, tp, (size_t)(ep - tp)))
                return (NULL);
            tp += strlen(tp);
            break;
        }
        advance = snprintf(tp, ep - tp, "%x", words[i]);
        if (advance <= 0 || advance >= ep - tp)
            return (NULL);
        tp += advance;
    }
    /** Was it a trailing run of 0x00's? */
    if (best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ)) {
        if (tp + 1 >= ep)
            return (NULL);
        *tp++ = ':';
    }
    if (tp + 1 >= ep)
        return (NULL);
    *tp++ = '\0';

    /**
    * Check for overflow, copy, and we're done.
    */
    if ((size_t)(tp - tmp) > size) {
        errno = ENOSPC;
        return (NULL);
    }
    strlcpy(dst, tmp, size);
    return (dst);
}

const char *  inet_ntop(int af, const void *src, char *dst, size_t size)
{
    switch (af) {
    case AF_INET:
        return (inet_ntop4(src, dst, size));
    case AF_INET6:
        return (inet_ntop6(src, dst, size));
    default:
        errno = EAFNOSUPPORT;
        return (NULL);
    }
}

int GetCpuHz( )
{
    int OutSize;
    PROCESSOR_POWER_INFORMATION cpuinfo;
    NTSTATUS iRet=CallNtPowerInformation(11,NULL,0,&cpuinfo,OutSize);
    if(iRet==STATUS_SUCCESS)
    {
        return cpuinfo.CurrentMhz;
    }else
    {
        return -1;
    }
}


#ifdef __TEST__
int main(int argc, const char *argv[])
{
	pid_t pid = fork();

	switch (pid) {
	case 0:
	{
		FILE *f = fopen("C:/Users/nenolod/Documents/forktest.dat", "w");
		fprintf(f, "ok\n");
		fclose(f);
		break;
	}
	default:
		printf("child %d\n", pid);
		while (1) { Sleep(1000); }
		break;
	}
}
#endif
