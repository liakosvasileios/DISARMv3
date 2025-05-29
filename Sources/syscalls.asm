PUBLIC mNtOpenProcess				; SSN
PUBLIC addrNtOpenProcess			; Address of process syscall in ntdll

PUBLIC mNtAllocateVirtualMemory
PUBLIC addrNtAllocateVirtualMemory

PUBLIC mNtWriteVirtualMemory
PUBLIC addrNtWriteVirtualMemory

PUBLIC mNtCreateThreadEx
PUBLIC addrNtCreateThreadEx

PUBLIC mNtClose
PUBLIC addrNtClose

PUBLIC mNtQueryInformationProcess
PUBLIC addrNtQueryInformationProcess

.DATA 
mNtOpenProcess DWORD 0
addrNtOpenProcess QWORD 0

mNtAllocateVirtualMemory DWORD 0
addrNtAllocateVirtualMemory QWORD 0

mNtWriteVirtualMemory DWORD 0
addrNtWriteVirtualMemory QWORD 0

mNtCreateThreadEx DWORD 0
addrNtCreateThreadEx QWORD 0

mNtClose DWORD 0
addrNtClose QWORD 0

mNtQueryInformationProcess DWORD 0
addrNtQueryInformationProcess QWORD 0

.CODE 
align 16

wNtOpenProcess PROC
	
	mov r10, rcx
	mov eax, mNtOpenProcess
	jmp QWORD PTR [addrNtOpenProcess]

wNtOpenProcess ENDP

wNtAllocateVirtualMemory PROC
	
	mov r10, rcx
	mov eax, mNtAllocateVirtualMemory
	jmp QWORD PTR [addrNtAllocateVirtualMemory]

wNtAllocateVirtualMemory ENDP

wNtWriteVirtualMemory PROC
	
	mov r10, rcx
	mov eax, mNtWriteVirtualMemory
	jmp QWORD PTR [addrNtWriteVirtualMemory]

wNtWriteVirtualMemory ENDP

wNtCreateThreadEx PROC
	
	mov r10, rcx
	mov eax, mNtCreateThreadEx
	jmp QWORD PTR [addrNtCreateThreadEx]

wNtCreateThreadEx ENDP

wNtClose PROC
	
	mov r10, rcx
	mov eax, mNtClose
	jmp QWORD PTR [addrNtClose]

wNtClose ENDP

wNtQueryInformationProcess PROC
	
	mov r10, rcx
	mov eax, mNtQueryInformationProcess
	jmp QWORD PTR [addrNtQueryInformationProcess]

wNtQueryInformationProcess ENDP

END 